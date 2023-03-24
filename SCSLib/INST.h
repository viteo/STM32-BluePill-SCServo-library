/*
 * INST.h
 * FeeTech Serial Servo Protocol Command Definition
 * Date: 2022.1.6
 * Author:
 */

#ifndef _INST_H
#define _INST_H

#include <stdint.h>

#ifndef NULL
#define NULL ((void *)0)
#endif


#define INST_PING 0x01
#define INST_READ 0x02
#define INST_WRITE 0x03
#define INST_REG_WRITE 0x04
#define INST_REG_ACTION 0x05
#define INST_SYNC_READ 0x82
#define INST_SYNC_WRITE 0x83

#endif
