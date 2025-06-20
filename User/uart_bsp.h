#ifndef __UART_BSP_H__
#define __UART_BSP_H__

#include "main.h"

#define BUFF_SIZE	25


extern uint8_t rx_buff[BUFF_SIZE * 2];

typedef struct
{
    uint16_t online;
    uint8_t data_updated;  // 数据更新标志位

    struct
    {
        int16_t ch[16];
    } rc;

    struct
    {
        /* STICK VALUE */
        int16_t left_vert;
        int16_t left_hori;
        int16_t right_vert;
        int16_t right_hori;
    } joy;

    struct
    {
        /* VAR VALUE */
        float a;
        float b;
    } var;

    struct
    {
        /* KEY VALUE */
        uint8_t a;
        uint8_t b;
        uint8_t c;
        uint8_t d;
        uint8_t e;
        uint8_t f;
        uint8_t g;
        uint8_t h;
    } key;
} remoter_t;
remoter_t * get_remoter(void);
void Process_Data(uint8_t*, uint16_t ,UART_HandleTypeDef *);
#endif /*__UART_BSP_H__ */

