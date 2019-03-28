#ifndef _MACROBLOCK_H_
#define _MACROBLOCK_H_

class CPicParamSet;

typedef struct IntraPredStruct // 官方文档 7.3.5.1 宏块预测语法
{
	UINT8 block_mode;	// block_mode 指定 transformSize 是按照 0:4x4 还是 1:8x8
	bool prev_intra_pred_mode_flag;
	UINT8 rem_intra_pred_mode;
	IntraPredStruct() // 结构体的构造函数
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
	// 官方文档 7.3.5 宏块层语法
	UINT8 *m_pSODB;
	UINT8 m_byteOffset;
	UINT8 m_bitOffset;
	UINT32 m_mbDataSize; // 当前宏块数据的大小

	CPicParamSet *m_pps_active;

	UINT32 m_mb_idx; // 宏块的索引值，当前宏块是Slice内的第几个宏块
	UINT8 m_mb_type;
	bool  m_transform_size_8x8_flag;

	// mb_pred( mb_type )
	IntraPredStruct *m_pred_struct;
	UINT8 m_intra_chroma_pred_mode; // 表示色度分量的帧内预测模式

	UINT8 m_coded_block_pattern;
	UINT8 m_mb_qp_delta;

	UINT8 m_cbp_luma; // 表示亮度cbp
	UINT8 m_cbp_chroma;// 表示色度cbp
};

#endif