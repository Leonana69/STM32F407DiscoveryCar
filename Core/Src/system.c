/*
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
 * system.c - Top level module implementation
 */
#define DEBUG_MODULE "SYS"

#include "_usart.h"
#include "cmsis_os2.h"
#include "debug.h"
// #include "version.h"
#include "config.h"
#include "system.h"
#include "crtp.h"
#include "static_mem.h"
#include "controller.h"
#include "usblink.h"
#include <string.h>

/* Private variable */
static bool selftestPassed;
static bool isInit = false;

STATIC_MEM_TASK_ALLOC(systemTask, SYSTEM_TASK_STACKSIZE);

/*! System wide synchronisation */
STATIC_MEM_SEMAPHORE_ALLOC(canStartSemaphore);

/* Private functions */
static void systemTask(void *arg);

/* Public functions */
void systemLaunch(void) {
  _UART_Init();
  STATIC_MEM_TASK_CREATE(systemTask, systemTask, SYSTEM_TASK_NAME, NULL, SYSTEM_TASK_PRI);
}

// This must be the first module to be initialized!
static void systemInit(void) {
  if (isInit)
    return;

  STATIC_SEMAPHORE_CREATE(canStartSemaphore, 1, 0);
  
  crtpInit();
  usblinkInit();
  crtpSetLink(usblinkGetLink());
  controllerInit();

  DEBUG_PRINT_UART("----------------------------\n");
  DEBUG_PRINT_UART("System Init.\n");

  isInit = true;
}

static bool systemTest() {
  bool pass = isInit;
  return pass;
}

void systemTask(void *arg) {
  /*! Init all modules */
  systemInit();

  /* Start the firmware */
  if (systemTest()) {
    DEBUG_PRINT_UART("Self test passed!\n");
    selftestPassed = 1;
    systemStart(); 
  }
  DEBUG_PRINT_UART("Free heap: %d bytes\n", xPortGetFreeHeapSize());

  while (1)
    osDelay(osWaitForever);
}


/* Global system variables */
void systemStart() {
  osSemaphoreRelease(canStartSemaphore);
#ifndef DEBUG
  // TODO: init IWDG
  // watchdogInit();
#endif
}

void systemWaitStart(void) {
  // This permits to guarantee that the system task is initialized before other
  // tasks waits for the start event.
  while (!isInit)
    osDelay(10);
  osSemaphoreAcquire(canStartSemaphore, osWaitForever);
  osSemaphoreRelease(canStartSemaphore);
}
