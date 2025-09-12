#ifndef __CAN_TASK_H__
#define __CAN_TASK_H__

#include "cmsis_os.h"

extern osThreadId canTaskHandle;

void CanTaskEntry(void const * argument);

#endif /* __CAN_TASK_H__ */