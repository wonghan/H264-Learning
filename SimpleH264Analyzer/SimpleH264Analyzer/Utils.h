#ifndef _UTILS_H_
#define _UTILS_H_
#include "Global.h"

// ����ض�ȡ
int Get_bit_at_position(UINT8 *buf, UINT8 &bytePosition, UINT8 &bitPosition);

// ����ָ�����ײ�����
int Get_uev_code_num(UINT8 *buf, UINT8 &bytePosition, UINT8 &bitPosition);

#endif