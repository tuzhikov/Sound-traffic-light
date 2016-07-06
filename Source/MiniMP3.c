/*----------------------------------------------------------------------------*/
//
// $Title:          MiniMP3.c
// $Description:    Mini mp3 Interface.
//
// $Rev:            0.0.1
//
// $Author:         ATA
// $Created:        23.06.16
// $Last Modified:
//
// $Copyright:                     `
//
// $Notes:
//
/*----------------------------------------------------------------------------*/
#include "MiniMP3.h"
#include "UART.h"
#include "DPS.h"
#include "Delay.h"
#include "DataTable.h"
#include "System.h"
#include "Pins.h"
#include "Inputs.h"
#include "vibro.h"
#include "TypeData.h"
#define TEST_MODUE_MP3

// 7E FF 06 0F 00 01 01 xx xx EF\
// 0	->	7E is start code
// 1	->	FF is version
// 2	->	06 is length
// 3	->	0F is command
// 4	->	00 is no receive
// 5~6	->	01 01 is argument
// 7~8	->	checksum = 0 - ( FF+06+0F+00+01+01 )
// 9	->	EF is end code

#define MAX_BUFF_MP3 10

#define MUSIC_GREEN_MODE 1
#define MUSIC_RED_MODE   2
#define MUSIC_TIK_TAK    200
#define MUSIC_TOP_TOP    210
#define MAX_MUSIC_GREEN  90
#define MAX_MUSIC_RED    190
#define TIME_GREEN_END   3
#define TIME_OFFSET      0
#define MAX_VALUE_CHANEL_ONE 0xFF
#define MAX_VALUE_CHANEL_TWO 0xFF

typedef struct{
    uint8 command;
    uint8 para1;
    uint8 para2;
}TypeAnswerMP3;

typedef enum{
    STAY_ONE    = 0x3C,
    STAY_TWO    = 0x3D,
    STAY_THREE  = 0x3E,
    START_POWER = 0x3F,
    RETURN_ERR  = 0x40,
    REPLY       = 0x41,
    CERR_VOLUME = 0x43
}TypeCommandMP3;

static uint8 send_buf[MAX_BUFF_MP3] = {0x7E, 0xFF, 06, 00, 00, 00, 00, 00, 00, 0xEF};
static const uint8 numberMusicGreen[] = {10,20,30,40,50,60,MAX_MUSIC_GREEN};
static const uint8 numberMusicRed[] = {110,120,130,140,150,160,MAX_MUSIC_RED};
static uint8 timeDelayAnswer;
//static bool is_reply = false;
static TypeStep step = ONE;
static const TStatus *pStatus = &Status; // состояние модуля
static TypeAnswerMP3 answerMP3;
//prototip
static bool disabledSignal(void);
static bool enabledRedSignal(void);
static bool enabledGreenSignal(void);
static int16 valueChanelOne(const uint8 val);
static int16 valueChanelTwo(const uint8 val);
//static void enableModuleOne(const bool on);
//static void enableModuleTwo(const bool on);

typedef enum{OFF_STATE,RED_STATE,GREEN_STAT,END_STAT}TMainState;
static bool (*operationModeMP3[])()={disabledSignal,enabledRedSignal,enabledGreenSignal};

