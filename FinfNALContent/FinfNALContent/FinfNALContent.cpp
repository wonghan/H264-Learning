// FinfNALContent.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

typedef unsigned char uint8;
using namespace std;

static int find_nal_prefix(FILE **pFileIn, vector<uint8> &nalBytes){

	FILE *pFile = *pFileIn;

	// 寻找 00 00 00 01 xx xx xx xx xx xx 00 00 00 01
	uint8 prefix[3] = { 0 }; // 定义长度为3的数组，里面的每一个值都为0
	uint8 fileByte;

	nalBytes.clear();

	int pos = 0, getPrefix = 0;
	for(int idx = 0; idx < 3; idx++){
		prefix[idx] = getc(pFile);
		nalBytes.push_back(prefix[idx]);
	}

	while (!feof(pFile)){
		if ((prefix[pos % 3] == 0) && (prefix[(pos + 1) % 3] == 0) && (prefix[(pos + 2) % 3] == 1)){
			// 找到 0x 00 00 01
			getPrefix = 1;
			nalBytes.pop_back();
			nalBytes.pop_back();
			nalBytes.pop_back();
			break;
		}
		else if((prefix[pos % 3] == 0) && (prefix[(pos + 1) % 3] == 0) && (prefix[(pos + 2) % 3] == 0)){
			if (getc(pFile) == 1){
				// 找到 0x 00 00 00 01
				getPrefix = 2;
				nalBytes.pop_back();
				nalBytes.pop_back();
				nalBytes.pop_back();
				break;
			}
		}
		else{
			fileByte = getc(pFile);
			prefix[(pos++) % 3] = fileByte;
			nalBytes.push_back(fileByte);
		}
	}

	return getPrefix;
}


int _tmain(int argc, _TCHAR* argv[])
{
	// 打开文件
	FILE *pFile_in = NULL; // 设置指针为空

	_tfopen_s(&pFile_in, argv[1], _T("rb"));

	// 若文件为空，报错
	if (!pFile_in){
		printf("Error: Opening input file failed.\n");
	}


	vector<uint8> nalBytes;

	find_nal_prefix(&pFile_in, nalBytes);
	find_nal_prefix(&pFile_in, nalBytes);

	for (int idx = 0; idx < nalBytes.size(); idx++){
		printf("%x ", nalBytes.at(idx));
	}
	printf("\n");

	// fclose(pFile_in);
	return 0;
}

