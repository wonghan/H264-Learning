// ExpColum.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <assert.h> // ���ԣ�������Ϊfalse�����������򲢱���

typedef unsigned char UINT8;

static int get_bit_at_position(UINT8 *buf, UINT8 &bytePosition, UINT8 &bitPosition) // ����ض�ȡ
{
	UINT8 mask = 0, val = 0;

	mask = 1 << (7 - bitPosition); // ģ�������1���ƣ����ڽ�ȡ�ڼ�λ
	val = ((buf[bytePosition] & mask) != 0);
	if (++bitPosition > 7)
	{
		bytePosition++;
		bitPosition = 0;
	}

	return val;
}

static int get_uev_code_num(UINT8 *buf, UINT8 &bytePosition, UINT8 &bitPosition) // ����ָ�����ײ�����
{
	assert(bitPosition < 8); // ���ԣ������� bitPosition < 8 ����������������򲢱���
	UINT8 val = 0, prefixZeroCount = 0; // ǰ׺0����
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
	prefix = (1 << prefixZeroCount) - 1; // ��ȡǰ׺��2^prefixZeroCount - 1
	for (size_t i = 0; i < prefixZeroCount; i++) // ��ȡ��׺
	{
		val = get_bit_at_position(buf, bytePosition, bitPosition); 
		surfix += val * (1 << (prefixZeroCount - i - 1));
	}

	prefix += surfix;

	return prefix; // ����ָ�����ײ������ֵ
}

int _tmain(int argc, _TCHAR* argv[])
{
	UINT8 strArray[6] = { 0xA6, 0x42, 0x98, 0xE2, 0x04, 0x8A };
	UINT8 bytePosition = 0, bitPosition = 0;
	UINT8 dataLengthInBits = sizeof(strArray)* 8; // �ж��ٸ�����

	int codeNum = 0;
	while ((bytePosition * 8 + bitPosition) < dataLengthInBits)
	{
		codeNum = get_uev_code_num(strArray, bytePosition, bitPosition);
		printf("ExpoColumb codeNum = %d\n", codeNum);
	}


	return 0;
}