static uint8 getNumberMusicRed(const uint16 currTime);
static uint8 getNumberMusicGreen(const uint16 currTime);
static uint8 (*pGetNumberMusic[])(const uint16)={0,getNumberMusicRed,getNumberMusicGreen};
/*----------------------------------------------------------------------------*/
/**/
static void fill_uint16_bigend(uint8 *thebuf, uint16 data)
{
    *thebuf = (uint8)(data>>8);
    *(thebuf+1) = (uint8)data;
}
//calc checksum (1~6 byte)
static uint16 mp3_get_checksum(uint8 *thebuf)
{
    uint16 sum = 0;
    for (int i=1; i<7; i++)
    {
        sum += thebuf[i];
    }
    return -sum;
}
//fill checksum to send_buf (7~8 byte)
static void mp3_fill_checksum(void)
{
    uint16 checksum = mp3_get_checksum(send_buf);
    fill_uint16_bigend(send_buf+7, checksum);
}
/**/
static void mp3_send_cmd_len(uint8 cmd, uint16 arg)
{
    send_buf[3] = cmd;
    fill_uint16_bigend ((send_buf+5), arg);
    mp3_fill_checksum ();
    sendBuffUART0(send_buf,sizeof(send_buf));
    // clear value answer
    answerMP3.command = 0;
    timeDelayAnswer = GetTimerTik(tTimer100ms);
}
/*
static void mp3_send_cmd(uint8 cmd)
{
    send_buf[3] = cmd;
    fill_uint16_bigend ((send_buf+5), 0);
    mp3_fill_checksum ();
    sendBuffUART0(send_buf,sizeof(send_buf));
    answerMP3.command = 0;
    timeDelayAnswer = GetTimerTik(tTimer100ms);
}*/
//0x06 set volume 0-30
static void mp3_set_volume(const uint16 volume)
{
    mp3_send_cmd_len(0x06, volume);
}
//specify a mp3 file in mp3 folder in your tf card, "mp3_play (1);" mean play "mp3/0001.mp3"
static void mp3_play_number(const uint16 num)
{
    mp3_send_cmd_len (0x12, num);
}
/*
//
static void mp3_play_physical_number(uint16 num)
{
    mp3_send_cmd_len (0x03, num);
}
//
void mp3_play_physical(void)
{
    mp3_send_cmd (0x03);
}
//Next track
static void mp3_next(void)
{
    mp3_send_cmd (0x01);
}
//Previous track
static void mp3_prev(void)
{
    mp3_send_cmd (0x02);
}
//0x07 set EQ0/1/2/3/4/5    Normal/Pop/Rock/Jazz/Classic/Bass
static void mp3_set_EQ (const uint16 eq)
{
    mp3_send_cmd_len(0x07, eq);
}
//0x09 set device 1/2/3/4/5 U/SD/AUX/SLEEP/FLASH
static void mp3_set_device(const uint16 device)
{
    mp3_send_cmd_len(0x09, device);
}
//
static void mp3_sleep(void)
{
    mp3_send_cmd (0x0a);
}
//
static void mp3_reset(void)
{
    mp3_send_cmd (0x0c);
}
//
static void mp3_play(void)
{
    mp3_send_cmd (0x0d);
}
//
static void mp3_pause(void)
{
    mp3_send_cmd (0x0e);
}
//
static void mp3_stop(void)
{
    mp3_send_cmd (0x16);
}
//
static void mp3_get_state(void)
{
    mp3_send_cmd (0x42);
}
//
static void mp3_get_volume(void)
{
    mp3_send_cmd(0x43);
}
static void mp3_get_mode(void)
{
    mp3_send_cmd (0x45);
}
//
static void mp3_get_u_sum(void)
{
    mp3_send_cmd (0x47);
}
//
static void mp3_get_tf_sum(void)
{
    mp3_send_cmd (0x48);
}
//
static void mp3_get_flash_sum (void)
{
    mp3_send_cmd (0x49);
}
//number file on TF
static void mp3_get_tf_current(void)
{
    mp3_send_cmd (0x4c);
}
static void mp3_get_u_current (void)
{
    mp3_send_cmd (0x4b);
}

static void mp3_get_flash_current(void)
{
    mp3_send_cmd (0x4d);
}
static void mp3_single_loop (bool state)
{
    mp3_send_cmd_len(0x19, !state);
}

//add
static void mp3_single_play (const uint16 num)
{
    mp3_play_number (num);
    mp3_single_loop (true);
}
static void mp3_DAC (bool state)
{
    mp3_send_cmd_len(0x1a, !state);
}
//
static void mp3_random_play(void)
{
    mp3_send_cmd (0x18);
}
static void mp3_set_reply(bool state)
{
    is_reply = state;
    send_buf[4] = is_reply;
}
*/
/*----------------------------------------------------------------------------*/
/**
*set parametr RDAC1 digital potentiometer
* return setting value RDAC1
* par: val 0...255
*/
static int16 valueChanelOne(const uint8 val)
{
    int16 retPosition = ERROR;
    const bool ret = setTimeDPS(ADDRDAC1,val);
    if(ret){
        uint8 position;
        getTimeDPS(&position);
        retPosition = position;
    }
    return retPosition;
}
/**
*set parametr RDAC2 digital potentiometer
* return setting value  RDAC2
* par: val 0...255
*/
static int16 valueChanelTwo(const uint8 val)
{
    int16 retPosition = ERROR;
    const bool ret = setTimeDPS(ADDRDAC2,val);
    if(ret){
        uint8 position;
        getTimeDPS(&position);
        retPosition = position;
    }
    return retPosition;
}
/*----------------------------------------------------------------------------*/
/**
* function read answer MP3 plaer
*/
static void takeMessageMp3(void)
{
    uint16 SizeIn;
    uint8 *pDataIn = UART_ReceiveMp3(&SizeIn);

    if(pDataIn != NULL)
    {
      if(pDataIn[0]==0xFF){
        answerMP3.command = pDataIn[2];// CMD
        answerMP3.para1 = pDataIn[4];
        answerMP3.para2 = pDataIn[5];
      }
    }
}
/**
*
*/

