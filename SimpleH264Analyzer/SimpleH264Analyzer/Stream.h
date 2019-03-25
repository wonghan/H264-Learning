#ifndef _STREAM_H_
#define _STREAM_H_

#include <vector> // vector Ϊ���Զ������Ķ�������

class CSeqParamSet;

class CStreamFile
{
public:
	CStreamFile(TCHAR *fileName); // ������ʾ�����ļ������ַ���
	~CStreamFile();

	// Open API
	int Parse_h264_bitstream();

private:
	FILE	*m_inputFile; //  �����ļ���ָ��
	TCHAR	*m_fileName; // ���������ļ��ļ���
	std::vector<UINT8> m_nalVec;

	void	file_info(); // ��ӡ�����ļ��ļ���
	void	file_error(int idx); // ��ӡ������Ϣ
	int		find_nal_prefix(); // ����NAL UNIT
	void	ebsp_to_sodb(); // EBSP תΪ RBSP,ȥ����ֹ����У���ֽ� 0x03

	void	dump_NAL_type(UINT8 nalType); // ���ƽ�������������̨����־�ļ���

	CSeqParamSet *m_sps;
};

#endif