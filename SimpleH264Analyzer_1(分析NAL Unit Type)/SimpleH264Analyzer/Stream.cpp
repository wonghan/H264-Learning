#include "stdafx.h"
#include "Stream.h"
#include "NALUnit.h" 

#include <iostream>

using namespace std;

// ���캯��
CStreamFile::CStreamFile(TCHAR *fileName)
{
	m_fileName = fileName;
	file_info();
	_tfopen_s(&m_inputFile, m_fileName, _T("rb")); // ��������ļ�����ֻ��2���Ƹ�ʽ��
	if (m_inputFile == NULL) // �����ʧ��
	{
		file_error(0);
	}
}

// ��������
CStreamFile::~CStreamFile()
{
	if (m_inputFile != NULL)
	{
		fclose(m_inputFile);
		m_inputFile = NULL;
	}
}

void CStreamFile::file_info()
{
	if (m_fileName) // �������ļ��ļ����ǿ�
	{
		wcout << L"File name: " << m_fileName << endl; // ���һ����־��Ϣ����ʾ�����ļ��ļ���
	}
}

void CStreamFile::file_error(int idx)
{
	switch (idx)
	{
	case 0:
		wcout << L"Error: opening input file failed." << endl;
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
		// ����NAL UNIT
		// ��ȡNAL Header��Ϣ��NAL Header�ڵ�1���ֽڣ�����m_nalVec[0]��
		if (m_nalVec.size()) // �� m_nalVec ���ݲ�Ϊ��
		{
			uint8 nalType = m_nalVec[0] & 0x1f; //��ȡ��5λ���� nal_unit_type ����Ϣ
			wcout << L"NAL Unit Type: " << nalType << endl;
			ebsp_to_sodb();
			CNALUnit nalUnit(&m_nalVec[1], m_nalVec.size() - 1); // ���� NAL Body ��Ϣ
		}
	} while (ret);
	return 0;
}

int CStreamFile::find_nal_prefix() // ����NAL UNIT
{
	// Ѱ�� 00 00 00 01 xx xx xx xx xx xx 00 00 00 01
	uint8 prefix[3] = { 0 }; // ���峤��Ϊ3�����飬�����ÿһ��ֵ��Ϊ0
	uint8 fileByte;

	m_nalVec.clear();

	int pos = 0, getPrefix = 0;// ��־λĬ��Ϊ0
	for (int idx = 0; idx < 3; idx++){
		prefix[idx] = getc(m_inputFile); // ���ļ�ָ��streamָ����ļ����ж�ȡһ���ַ�����������Ϊ����ֵ���ظ����ͱ���ch������λ�ñ�ʶ����ǰ�ƶ�
		m_nalVec.push_back(prefix[idx]); // ��vector��������Ϊ��vectorβ������һ������
	}

	while (!feof(m_inputFile)){ // ���ļ�δ����
		if ((prefix[pos % 3] == 0) && (prefix[(pos + 1) % 3] == 0) && (prefix[(pos + 2) % 3] == 1)){
			// �ҵ� 0x 00 00 01
			getPrefix = 1; // ��־λ��Ϊ1
			m_nalVec.pop_back(); // ��Ϊ0x 00 00 01Ϊ��ֹλ���������ݣ���˽� 00 00 01 �Ƴ�
			m_nalVec.pop_back();
			m_nalVec.pop_back();
			break;
		}
		else if ((prefix[pos % 3] == 0) && (prefix[(pos + 1) % 3] == 0) && (prefix[(pos + 2) % 3] == 0)){
			if (getc(m_inputFile) == 1){
				// �ҵ� 0x 00 00 00 01
				getPrefix = 2; // ��־λ��Ϊ2
				m_nalVec.pop_back(); // ��Ϊ0x 00 00 00 01Ϊ��ֹλ���������ݣ���˽� 00 00 00 �Ƴ���01û���ȥ��
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

void CStreamFile::ebsp_to_sodb() // ȥ����ֹ����У���ֽ� 0x03
{
	// 0x 00 00 03 -> 0x 00 00
	if (m_nalVec.size() < 3)
	{
		return;
	}
	for (vector<uint8>::iterator itor = m_nalVec.begin() + 2; itor != m_nalVec.end(); )
	{
		if ((*itor == 3) && (*(itor - 1) == 0) && (*(itor - 2) == 0))
		{
			vector<uint8>::iterator temp = m_nalVec.erase(itor); // ����һ����ʱ������������ɾ��֮�����һ��Ԫ��
			itor = temp;
		}
		else
		{
			itor++;
		}
	}
}