/**
 ******************************************************************************
 * @file    bluetooth.h
 * @brief   Header for bluetooth.c - UART Bluetooth Communication
 ******************************************************************************
 * @attention
 *
 * Robot Soccer - LAUNCHTIME 9th
 * Hardware: HC-05/HC-06 Bluetooth Module via USART1
 *
 ******************************************************************************
 */

#ifndef __BLUETOOTH_H
#define __BLUETOOTH_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "motor.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
#define BLUETOOTH_RX_BUFFER_SIZE    1
#define BLUETOOTH_TIMEOUT           1000  /* ms */

/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Bluetooth_Init(void);
void Bluetooth_StartReceive(void);
void Bluetooth_ProcessCommand(char command);
uint8_t Bluetooth_IsDataAvailable(void);
char Bluetooth_GetCommand(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* UART RX Buffer */
extern uint8_t bluetoothRxData;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __BLUETOOTH_H */