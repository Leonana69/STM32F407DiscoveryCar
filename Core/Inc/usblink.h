#ifndef __USBLINK_H__
#define __USBLINK_H__

#include <stdbool.h>
#include "crtp.h"

//#define SYSLINK_MTU 32

#define CRTP_START_BYTE  0xAA
#define SYSLINK_START_BYTE1 0xBC
#define SYSLINK_START_BYTE2 0xCF

// Defined packet types
//#define SYSLINK_RADIO_RAW      0x00
//#define SYSLINK_RADIO_CHANNEL  0x01
//#define SYSLINK_RADIO_DATARATE 0x02


//#define SYSLINK_PM_SOURCE 0x10

//#define SYSLINK_PM_ONOFF_SWITCHOFF 0x11

//#define SYSLINK_PM_BATTERY_VOLTAGE 0x12
//#define SYSLINK_PM_BATTERY_STATE   0x13
//#define SYSLINK_PM_BATTERY_AUTOUPDATE 0x14

//#define SYSLINK_OW_SCAN 0x20
//#define SYSLINK_OW_READ 0x21
/*
typedef struct _SyslinkPacket
{
  uint8_t type;
  uint8_t length;
  char data[SYSLINK_MTU];
} __attribute__((packed)) SyslinkPacket;

typedef enum
{
  waitForFirstStart,
  waitForSecondStart,
  waitForType,
  waitForLengt,
  waitForData,
  waitForChksum1,
  waitForChksum2
} SyslinkRxState;
*/

void usblinkInit();
bool usblinkTest();
void usblinkMessagePut(CRTPPacket *p);
struct crtpLinkOperations * usblinkGetLink();

#endif //__USBLINK_H__
