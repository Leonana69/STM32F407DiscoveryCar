#ifndef __CONFIG__
#define __CONFIG__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	LEFT, RIGHT, FRONT, BACK,
} DIRECTION;

#define MOTOR_NBR 4
#define MOTOR_TIM_PERIOD 1000
#define MOTOR_MAX_THRUST (1 << 15) // int16

#define MOTOR1_F_TIM	 htim1
#define MOTOR1_B_TIM	 htim1
#define MOTOR1_F_CHANNEL TIM_CHANNEL_1
#define MOTOR1_B_CHANNEL TIM_CHANNEL_2

#define MOTOR2_F_TIM	 htim1
#define MOTOR2_B_TIM	 htim1
#define MOTOR2_F_CHANNEL TIM_CHANNEL_3
#define MOTOR2_B_CHANNEL TIM_CHANNEL_4

#define MOTOR3_F_TIM	 htim2
#define MOTOR3_B_TIM	 htim2
#define MOTOR3_F_CHANNEL TIM_CHANNEL_1
#define MOTOR3_B_CHANNEL TIM_CHANNEL_2

#define MOTOR4_F_TIM	 htim2
#define MOTOR4_B_TIM	 htim2
#define MOTOR4_F_CHANNEL TIM_CHANNEL_3
#define MOTOR4_B_CHANNEL TIM_CHANNEL_4

#ifdef __cplusplus
}
#endif
#endif //__CONFIG__