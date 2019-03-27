#ifndef _UTILS_H_
#define _UTILS_H_
#include "Global.h"

// 逐比特读取
int Get_bit_at_position(UINT8 *buf, UINT8 &bytePosition, UINT8 &bitPosition);

// 解析无符号指数哥伦布编码
int Get_uev_code_num(UINT8 *buf, UINT8 &bytePosition, UINT8 &bitPosition);

// 解析有符号指数哥伦布编码
int Get_sev_code_num(UINT8 *buf, UINT8 &bytePosition, UINT8 &bitPosition);

// 获取多个比特的无符号整数
int Get_uint_code_num(UINT8 *buf, UINT8 &bytePosition, UINT8 &bitPosition, UINT8 length);

#endif