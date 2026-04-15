/**
 ******************************************************************************
 * @file    motor.c
 * @brief   BTS7960 Motor Driver Control Implementation
 ******************************************************************************
 * @attention
 *
 * Robot Soccer - LAUNCHTIME 9th
 * Hardware: STM32F103C8T6, 2x BTS7960 modules
 *
 * BTS7960 Logic:
 * | LPWM | RPWM | Action    |
 * |------|------|-----------|
 * | PWM  | 0    | Forward   |
 * | 0    | PWM  | Backward  |
 * | 0    | 0    | Coast     |
 *
 ******************************************************************************
 */

/* USER CODE BEGIN 0 */
#include "motor.h"

/* Private variables ---------------------------------------------------------*/
static uint16_t currentSpeed = 0;
static Motor_Direction_t currentDirection = MOTOR_STOP;

/* Auto-stop timeout variables */
static uint32_t lastCommandTime = 0;
static uint8_t autoStopEnabled = 1;
#define AUTO_STOP_TIMEOUT_MS  1000  /* 1 second timeout */

/* USER CODE END 0 */

/**
 * @brief  Initialize Motor PWM outputs
 * @param  None
 * @retval None
 */
void Motor_Init(void)
{
    /* Start PWM channels on TIM3 */
    /* All channels start at 0 (motors stopped) */
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);  // PA6 - Left RPWM
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);  // PA7 - Left LPWM
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);  // PB0 - Right RPWM
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);  // PB1 - Right LPWM

    /* Set PWM to 0 initially */
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 0);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 0);

    /* Set default speed */
    currentSpeed = (MOTOR_PWM_MAX * MOTOR_DEFAULT_SPEED) / 100;
    currentDirection = MOTOR_STOP;

    /* Initialize last command time to avoid immediate auto-stop */
    lastCommandTime = HAL_GetTick();
}

/**
 * @brief  Set motor speed
 * @param  speedPercent: Speed 0-100%
 * @retval None
 */
void Motor_SetSpeed(uint8_t speedPercent)
{
    if (speedPercent > 100) {
        speedPercent = 100;
    }
    currentSpeed = (MOTOR_PWM_MAX * speedPercent) / 100;

    /* Update speed if moving */
    if (currentDirection != MOTOR_STOP) {
        switch (currentDirection) {
            case MOTOR_FORWARD:
                Motor_Forward();
                break;
            case MOTOR_BACKWARD:
                Motor_Backward();
                break;
            case MOTOR_TURN_LEFT:
                Motor_TurnLeft();
                break;
            case MOTOR_TURN_RIGHT:
                Motor_TurnRight();
                break;
            case MOTOR_FORWARD_LEFT:
                Motor_ForwardLeft();
                break;
            case MOTOR_FORWARD_RIGHT:
                Motor_ForwardRight();
                break;
            case MOTOR_BACK_LEFT:
                Motor_BackLeft();
                break;
            case MOTOR_BACK_RIGHT:
                Motor_BackRight();
                break;
            default:
                break;
        }
    }
}

/**
 * @brief  Stop all motors (brake)
 * @param  None
 * @retval None
 */
void Motor_Stop(void)
{
    /* Set both PWM to 0 - BTS7960 will brake */
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);  // Left RPWM
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);  // Left LPWM
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 0);  // Right RPWM
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 0);  // Right LPWM

    currentDirection = MOTOR_STOP;
}

/**
 * @brief  Move forward - all wheels forward
 * @param  None
 * @retval None
 *
 * Left Module:  LPWM=PWM, RPWM=0 -> Forward
 * Right Module: LPWM=PWM, RPWM=0 -> Forward
 */
void Motor_Forward(void)
{
    /* Left wheels forward */
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, currentSpeed);  // LPWM = Speed
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);               // RPWM = 0

    /* Right wheels forward */
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, currentSpeed);  // LPWM = Speed
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 0);               // RPWM = 0

    currentDirection = MOTOR_FORWARD;
}

/**
 * @brief  Move backward - all wheels backward
 * @param  None
 * @retval None
 *
 * Left Module:  LPWM=0, RPWM=PWM -> Backward
 * Right Module: LPWM=0, RPWM=PWM -> Backward
 */
void Motor_Backward(void)
{
    /* Left wheels backward */
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);               // LPWM = 0
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, currentSpeed);  // RPWM = Speed

    /* Right wheels backward */
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 0);               // LPWM = 0
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, currentSpeed);  // RPWM = Speed

    currentDirection = MOTOR_BACKWARD;
}

/**
 * @brief  Turn left in place
 * @param  None
 * @retval None
 *
 * Left wheels:  backward
 * Right wheels: forward
 */
void Motor_TurnLeft(void)
{
    /* Left wheels backward */
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);            // LPWM = 0
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, currentSpeed); // RPWM = Speed

    /* Right wheels forward */
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, currentSpeed); // LPWM = Speed
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 0);            // RPWM = 0

    currentDirection = MOTOR_TURN_LEFT;
}

/**
 * @brief  Turn right in place
 * @param  None
 * @retval None
 *
 * Left wheels:  forward
 * Right wheels: backward
 */
void Motor_TurnRight(void)
{
    /* Left wheels forward */
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, currentSpeed); // LPWM = Speed
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);             // RPWM = 0

    /* Right wheels backward */
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 0);             // LPWM = 0
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, currentSpeed); // RPWM = Speed

    currentDirection = MOTOR_TURN_RIGHT;
}

