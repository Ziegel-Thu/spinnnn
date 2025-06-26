/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "dma.h"
#include "fdcan.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "uart_bsp.h"
#include "bsp_fdcan.h"
#include "dm_motor_ctrl.h"
#include <stdarg.h>
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
remoter_t * channels;
void usart_printf(const char *fmt,...)
{
    static uint8_t tx_buf[256] = {0};
    static va_list ap;
    static uint16_t len;
    va_start(ap, fmt);


    len = vsprintf((char *)tx_buf, fmt, ap);

    va_end(ap);
    HAL_UART_Transmit(&huart1, tx_buf, len, 0xffff);

}
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    /* USER CODE BEGIN Callback 0 */
    // if (htim->Instance == TIM3) {

    //     read_all_motor_data(&motor[Motor1]);

    //     if(motor[Motor1].tmp.read_flag == 0)
    //         dm_motor_ctrl_send(&hfdcan1, &motor[Motor1]);
    // }
}
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
    MX_DMA_Init();
    MX_USART1_UART_Init();
    MX_UART5_Init();
  MX_FDCAN1_Init();
  MX_FDCAN2_Init();
  MX_FDCAN3_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
    power(1);
    //HAL_Delay(1000);
  bsp_fdcan_set_baud(&hfdcan1, CAN_CLASS, CAN_BR_1M);
  
	bsp_can_init();
	dm_motor_init();

	motor[Motor1].ctrl.mode = mit_mode;
    motor[Motor2].ctrl.mode = mit_mode;
	//HAL_Delay(100);
	write_motor_data(motor[Motor1].id, 10, mit_mode, 0, 0, 0);
    write_motor_data(motor[Motor2].id, 10, mit_mode, 0, 0, 0);
    //HAL_Delay(100);
    read_motor_data(motor[Motor1].id, RID_CAN_BR);
	dm_motor_disable(&hfdcan1, &motor[Motor1]);
    read_motor_data(motor[Motor2].id, RID_CAN_BR);
    dm_motor_disable(&hfdcan1, &motor[Motor2]);
	//HAL_Delay(100);
	save_motor_data(motor[Motor1].id, 10);
    save_motor_data(motor[Motor2].id, 10);
	//HAL_Delay(100);
	dm_motor_enable(&hfdcan1, &motor[Motor1]);
    dm_motor_enable(&hfdcan1, &motor[Motor2]);
	//HAL_Delay(1000);
    uint8_t data2[8] = {127,255,127,240,0,0,8,9};
    fdcanx_send_data(&hfdcan1, motor[Motor1].id, data2, 8);
    fdcanx_send_data(&hfdcan1, motor[Motor2].id, data2, 8);

    HAL_TIM_Base_Start_IT(&htim3);

  for (int i =0; i< BUFF_SIZE*2; i++){
      rx_buff[i]=0;
  }
  channels = get_remoter();
  channels->data_updated = 0;  // 初始化时清零标志位
//
//  motor[Motor1].ctrl.pos_set = 0.0f;    // 设置目标位置为1圈（2π）
//  motor[Motor1].ctrl.vel_set = 0.0f;     // 设置速度
//  motor[Motor1].ctrl.kp_set = 0.0f;      // 设置位置环比例增益
//  motor[Motor1].ctrl.kd_set = 0.0f;      // 设置位置环微分增益
//  motor[Motor1].ctrl.tor_set = 0.05f;     // 设置转矩为0


//    dm_motor_ctrl_send(&hfdcan1, &motor[Motor1]);
    HAL_UARTEx_ReceiveToIdle_DMA(&huart5, rx_buff, BUFF_SIZE*2);
//	HAL_UARTEx_ReceiveToIdle_DMA(&huart5, rx_buff, BUFF_SIZE*2);
//	HAL_UARTEx_ReceiveToIdle_DMA(&huart8, rx_buff, BUFF_SIZE*2);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    // 检查数据更新标志位
     if (channels->data_updated) {
         usart_printf("ch[0]:%d\r\n", channels->rc.ch[0]);
         usart_printf("ch[1]:%d\r\n", channels->rc.ch[1]);
         usart_printf("ch[2]:%d\r\n", channels->rc.ch[2]);
         usart_printf("ch[3]:%d\r\n", channels->rc.ch[3]);
         usart_printf("ch[4]:%d\r\n", channels->rc.ch[4]);
         usart_printf("ch[5]:%d\r\n", channels->rc.ch[5]);
 //        usart_printf("ch[6]:%d\r\n", channels->rc.ch[6]);
 //        usart_printf("ch[7]:%d\r\n", channels->rc.ch[7]);
 //        usart_printf("ch[8]:%d\r\n", channels->rc.ch[8]);
 //        usart_printf("ch[9]:%d\r\n", channels->rc.ch[9]);
 //        usart_printf("ch[10]:%d\r\n", channels->rc.ch[10]);
 //        usart_printf("ch[11]:%d\r\n", channels->rc.ch[11]);
 //        usart_printf("ch[12]:%d\r\n", channels->rc.ch[12]);
 //        usart_printf("ch[13]:%d\r\n", channels->rc.ch[13]);
 //        usart_printf("ch[14]:%d\r\n", channels->rc.ch[14]);
 //        usart_printf("ch[15]:%d\r\n", channels->rc.ch[15]);
         usart_printf("online:%d\r\n", channels->online);
         usart_printf("-------------------\r\n");
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
//         motor[Motor3].ctrl.pos_set = 0.0f;    // 设置目标位置为1圈（2π）
//         motor[Motor3].ctrl.vel_set = 0.0f;     // 设置速度
//         motor[Motor3].ctrl.kp_set = 0.0f;      // 设置位置环比例增益
//         motor[Motor3].ctrl.kd_set = 0.0f;      // 设置位置环微分增益
//         motor[Motor3].ctrl.tor_set = (channels->rc.ch[3] - 1024) * 0.0001f;     // 设置转矩为0
//         motor[Motor4].ctrl.pos_set = 0.0f;    // 设置目标位置为1圈（2π）
//         motor[Motor4].ctrl.vel_set = 0.0f;     // 设置速度
//         motor[Motor4].ctrl.kp_set = 0.0f;      // 设置位置环比例增益
//         motor[Motor4].ctrl.kd_set = 0.0f;      // 设置位置环微分增益
//         motor[Motor4].ctrl.tor_set = (channels->rc.ch[3] - 1024) * 0.0001f;     // 设置转矩为0
         dm_motor_ctrl_send(&hfdcan1, &motor[Motor1]);
         dm_motor_ctrl_send(&hfdcan1, &motor[Motor2]);
//         dm_motor_ctrl_send(&hfdcan1, &motor[Motor3]);
//         dm_motor_ctrl_send(&hfdcan1, &motor[Motor4]);


         HAL_Delay(100);

     }

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

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 2;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLP = 1;
  RCC_OscInitStruct.PLL.PLLQ = 6;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV4;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

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
