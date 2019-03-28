#include "stdafx.h"
#include "PicParamSet.h"
#include "Macroblock.h"

CMacroblock::CMacroblock(UINT8 *pSODB, UINT32 offset, UINT32 idx)
{
	m_pSODB = pSODB;
	m_byteOffset = offset / 8;
	m_bitOffset = offset % 8;

	m_mb_idx = idx;
	m_mbDataSize = offset;

	// 初始化
	m_pps_active = NULL;
	m_mb_type = 0;
	m_transform_size_8x8_flag = false;
	m_pred_struct = NULL;
	m_intra_chroma_pred_mode = 0;

	m_coded_block_pattern = 0;
	m_mb_qp_delta = 0;

	m_cbp_luma = 0;
	m_cbp_chroma = 0;
}

CMacroblock::~CMacroblock()
{
	if (m_pred_struct)
	{
		delete[] m_pred_struct;
		m_pred_struct = NULL;
	}
}

void CMacroblock::Set_pic_param_set(CPicParamSet *pps)
{
	m_pps_active = pps;
}

UINT32 CMacroblock::Parse_macroblock()
{
	m_mb_type = Get_uev_code_num(m_pSODB, m_byteOffset, m_bitOffset);
	if (m_mb_type == 25)
	{
		// I-PCM MODE...
	}
	else if (m_mb_type == 0)
	{
		// Intra_NxN
		if (m_pps_active->Get_transform_8x8_mode_flag())
		{
			m_transform_size_8x8_flag = Get_bit_at_position(m_pSODB, m_byteOffset, m_bitOffset);
		}

		if (m_transform_size_8x8_flag) // mb_pred(mb_type)
		{
			// 使用 intra8x8 模式
			m_pred_struct = new IntraPredStruct[4]; // 1个 16x16 的宏块包含 4 个 8x8
			for (int luma8x8BlkIdx = 0; luma8x8BlkIdx < 4; luma8x8BlkIdx++)
			{
				m_pred_struct[luma8x8BlkIdx].block_mode = 0; // 使用 4x4 模式
				m_pred_struct[luma8x8BlkIdx].prev_intra_pred_mode_flag = Get_bit_at_position(m_pSODB, m_byteOffset, m_bitOffset);
				if (!m_pred_struct[luma8x8BlkIdx].prev_intra_pred_mode_flag)
				{
					m_pred_struct[luma8x8BlkIdx].rem_intra_pred_mode = Get_uint_code_num(m_pSODB, m_byteOffset, m_bitOffset, 3);
				}
			}
		}
		else
		{
			// 使用 intra4x4 模式
			m_pred_struct = new IntraPredStruct[16];  // 1个 16x16 的宏块包含 16 个 4x4
			for (int luma4x4BlkIdx = 0; luma4x4BlkIdx < 16; luma4x4BlkIdx++)
			{
				m_pred_struct[luma4x4BlkIdx].block_mode = 0; // 使用 8x8 模式
				m_pred_struct[luma4x4BlkIdx].prev_intra_pred_mode_flag = Get_bit_at_position(m_pSODB, m_byteOffset, m_bitOffset);
				if (!m_pred_struct[luma4x4BlkIdx].prev_intra_pred_mode_flag)
				{
					m_pred_struct[luma4x4BlkIdx].rem_intra_pred_mode = Get_uint_code_num(m_pSODB, m_byteOffset, m_bitOffset, 3);
				}
			}
		}

		// intra_chroma_pred_mode
		m_intra_chroma_pred_mode = Get_uev_code_num(m_pSODB, m_byteOffset, m_bitOffset);
	}
	else
	{
		// Intra_16x16...
	}

	if (m_mb_type == 0 || m_mb_type == 25) // 非 Intra_16x16 模式，即 Intra_NxN 或 I_PCM 模式
	{
		m_coded_block_pattern = Get_mev_code_num(m_pSODB, m_byteOffset, m_bitOffset);
		m_cbp_luma = m_coded_block_pattern % 16; // CodedBlockPatternLuma = coded_block_pattern % 16
		m_cbp_chroma = m_coded_block_pattern / 16; // CodedBlockPatternChroma = coded_block_pattern / 16
	}

	if (m_cbp_luma > 0 || m_cbp_chroma > 0 || (m_mb_type > 0 && m_mb_type < 25))
	{
		m_mb_qp_delta = Get_sev_code_num(m_pSODB, m_byteOffset, m_bitOffset);
	}

	m_mbDataSize = m_byteOffset * 8 + m_bitOffset - m_mbDataSize; // 当前宏块在二进制码流中所占的体积

	return m_mbDataSize;
}