/**
 * @brief  Move forward and turn left
 * @param  None
 * @retval None
 *
 * Left wheels:  stop
 * Right wheels: forward
 */
void Motor_ForwardLeft(void)
{
    /* Left wheels stop */
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);  // LPWM = 0
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);  // RPWM = 0

    /* Right wheels forward */
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, currentSpeed);  // LPWM = Speed
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 0);               // RPWM = 0

    currentDirection = MOTOR_FORWARD_LEFT;
}

/**
 * @brief  Move forward and turn right
 * @param  None
 * @retval None
 *
 * Left wheels:  forward
 * Right wheels: stop
 */
void Motor_ForwardRight(void)
{
    /* Left wheels forward */
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, currentSpeed);  // LPWM = Speed
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);               // RPWM = 0

    /* Right wheels stop */
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 0);  // LPWM = 0
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 0);  // RPWM = 0

    currentDirection = MOTOR_FORWARD_RIGHT;
}

/**
 * @brief  Move backward and turn left
 * @param  None
 * @retval None
 *
 * Left wheels:  stop
 * Right wheels: backward
 */
void Motor_BackLeft(void)
{
    /* Left wheels stop */
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);  // LPWM = 0
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);  // RPWM = 0

    /* Right wheels backward */
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 0);               // LPWM = 0
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, currentSpeed);  // RPWM = Speed

    currentDirection = MOTOR_BACK_LEFT;
}

/**
 * @brief  Move backward and turn right
 * @param  None
 * @retval None
 *
 * Left wheels:  backward
 * Right wheels: stop
 */
void Motor_BackRight(void)
{
    /* Left wheels backward */
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);               // LPWM = 0
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, currentSpeed);  // RPWM = Speed

    /* Right wheels stop */
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 0);  // LPWM = 0
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 0);  // RPWM = 0

    currentDirection = MOTOR_BACK_RIGHT;
}

/**
 * @brief  Process command from Bluetooth
 * @param  command: Command character
 * @retval None
 *
 * Command mapping (compatible with sample code):
 * | Cmd | Action         |
 * |-----|----------------|
 * | F   | Forward        |
 * | B   | Backward       |
 * | L   | Turn Left      |
 * | R   | Turn Right     |
 * | G   | Forward Left   |
 * | I   | Forward Right  |
 * | H   | Back Left      |
 * | J   | Back Right     |
 * | S   | Stop           |
 * | 0-9, q | Speed      |
 */
void Motor_ProcessCommand(char command)
{
    /* Update last command time for auto-stop */
    lastCommandTime = HAL_GetTick();

    switch (command) {
        case 'F':
            Motor_Forward();
            break;
        case 'B':
            Motor_Backward();
            break;
        case 'L':
            Motor_TurnLeft();
            break;
        case 'R':
            Motor_TurnRight();
            break;
        case 'G':
            Motor_ForwardLeft();
            break;
        case 'I':
            Motor_ForwardRight();
            break;
        case 'H':
            Motor_BackLeft();
            break;
        case 'J':
            Motor_BackRight();
            break;
        case 'S':
            Motor_Stop();
            break;
        /* Speed commands: 0-9, q (5V to 10V range from 12.6V) */
        /* 5V = 40%, 10V = 80% of 12.6V */
        case '0':
            Motor_SetSpeed(40);  /* ~5V */
            break;
        case '1':
            Motor_SetSpeed(44);  /* ~5.5V */
            break;
        case '2':
            Motor_SetSpeed(48);  /* ~6V */
            break;
        case '3':
            Motor_SetSpeed(52);  /* ~6.5V */
            break;
        case '4':
            Motor_SetSpeed(56);  /* ~7V */
            break;
        case '5':
            Motor_SetSpeed(60);  /* ~7.5V */
            break;
        case '6':
            Motor_SetSpeed(64);  /* ~8V */
            break;
        case '7':
            Motor_SetSpeed(68);  /* ~8.5V */
            break;
        case '8':
            Motor_SetSpeed(72);  /* ~9V */
            break;
        case '9':
            Motor_SetSpeed(76);  /* ~9.5V */
            break;
        case 'q':
            Motor_SetSpeed(80);  /* ~10V (max) */
            break;
        default:
            /* Unknown command - ignore */
            break;
    }
}

/**
 * @brief  Check for auto-stop timeout and stop if needed
 * @param  None
 * @retval None
 *
 * Call this periodically from a task to implement auto-stop
 * if no command received within timeout period
 */
void Motor_CheckTimeout(void)
{
    if (!autoStopEnabled || currentDirection == MOTOR_STOP) {
        return;
    }

    uint32_t currentTime = HAL_GetTick();

    /* Handle wrap-around of HAL_GetTick() */
    if (currentTime >= lastCommandTime) {
        if ((currentTime - lastCommandTime) >= AUTO_STOP_TIMEOUT_MS) {
            Motor_Stop();
        }
    }
}

/**
 * @brief  Enable or disable auto-stop feature
 * @param  enable: 1 to enable, 0 to disable
 * @retval None
 */
void Motor_SetAutoStop(uint8_t enable)
{
    autoStopEnabled = enable;
}

/**
 * @brief  Get time since last command
 * @param  None
 * @retval milliseconds since last command
 */
uint32_t Motor_GetLastCommandAge(void)
{
    if (lastCommandTime == 0) {
        return 0;
    }
    uint32_t currentTime = HAL_GetTick();
    if (currentTime >= lastCommandTime) {
        return currentTime - lastCommandTime;
    }
    return 0;
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */