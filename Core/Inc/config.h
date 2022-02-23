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

#define debugUart       huart2


#define SYSTEM_TASK_STACKSIZE   (2 * configMINIMAL_STACK_SIZE)
#define SYSTEM_TASK_PRI         2
#define SYSTEM_TASK_NAME        "SYSTEM"

#define CRTP_TX_TASK_NAME       "CRTP-TX"
#define CRTP_RX_TASK_NAME       "CRTP-RX"
#define CRTP_RXTX_TASK_NAME     "CRTP-RXTX"
#define CRTP_TX_TASK_PRI        2
#define CRTP_RX_TASK_PRI        2
#define CRTP_TX_TASK_STACKSIZE        configMINIMAL_STACK_SIZE
#define CRTP_RX_TASK_STACKSIZE        (2 * configMINIMAL_STACK_SIZE)
#define CRTP_RXTX_TASK_STACKSIZE      configMINIMAL_STACK_SIZE

#define USBLINK_TASK_NAME       "USBLINK"
#define USBLINK_TASK_PRI        3
#define USBLINK_TASK_STACKSIZE  configMINIMAL_STACK_SIZE

#define CONTROLLER_TASK_NAME	"CONTROLLER"
#define CONTROLLER_TASK_PRI		3
#define CONTROLLER_TASK_STACKSIZE configMINIMAL_STACK_SIZE

#ifdef __cplusplus
}
#endif
#endif //__CONFIG__