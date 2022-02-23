#include <stdbool.h>
#include <string.h>

#include "config.h"
#include "usblink.h"
#include "crtp.h"
#include "static_mem.h"
#include "cfassert.h"
#include "debug.h"

#include "usbd_cdc_if.h"

static bool isInit = false;
STATIC_MEM_QUEUE_ALLOC(crtpPacketDelivery, 16, sizeof(CRTPPacket));
static uint8_t sendBuffer[64];

static int usblinkSendPacket(CRTPPacket *p);
static int usblinkSetEnable(bool enable);
static int usblinkReceivePacket(CRTPPacket *p);

// STATIC_MEM_TASK_ALLOC(usblinkTask, USBLINK_TASK_STACKSIZE);

static struct crtpLinkOperations usblinkOp = {
  .setEnable         = usblinkSetEnable,
  .sendPacket        = usblinkSendPacket,
  .receivePacket     = usblinkReceivePacket,
};

/* Radio task handles the CRTP packet transfers as well as the radio link
 * specific communications (eg. Scann and ID ports, communication error handling
 * and so much other cool things that I don't have time for it ...)
 */

// static CRTPPacket p;
// static void usblinkTask(void *param) {
//   while (1) {
//     // Fetch a USB packet off the queue
//     usbGetDataBlocking(&usbIn);
//     p.size = usbIn.size - 1;
//     memcpy(&p.raw, usbIn.data, usbIn.size);
//     // This queuing will copy a CRTP packet size from usbIn
//     osMessageQueuePut(crtpPacketDelivery, &p, 0, osWaitForever);
//   }
// }

void usblinkMessagePut(CRTPPacket *p) {
  DEBUG_PRINT_UART("lk: %s\n", (char *)p);
  osMessageQueuePut(crtpPacketDelivery, p, 0, 0);
}

static int usblinkReceivePacket(CRTPPacket *p) {
  //TODO: check the delay here
  if (osMessageQueueGet(crtpPacketDelivery, p, NULL, 100) == osOK)
    return 0;
  return -1;
}

static int usblinkSendPacket(CRTPPacket *p) {
  int dataSize;

  ASSERT(p->size < 32);

  sendBuffer[0] = p->header;

  if (p->size <= CRTP_MAX_DATA_SIZE) {
    memcpy(&sendBuffer[1], p->data, p->size);
  }
  dataSize = p->size + 1;

  return CDC_Transmit_FS(sendBuffer, dataSize);
}

// TODO: implement this
static int usblinkSetEnable(bool enable) {
  return 0;
}

/*
 * Public functions
 */

void usblinkInit() {
  if (isInit)
    return;

  STATIC_MEM_QUEUE_CREATE(crtpPacketDelivery);
  // STATIC_MEM_TASK_CREATE(usblinkTask, usblinkTask, USBLINK_TASK_NAME, NULL, USBLINK_TASK_PRI);

  isInit = true;
}

bool usblinkTest() {
  return isInit;
}

struct crtpLinkOperations * usblinkGetLink() {
  return &usblinkOp;
}
