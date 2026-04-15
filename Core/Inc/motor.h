/**
 ******************************************************************************
 * @file    motor.h
 * @brief   Header for motor.c - BTS7960 Motor Driver Control
 ******************************************************************************
 * @attention
 *
 * Robot Soccer - LAUNCHTIME 9th
 * Hardware: STM32F103C8T6, 2x BTS7960 modules
 *
 ******************************************************************************
 */

#ifndef __MOTOR_H
#define __MOTOR_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "stdint.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
typedef enum {
    MOTOR_STOP = 0,
    MOTOR_FORWARD,
    MOTOR_BACKWARD,
    MOTOR_TURN_LEFT,
    MOTOR_TURN_RIGHT,
    MOTOR_FORWARD_LEFT,
    MOTOR_FORWARD_RIGHT,
    MOTOR_BACK_LEFT,
    MOTOR_BACK_RIGHT
} Motor_Direction_t;

/* Exported constants --------------------------------------------------------*/
/* PWM Period - same as TIM3 Period (3599) */
#define MOTOR_PWM_MAX         3599

/* Default speed (0-100%) - 40% = ~5V from 12.6V */
#define MOTOR_DEFAULT_SPEED   40

/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Motor_Init(void);
void Motor_SetSpeed(uint8_t speedPercent);
void Motor_Stop(void);
void Motor_Forward(void);
void Motor_Backward(void);
void Motor_TurnLeft(void);
void Motor_TurnRight(void);
void Motor_ForwardLeft(void);
void Motor_ForwardRight(void);
void Motor_BackLeft(void);
void Motor_BackRight(void);
void Motor_ProcessCommand(char command);
void Motor_CheckTimeout(void);
void Motor_SetAutoStop(uint8_t enable);
uint32_t Motor_GetLastCommandAge(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* BTS7960 Left Module (Controls 2 left wheels)
 * PA6 -> TIM3_CH1 -> RPWM
 * PA7 -> TIM3_CH2 -> LPWM
 */
#define MOTOR_L_LPWM_PORT     GPIOA
#define MOTOR_L_LPWM_PIN      GPIO_PIN_7
#define MOTOR_L_RPWM_PORT     GPIOA
#define MOTOR_L_RPWM_PIN      GPIO_PIN_6

/* BTS7960 Right Module (Controls 2 right wheels)
 * PB0 -> TIM3_CH3 -> RPWM
 * PB1 -> TIM3_CH4 -> LPWM
 */
#define MOTOR_R_LPWM_PORT     GPIOB
#define MOTOR_R_LPWM_PIN      GPIO_PIN_1
#define MOTOR_R_RPWM_PORT     GPIOB
#define MOTOR_R_RPWM_PIN      GPIO_PIN_0

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MOTOR_H */