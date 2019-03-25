#include "stdafx.h"
#include "Utils.h"

int Get_bit_at_position(UINT8 *buf, UINT8 &bytePosition, UINT8 &bitPosition)
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

int Get_uev_code_num(UINT8 *buf, UINT8 &bytePosition, UINT8 &bitPosition)
{
	assert(bitPosition < 8); // ���ԣ������� bitPosition < 8 ����������������򲢱���
	UINT8 val = 0, prefixZeroCount = 0; // ǰ׺0����
	int prefix = 0, surfix = 0; // ����ǰ׺����׺

	while (true) // ͳ��ǰ׺0�ĸ���
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
	prefix = (1 << prefixZeroCount) - 1; // ��ȡǰ׺��2^prefixZeroCount - 1
	for (size_t i = 0; i < prefixZeroCount; i++) // ��ȡ��׺
	{
		val = Get_bit_at_position(buf, bytePosition, bitPosition);
		surfix += val * (1 << (prefixZeroCount - i - 1));
	}

	prefix += surfix;

	return prefix; // ����ָ�����ײ������ֵ
}