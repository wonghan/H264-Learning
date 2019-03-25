#pragma once

class CSeqParamSet;
class CNALUnit
{
public:
	CNALUnit(UINT8 *pSODB, UINT32 SODBLength);
	~CNALUnit();

	int Parse_as_seq_param_set(CSeqParamSet *sps);

private:
	UINT8	*m_pSODB;
	UINT32	m_SODBLength;
};

