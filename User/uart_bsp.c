#include "uart_bsp.h"
#include "string.h"
#include "usart.h"

#define SBUS_HEAD 0X0F
#define SBUS_END 0X00

__attribute__((section(".sram4"))) uint8_t rx_buff[BUFF_SIZE * 2];
remoter_t remoter;
// 发送uint16数据
remoter_t * get_remoter(void)
{
    return &remoter;
}

void sbus_frame_parse(remoter_t *remoter, uint8_t *buf)
{
    if ((buf[0] != SBUS_HEAD) || (buf[24] != SBUS_END))
        return;

    if (buf[23] == 0x0C)
        remoter->online = 0;
    else
        remoter->online = 1;

    remoter->rc.ch[ 0] = ((int16_t)buf[ 1] >> 0 | ((int16_t)buf[ 2] << 8 )) & 0x07FF;
    remoter->rc.ch[ 1] = ((int16_t)buf[ 2] >> 3 | ((int16_t)buf[ 3] << 5 )) & 0x07FF;
    remoter->rc.ch[ 2] = ((int16_t)buf[ 3] >> 6 | ((int16_t)buf[ 4] << 2 )  | (int16_t)buf[ 5] << 10 ) & 0x07FF;
    remoter->rc.ch[ 3] = ((int16_t)buf[ 5] >> 1 | ((int16_t)buf[ 6] << 7 )) & 0x07FF;
    remoter->rc.ch[ 4] = ((int16_t)buf[ 6] >> 4 | ((int16_t)buf[ 7] << 4 )) & 0x07FF;
    remoter->rc.ch[ 5] = ((int16_t)buf[ 7] >> 7 | ((int16_t)buf[ 8] << 1 )  | (int16_t)buf[ 9] <<  9 ) & 0x07FF;
    remoter->rc.ch[ 6] = ((int16_t)buf[ 9] >> 2 | ((int16_t)buf[10] << 6 )) & 0x07FF;
    remoter->rc.ch[ 7] = ((int16_t)buf[10] >> 5 | ((int16_t)buf[11] << 3 )) & 0x07FF;
    remoter->rc.ch[ 8] = ((int16_t)buf[12] << 0 | ((int16_t)buf[13] << 8 )) & 0x07FF;
    remoter->rc.ch[ 9] = ((int16_t)buf[13] >> 3 | ((int16_t)buf[14] << 5 )) & 0x07FF;
    remoter->rc.ch[10] = ((int16_t)buf[14] >> 6 | ((int16_t)buf[15] << 2 )  | (int16_t)buf[16] << 10 ) & 0x07FF;
    remoter->rc.ch[11] = ((int16_t)buf[16] >> 1 | ((int16_t)buf[17] << 7 )) & 0x07FF;
    remoter->rc.ch[12] = ((int16_t)buf[17] >> 4 | ((int16_t)buf[18] << 4 )) & 0x07FF;
    remoter->rc.ch[13] = ((int16_t)buf[18] >> 7 | ((int16_t)buf[19] << 1 )  | (int16_t)buf[20] <<  9 ) & 0x07FF;
    remoter->rc.ch[14] = ((int16_t)buf[20] >> 2 | ((int16_t)buf[21] << 6 )) & 0x07FF;
    remoter->rc.ch[15] = ((int16_t)buf[21] >> 5 | ((int16_t)buf[22] << 3 )) & 0x07FF;

    remoter->data_updated = 1;  // 设置数据更新标志位
}
void Process_Data(uint8_t *buff, uint16_t len, UART_HandleTypeDef * huart)
{
    if (len == BUFF_SIZE)
    {
        sbus_frame_parse(&remoter, buff);
        memset(buff, 0, BUFF_SIZE * 2);
        HAL_UARTEx_ReceiveToIdle_IT(huart, buff, BUFF_SIZE*2); // ������Ϻ�����
    }
    else  // �������ݳ��ȴ���BUFF_SIZE��������
    {
        memset(buff, 0, BUFF_SIZE * 2);
        HAL_UARTEx_ReceiveToIdle_IT(huart, buff, BUFF_SIZE*2); // ������Ϻ�����
    }
}
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef * huart, uint16_t Size)
{

    if(huart->Instance == UART5)
	{
        Process_Data(rx_buff, Size,huart);
	}
    if(huart->Instance == USART1)
    {

        if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE))
        {
            __HAL_UART_CLEAR_IDLEFLAG(&huart1);  // 清除 IDLE 标志，必须！

            HAL_UART_DMAStop(&huart1);  // 停止 DMA，准备读取数据

            uint16_t len = Size - __HAL_DMA_GET_COUNTER(huart1.hdmarx);  // 实际接收长度


            // ✅ 此处你处理接收到的数据 rx_buf[0..len-1]
            Process_Data(rx_buff, len,huart);

        }
    }
}



void HAL_UART_ErrorCallback(UART_HandleTypeDef * huart)
{
	if(huart->Instance == UART5)
	{
		HAL_UARTEx_ReceiveToIdle_DMA(&huart5, rx_buff, BUFF_SIZE*2); // ���շ������������
		memset(rx_buff, 0, BUFF_SIZE);							   // ������ջ���		
	}
}


