#ifndef _UTILS_H_
#define _UTILS_H_
#include "Global.h"

// 逐比特读取
int Get_bit_at_position(UINT8 *buf, UINT8 &bytePosition, UINT8 &bitPosition);

// 解析指数哥伦布编码
int Get_uev_code_num(UINT8 *buf, UINT8 &bytePosition, UINT8 &bitPosition);

#endif