#include "stdafx.h"
#include "CAVLC.h"
#include "CAVLC_Map.h"

using namespace std;

int get_total_coeffs(const int coeff[16])
{
	int ret = 0;
	for (int idx = 0; idx < 16; idx++)
	{
		if (coeff[idx]){
			ret++;
		}
	}
	return ret;
}

int get_trailing_ones(const int coeff[16], int trailingSign[3])
{
	int ret = 0;

	for (int idx = 15; idx >= 0; idx--)
	{
		if (abs(coeff[idx]) > 1 || ret == 3)
		{
			break;
		}
		else if (abs(coeff[idx]) == 1)
		{
			if (coeff[idx] == 1)
			{
				trailingSign[ret] = 1;
			}
			else
			{
				trailingSign[ret] = -1;
			}
			ret++;
		}
	}

	return ret;
}

int get_levels(const int coeff[16], int levels[],int levelCnt)
{
	int levelIdx = levelCnt - 1;
	for (int idx = 0; idx < 16; idx++)
	{
		if (coeff[idx] != 0)
		{
			levels[levelIdx--] = coeff[idx];
			if (levelIdx < 0)
			{
				break;
			}
		}
	}
	return 0;
}

int get_totalzeros_runbefore(const int coeff[16], int *runBefore, int *zerosLeft, int totalCoeffs)
{
	int totalZeros = 0, idx = 15, runIdx = 0;
	for (; idx >= 0; idx--)
	{
		if (coeff[idx]) // �Ƿ���ϵ��
		{
			break;
		}
	}
	totalZeros = idx - totalCoeffs + 1; // ���һ�������Ƶ����0ϵ��֮ǰ�ܹ��ж��ٸ�0

	for (; idx >= 0; idx--)
	{
		if (coeff[idx] == 0) // ����ϵ��
		{
			continue;
		}

		// �Ƿ���ϵ��
		for (int run = 0; run <= idx; run++)
		{
			if (coeff[idx - 1 - run] == 0)
			{
				runBefore[runIdx]++;
				zerosLeft[runIdx]++;
			}
			else
			{
				runIdx++;
				break;
			}
		}
	}

	for (int a = 0; a < runIdx; a++)
	{
		for (int b = a + 1; b < runIdx; b++)
		{
			zerosLeft[a] += zerosLeft[b];
		}
	}

	return totalZeros;
}

string encode_levels(int level, int &suffixLength) // ������βϵ��֮�����ͨ����ϵ���ķ�ֵ
{
	string retStr;

	int levelCode = 0; // ��Ҫ�������ɴ����levelȷ��
	if (level > 0)
	{
		levelCode = (level << 1) - 2;
	}
	else
	{
		levelCode = -(level << 1) - 1;
	}

	int levelPrefix = levelCode / (1 << suffixLength); // ǰ׺ֵ
	int levelSuffix = levelCode % (1 << suffixLength); // ��׺ֵ

	for (int idx = 0; idx < levelPrefix; idx++) // ����ǰ׺�������������ݱ�9-6�������levelPrefix������ = level_prefix��0 + һ��1
	{
		retStr += "0";
	}
	retStr += "1";

	for (int idx = 0; idx < suffixLength; idx++) // �����׺��������
	{
		if ((levelSuffix >> (suffixLength - idx -1) & 1) == 1) // ���ݺ�׺���ȣ���ȡ��׺ÿһλ�ı���ֵ
		{
			retStr += "1";
		}
		else
		{
			retStr += "0";
		}
	}

	return retStr;
}

string Encoding_cavlc_16x16(const int coeff[16])
{
	string cavlcCode;

	int totalCoeffs = get_total_coeffs(coeff); // ����ϵ���ĸ���
	int trailingSign[3] = {0}; // ��βϵ���ķ���,0��ʾ+��1��ʾ-
	int trailingOnes = get_trailing_ones(coeff, trailingSign); // ��βϵ���ĸ���

	int levelCnt = totalCoeffs - trailingOnes; // ������ͨ����ϵ���ĸ���������βϵ��֮��ģ�
	int *levels = new int[levelCnt]; // ����һ�����鱣����ͨ����ϵ���������ţ�
	memset(levels, 0, sizeof(int)* levelCnt);
	get_levels(coeff, levels, levelCnt);

	int *runBefore = new int[totalCoeffs]; // ÿ����0ϵ��֮ǰ��֮�䣩����0�ĸ���
	int *zerosLeft = new int[totalCoeffs]; // ÿ����0ϵ��֮ǰ�ܹ���0�ĸ���
	memset(runBefore, 0, sizeof(int)* totalCoeffs);
	memset(zerosLeft, 0, sizeof(int)* totalCoeffs);
	int totalZeros = get_totalzeros_runbefore(coeff, runBefore, zerosLeft, totalCoeffs); // ���һ�������Ƶ����0ϵ��֮ǰ�ܵ�0�ĸ���

	// coeff_token
	if (totalCoeffs >= 3)
	{
		cavlcCode += coeffTokenMap[0][(totalCoeffs - 3) * 4 + trailingOnes + 6];
	}
	else if (totalCoeffs <= 1)
	{
		cavlcCode += coeffTokenMap[0][totalCoeffs + trailingOnes];
	}
	else if (totalCoeffs == 2)
	{
		cavlcCode += coeffTokenMap[0][totalCoeffs + trailingOnes + 1];
	}

	// trailing_sign
	for (int idx = 0; idx < trailingOnes; idx++)
	{
		if (trailingSign[idx] == 1)
		{
			cavlcCode += "0";
		}
		else if (trailingSign[idx] == -1)
		{
			cavlcCode += "1";
		}
	}

	// levels:��ͨ����ϵ��(����βϵ���ķ���ϵ��)
	int suffixLength = 0; // ��׺���ȳ�ʼ���� (TotalCoeff( coeff_token )С�ڻ���� 10������TrailingOnes( coeff_token )����3)��suffixLength����0
	if (totalCoeffs > 10 && trailingOnes < 3) // ��TotalCoeff( coeff_token )����10������TrailingOnes( coeff_token )С��3����suffixLength����1
	{
		suffixLength = 1;
	}
	for (int idx = 0; idx < levelCnt; idx++)
	{
		cavlcCode += encode_levels(levels[idx], suffixLength);
		if ((abs(levels[idx]) >(suffixLength == 0 ? 0 : (3 << (suffixLength - 1)))) && suffixLength < 6)
		{
			suffixLength++;
		}
	}

	// totalZeros������totalZeros�����ݱ�9-7/��9-8
	cavlcCode += totalZerosMap[totalZeros][totalCoeffs];

	// runBefore������runBefore�����ݱ�9-10
	for (int idx = 0; idx < totalCoeffs - 1; idx++) // totalCoeffs - 1����Ϊ�����Ƶ�ķ���ϵ��ǰ���0����Ҫ����
	{
		if (zerosLeft[idx] == 0) // û��ʣ���0ϵ����Ҫ����ʱ������Ҫ�ټ������б���
		{
			break;
		}
		cavlcCode += runBeforeMap[runBefore[idx]][zerosLeft[idx]];
	}

	// ɾ������ֹ�ڴ�й©
	delete[] levels;
	delete[] runBefore;
	delete[] zerosLeft;
	return cavlcCode;
}