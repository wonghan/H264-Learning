#include "stdafx.h"
#include "Utils.h"

int Get_bit_at_position(UINT8 *buf, UINT8 &bytePosition, UINT8 &bitPosition)
{
	UINT8 mask = 0, val = 0;

	mask = 1 << (7 - bitPosition); // 模版变量，1左移，用于截取第几位
	val = ((buf[bytePosition] & mask) != 0);
	if (++bitPosition > 7)
	{
		bytePosition++;
		bitPosition = 0;
	}

	return val;
}

int Get_uev_code_num(UINT8 *buf, UINT8 &bytePosition, UINT8 &bitPosition)
{
	assert(bitPosition < 8); // 断言，若出现 bitPosition < 8 的情况，则跳出程序并报错
	UINT8 val = 0, prefixZeroCount = 0; // 前缀0个数
	int prefix = 0, surfix = 0; // 定义前缀、后缀

	while (true) // 统计前缀0的个数
	{
		val = Get_bit_at_position(buf, bytePosition, bitPosition);
		if (val == 0)
		{
			prefixZeroCount++;
		}
		else
		{
			break;
		}
	}
	// codeNum = 2^prefixZeroCount - 1 + read_bits(prefixZeroCount);
	prefix = (1 << prefixZeroCount) - 1; // 获取前缀：2^prefixZeroCount - 1
	for (size_t i = 0; i < prefixZeroCount; i++) // 获取后缀
	{
		val = Get_bit_at_position(buf, bytePosition, bitPosition);
		surfix += val * (1 << (prefixZeroCount - i - 1));
	}

	prefix += surfix;

	return prefix; // 返回指数哥伦布编码的值
}