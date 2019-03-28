#include "stdafx.h"
#include "I_Slice.h"
#include "SliceHeader.h"
#include "SeqParamSet.h"
#include "Macroblock.h"

I_Slice::I_Slice(UINT8 *pSODB, CSeqParamSet *sps, CPicParamSet *pps, UINT8 nalType)
{
	memset(this, 0, sizeof(I_Slice)); // 置零
	m_pSODB = pSODB;
	m_sps_active = sps;
	m_pps_active = pps;
	m_nalType = nalType;

	m_max_mb_number = m_sps_active->Get_pic_width_in_mbs() * m_sps_active->Get_pic_height_in_mbs(); // 记录当前Slice的宏块个数

	m_macroblocks = new CMacroblock *[m_max_mb_number];
	memset(m_macroblocks, NULL, m_max_mb_number * sizeof(CMacroblock *));
}

I_Slice::~I_Slice()
{
	if (m_sliceHeader != NULL)
	{
		delete m_sliceHeader;
		m_sliceHeader = NULL;
	}

	if (m_macroblocks) // 删除宏块的数组
	{
		for (int idx = 0; idx < m_max_mb_number; idx++)
		{
			if (m_macroblocks[idx])
			{
				delete m_macroblocks[idx];
				m_macroblocks[idx] = NULL;
			}
		}

		delete m_macroblocks;
		m_macroblocks = NULL;
	}
}

UINT32 I_Slice::Parse()
{
	UINT32 sliceHeaderLength = 0, macroblockOffset = 0; // 表示宏块在数据缓存区里面的偏移量
	m_sliceHeader = new CSliceHeader(m_pSODB, m_sps_active, m_pps_active, m_nalType);
	macroblockOffset = sliceHeaderLength = m_sliceHeader->Parse_slice_header();

	for (int idx = 0; idx < m_max_mb_number; idx)
	{
		m_macroblocks[idx] = new CMacroblock(m_pSODB, macroblockOffset, idx);
		m_macroblocks[idx]->Set_pic_param_set(m_pps_active); // 设置当前Slice的pps
		macroblockOffset += m_macroblocks[idx]->Parse_macroblock();

		break; // 暂时解析Slice的第一部分，待修改
	}

	return 0;
}