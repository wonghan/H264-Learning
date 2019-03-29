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
		if (coeff[idx]) // 是非零系数
		{
			break;
		}
	}
	totalZeros = idx - totalCoeffs + 1; // 最后一个（最高频）非0系数之前总共有多少个0

	for (; idx >= 0; idx--)
	{
		if (coeff[idx] == 0) // 是零系数
		{
			continue;
		}

		// 是非零系数
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

string encode_levels(int level, int &suffixLength) // 编码拖尾系数之外的普通非零系数的幅值
{
	string retStr;

	int levelCode = 0; // 重要变量，由传入的level确定
	if (level > 0)
	{
		levelCode = (level << 1) - 2;
	}
	else
	{
		levelCode = -(level << 1) - 1;
	}

	int levelPrefix = levelCode / (1 << suffixLength); // 前缀值
	int levelSuffix = levelCode % (1 << suffixLength); // 后缀值

	for (int idx = 0; idx < levelPrefix; idx++) // 编码前缀的数据流，根据表9-6编码规则，levelPrefix比特流 = level_prefix个0 + 一个1
	{
		retStr += "0";
	}
	retStr += "1";

	for (int idx = 0; idx < suffixLength; idx++) // 编码后缀的数据流
	{
		if ((levelSuffix >> (suffixLength - idx -1) & 1) == 1) // 根据后缀长度，获取后缀每一位的比特值
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

	int totalCoeffs = get_total_coeffs(coeff); // 非零系数的个数
	int trailingSign[3] = {0}; // 拖尾系数的符号,0表示+，1表示-
	int trailingOnes = get_trailing_ones(coeff, trailingSign); // 拖尾系数的个数

	int levelCnt = totalCoeffs - trailingOnes; // 其他普通非零系数的个数（除拖尾系数之外的）
	int *levels = new int[levelCnt]; // 定义一个数组保存普通非零系数（逆序存放）
	memset(levels, 0, sizeof(int)* levelCnt);
	get_levels(coeff, levels, levelCnt);

	int *runBefore = new int[totalCoeffs]; // 每个非0系数之前（之间）连续0的个数
	int *zerosLeft = new int[totalCoeffs]; // 每个非0系数之前总共有0的个数
	memset(runBefore, 0, sizeof(int)* totalCoeffs);
	memset(zerosLeft, 0, sizeof(int)* totalCoeffs);
	int totalZeros = get_totalzeros_runbefore(coeff, runBefore, zerosLeft, totalCoeffs); // 最后一个（最高频）非0系数之前总的0的个数

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

	// levels:普通非零系数(非拖尾系数的非零系数)
	int suffixLength = 0; // 后缀长度初始化： (TotalCoeff( coeff_token )小于或等于 10，或者TrailingOnes( coeff_token )等于3)，suffixLength等于0
	if (totalCoeffs > 10 && trailingOnes < 3) // （TotalCoeff( coeff_token )大于10，并且TrailingOnes( coeff_token )小于3），suffixLength等于1
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

	// totalZeros：编码totalZeros，根据表9-7/表9-8
	cavlcCode += totalZerosMap[totalZeros][totalCoeffs];

	// runBefore：编码runBefore，根据表9-10
	for (int idx = 0; idx < totalCoeffs - 1; idx++) // totalCoeffs - 1是因为，最低频的非零系数前面的0不需要编码
	{
		if (zerosLeft[idx] == 0) // 没有剩余的0系数需要编码时，不需要再继续进行编码
		{
			break;
		}
		cavlcCode += runBeforeMap[runBefore[idx]][zerosLeft[idx]];
	}

	// 删掉，防止内存泄漏
	delete[] levels;
	delete[] runBefore;
	delete[] zerosLeft;
	return cavlcCode;
}