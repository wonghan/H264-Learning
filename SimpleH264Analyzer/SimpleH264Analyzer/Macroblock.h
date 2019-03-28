#ifndef _MACROBLOCK_H_
#define _MACROBLOCK_H_

class CPicParamSet;

typedef struct IntraPredStruct // �ٷ��ĵ� 7.3.5.1 ���Ԥ���﷨
{
	UINT8 block_mode;	// block_mode ָ�� transformSize �ǰ��� 0:4x4 ���� 1:8x8
	bool prev_intra_pred_mode_flag;
	UINT8 rem_intra_pred_mode;
	IntraPredStruct() // �ṹ��Ĺ��캯��
	{
		block_mode = 0;
		prev_intra_pred_mode_flag = false;
		rem_intra_pred_mode = 0;
	}
} IntraPredStruct;

class CMacroblock
{
public:
	CMacroblock(UINT8 *pSODB, UINT32 offset, UINT32 idx);
	~CMacroblock();

	void Set_pic_param_set(CPicParamSet *pps);
	UINT32 Parse_macroblock();

private: 
	// �ٷ��ĵ� 7.3.5 �����﷨
	UINT8 *m_pSODB;
	UINT8 m_byteOffset;
	UINT8 m_bitOffset;
	UINT32 m_mbDataSize; // ��ǰ������ݵĴ�С

	CPicParamSet *m_pps_active;

	UINT32 m_mb_idx; // ��������ֵ����ǰ�����Slice�ڵĵڼ������
	UINT8 m_mb_type;
	bool  m_transform_size_8x8_flag;

	// mb_pred( mb_type )
	IntraPredStruct *m_pred_struct;
	UINT8 m_intra_chroma_pred_mode; // ��ʾɫ�ȷ�����֡��Ԥ��ģʽ

	UINT8 m_coded_block_pattern;
	UINT8 m_mb_qp_delta;

	UINT8 m_cbp_luma; // ��ʾ����cbp
	UINT8 m_cbp_chroma;// ��ʾɫ��cbp
};

#endif