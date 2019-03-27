#ifndef _SEQ_PARAM_SET_H_
#define _SEQ_PARAM_SET_H_

class CSeqParamSet
{
public:
	CSeqParamSet();
	~CSeqParamSet();

	// ���Ⱪ¶��˽�б������и�ֵ�ķ���
	void  Set_profile_level(UINT8 profile_idc, UINT8 level_idc);
	void  Set_sps_id(UINT8 spsID);
	void  Set_chroma_format_idc(UINT8 chromaFormatIdc);
	void  Set_bit_depth(UINT8 bit_depth_luma, UINT8 bit_depth_chroma);

	void  Set_max_frame_num(UINT32 maxFrameNum);
	void  Set_poc_type(UINT8 pocType);
	void  Set_max_poc_cnt(UINT32 maxPocCnt);
	void  Set_max_num_ref_frames(UINT32 maxRefFrames);
	void  Set_sps_multiple_flags(UINT32 flags);
	void  Set_pic_reslution_in_mbs(UINT16 widthInMBs, UINT16 heightInMapUnits);
	void  Set_frame_crop_offset(UINT32 offsets[4]);

	bool  Get_separate_colour_plane_flag(); // ���� separate_colour_plane_flag ���� SliceHeader �е� colour_plane_id ʹ��
	UINT32 Get_log2_max_frame_num(); // ���� log2_max_frame_num_minus4 + 4 ���� SliceHeader �е� frame_num ʹ��
	bool  Get_frame_mbs_only_flag(); // ���� frame_mbs_only_flag ���� SliceHeader �е� field_pic_flag ʹ��
	UINT8 Get_poc_type(); // ���� poc_type ���� SliceHeader �е� pic_order_cnt_lsb ʹ��
	UINT32 Get_log2_max_poc_cnt(); // // ���� log2_max_poc_cnt ���� SliceHeader �е� pic_order_cnt_lsb ʹ��

private: // �����ĵ������SPS��Ա
	UINT8 m_profile_idc; // profile_idc, ��ʾ profile ������ֵ
	UINT8 m_level_idc; // level_idc, ��ʾlevelֵ
	UINT8 m_sps_id; // seq_parameter_set_id, ��ʾ��ǰsps��id, pps�ṹ���Ը���sps_id�ҵ���Ӧ��sps�ṹ���������еĲ�����ȡֵ��ΧΪ0-31��

	// if( profile_idc == 100 || profile_idc == 110 || profile_idc == 122 || profile_idc == 144 )��     ���������²���
	UINT8 m_chroma_format_idc; // chroma_format_idc����ʾɫ�ȸ�ʽ����������ʾɫ�Ȳ�����������Ȳ����ı�����Ĭ��ֵΪ1����ʾ��4:2:0��ɫ��ģʽ��ȡֵ��Χ0-3��
	// if( chroma_format_idc = = 3 ){     ���������²���
	bool  m_separate_colour_plane_flag; // �����ĵ�д�� residual_colour_transform_flag, ��־λ��Ĭ��Ϊ0����ֵΪ1�����ʾ��4:4:4��ɫ��ģʽ����3��ͼ��ֱ���б��룻��Ϊ0����������ɫ��ͳһ���б���
	// ��
	UINT8 m_bit_depth_luma; // ���ȱ�����ȣ���ʾһ������ռ�ö��ٱ��أ�һ��ֵΪ8����Щprofile��ȡ10
	UINT8 m_bit_depth_chroma; // ɫ�ȱ�����ȣ���ʾһ������ռ�ö��ٱ��أ�һ��ֵΪ8����Щprofile��ȡ10
	bool  m_qpprime_y_zero_transform_bypass_flag; // ��־λ�����ڽ�ʡ���㸴�ӶȵĿ��ǣ�Ĭ��Ϊ0����ֵΪ1����ָ�� QP'Y ���� 0 ʱ�任ϵ��������̵ı任��·������ͼ�񹹽����̽�����ȥ��ЧӦ�˲�����֮ǰִ�У���ֵΪ0����ָ�任ϵ��������̺�ͼ�񹹽�������ȥ��ЧӦ�˲�����֮ǰִ�ж���ʹ�ñ任��·����
	bool  m_seq_scaling_matrix_present_flag; // ��־λ��һ�㲻�ã�Ĭ��Ϊ0����ֵΪ1����ʾ���滹���ĵ��е�������������ֵΪ0��������ĵ��е��������������ǡ�
	// }

