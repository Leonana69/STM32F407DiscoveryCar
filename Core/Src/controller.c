#include "controller.h"

#include "static_mem.h"
#include "crtp.h"
#include "car_driver.h"
#include "debug.h"
#include "config.h"

STATIC_MEM_TASK_ALLOC(controllerTask, CONTROLLER_TASK_STACKSIZE);
static osMessageQueueId_t rxQueue;
static bool isInit = false;
static void controllerTask();
static void controllerDispatchPacket(CRTPPacket *p);

CRTPPacket cp;

void controllerInit() {
	if (isInit)
		return;

	rxQueue = osMessageQueueNew(10, sizeof(CRTPPacket), NULL);
	crtpRegisterPortCB(CRTP_PORT_SETPOINT, controllerDispatchPacket);

	STATIC_MEM_TASK_CREATE(controllerTask, controllerTask, CONTROLLER_TASK_NAME, NULL, CONTROLLER_TASK_PRI);
	isInit = true;
}

void controllerDispatchPacket(CRTPPacket *p) {
	osMessageQueuePut(rxQueue, p, 0, osWaitForever);
}

void controllerTask() {
	setpoint_t* sp;
	while (1) {
		if (osMessageQueueGet(rxQueue, &cp, NULL, 500) == osOK) {
			sp = (setpoint_t *) cp.data;
			DEBUG_PRINT_UART("Set: %f %f %f %d\n", sp->roll, sp->pitch, sp->yaw, sp->thrust);
			carSet(sp);
		} else {
			carStop();
		}
		osDelay(1);
	}
}