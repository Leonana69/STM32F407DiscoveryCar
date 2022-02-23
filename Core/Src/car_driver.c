#include "car_driver.h"
#include "config.h"
#include "tim.h"

typedef struct {
	// 0: forward control, 1: backward control
	TIM_HandleTypeDef* tim[2];
	uint32_t channel[2];
} MotorTim;

static int thrustBase = 18000;

static MotorTim motorTim[4] = {
	{
		.tim = { &MOTOR1_F_TIM, &MOTOR1_B_TIM },
		.channel = { MOTOR1_F_CHANNEL, MOTOR1_B_CHANNEL },
	},
	{
		.tim = { &MOTOR2_F_TIM, &MOTOR2_B_TIM },
		.channel = { MOTOR2_F_CHANNEL, MOTOR2_B_CHANNEL },
	},
	{
		.tim = { &MOTOR3_F_TIM, &MOTOR3_B_TIM },
		.channel = { MOTOR3_F_CHANNEL, MOTOR3_B_CHANNEL },
	},
	{
		.tim = { &MOTOR4_F_TIM, &MOTOR4_B_TIM },
		.channel = { MOTOR4_F_CHANNEL, MOTOR4_B_CHANNEL },
	},
};

void motorInit() {
	for (int i = 0; i < MOTOR_NBR; i++) {
		HAL_TIM_PWM_Start(motorTim[i].tim[0], motorTim[i].channel[0]);
		HAL_TIM_PWM_Start(motorTim[i].tim[1], motorTim[i].channel[1]);
	}
}

void motorSetRatio(uint8_t id, int16_t thrust) {
	bool dir = thrust < 0;
	if (dir) thrust = -thrust;

	float ratio = thrust * (float)MOTOR_TIM_PERIOD / MOTOR_MAX_THRUST;
	__HAL_TIM_SET_COMPARE(motorTim[id].tim[!dir], motorTim[id].channel[!dir], 0);
	__HAL_TIM_SET_COMPARE(motorTim[id].tim[dir], motorTim[id].channel[dir], ratio);
}

void carMove(int16_t v, int dir) {
	int16_t motorValue[MOTOR_NBR] = { v, v, v, v };
	switch (dir) {
		case LEFT:
			motorValue[1] = -motorValue[1];
			motorValue[3] = -motorValue[3];
			break;
		case RIGHT:
			motorValue[0] = -motorValue[0];
			motorValue[2] = -motorValue[2];
			break;
		case FRONT:
			break;
		case BACK:
			for (int i = 0; i < MOTOR_NBR; i++)
				motorValue[i] = -motorValue[i];
	}
	for (int i = 0; i < MOTOR_NBR; i++)
		motorSetRatio(i, motorValue[i]);
}

void carRotate(int16_t r) {
	bool dir = r < 0;
	if (dir) r = -r;
	int16_t motorValue[MOTOR_NBR] = { -r, -r, r, r };
	if (dir) {
		for (int i = 0; i < MOTOR_NBR; i++)
			motorValue[i] = -motorValue[i];
	}
	for (int i = 0; i < MOTOR_NBR; i++)
		motorSetRatio(i, motorValue[i]);
}

void carStart(){
	motorSetRatio(0, 20000);
	motorSetRatio(1, -20000);
	motorSetRatio(2, -20000);
	motorSetRatio(3, 20000);
	HAL_Delay(100);
	carStop();
}
	
void carStop() {
	for (int i = 0; i < MOTOR_NBR; i++)
		motorSetRatio(i, 0);
}



void carSet(setpoint_t *sp) {
	int motorValue[MOTOR_NBR] = { 0 };
	motorValue[0] = sp->thrust * (sp->pitch + sp->roll - sp->yaw);
	motorValue[1] = sp->thrust * (sp->pitch - sp->roll - sp->yaw);
	motorValue[2] = sp->thrust * (sp->pitch + sp->roll + sp->yaw);
	motorValue[3] = sp->thrust * (sp->pitch - sp->roll + sp->yaw);
	for (int i = 0; i < MOTOR_NBR; i++) {
		if (motorValue[i] > 0) {
			motorValue[i] += thrustBase;
			if (motorValue[i] > MOTOR_MAX_THRUST) motorValue[i] = MOTOR_MAX_THRUST;
		}
		else {
			motorValue[i] -= thrustBase;
			if (motorValue[i] < -MOTOR_MAX_THRUST) motorValue[i] = -MOTOR_MAX_THRUST;
		}
		motorSetRatio(i, motorValue[i]);
	}
}