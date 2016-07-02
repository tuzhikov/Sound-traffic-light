/*----------------------------------------------------------------------------*/
//
// $Title:          vibro.h
// $Description:    Mini mp3 Interface.
//
// $Rev:            0.0.1
//
// $Author:         ATA
// $Created:        23.06.16
// $Last Modified:
//
// $Copyright:
//
// $Notes:
//
/*----------------------------------------------------------------------------*/
#include "EEA_port.h"

void machineVibro(void);
void clearParametrVibro(void);
void setParametrVibro(const bool enabled,
                      const uint8 timeOn,
                      const uint8 timeOff,
                      const uint8 timeDelay,
                      const uint8 timeCount);

