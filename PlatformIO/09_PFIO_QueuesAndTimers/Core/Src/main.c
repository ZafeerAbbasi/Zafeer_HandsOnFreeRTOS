/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "rtc.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

xTaskHandle tskHdl_zInputHandleTask;
xTaskHandle tskHdl_zMenuTask;
xTaskHandle tskHdl_zPrintGUITask;
xTaskHandle tskHdl_zLEDTask;
xTaskHandle tskHdl_zRTCTask;

QueueHandle_t queueHdl_zInputData;
QueueHandle_t queueHdl_zOptionsPrint;

TimerHandle_t timerHdl_zLEDTimer[ 4 ];
TimerHandle_t timerHdl_zRTCTimer;

volatile uint8_t userData;

input_zInputCommand_t inputCmd;

state_zCurrAppState currAppState;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

void timer_LEDTimerCallBack( TimerHandle_t xTimer );

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  BaseType_t status;

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */

  status = xTaskCreate( USRTSK_MenuTask, "Menu_Task", 250, NULL, 2, &tskHdl_zMenuTask );
  configASSERT( status == pdPASS );
  status = xTaskCreate( USRTSK_InputHandleTask, "Cmd_Task", 250, NULL, 2, &tskHdl_zInputHandleTask );
  configASSERT( status == pdPASS );
  status = xTaskCreate( USRTSK_PrintGUITask, "Print_Task", 250, NULL, 2, &tskHdl_zPrintGUITask );
  configASSERT( status == pdPASS );
  status = xTaskCreate( USRTSK_LEDTask, "LED_Task", 250, NULL, 2, &tskHdl_zLEDTask );
  configASSERT( status == pdPASS );
  // status = xTaskCreate( USRTSK_RTCTask, "RTC_Task", 250, NULL, 2, &tskHdl_zRTCTask );
  // configASSERT( status == pdPASS );

  /*Queue to hold user choice*/
  queueHdl_zInputData = xQueueCreate( 10, sizeof( char ) );
  configASSERT(  queueHdl_zInputData != NULL );

  /*Queue to hold pointers to each menu GUI char pointer, GUI is just a formatted string*/
  queueHdl_zOptionsPrint = xQueueCreate( 10, sizeof( char * ) );
  configASSERT( queueHdl_zOptionsPrint != NULL );

  /*Create Timer Software Timers*/
  for( int i = 0; i < 4; i++ )
  {
    timerHdl_zLEDTimer[ i ] = xTimerCreate( "LED_Timer", pdMS_TO_TICKS( 500 ), pdTRUE, ( void * )i + 1, timer_LEDTimerCallBack );
    configASSERT( timerHdl_zLEDTimer[ i ] != NULL );
  }

  /*Enable UART in Receive Interrupt Mode*/
  HAL_UART_Receive_IT( &huart2, ( uint8_t *)&userData, 1 );

  vTaskStartScheduler( );

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
    // HAL_UART_Transmit( &huart2, "Hello", strlen( "Hello" ), HAL_MAX_DELAY );
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* CallBack Function Implementations --------------------------------------------------------------------------------------------*/

void timer_LEDTimerCallBack( TimerHandle_t xTimer )
{
    configASSERT( xTimer != NULL );
    uint32_t timerID = ( uint32_t )pvTimerGetTimerID( xTimer );

    switch( timerID )
    {
        case 1:
        {
            LED_LEDEffect1( );
            break;
        }
        case 2:
        {
            LED_LEDEffect2( );
            break;
        }
        case 3:
        {
            LED_LEDEffect3( );
            break;
        }
        case 4:
        {
            LED_LEDEffect4( );
            break;
        }
        default:
        {
            break;
        }
    }
}

/**
 * @brief USART2 Reception Complete Callback
 * 
 * @param huart 
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  uint8_t extraData;
  BaseType_t status = pdFALSE;
  status = xQueueIsQueueFullFromISR( queueHdl_zInputData );

  if( status == pdFALSE )
  {
    /* Queue is NOT Full*/
    xQueueSendFromISR( queueHdl_zInputData, ( void * )&userData, NULL );
  }
  else
  {
    /* Queue is FULL */
    /* If The receieved Data is '\n' then add to end otherwise ignore*/
    if( userData == '\n' )
    {
      /* Remove First Element and Add '\n' to Last */
      xQueueReceiveFromISR( queueHdl_zInputData, ( void * )&extraData, NULL );
      xQueueSendFromISR( queueHdl_zInputData, ( void * )&userData, NULL );
    }
  }
  if( userData == '\n' )
  {
    /* Notify Input Handling Task*/
    xTaskNotifyFromISR( tskHdl_zInputHandleTask, 0, eNoAction, NULL ); 
  }

  /* Enable UART in Receive Interrupt Mode*/
  HAL_UART_Receive_IT( &huart2, ( uint8_t *)&userData, 1 );
}

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
