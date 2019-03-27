#ifndef _SEQ_PARAM_SET_H_
#define _SEQ_PARAM_SET_H_

class CSeqParamSet
{
public:
	CSeqParamSet();
	~CSeqParamSet();

	// 对外暴露对私有变量进行赋值的方法
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

	bool  Get_separate_colour_plane_flag(); // 返回 separate_colour_plane_flag ，给 SliceHeader 中的 colour_plane_id 使用
	UINT32 Get_log2_max_frame_num(); // 返回 log2_max_frame_num_minus4 + 4 ，给 SliceHeader 中的 frame_num 使用
	bool  Get_frame_mbs_only_flag(); // 返回 frame_mbs_only_flag ，给 SliceHeader 中的 field_pic_flag 使用
	UINT8 Get_poc_type(); // 返回 poc_type ，给 SliceHeader 中的 pic_order_cnt_lsb 使用
	UINT32 Get_log2_max_poc_cnt(); // // 返回 log2_max_poc_cnt ，给 SliceHeader 中的 pic_order_cnt_lsb 使用

private: // 根据文档，添加SPS成员
	UINT8 m_profile_idc; // profile_idc, 表示 profile 的索引值
	UINT8 m_level_idc; // level_idc, 表示level值
	UINT8 m_sps_id; // seq_parameter_set_id, 表示当前sps的id, pps结构可以根据sps_id找到相应的sps结构，引用其中的参数【取值范围为0-31】

	// if( profile_idc == 100 || profile_idc == 110 || profile_idc == 122 || profile_idc == 144 )｛     ，则有以下参数
	UINT8 m_chroma_format_idc; // chroma_format_idc，表示色度格式的索引，表示色度采样相对于亮度采样的比例，默认值为1，表示（4:2:0）色度模式【取值范围0-3】
	// if( chroma_format_idc = = 3 ){     ，则有以下参数
	bool  m_separate_colour_plane_flag; // 中文文档写作 residual_colour_transform_flag, 标志位，默认为0。若值为1，则表示（4:4:4）色度模式当作3个图像分别进行编码；若为0，则亮度与色度统一进行编码
	// ｝
	UINT8 m_bit_depth_luma; // 亮度比特深度，表示一个像素占用多少比特，一般值为8，有些profile会取10
	UINT8 m_bit_depth_chroma; // 色度比特深度，表示一个像素占用多少比特，一般值为8，有些profile会取10
	bool  m_qpprime_y_zero_transform_bypass_flag; // 标志位，用于节省运算复杂度的考虑，默认为0。若值为1，是指当 QP'Y 等于 0 时变换系数解码过程的变换旁路操作和图像构建过程将会在去块效应滤波过程之前执行；若值为0，是指变换系数解码过程和图像构建过程在去块效应滤波过程之前执行而不使用变换旁路操作
	bool  m_seq_scaling_matrix_present_flag; // 标志位，一般不用，默认为0。若值为1，表示后面还有文档中的其他参数，若值为0，后面的文档中的其他参数不考虑。
	// }

	UINT32 m_log2_max_frame_num; // frame num的上限，此处保存MaxFrameNum的值，MaxFrameNum = 2^( log2_max_frame_num_minus4 + 4 ),其中SPS的参数 log2_max_frame_num_minus4 按照前面的公式用于得出frame_num 相关的变量 MaxFrameNum 的值，log2_max_frame_num_minus4 的【取值范围0-12】
	UINT8 m_poc_type; // pic_order_cnt_type，poc的类型，是指解码图像顺序的计数方法，一般为0，值为1不常用【取值范围0-2】
	// if( pic_order_cnt_type == 0 ){
	UINT32 m_log2_max_poc_cnt; // log2_max_pic_order_cnt_lsb_minus4，表示poc图像索引值的上限，表示图像顺序数解码过程中的变量 MaxPicOrderCntLsb 的值，公式 MaxPicOrderCntLsb = 2^( log2_max_pic_order_cnt_lsb_minus4 + 4 ),log2_max_pic_order_cnt_lsb_minus4 的【取值范围0-12】
	// }
	UINT32 m_max_num_ref_frames; // 允许参考帧的最大数量
	bool m_gaps_in_frame_num_value_allowed_flag; // gaps_in_frame_num_value_allowed_flag，每一帧的 frame num 是否允许出现不连续的情况，比如从1跳到3这种情况
	UINT16 m_pic_width_in_mbs;	// 表示以宏块为单元的每个解码图像的宽度（如64*64的图像，宏块为16*16，此值为4），以宏块为单元的图像宽度变量 PicWidthInMbs = pic_width_in_mbs_minus1 + 1
	UINT16 m_pic_height_in_map_units;  // 表示以条带组映射为单位的一个解码帧或场的高度,（如64*64的图像，宏块为16*16，此值为4），变量 PicHeightInMapUnits = pic_height_in_map_units_minus1 + 1
	UINT16 m_pic_height_in_mbs; // 自定义的值，表示图像实际高度
	bool m_frame_mbs_only_flag; // 值为1表示是帧编码，值为0表示可能是帧编码或场编码
	bool m_mb_adaptive_frame_field_flag; // 表示是否采用宏块级别的帧场自适应，即宏块间是否可以根据情况在帧编码或场编码间切换，默认为0，0表示无切换，1表示可能会进行切换
	bool m_direct_8x8_inference_flag; // 表示在 B_Skip、B_Direct_16x16 和 B_Direct_8x8 亮度运动矢量的计算过程使用的方法
	bool m_frame_cropping_flag; // 等于 1 表示帧剪切偏移参数遵从视频序列参数集中的下一个值, 等于 0 表示不存在帧剪切偏移参数
	// if(frame_cropping_flag){
	UINT32 m_frame_crop_offset[4]; // 将4个偏移量放进数组里
	// }
	bool m_vui_parameters_present_flag; // 等于1 表示存在 vui_parameters() 语法结构,等于 0 表示不存在 vui_parameters() 语法结构

};

#endif


