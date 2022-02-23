#ifndef __CAR_DRIVER__
#define __CAR_DRIVER__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

typedef struct setpoint_s {
	float roll;
	float pitch;
	float yaw;
	uint16_t thrust;
}__attribute__((packed)) setpoint_t;

void motorInit();
void motorSetRatio(uint8_t id, int16_t thrust);
void carSet(setpoint_t *sp);
void carMove(int16_t v, int dir);
void carRotate(int16_t r);
void carStart();
void carStop();

#ifdef __cplusplus
}
#endif
#endif //__CAR_DRIVER__