/*static void enableModuleOne(const bool on)
{

}
*/
/**
*
*/
/*
static void enableModuleTwo(const bool on)
{

}
*/
/**
*
*/
static void enableButtonLed(const bool on)
{

}
/**
* LOW level sdh On
*/
static void enableShdMode(const bool sdnOn)
{
    if(sdnOn){
        FIOSET |= SHDN_AMPL_MASK;
    }else{
        FIOCLR |= SHDN_AMPL_MASK;
    }
}
/**
*
*/
static uint8 getNumberMusicRed(const uint16 currTime)
{
    const uint8 indexTime = currTime/10;
    const uint8 index = indexTime>=sizeof(numberMusicRed)?sizeof(numberMusicRed)-1:indexTime;
    return numberMusicRed[index];
}
/**
*
*/
static uint8 getNumberMusicGreen(const uint16 currTime)
{
    const uint8 indexTime = currTime/10;
    const uint8 index = indexTime>=sizeof(numberMusicGreen)?sizeof(numberMusicGreen)-1:indexTime;
    return numberMusicGreen[index];
}
/**
*
*/
static uint8 getNumberMusic(const uint16 currTime)
{
    const uint8 status = pStatus->MainState;
    return (*pGetNumberMusic[status])(currTime+TIME_OFFSET);
}
/**
* val: 0...100
* ret: 0...30
*/
static uint16 retValueMP3(const uint16 value)
{
  #ifdef TEST_MODUE_MP3
    return 27; //27
  #else
    const uint16 maxValueMP3 = 30;
    if(value==0)return 0;
    if(value<10)return 1;
    if(value==100)return 30;
    return (uint16)(value*(double)(maxValueMP3/100));
  #endif
}
/**
* check command MP3
* val: commmand
* return:
*/
static bool checkCommand(const TypeCommandMP3 command)
{
    if(answerMP3.command == command){
        answerMP3.command = 0; // clear answer command
        return true;
    }
    return false;
}
/**
*
*/
static TypeAnswer waitingResponseMP3(const TypeCommandMP3 command, const uint8 delay)
{
    TypeAnswer retAnswer = DELAY;

    if(answerMP3.command == command){
        answerMP3.command = 0; // clear answer command
        retAnswer = OK;
    }else{
        if(DelayOut(timeDelayAnswer,GetTimerTik(tTimer100ms),delay)){
            retAnswer = NO_ANSWER;
        }
    }
    return retAnswer;
}
/**
* Call red signal
*
*/
static bool enabledRedSignal(void)
{
    const uint8 numberTimer = 0;
    static uint32 numberMusic = MUSIC_RED_MODE;
    static bool buttonOk = false;
    bool result = false;

    // working green sygnal
    if(pStatus->GreenState){
        step = ONE;
        return true;
    }
    // check button
    if(retStatusButton()){
        buttonOk = true;
        setParametrVibro(true,10,10,20,1,2);
        enableButtonLed(true);
    }
    // working red sygnal
    switch(step)
    {
        case ONE:
            enableShdMode(false);
            clearParametrVibro();
            if(ms10TimeDelay(50,numberTimer)){
                //enableShdMode(false);
                mp3_set_volume(retValueMP3(pStatus->CurVolume));
                valueChanelOne(MAX_VALUE_CHANEL_ONE);
                valueChanelTwo(MAX_VALUE_CHANEL_TWO);
                //clearParametrVibro();
                step = TWO;
            }
        break;
        // play first music
        case TWO:
            if(checkCommand(RETURN_ERR)){
                step = ONE;
            }
            if(ms10TimeDelay(10,numberTimer)){
                mp3_play_number(MUSIC_RED_MODE);
                step = THREE;
            }

        break;
        // aswer
        case THREE:
            // erorr cmd
            if(checkCommand(RETURN_ERR)){
                step = ONE;
            }
            // music end
            if(checkCommand(STAY_TWO)){
                step = FOUR;
            }
            /*
            if(retStatusButton()){
                buttonOk = true;
                setParametrVibro(true,10,10,20,1,2);
                enableButtonLed(true);
            }
            */
        break;
        // check button
        case FOUR:
            // button OK
            if(buttonOk){
                buttonOk = false;
                mp3_play_number(MUSIC_RED_MODE);
                //setParametrVibro(true,5,5,10,1);
                //enableButtonLed(true);
                step = FIVE;
            }else{
                mp3_play_number(MUSIC_TIK_TAK);
                step = THREE;
            }
        break;
        // button OK
        case FIVE:
            if(ms10TimeDelay(50,numberTimer)){
                //clearParametrVibro();
                enableButtonLed(false);
                step = SIX;
            }
        break;
        case SIX:
            // chesk
            if((checkCommand(RETURN_ERR))||(pStatus->RCount == ERROR)){
                step = SEVEN;
                break;
            }else{
                if(checkCommand(STAY_TWO)){
                    numberMusic = getNumberMusic(pStatus->RCount);
                    mp3_play_number(numberMusic);
                    buttonOk = false;
                    step = THREE; // waiting for an answer
                }
            }
        break;
        case SEVEN:
            if(ms10TimeDelay(250,numberTimer)){
                step = FOUR; // check button
                retStatusButton();
                buttonOk = false;
            }
        break;
        default:
          step = ONE;
        break;
    }
    return result;
}
/** call green signal
* 1-set value and learning test
* 2-play musik number 0001 and 0002
* 3-enabled buser
* 4-clicking button
*/
static bool enabledGreenSignal(void)
{
    const uint8 numderTimer = 2;
    static uint32 numberMusic = MUSIC_GREEN_MODE;
    static uint32 oldNumberMusic;
    TypeAnswer answer;
    bool result = false;



    //read collor exit
    if((pStatus->RedState)||
       ((pStatus->GCount < TIME_GREEN_END)&&(pStatus->GCount != ERROR))){
        step = ONE;
        clearParametrVibro();
        enableShdMode(true);
        return true;
    }
    // green color working
    switch(step)
    {
        case ONE:
          if(ms10TimeDelay(10,numderTimer)){
            valueChanelOne(MAX_VALUE_CHANEL_ONE);
            valueChanelTwo(MAX_VALUE_CHANEL_TWO);
            step = TWO;
          }
        break;
        // START POWER MP3
        case TWO:
            if((ms10TimeDelay(30,numderTimer))||(checkCommand(START_POWER))){
                setParametrVibro(true,1,1,80,30,0);
                numberMusic = MUSIC_GREEN_MODE;
                step = THREE;
            }
        break;
        // MP3 is WORKING
        case THREE:
            if(ms10TimeDelay(50,numderTimer)){
                enableShdMode(false);
                const uint16 value = retValueMP3(pStatus->CurVolume);
                mp3_set_volume(value);
                step = FOUR;
            }
        break;
        case FOUR:
            if(checkCommand(RETURN_ERR)){
                step = THREE;
            }else{
                if(ms10TimeDelay(30,numderTimer)){
                    mp3_play_number(numberMusic);
                    step = FIVE;
                }
            }
        break;
        case FIVE: // delay answer
            // error
            if(checkCommand(RETURN_ERR)){
                step = ONE;
            }
            // music end
            //if(checkCommand(STAY_TWO))
            answer = waitingResponseMP3(STAY_TWO,100);
            if((answer==OK)||(answer==NO_ANSWER)){
                if(pStatus->GCount == ERROR){
                    mp3_play_number(MUSIC_TOP_TOP); // mp3_play(); LERNING NO, play tik-tak
                }else{
                  if(pStatus->GCount > TIME_GREEN_END){
                    numberMusic = getNumberMusic(pStatus->GCount);
                    if(numberMusic == oldNumberMusic){
                        mp3_play_number(MUSIC_TOP_TOP); // play TOP-TOP
                    }else{
                        mp3_play_number(numberMusic);
                    }
                    oldNumberMusic = numberMusic;
                  }
                }
            }
        break;
        default:
            step = ONE;
        break;
    }
    return result;
}
/**
* 1-off amplifile and buzer
*/
static bool disabledSignal(void)
{
    step = ONE;
    return true;
}
/**
* global functions coll main module
*/
void machineMP3(void)
{
    const uint8 status = pStatus->MainState;
    static uint8 oldStatus = 0;
    static TypeStep step=ONE;

    takeMessageMp3();

    if(step==ONE)
    {
        disabledMode();
        step = TWO;
    }
    else
    {
        //status change
        if(oldStatus!=status){
            oldStatus = status;
            step =  ONE;
        }else{
            // working mode
            if(status<END_STAT){
                (*operationModeMP3[status])(); // current mode
            }else{
                (*operationModeMP3[0])(); // off mode
            }
            //Vibro for green signal
            machineVibro();
        }
    }
}
/**
*  coll fast loop disabled power
*/
void disabledMode(void)
{
    disabledSignal();
    enableShdMode(true);
    clearParametrVibro();
}