/**
 * @file h26x.h  Interface to H.26x video codecs
 *
 * Copyright (C) 2010 Creytiv.com
 */
 
/*
 * H.264
 */


/** NAL unit types (RFC 3984, Table 1) */
enum {
	H264_NAL_UNKNOWN      = 0,
	/* 1-23   NAL unit  Single NAL unit packet per H.264 */
	H264_NAL_SLICE        = 1,
	H264_NAL_DPA          = 2,
	H264_NAL_DPB          = 3,
	H264_NAL_DPC          = 4,
	H264_NAL_IDR_SLICE    = 5,
	H264_NAL_SEI          = 6,
	H264_NAL_SPS          = 7,
	H264_NAL_PPS          = 8,
	H264_NAL_AUD          = 9,
	H264_NAL_END_SEQUENCE = 10,
	H264_NAL_END_STREAM   = 11,
	H264_NAL_FILLER_DATA  = 12,
	H264_NAL_SPS_EXT      = 13,
	H264_NAL_AUX_SLICE    = 19,

	H264_NAL_STAP_A       = 24,  /**< Single-time aggregation packet */
	H264_NAL_STAP_B       = 25,  /**< Single-time aggregation packet */
	H264_NAL_MTAP16       = 26,  /**< Multi-time aggregation packet  */
	H264_NAL_MTAP24       = 27,  /**< Multi-time aggregation packet  */
	H264_NAL_FU_A         = 28,  /**< Fragmentation unit             */
	H264_NAL_FU_B         = 29,  /**< Fragmentation unit             */
};

/**
 * H.264 Header defined in RFC 3984
 *
 * <pre>
      +---------------+
      |0|1|2|3|4|5|6|7|
      +-+-+-+-+-+-+-+-+
      |F|NRI|  Type   |
      +---------------+
 * </pre>
 */ 
 
const uint8_t *h264_find_startcode(const uint8_t *p, const uint8_t *end);
 
