/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "uart_bsp.h"
#include "dm_motor_ctrl.h"
#include "usart.h"
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
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId canTaskHandle;
osThreadId MotorTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void CanTaskEntry(void const * argument);
void MotorTaskEntry(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityLow, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of canTask */
  osThreadDef(canTask, CanTaskEntry, osPriorityLow, 0, 128);
  canTaskHandle = osThreadCreate(osThread(canTask), NULL);

  /* definition and creation of MotorTask */
  osThreadDef(MotorTask, MotorTaskEntry, osPriorityNormal, 0, 128);
  MotorTaskHandle = osThreadCreate(osThread(MotorTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(10);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_CanTaskEntry */
/**
* @brief Function implementing the canTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_CanTaskEntry */
void CanTaskEntry(void const * argument)
{
  /* USER CODE BEGIN CanTaskEntry */
  dm_motor_6215_init();
  /* Infinite loop */
  for(;;)
  {
    dm_motor_ctrl_send(&hfdcan1, &motor[Motor3]);
    dm_motor_ctrl_send(&hfdcan1, &motor[Motor1]);
      //usart_printf("Torque, %.3f\r\n", motor[Motor1].ctrl.tor_set);
    osDelay(1000);

  }
  /* USER CODE END CanTaskEntry */
}

/* USER CODE BEGIN Header_MotorTaskEntry */
/**
* @brief Function implementing the MotorTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_MotorTaskEntry */
void MotorTaskEntry(void const * argument)
{
  /* USER CODE BEGIN MotorTaskEntry */
  //osDelay(4000);

  /* Infinite loop */
  for(;;)
  {
      //usart_printf("Hello\r\n");
              // 检查数据更新标志位
        if (channels->data_updated) {
            usart_printf("ch[0]:%d\r\n", channels->rc.ch[0]);
            usart_printf("ch[1]:%d\r\n", channels->rc.ch[1]);
            usart_printf("ch[2]:%d\r\n", channels->rc.ch[2]);
            usart_printf("ch[3]:%d\r\n", channels->rc.ch[3]);
            // usart_printf("ch[4]:%d\r\n", channels->rc.ch[4]);
            // usart_printf("ch[5]:%d\r\n", channels->rc.ch[5]);
            // usart_printf("ch[6]:%d\r\n", channels->rc.ch[6]);
            // usart_printf("ch[7]:%d\r\n", channels->rc.ch[7]);
            // usart_printf("ch[8]:%d\r\n", channels->rc.ch[8]);
            // usart_printf("ch[9]:%d\r\n", channels->rc.ch[9]);
            // usart_printf("ch[10]:%d\r\n", channels->rc.ch[10]);
            // usart_printf("ch[11]:%d\r\n", channels->rc.ch[11]);
            // usart_printf("ch[12]:%d\r\n", channels->rc.ch[12]);
            // usart_printf("ch[13]:%d\r\n", channels->rc.ch[13]);
            // usart_printf("ch[14]:%d\r\n", channels->rc.ch[14]);
            // usart_printf("ch[15]:%d\r\n", channels->rc.ch[15]);
            // usart_printf("online:%d\r\n", channels->online);
            // usart_printf("-------------------\r\n");
            channels->data_updated = 0;  // 清除标志位
         motor[Motor1].ctrl.pos_set = 0.0f;    // 设置目标位置为1圈（2π）
         motor[Motor1].ctrl.vel_set = 0.0f;     // 设置速度
         motor[Motor1].ctrl.kp_set = 0.0f;      // 设置位置环比例增益
         motor[Motor1].ctrl.kd_set = 0.0f;      // 设置位置环微分增益
         motor[Motor1].ctrl.tor_set = (channels->rc.ch[3] - 1024) * 0.0001f;     // 设置转矩为0
         motor[Motor2].ctrl.pos_set = 0.0f;    // 设置目标位置为1圈（2π）
         motor[Motor2].ctrl.vel_set = 0.0f;     // 设置速度
         motor[Motor2].ctrl.kp_set = 0.0f;      // 设置位置环比例增益
         motor[Motor2].ctrl.kd_set = 0.0f;      // 设置位置环微分增益
         motor[Motor2].ctrl.tor_set = (channels->rc.ch[3] - 1024) * 0.0001f;     // 设置转矩为0
         motor[Motor3].ctrl.pos_set = 0.0f;    // 设置目标位置为1圈（2π）
         motor[Motor3].ctrl.vel_set = 0.0f;     // 设置速度
         motor[Motor3].ctrl.kp_set = 0.0f;      // 设置位置环比例增益
         motor[Motor3].ctrl.kd_set = 0.0f;      // 设置位置环微分增益
         motor[Motor3].ctrl.tor_set = (channels->rc.ch[3] - 1024) * 0.0001f;     // 设置转矩为0
         motor[Motor4].ctrl.pos_set = 0.0f;    // 设置目标位置为1圈（2π）
         motor[Motor4].ctrl.vel_set = 0.0f;     // 设置速度
         motor[Motor4].ctrl.kp_set = 0.0f;      // 设置位置环比例增益
         motor[Motor4].ctrl.kd_set = 0.0f;      // 设置位置环微分增益
         motor[Motor4].ctrl.tor_set = (channels->rc.ch[3] - 1024) * 0.0001f;     // 设置转矩为0

//         dm_motor_ctrl_send(&hfdcan1, &motor[Motor3]);
//         dm_motor_ctrl_send(&hfdcan1, &motor[Motor4]);


    }
      osDelay(100);

  }
  /* USER CODE END MotorTaskEntry */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
