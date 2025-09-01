#include "uart_bsp.h"
#include "string.h"
#include "usart.h"

#define SBUS_HEAD 0X0F
#define SBUS_END 0X00
#define TELEOP_HEAD 0XFF

__attribute__((section(".sram4"))) uint8_t SBUS_MultiRx_Buf[2][BUFF_SIZE];
__attribute__((section(".sram4"))) uint8_t TELEOP_MultiRx_Buf[2][TELEOP_BUFF_SIZE];
remoter_t remoter;
remoter_t *channels = &remoter;
// 发送uint16数据
remoter_t *get_remoter(void)
{
    return &remoter;
}
static void USART_RxDMA_MultiBufferStart(UART_HandleTypeDef *huart, uint32_t *SrcAddress, uint32_t *DstAddress, uint32_t *SecondMemAddress, uint32_t DataLength)
{
    huart->ReceptionType = HAL_UART_RECEPTION_TOIDLE;
    huart->RxEventType = HAL_UART_RXEVENT_TC;
    huart->RxXferSize = DataLength;
    SET_BIT(huart->Instance->CR3, USART_CR3_DMAR);
    __HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);
    HAL_DMAEx_MultiBufferStart(huart->hdmarx, (uint32_t)SrcAddress, (uint32_t)DstAddress, (uint32_t)SecondMemAddress, DataLength);
}

void BSP_USART_Init(void)
{
    memset(SBUS_MultiRx_Buf[0], 0, BUFF_SIZE);
    memset(SBUS_MultiRx_Buf[1], 0, BUFF_SIZE);
    memset(TELEOP_MultiRx_Buf[0], 0, TELEOP_BUFF_SIZE);
    memset(TELEOP_MultiRx_Buf[1], 0, TELEOP_BUFF_SIZE);
    USART_RxDMA_MultiBufferStart(&huart5, (uint32_t *)&(huart5.Instance->RDR), (uint32_t *)SBUS_MultiRx_Buf[0], (uint32_t *)SBUS_MultiRx_Buf[1], BUFF_SIZE * 2);
    USART_RxDMA_MultiBufferStart(&huart1, (uint32_t *)&(huart1.Instance->RDR), (uint32_t *)TELEOP_MultiRx_Buf[0], (uint32_t *)TELEOP_MultiRx_Buf[1], TELEOP_BUFF_SIZE * 2);
}

