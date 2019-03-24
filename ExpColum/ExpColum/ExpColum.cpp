// ExpColum.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <assert.h> // 断言，若断言为false，则跳出程序并报错

typedef unsigned char UINT8;

static int get_bit_at_position(UINT8 *buf, UINT8 &bytePosition, UINT8 &bitPosition) // 逐比特读取
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

static int get_uev_code_num(UINT8 *buf, UINT8 &bytePosition, UINT8 &bitPosition) // 解析指数哥伦布编码
{
	assert(bitPosition < 8); // 断言，若出现 bitPosition < 8 的情况，则跳出程序并报错
	UINT8 val = 0, prefixZeroCount = 0; // 前缀0个数
	int prefix = 0, surfix = 0;

	while (true)
	{
		val = get_bit_at_position(buf, bytePosition, bitPosition);
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
		val = get_bit_at_position(buf, bytePosition, bitPosition); 
		surfix += val * (1 << (prefixZeroCount - i - 1));
	}

	prefix += surfix;

	return prefix; // 返回指数哥伦布编码的值
}

int _tmain(int argc, _TCHAR* argv[])
{
	UINT8 strArray[6] = { 0xA6, 0x42, 0x98, 0xE2, 0x04, 0x8A };
	UINT8 bytePosition = 0, bitPosition = 0;
	UINT8 dataLengthInBits = sizeof(strArray)* 8; // 有多少个比特

	int codeNum = 0;
	while ((bytePosition * 8 + bitPosition) < dataLengthInBits)
	{
		codeNum = get_uev_code_num(strArray, bytePosition, bitPosition);
		printf("ExpoColumb codeNum = %d\n", codeNum);
	}


	return 0;
}

