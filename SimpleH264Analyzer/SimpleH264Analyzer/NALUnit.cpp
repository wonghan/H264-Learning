#include "stdafx.h"
#include "NALUnit.h"
#include "SeqParamSet.h"


CNALUnit::CNALUnit(UINT8 *pSODB, UINT32 SODBLength)
{
	m_pSODB = pSODB;
	m_SODBLength = SODBLength;
}


CNALUnit::~CNALUnit()
{
}

int CNALUnit::Parse_as_seq_param_set(CSeqParamSet *sps) // 解析序列参数集（SPS）
{
	// 定义参数，参数的意思详见 SeqParamSet.h
	UINT8 profile_idc = 0; // 第一个字节
	UINT8 level_idc = 0; // 第三个字节
	UINT8 sps_id = 0; // 格式是无符号的指数哥伦布编码
	
	UINT8 chroma_format_idc = 0; // 表示色度采样相对于亮度采样的比例，默认值为1，表示（4:2 : 0）色度模式【取值范围0 - 3】
	bool  separate_colour_plane_flag = 0;
	UINT8 bit_depth_luma = 0;
	UINT8 bit_depth_chroma = 0;
	bool  qpprime_y_zero_transform_bypass_flag = 0;
	bool  seq_scaling_matrix_present_flag = 0;


	UINT32 max_frame_num = 0;
	UINT8  poc_type = 0;
	UINT32 max_poc_cnt = 0;
	UINT32 max_num_ref_frames = 0;
	bool   gaps_in_frame_num_value_allowed_flag = 0;
	UINT16 pic_width_in_mbs = 0;
	UINT16 pic_height_in_map_units = 0;
	UINT16 pic_height_in_mbs = 0;
	bool  frame_mbs_only_flag = 0;
	bool  mb_adaptive_frame_field_flag = 0;
	bool  direct_8x8_inference_flag = 0;
	bool  frame_cropping_flag = 0;
	UINT32 frame_crop_offset[4] = { 0 };
	bool  vui_parameters_present_flag = 0;


	//
	UINT8 bytePosition = 3, bitPosition = 0;
	UINT32 flags = 0; // 用于存储标志位

	profile_idc = m_pSODB[0];
	level_idc = m_pSODB[2];
	sps_id = Get_uev_code_num(m_pSODB, bytePosition, bitPosition);
	if ( profile_idc == 100 || profile_idc == 110 || profile_idc == 122 || profile_idc == 244 || profile_idc == 44 || profile_idc == 83 || profile_idc == 86 || profile_idc == 118 || profile_idc == 128)
	{
		chroma_format_idc = Get_uev_code_num(m_pSODB, bytePosition, bitPosition);
		if (chroma_format_idc == 3)
		{
			separate_colour_plane_flag = Get_bit_at_position(m_pSODB, bytePosition, bitPosition);
			flags |= (separate_colour_plane_flag << 21); // 整合到flag中
		}
		bit_depth_luma = Get_uev_code_num(m_pSODB, bytePosition, bitPosition) + 8;
		bit_depth_chroma = Get_uev_code_num(m_pSODB, bytePosition, bitPosition) + 8;

		qpprime_y_zero_transform_bypass_flag = Get_bit_at_position(m_pSODB, bytePosition, bitPosition);
		flags |= (qpprime_y_zero_transform_bypass_flag << 20); // 整合到flag中

		seq_scaling_matrix_present_flag = Get_bit_at_position(m_pSODB, bytePosition, bitPosition);
		flags |= (seq_scaling_matrix_present_flag << 19); // 整合到flag中
		if (seq_scaling_matrix_present_flag)
		{
			return -1; // 待补充，补充内容参见H.264官方手册
		}
	}
	
	max_frame_num = 1 << (Get_uev_code_num(m_pSODB, bytePosition, bitPosition) + 4); // MaxFrameNum = 2^( log2_max_frame_num_minus4 + 4 )
	poc_type = Get_uev_code_num(m_pSODB, bytePosition, bitPosition);
	if (poc_type == 0)
	{
		max_poc_cnt = 1 << (Get_uev_code_num(m_pSODB, bytePosition, bitPosition) + 4);
	}
	else
	{
		return -1; // 待补充，补充内容参见H.264官方手册
	}

	max_num_ref_frames = Get_uev_code_num(m_pSODB, bytePosition, bitPosition);
	gaps_in_frame_num_value_allowed_flag = Get_bit_at_position(m_pSODB, bytePosition, bitPosition);
	flags |= (gaps_in_frame_num_value_allowed_flag << 5); // 整合到flag中

	pic_width_in_mbs = Get_uev_code_num(m_pSODB, bytePosition, bitPosition) + 1;
	pic_height_in_map_units = Get_uev_code_num(m_pSODB, bytePosition, bitPosition) + 1;
	frame_mbs_only_flag = Get_bit_at_position(m_pSODB, bytePosition, bitPosition);
	flags |= (frame_mbs_only_flag << 4); // 整合到flag中
	if (!frame_mbs_only_flag)
	{
		mb_adaptive_frame_field_flag = Get_bit_at_position(m_pSODB, bytePosition, bitPosition);
		flags |= (frame_mbs_only_flag << 3); // 整合到flag中
	}

	direct_8x8_inference_flag = Get_bit_at_position(m_pSODB, bytePosition, bitPosition);
	flags |= (direct_8x8_inference_flag << 2); // 整合到flag中
	frame_cropping_flag = Get_bit_at_position(m_pSODB, bytePosition, bitPosition);
	flags |= (frame_cropping_flag << 1); // 整合到flag中
	if (frame_cropping_flag)
	{
		for (int idx = 0; idx < 4; idx++)
		{
			frame_crop_offset[idx] = Get_uev_code_num(m_pSODB, bytePosition, bitPosition);
		}
	}
	vui_parameters_present_flag = Get_bit_at_position(m_pSODB, bytePosition, bitPosition);
	flags |= (vui_parameters_present_flag << 0); // 整合到flag中
	if (vui_parameters_present_flag)
	{
		return -1; // 待补充，补充内容参见H.264官方手册
	}

	sps->Set_profile_level(profile_idc, level_idc);
	sps->Set_sps_id(sps_id);
	sps->Set_chroma_format_idc(chroma_format_idc);
	sps->Set_bit_depth(bit_depth_luma, bit_depth_chroma);
	sps->Set_max_frame_num(max_frame_num);
	sps->Set_poc_type(poc_type);
	sps->Set_max_poc_cnt(max_poc_cnt);
	sps->Set_max_num_ref_frames(max_num_ref_frames);
	sps->Set_sps_multiple_flags(flags);
	sps->Set_pic_reslution_in_mbs(pic_width_in_mbs, pic_height_in_map_units);
	if (frame_cropping_flag)
	{
		sps->Set_frame_crop_offset(frame_crop_offset);
	}

	return 0;
}