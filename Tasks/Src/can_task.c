#include "can_task.h"
#include "dm_motor_ctrl.h"
#include "tim.h"
#include "fdcan.h"


void CanTaskEntry(void const * argument)
{
  dm_motor_6215_init();
  dm_motor_6220_init();
  osDelay(200);
  __HAL_TIM_SET_AUTORELOAD(&htim12, 60);

  HAL_TIM_PWM_Start(&htim12, TIM_CHANNEL_2);
  osDelay(100);
  HAL_TIM_PWM_Stop(&htim12, TIM_CHANNEL_2);
  osDelay(10);
  __HAL_TIM_SET_AUTORELOAD(&htim12, 70);
  HAL_TIM_PWM_Start(&htim12, TIM_CHANNEL_2);
  osDelay(100);
  HAL_TIM_PWM_Stop(&htim12, TIM_CHANNEL_2);

  for(;;)
  {
    dm_motor_ctrl_send(&hfdcan1, &motor[Motor1]);
    osDelay(3);
    dm_motor_ctrl_send(&hfdcan1, &motor[Motor2]);
    osDelay(3);
    dm_motor_ctrl_send(&hfdcan1, &motor[Motor3]);
    osDelay(3);
    dm_motor_ctrl_send(&hfdcan1, &motor[Motor4]);
    osDelay(3);
    dm_motor_ctrl_send(&hfdcan1, &motor[Motor5]);
    osDelay(3);
    dm_motor_ctrl_send(&hfdcan1, &motor[Motor6]);
    osDelay(3);
    dm_motor_ctrl_send(&hfdcan1, &motor[Motor7]);
    osDelay(3);
    dm_motor_ctrl_send(&hfdcan1, &motor[Motor8]);

    osDelay(10);
  }
}