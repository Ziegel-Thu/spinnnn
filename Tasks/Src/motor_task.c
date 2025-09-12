#include "motor_task.h"
#include "FreeRTOS.h"
#include "main.h"
#include "dm_motor_ctrl.h"
#include "dm_motor_drv.h"
#include "math.h"
#include "arm_math.h"
#include "cmsis_os.h"

int16_t neglect_noise(int16_t input){
   
	return input * ((input >= 	50.f) + (input <= -50.f));

}
Control_Info_Typedef Control_Info;
float theta = M_PI_4;
void calculate_2d_module(float *vector, float* address){
    arm_sqrt_f32(vector[0] * vector[0] + vector[1] * vector[1], address);
}
void spin_2d_vector(float *vector, float angle, float* address){
    float temp_x = vector[0] * arm_cos_f32(angle) - vector[1] * arm_sin_f32(angle);
    float temp_y = vector[0] * arm_sin_f32(angle) + vector[1] * arm_cos_f32(angle);
    address[0] = temp_x;
    address[1] = temp_y;
}

void calculate_motor_data(remoter_t * channels){

    
    Control_Info.Vx = neglect_noise(channels->rc.ch[3]);
    Control_Info.Vy = -1 * neglect_noise(channels->rc.ch[2]);
    Control_Info.Vomega = neglect_noise(-channels->rc.ch[0]);
    float tangent[2] = {Control_Info.Vomega,0};
    for(int i = 0; i < 4; i++){
        spin_2d_vector(tangent,M_PI_2 * i + M_PI_4, Control_Info.component[i]);
        //right back motor is 0 and counterclockwise
        Control_Info.component[i][0] += Control_Info.Vx;
        Control_Info.component[i][1] += Control_Info.Vy;
        calculate_2d_module(Control_Info.component[i],&Control_Info.Target_Velocity[i]);
        Control_Info.Target_Velocity[i] = Control_Info.Target_Velocity[i] * 0.013f;
        Control_Info.Target_Position[i] = atan2(Control_Info.component[i][1],Control_Info.component[i][0]);
    }

    
}
void MotorTaskEntry(void const * argument)
{
    /* USER CODE BEGIN MotorTaskEntry */
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
//          motor[Motor1].ctrl.pos_set = 0.0f;    // 设置目标位置为1圈（2π）
//          motor[Motor1].ctrl.vel_set = 0.0f;     // 设置速度
//          motor[Motor1].ctrl.kp_set = 0.0f;      // 设置位置环比例增益
//          motor[Motor1].ctrl.kd_set = 0.0f;      // 设置位置环微分增益
//          motor[Motor1].ctrl.tor_set = (channels->rc.ch[3] ) * 0.0001f;     // 设置转矩为0
//          motor[Motor2].ctrl.pos_set = 0.0f;    // 设置目标位置为1圈（2π）
//          motor[Motor2].ctrl.vel_set = 0.0f;     // 设置速度
//          motor[Motor2].ctrl.kp_set = 0.0f;      // 设置位置环比例增益
//          motor[Motor2].ctrl.kd_set = 0.0f;      // 设置位置环微分增益
//          motor[Motor2].ctrl.tor_set = (channels->rc.ch[3] ) * 0.0001f;     // 设置转矩为0
//          motor[Motor3].ctrl.pos_set = 0.0f;    // 设置目标位置为1圈（2π）
//          motor[Motor3].ctrl.vel_set = 0.0f;     // 设置速度
//          motor[Motor3].ctrl.kp_set = 0.0f;      // 设置位置环比例增益
//          motor[Motor3].ctrl.kd_set = 0.0f;      // 设置位置环微分增益
//          motor[Motor3].ctrl.tor_set = (channels->rc.ch[3] ) * 0.0001f;     // 设置转矩为0
//          motor[Motor4].ctrl.pos_set = 0.0f;    // 设置目标位置为1圈（2π）
//          motor[Motor4].ctrl.vel_set = 0.0f;     // 设置速度
//          motor[Motor4].ctrl.kp_set = 0.0f;      // 设置位置环比例增益
//          motor[Motor4].ctrl.kd_set = 0.0f;      // 设置位置环微分增益
//          motor[Motor4].ctrl.tor_set = (channels->rc.ch[3] ) * 0.0001f;     // 设置转矩为0
            calculate_motor_data(channels);
            for(int i = 0; i < 4; i++){
                motor[i].ctrl.vel_set = Control_Info.Target_Velocity[i];
            }
            for(int i = 4; i < 8; i++){
                motor[i].ctrl.pos_set = Control_Info.Target_Position[i-4];
            }
//         dm_motor_ctrl_send(&hfdcan1, &motor[Motor3]);
//         dm_motor_ctrl_send(&hfdcan1, &motor[Motor4]);


    }
      osDelay(30);

  }

    /* USER CODE END MotorTaskEntry */
}