void sbus_frame_parse(remoter_t *remoter, uint8_t *buf)
{
    if ((buf[0] != SBUS_HEAD) || (buf[24] != SBUS_END))
        return;

    if (buf[23] == 0x0C)
        remoter->online = 0;
    else
        remoter->online = 1;

    remoter->rc.ch[0] = ((int16_t)buf[1] >> 0 | ((int16_t)buf[2] << 8)) & 0x07FF;
    remoter->rc.ch[1] = ((int16_t)buf[2] >> 3 | ((int16_t)buf[3] << 5)) & 0x07FF;
    remoter->rc.ch[2] = ((int16_t)buf[3] >> 6 | ((int16_t)buf[4] << 2) | (int16_t)buf[5] << 10) & 0x07FF;
    remoter->rc.ch[3] = ((int16_t)buf[5] >> 1 | ((int16_t)buf[6] << 7)) & 0x07FF;
    remoter->rc.ch[4] = ((int16_t)buf[6] >> 4 | ((int16_t)buf[7] << 4)) & 0x07FF;
    remoter->rc.ch[5] = ((int16_t)buf[7] >> 7 | ((int16_t)buf[8] << 1) | (int16_t)buf[9] << 9) & 0x07FF;
    remoter->rc.ch[6] = ((int16_t)buf[9] >> 2 | ((int16_t)buf[10] << 6)) & 0x07FF;
    remoter->rc.ch[7] = ((int16_t)buf[10] >> 5 | ((int16_t)buf[11] << 3)) & 0x07FF;
    remoter->rc.ch[8] = ((int16_t)buf[12] << 0 | ((int16_t)buf[13] << 8)) & 0x07FF;
    remoter->rc.ch[9] = ((int16_t)buf[13] >> 3 | ((int16_t)buf[14] << 5)) & 0x07FF;
    remoter->rc.ch[10] = ((int16_t)buf[14] >> 6 | ((int16_t)buf[15] << 2) | (int16_t)buf[16] << 10) & 0x07FF;
    remoter->rc.ch[11] = ((int16_t)buf[16] >> 1 | ((int16_t)buf[17] << 7)) & 0x07FF;
    remoter->rc.ch[12] = ((int16_t)buf[17] >> 4 | ((int16_t)buf[18] << 4)) & 0x07FF;
    remoter->rc.ch[13] = ((int16_t)buf[18] >> 7 | ((int16_t)buf[19] << 1) | (int16_t)buf[20] << 9) & 0x07FF;
    remoter->rc.ch[14] = ((int16_t)buf[20] >> 2 | ((int16_t)buf[21] << 6)) & 0x07FF;
    remoter->rc.ch[15] = ((int16_t)buf[21] >> 5 | ((int16_t)buf[22] << 3)) & 0x07FF;
    for (int i = 0; i < 4; i++)
    {
        remoter->rc.ch[i] -= CHANNEL_OFFSET;
    }

    remoter->data_updated = 1; // 设置数据更新标志位
}
void teleop_frame_parse(remoter_t *remoter, uint8_t *buf)
{
    if(buf[0] != TELEOP_HEAD)
    {
        return;
    }
    remoter->rc.ch[3] = ((int16_t)buf[1] >> 0 | ((int16_t)buf[2] << 8)) & 0x07FF;
    remoter->rc.ch[3] -= CHANNEL_OFFSET;
    remoter->rc.ch[2] = ((int16_t)buf[3] >> 0 | ((int16_t)buf[4] << 8)) & 0x07FF;
    remoter->rc.ch[2] -= CHANNEL_OFFSET;
    remoter->rc.ch[0] = ((int16_t)buf[5] >> 0 | ((int16_t)buf[6] << 8)) & 0x07FF;
    remoter->rc.ch[0] -= CHANNEL_OFFSET;
    remoter->data_updated = 1; // 设置数据更新标志位
}
void Process_Data(uint8_t *buff, uint16_t len, UART_HandleTypeDef *huart)
{
    if (huart->Instance == UART5)
    {
        sbus_frame_parse(&remoter, buff);
    }
    else if (huart->Instance == USART1)
    {
        teleop_frame_parse(&remoter, buff);
        //HAL_UART_Transmit(huart, buff, TELEOP_BUFF_SIZE, 1000);
    }
}
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == UART5)
    {
        if (((((DMA_Stream_TypeDef *)huart->hdmarx->Instance)->CR) & DMA_SxCR_CT) == RESET)
        {
            // Disable DMA
            __HAL_DMA_DISABLE(huart->hdmarx);

            ((DMA_Stream_TypeDef *)huart->hdmarx->Instance)->CR |= DMA_SxCR_CT;
            /* reset the receive count */
            __HAL_DMA_SET_COUNTER(huart->hdmarx, BUFF_SIZE * 2);

            if (Size == BUFF_SIZE)
            {
                Process_Data(SBUS_MultiRx_Buf[0],  Size, &huart5);
            }
            memset(SBUS_MultiRx_Buf[0], 0, BUFF_SIZE);
            
        }
        /* Current memory buffer used is Memory 1 */
        else
        {
            // Disable DMA
            __HAL_DMA_DISABLE(huart->hdmarx);

            ((DMA_Stream_TypeDef *)huart->hdmarx->Instance)->CR &= ~(DMA_SxCR_CT);

            /* reset the receive count */
            __HAL_DMA_SET_COUNTER(huart->hdmarx, BUFF_SIZE * 2);

            if (Size == BUFF_SIZE)
            {
                Process_Data(SBUS_MultiRx_Buf[1], Size, &huart5);
            }
            memset(SBUS_MultiRx_Buf[1], 0, BUFF_SIZE);
        }

        huart->ReceptionType = HAL_UART_RECEPTION_TOIDLE;

        huart->RxEventType = HAL_UART_RXEVENT_TC;

        /* Enalbe IDLE interrupt */
        __HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);

        /* Enable the DMA transfer for the receiver request */
        SET_BIT(huart->Instance->CR3, USART_CR3_DMAR);

        /* Enable DMA */
        __HAL_DMA_ENABLE(huart->hdmarx);
    }
    if (huart->Instance == USART1)
    {
        if (((((DMA_Stream_TypeDef *)huart->hdmarx->Instance)->CR) & DMA_SxCR_CT) == RESET)
        {
            // Disable DMA
            __HAL_DMA_DISABLE(huart->hdmarx);

            ((DMA_Stream_TypeDef *)huart->hdmarx->Instance)->CR |= DMA_SxCR_CT;
            /* reset the receive count */
            __HAL_DMA_SET_COUNTER(huart->hdmarx, TELEOP_BUFF_SIZE * 2);

            if (Size == TELEOP_BUFF_SIZE)
            {
                Process_Data(TELEOP_MultiRx_Buf[0], TELEOP_BUFF_SIZE * 2, &huart1);
            }
            memset(TELEOP_MultiRx_Buf[0], 0, TELEOP_BUFF_SIZE);
        }
        /* Current memory buffer used is Memory 1 */
        else
        {
            // Disable DMA
            __HAL_DMA_DISABLE(huart->hdmarx);

            ((DMA_Stream_TypeDef *)huart->hdmarx->Instance)->CR &= ~(DMA_SxCR_CT);

            /* reset the receive count */
            __HAL_DMA_SET_COUNTER(huart->hdmarx, TELEOP_BUFF_SIZE * 2);

            if (Size == TELEOP_BUFF_SIZE)
            {
                Process_Data(TELEOP_MultiRx_Buf[1], TELEOP_BUFF_SIZE * 2, &huart1);
                memset(TELEOP_MultiRx_Buf[1], 0, TELEOP_BUFF_SIZE);
            }
        }

        huart->ReceptionType = HAL_UART_RECEPTION_TOIDLE;

        huart->RxEventType = HAL_UART_RXEVENT_TC;

        /* Enalbe IDLE interrupt */
        __HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);

        /* Enable the DMA transfer for the receiver request */
        SET_BIT(huart->Instance->CR3, USART_CR3_DMAR);

        /* Enable DMA */
        __HAL_DMA_ENABLE(huart->hdmarx);
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == UART5)
    {
        memset(SBUS_MultiRx_Buf[0], 0, BUFF_SIZE);
        memset(SBUS_MultiRx_Buf[1], 0, BUFF_SIZE);
    }
    if (huart->Instance == USART1)
    {
        memset(TELEOP_MultiRx_Buf[0], 0, TELEOP_BUFF_SIZE);
        memset(TELEOP_MultiRx_Buf[1], 0, TELEOP_BUFF_SIZE);
        // 清空接收缓冲区
    }
}
