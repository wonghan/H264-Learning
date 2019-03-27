#pragma once

class CSeqParamSet;
class CPicParamSet;
class CNALUnit
{
public:
	CNALUnit(UINT8 *pSODB, UINT32 SODBLength);
	~CNALUnit();

	int Parse_as_seq_param_set(CSeqParamSet *sps);
	int Parse_as_pic_param_set(CPicParamSet *pps);

	UINT8* Get_SODB(); // 获取保存数据的指针

private:
	UINT8	*m_pSODB;
	UINT32	m_SODBLength;
};