	UINT32 m_log2_max_frame_num; // frame num�����ޣ��˴�����MaxFrameNum��ֵ��MaxFrameNum = 2^( log2_max_frame_num_minus4 + 4 ),����SPS�Ĳ��� log2_max_frame_num_minus4 ����ǰ��Ĺ�ʽ���ڵó�frame_num ��صı��� MaxFrameNum ��ֵ��log2_max_frame_num_minus4 �ġ�ȡֵ��Χ0-12��
	UINT8 m_poc_type; // pic_order_cnt_type��poc�����ͣ���ָ����ͼ��˳��ļ���������һ��Ϊ0��ֵΪ1�����á�ȡֵ��Χ0-2��
	// if( pic_order_cnt_type == 0 ){
	UINT32 m_log2_max_poc_cnt; // log2_max_pic_order_cnt_lsb_minus4����ʾpocͼ������ֵ�����ޣ���ʾͼ��˳������������еı��� MaxPicOrderCntLsb ��ֵ����ʽ MaxPicOrderCntLsb = 2^( log2_max_pic_order_cnt_lsb_minus4 + 4 ),log2_max_pic_order_cnt_lsb_minus4 �ġ�ȡֵ��Χ0-12��
	// }
	UINT32 m_max_num_ref_frames; // ����ο�֡���������
	bool m_gaps_in_frame_num_value_allowed_flag; // gaps_in_frame_num_value_allowed_flag��ÿһ֡�� frame num �Ƿ�������ֲ�����������������1����3�������
	UINT16 m_pic_width_in_mbs;	// ��ʾ�Ժ��Ϊ��Ԫ��ÿ������ͼ��Ŀ�ȣ���64*64��ͼ�񣬺��Ϊ16*16����ֵΪ4�����Ժ��Ϊ��Ԫ��ͼ���ȱ��� PicWidthInMbs = pic_width_in_mbs_minus1 + 1
	UINT16 m_pic_height_in_map_units;  // ��ʾ��������ӳ��Ϊ��λ��һ������֡�򳡵ĸ߶�,����64*64��ͼ�񣬺��Ϊ16*16����ֵΪ4�������� PicHeightInMapUnits = pic_height_in_map_units_minus1 + 1
	UINT16 m_pic_height_in_mbs; // �Զ����ֵ����ʾͼ��ʵ�ʸ߶�
	bool m_frame_mbs_only_flag; // ֵΪ1��ʾ��֡���룬ֵΪ0��ʾ������֡����򳡱���
	bool m_mb_adaptive_frame_field_flag; // ��ʾ�Ƿ���ú�鼶���֡������Ӧ���������Ƿ���Ը��������֡����򳡱�����л���Ĭ��Ϊ0��0��ʾ���л���1��ʾ���ܻ�����л�
	bool m_direct_8x8_inference_flag; // ��ʾ�� B_Skip��B_Direct_16x16 �� B_Direct_8x8 �����˶�ʸ���ļ������ʹ�õķ���
	bool m_frame_cropping_flag; // ���� 1 ��ʾ֡����ƫ�Ʋ��������Ƶ���в������е���һ��ֵ, ���� 0 ��ʾ������֡����ƫ�Ʋ���
	// if(frame_cropping_flag){
	UINT32 m_frame_crop_offset[4]; // ��4��ƫ�����Ž�������
	// }
	bool m_vui_parameters_present_flag; // ����1 ��ʾ���� vui_parameters() �﷨�ṹ,���� 0 ��ʾ������ vui_parameters() �﷨�ṹ

};

#endif


