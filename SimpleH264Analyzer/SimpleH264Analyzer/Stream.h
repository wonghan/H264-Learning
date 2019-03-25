#ifndef _STREAM_H_
#define _STREAM_H_

#include <vector> // vector 为可自动增长的对象数组

class CSeqParamSet;

class CStreamFile
{
public:
	CStreamFile(TCHAR *fileName); // 参数表示传入文件名的字符串
	~CStreamFile();

	// Open API
	int Parse_h264_bitstream();

private:
	FILE	*m_inputFile; //  输入文件的指针
	TCHAR	*m_fileName; // 保存输入文件文件名
	std::vector<UINT8> m_nalVec;

	void	file_info(); // 打印输入文件文件名
	void	file_error(int idx); // 打印错误信息
	int		find_nal_prefix(); // 解析NAL UNIT
	void	ebsp_to_sodb(); // EBSP 转为 RBSP,去掉防止竞争校验字节 0x03

	void	dump_NAL_type(UINT8 nalType); // 控制将结果输出到控制台或日志文件中

	CSeqParamSet *m_sps;
};

#endif