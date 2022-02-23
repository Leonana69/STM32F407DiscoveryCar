/**
 *    ||          ____  _ __
 * +------+      / __ )(_) /_______________ _____  ___
 * | 0xBC |     / __  / / __/ ___/ ___/ __ `/_  / / _ \
 * +------+    / /_/ / / /_/ /__/ /  / /_/ / / /_/  __/
 *  ||  ||    /_____/_/\__/\___/_/   \__,_/ /___/\___/
 *
 * Crazyflie control firmware
 *
 * Copyright (C) 2011-2012 Bitcraze AB
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, in version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * crtp.c - CrazyRealtimeTransferProtocol stack
 */

#include <stdbool.h>
#include <errno.h>

#include "config.h"
#include "crtp.h"
#include "static_mem.h"
#include "debug.h"
#include "cfassert.h"

static bool isInit;

static int nopFunc(void);
static struct crtpLinkOperations nopLink = {
  .setEnable         = (void*) nopFunc,
  .sendPacket        = (void*) nopFunc,
  .receivePacket     = (void*) nopFunc,
};

static struct crtpLinkOperations *link = &nopLink;

#define CRTP_NBR_OF_PORTS 16
#define CRTP_TX_QUEUE_SIZE 120
#define CRTP_RX_QUEUE_SIZE 16

static osMessageQueueId_t txQueue;
static osMessageQueueId_t queues[CRTP_NBR_OF_PORTS];

static void crtpTxTask(void *param);
static void crtpRxTask(void *param);

static volatile CrtpCallback callbacks[CRTP_NBR_OF_PORTS];

STATIC_MEM_TASK_ALLOC_STACK_NO_DMA_CCM_SAFE(crtpTxTask, CRTP_TX_TASK_STACKSIZE);
STATIC_MEM_TASK_ALLOC_STACK_NO_DMA_CCM_SAFE(crtpRxTask, CRTP_RX_TASK_STACKSIZE);

void crtpInit(void) {
  if (isInit)
    return;

  txQueue = osMessageQueueNew(CRTP_TX_QUEUE_SIZE, sizeof(CRTPPacket), NULL);

  STATIC_MEM_TASK_CREATE(crtpTxTask, crtpTxTask, CRTP_TX_TASK_NAME, NULL, CRTP_TX_TASK_PRI);
  STATIC_MEM_TASK_CREATE(crtpRxTask, crtpRxTask, CRTP_RX_TASK_NAME, NULL, CRTP_RX_TASK_PRI);

  isInit = true;
}

bool crtpTest(void) {
  return isInit;
}

void crtpInitTaskQueue(CRTPPort portId) {
  ASSERT(queues[portId] == NULL);
  queues[portId] = osMessageQueueNew(CRTP_RX_QUEUE_SIZE, sizeof(CRTPPacket), NULL);
}

int crtpReceivePacket(CRTPPort portId, CRTPPacket *p) {
  ASSERT(queues[portId]);
  ASSERT(p);
  return osMessageQueueGet(queues[portId], p, NULL, 0);
}

int crtpReceivePacketBlock(CRTPPort portId, CRTPPacket *p) {
  ASSERT(queues[portId]);
  ASSERT(p);
  return osMessageQueueGet(queues[portId], p, NULL, osWaitForever);
}

int crtpReceivePacketWait(CRTPPort portId, CRTPPacket *p, int wait) {
  ASSERT(queues[portId]);
  ASSERT(p);
  return osMessageQueueGet(queues[portId], p, NULL, wait);
}

int crtpGetFreeTxQueuePackets(void) {
  return osMessageQueueGetSpace(txQueue);
}

void crtpTxTask(void *param) {
  CRTPPacket p;

  while (1) {
    if (link != &nopLink) {
      if (osMessageQueueGet(txQueue, &p, 0, osWaitForever) == osOK) {
        /*! Keep testing, if the link changes to USB it will go though */
        while (link->sendPacket(&p) == false)
          osDelay(10);
      }
    } else
      osDelay(10);
  }
}

void crtpRxTask(void *param) {
  CRTPPacket p;

  while (1) {
    if (link != &nopLink) {
      if (!link->receivePacket(&p)) {
        if (queues[p.port])
          /*! Block, since we should never drop a packet */
          osMessageQueuePut(queues[p.port], &p, 0, osWaitForever);

        if (callbacks[p.port])
          callbacks[p.port](&p);
      }
    } else
			osDelay(10);
  }
}

void crtpRegisterPortCB(int port, CrtpCallback cb) {
  if (port > CRTP_NBR_OF_PORTS)
    return;

  callbacks[port] = cb;
}

int crtpSendPacket(CRTPPacket *p) {
  ASSERT(p);
  ASSERT(p->size <= CRTP_MAX_DATA_SIZE);

  return osMessageQueuePut(txQueue, p, 0, 0);
}

int crtpSendPacketBlock(CRTPPacket *p) {
  ASSERT(p);
  ASSERT(p->size <= CRTP_MAX_DATA_SIZE);

  return osMessageQueuePut(txQueue, p, 0, osWaitForever);
}

int crtpReset(void) {
  osMessageQueueReset(txQueue);
  if (link->reset) {
    link->reset();
  }

  return 0;
}

bool crtpIsConnected(void) {
  if (link->isConnected)
    return link->isConnected();
  return true;
}

void crtpSetLink(struct crtpLinkOperations * lk) {
  if (link)
    link->setEnable(false);

  if (lk)
    link = lk;
  else
    link = &nopLink;

  link->setEnable(true);
}

static int nopFunc(void) {
  return ENETDOWN;
}
