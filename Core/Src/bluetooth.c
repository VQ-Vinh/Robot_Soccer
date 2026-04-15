/**
 ******************************************************************************
 * @file    bluetooth.c
 * @brief   UART Bluetooth Communication Implementation
 ******************************************************************************
 * @attention
 *
 * Robot Soccer - LAUNCHTIME 9th
 * Hardware: HC-05/HC-06 Bluetooth Module via USART1
 *
 * Communication: 9600 baud (default for HC-05/HC-06)
 *
 ******************************************************************************
 */

/* USER CODE BEGIN 0 */
#include "bluetooth.h"
#include "stdio.h"

/* Private variables ---------------------------------------------------------*/
uint8_t bluetoothRxData = 0;
static uint8_t commandBuffer = 0;
static uint8_t dataReady = 0;

/* USER CODE END 0 */

/**
 * @brief  Initialize Bluetooth communication
 * @param  None
 * @retval None
 */
void Bluetooth_Init(void)
{
    /* Start receive interrupt */
    Bluetooth_StartReceive();
}

/**
 * @brief  Start receiving data from Bluetooth
 * @param  None
 * @retval None
 */
void Bluetooth_StartReceive(void)
{
    /* Start UART receive in interrupt mode */
    HAL_UART_Receive_IT(&huart1, &bluetoothRxData, BLUETOOTH_RX_BUFFER_SIZE);
}

/**
 * @brief  Process command received from Bluetooth
 * @param  command: Command character to process
 * @retval None
 */
void Bluetooth_ProcessCommand(char command)
{
    /* Forward command to motor control */
    Motor_ProcessCommand(command);
}

/**
 * @brief  Check if data is available
 * @param  None
 * @retval 1 if data ready, 0 otherwise
 */
uint8_t Bluetooth_IsDataAvailable(void)
{
    return dataReady;
}

/**
 * @brief  Get command from buffer
 * @param  None
 * @retval Command character
 */
char Bluetooth_GetCommand(void)
{
    dataReady = 0;
    return (char)commandBuffer;
}

/**
 * @brief  UART Rx Complete Callback
 * @param  huart: UART handle
 * @retval None
 *
 * This function is called by HAL when RX completes
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        /* Store command and set ready flag */
        commandBuffer = bluetoothRxData;
        dataReady = 1;

        /* Process command immediately */
        Bluetooth_ProcessCommand((char)commandBuffer);

        /* Restart receive */
        Bluetooth_StartReceive();
    }
}

/**
 * @brief  UART Error Callback
 * @param  huart: UART handle
 * @retval None
 */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        /* Restart receive on error */
        Bluetooth_StartReceive();
    }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */