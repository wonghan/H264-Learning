#include "stdafx.h"
#include "Stream.h"
#include "NALUnit.h" 
#include "SeqParamSet.h"
#include "PicParamSet.h"
#include "I_Slice.h"

#include <iostream>

using namespace std;

// 构造函数
CStreamFile::CStreamFile(TCHAR *fileName)
{
	m_fileName = fileName;
	m_sps = NULL;
	m_pps = NULL;
	m_IDRSlice = NULL;

	file_info();
	_tfopen_s(&m_inputFile, m_fileName, _T("rb")); // 打开输入的文件，以只读2进制格式打开
	if (m_inputFile == NULL) // 如果打开失败
	{
		file_error(0);
	}

#if TRACE_CONFIG_LOGOUT
	g_traceFile.open(L"trace.txt");
	if (!g_traceFile.is_open())
	{
		file_error(1);
	}
#endif
}

// 析构函数
CStreamFile::~CStreamFile()
{
	if (m_inputFile != NULL)
	{
		fclose(m_inputFile);
		m_inputFile = NULL;
	}

	if (m_sps != NULL){
		delete m_sps;
		m_sps = NULL;
	}

	if (m_pps != NULL){
		delete m_pps;
		m_pps = NULL;
	}

	if (m_IDRSlice != NULL)
	{
		delete m_IDRSlice;
		m_IDRSlice = NULL;
	}

#if TRACE_CONFIG_LOGOUT
	if (g_traceFile.is_open())
	{
		g_traceFile.close();
	}
#endif
}

void CStreamFile::file_info()
{
	if (m_fileName) // 若输入文件文件名非空
	{
		wcout << L"File name: " << m_fileName << endl; // 输出一条日志信息，显示输入文件文件名
	}
}

void CStreamFile::file_error(int idx)
{
	switch (idx)
	{
	case 0:
		wcout << L"Error: opening input file failed." << endl;
		break;
	case 1:
		wcout << L"Error: opening trace file failed." << endl;
		break;
	default:
		break;
	}
}

int CStreamFile::Parse_h264_bitstream()
{
	int ret = 0;
	do
	{
		ret = find_nal_prefix();
		// 解析NAL UNIT
		// 提取NAL Header信息（NAL Header在第1个字节，即在m_nalVec[0]）
		if (m_nalVec.size()) // 若 m_nalVec 数据不为空
		{
			UINT8 nalType = m_nalVec[0] & 0x1f; //提取后5位，即 nal_unit_type 的信息
			dump_NAL_type(nalType);
			ebsp_to_sodb();
			CNALUnit nalUnit(&m_nalVec[1], m_nalVec.size() - 1); // 传入 NAL Body 信息

			switch (nalType)
			{
			case 5:
				// 解析 IDR NAL
				if (m_IDRSlice)
				{
					delete m_IDRSlice;
					m_IDRSlice = NULL;
				}
				m_IDRSlice = new I_Slice(nalUnit.Get_SODB(), m_sps, m_pps, nalType);
				m_IDRSlice->Parse();
				break;
			case 7:
				// 解析 SPS NAL
				if (m_sps) // m_sps非空，表示前面有sps，需更新sps
				{
					delete m_sps;
				}

				m_sps = new CSeqParamSet;
				nalUnit.Parse_as_seq_param_set(m_sps); // 解析并将结果赋值给 m_sps
				break;
			case 8:
				// 解析 PPS NAL
				if (m_pps) // m_pps非空，表示前面有pps，需更新pps
				{
					delete m_pps;
				}

				m_pps = new CPicParamSet;
				nalUnit.Parse_as_pic_param_set(m_pps); // 解析并将结果赋值给 m_pps
				break;
			default:
				break;
			}
		}
	} while (ret);
	return 0;
}

int CStreamFile::find_nal_prefix() // 解析NAL UNIT
{
	// 寻找 00 00 00 01 xx xx xx xx xx xx 00 00 00 01
	UINT8 prefix[3] = { 0 }; // 定义长度为3的数组，里面的每一个值都为0
	UINT8 fileByte;

	m_nalVec.clear();

	int pos = 0, getPrefix = 0;// 标志位默认为0
	for (int idx = 0; idx < 3; idx++){
		prefix[idx] = getc(m_inputFile); // 从文件指针stream指向的文件流中读取一个字符，并把它作为函数值返回给整型变量ch，并把位置标识符往前移动
		m_nalVec.push_back(prefix[idx]); // 在vector类中作用为在vector尾部加入一个数据
	}

	while (!feof(m_inputFile)){ // 若文件未结束
		if ((prefix[pos % 3] == 0) && (prefix[(pos + 1) % 3] == 0) && (prefix[(pos + 2) % 3] == 1)){
			// 找到 0x 00 00 01
			getPrefix = 1; // 标志位设为1
			m_nalVec.pop_back(); // 因为0x 00 00 01为起止位，不是数据，因此将 00 00 01 移出
			m_nalVec.pop_back();
			m_nalVec.pop_back();
			break;
		}
		else if ((prefix[pos % 3] == 0) && (prefix[(pos + 1) % 3] == 0) && (prefix[(pos + 2) % 3] == 0)){
			if (getc(m_inputFile) == 1){
				// 找到 0x 00 00 00 01
				getPrefix = 2; // 标志位设为2
				m_nalVec.pop_back(); // 因为0x 00 00 00 01为起止位，不是数据，因此将 00 00 00 移出（01没存进去）
				m_nalVec.pop_back();
				m_nalVec.pop_back();
				break;
			}
		}
		else{
			fileByte = getc(m_inputFile);
			prefix[(pos++) % 3] = fileByte;
			m_nalVec.push_back(fileByte);
		}
	}

	return getPrefix;
}

void CStreamFile::ebsp_to_sodb() // 去掉防止竞争校验字节 0x03
{
	// 0x 00 00 03 -> 0x 00 00
	if (m_nalVec.size() < 3)
	{
		return;
	}
	for (vector<UINT8>::iterator itor = m_nalVec.begin() + 2; itor != m_nalVec.end(); )
	{
		if ((*itor == 3) && (*(itor - 1) == 0) && (*(itor - 2) == 0))
		{
			vector<UINT8>::iterator temp = m_nalVec.erase(itor); // 定义一个临时迭代器，保存删除之后的下一个元素
			itor = temp;
		}
		else
		{
			itor++;
		}
	}
}

void CStreamFile::dump_NAL_type(UINT8 nalType)
{
#if TRACE_CONFIG_CONSOLE // 在控制台输出
	wcout << L"NAL Unit Type: " << nalType << endl;
#endif

#if TRACE_CONFIG_LOGOUT // 在日志文件中输出
	g_traceFile << "NAL Unit Type: " << to_string(nalType) << endl;
#endif
}