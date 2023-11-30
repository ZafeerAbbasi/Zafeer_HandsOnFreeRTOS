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

/* Uncomment this to test without Mutex */
#define USE_MUTEX

/* Dimensions the buffer into which messages for stdout are placed */
#define MAIN_MAX_MSF_SIZE	( 80 )

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

//global space for some variable
char usr_msg[250]={0};
TaskHandle_t xTaskHandle1 = NULL;
TaskHandle_t xTaskHandle2 = NULL;

#ifdef USE_MUTEX
/* Declare a variable of SemaphoreHandle_t . This is used to reference 
the mutex type semaphore that is used to ensure mutual exclusive access to
UART Peripheral */
SemaphoreHandle_t xMutex;
#endif

char cBuffer[ MAIN_MAX_MSF_SIZE ];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* The task to be created.  Two instances of this task are created. */
static void prvPrintTask( void *pvParameters );

/* The function that uses a mutex to control access to standard out. */
static void prvNewPrintString( const portCHAR *pcString );

static void UARTPrintChar( uint8_t *msg );

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

#ifdef USE_MUTEX
  /* Before a mutex is used, it must be explicitly created. In this example
  a mutex type semaphore is created */
  xMutex = xSemaphoreCreateMutex();
#endif

  /* The tasks are going to use a pseudo random delay, see the random number
  generator */
  srand( 567 );

#ifdef USE_MUTEX
  /* Only create the tasks if the semaphore was created successfully */
  if( xMutex != NULL )
  {
#endif

    /* Create two instances of the tasks that attempt to write stdout.  The
    string they attempt to write is passed in as the task parameter.  The tasks
    are created at different priorities so some pre-emption will occur. */
    xTaskCreate( prvPrintTask, "Print1", 240, "Task 1 ******************************************\r\n", 1, NULL );
    xTaskCreate( prvPrintTask, "Print2", 240, "Task 2 ------------------------------------------\r\n", 2, NULL );

    /* Start the scheduler so the created tasks start executing. */
    vTaskStartScheduler();

#ifdef USE_MUTEX
  }
#endif

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
    // memcpy(cBuffer, "Hello World\r\n", strlen("Hello World\r\n"));
    // UARTPrintChar( ( uint8_t * ) cBuffer );
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

static void prvNewPrintString( const portCHAR *pcString )
{

#ifdef USE_MUTEX
  /* The semaphore is created before the scheduler is started so already exists by the time
  this task executes 
  
  Attempt to take the semaphore, blocking indefinitely if the mutex is not available 
  immediately. The call to xSemaphoreTake( ) will only return when the semaphore has been 
  successfully obtanied so there is no need to check the return value. If any other delay period was
  used then the code must check that xSemaphoreTake( ) returns pdTRUE before accessing the resource
  ( in this case stdout or UART ) */
  xSemaphoreTake( xMutex, portMAX_DELAY );
#endif

  /* The following line will only execute once the semaphore has been sucessfully obtained. 
  So the stdout can be accessed freely */
  sprintf( cBuffer, "%s", pcString );
  UARTPrintChar( ( uint8_t * ) cBuffer );

#ifdef USE_MUTEX
  /* We have finished accessing the shared resource.  Release the semaphore. */
  xSemaphoreGive( xMutex );
#endif
}

static void prvPrintTask( void *pvParameters )
{
  char *pcStringToPrint;

  /* Two instances of this task are created so the string task will send to prvNewPrintString( )
  is passed in the task parameter. Cast this to the required type*/
  pcStringToPrint = ( char * ) pvParameters;

  while( 1 )
  {
    /* Print out the string using the newly defined function */
    prvNewPrintString( pcStringToPrint );

    /* Wait a pseudo random time. Note that rand( ) is not necessarily re-entrant, but in this
    case it does not really matter as the code does not care what value is returned. In a more
    secure application a version of rand( ) that is known to be re-entrant should be used - or calls
    to rand( ) should be protected using a critical section */
    vTaskDelay( rand( ) & 0x1FF );
  }
}

void UARTPrintChar( uint8_t *msg )
{
  for( int i = 0; i < strlen( ( char * )msg ); i++ )
  {
    HAL_UART_Transmit( &huart2, &cBuffer[i], 1, HAL_MAX_DELAY );
  }
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
