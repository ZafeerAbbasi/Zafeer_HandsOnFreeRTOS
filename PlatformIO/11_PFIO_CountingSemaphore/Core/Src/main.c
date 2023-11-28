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

/* Library includes. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

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

//global space for some variable
char usr_msg[250]={0};
TaskHandle_t xTaskHandle1=NULL;
TaskHandle_t xTaskHandle2=NULL;

/* Declare a variable of type xSemaphoreHandle.  This is used to reference the
semaphore that is used to synchronize a task with an interrupt. */
xSemaphoreHandle xCountingSemaphore;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* The tasks to be created. */
static void vHandlerTask( void *pvParameters );
static void vPeriodicTask( void *pvParameters );

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
  /* USER CODE BEGIN 2 */

  /* Before a semaphore is used it must be explicitly created.  In this example
  a counting semaphore is created.  The semaphore is created to have a maximum
  count value of 10, and an initial count value of 0. */
  xCountingSemaphore = xSemaphoreCreateCounting( 10, 0 );

  /* Check the semaphore was created successfully. */
  if( xCountingSemaphore != NULL )
  {

    /* Create the 'handler' task.  This is the task that will be synchronized
    with the interrupt.  The handler task is created with a high priority to
    ensure it runs immediately after the interrupt exits.  In this case a
    priority of 3 is chosen. */
    xTaskCreate( vHandlerTask, "Handler", 500, NULL, 3, NULL );

    /* Create the task that will periodically generate a software interrupt.
    This is created with a priority below the handler task to ensure it will
    get preempted each time the handler task exist the Blocked state. */
    xTaskCreate( vPeriodicTask, "Periodic", 500, NULL, 2, NULL );

    /* Start the scheduler so the created tasks start executing. */
    vTaskStartScheduler();
  }

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

static void vHandlerTask( void *pvParameters )
{
	/* As per most tasks, this task is implemented within an infinite loop. */
	for( ;; )
	{
		/* Use the semaphore to wait for the event.  The semaphore was created
		before the scheduler was started so before this task ran for the first
		time.  The task blocks indefinitely meaning this function call will only
		return once the semaphore has been successfully obtained - so there is no
		need to check the returned value. */
		xSemaphoreTake( xCountingSemaphore, portMAX_DELAY );

		/* To get here the event must have occurred.  Process the event (in this
		case we just print out a message). */
		sprintf(usr_msg, "Handler task - Processing event.\r\n");
		HAL_UART_Transmit(&huart2,(uint8_t*)usr_msg,strlen(usr_msg),HAL_MAX_DELAY);
	}
}

static void vPeriodicTask( void *pvParameters )
{
  /* As per most tasks, this task is implemented within an infinite loop. */
  for( ;; )
  {
    /* This task is just used to 'simulate' an interrupt.  This is done by
    periodically generating a software interrupt. */
    vTaskDelay( pdMS_TO_TICKS(2000) );

    /* Generate the interrupt, printing a message both before hand and
    afterwards so the sequence of execution is evident from the output. */
    sprintf(usr_msg, "Periodic task - Pending the interrupt.\r\n" );
    HAL_UART_Transmit(&huart2,(uint8_t*)usr_msg,strlen(usr_msg),HAL_MAX_DELAY);

    //pend the interrupt
    HAL_NVIC_SetPendingIRQ( EXTI0_IRQn );
    __HAL_GPIO_EXTI_GENERATE_SWIT( B1_Pin );

    sprintf(usr_msg, "Periodic task - Resuming.\r\n" );
    HAL_UART_Transmit(&huart2,(uint8_t*)usr_msg,strlen(usr_msg),HAL_MAX_DELAY);
  }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

  /* 'Give' the semaphore multiple times.  The first will unblock the handler
  task, the following 'gives' are to demonstrate that the semaphore latches
  the events to allow the handler task to process them in turn without any
  events getting lost.  This simulates multiple interrupts being taken by the
  processor, even though in this case the events are simulated within a single
  interrupt occurrence.*/
  sprintf(usr_msg,"==>Button_Handler\r\n");
  HAL_UART_Transmit(&huart2,(uint8_t*)usr_msg,strlen(usr_msg),HAL_MAX_DELAY);

  xSemaphoreGiveFromISR( xCountingSemaphore, NULL );
  xSemaphoreGiveFromISR( xCountingSemaphore, NULL );
  xSemaphoreGiveFromISR( xCountingSemaphore, NULL );
  xSemaphoreGiveFromISR( xCountingSemaphore, NULL );
  xSemaphoreGiveFromISR( xCountingSemaphore, NULL );

  /* Giving the semaphore may have unblocked a task - if it did and the
  unblocked task has a priority equal to or above the currently executing
  task then xHigherPriorityTaskWoken will have been set to pdTRUE and
  portEND_SWITCHING_ISR() will force a context switch to the newly unblocked
  higher priority task.

  NOTE: The syntax for forcing a context switch within an ISR varies between
  FreeRTOS ports.  The portEND_SWITCHING_ISR() macro is provided as part of
  the Cortex M3 port layer for this purpose.  taskYIELD() must never be called
  from an ISR! */
  portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
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
