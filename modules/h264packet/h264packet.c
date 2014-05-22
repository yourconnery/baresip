/**
 * @file h264packet.c  Packet H.264 out stream and discard in stream
 *
 * Copyright (C) 2010 Creytiv.com
 */
#include <re.h>
#include <rem.h>
#include <baresip.h>
#include "h26x.h"
#include "h264packet.h"
 
static int h264_fmtp_enc(struct mbuf *mb, const struct sdp_format *fmt,
			 bool offer, void *arg)
{
	struct vidcodec *vc = arg;
	const uint8_t profile_idc = 0x42; /* baseline profile */
	const uint8_t profile_iop = 0x80;
	(void)offer;

	if (!mb || !fmt || !vc)
		return 0;

	return mbuf_printf(mb, "a=fmtp:%s"
			   " packetization-mode=0"
			   ";profile-level-id=%02x%02x%02x"
			   "\r\n",
			   fmt->id, profile_idc, profile_iop, h264_level_idc);
}

static uint32_t packetization_mode(const char *fmtp)
{
	struct pl pl, mode;

	if (!fmtp)
		return 0;

	pl_set_str(&pl, fmtp);

	if (fmt_param_get(&pl, "packetization-mode", &mode))
		return pl_u32(&mode);

	return 0;
}

static bool h264_fmtp_cmp(const char *fmtp1, const char *fmtp2, void *data)
{
	(void)data;

	return packetization_mode(fmtp1) == packetization_mode(fmtp2);
}
 

static struct vidcodec h264 = {
	.name      = "H264",
	.variant   = "packetization-mode=0",
	.encupdh   = encode_update,
	.ench      = encode, 
	.decupdh   = decode_update,
	.dech      = decode_h264,
	.fmtp_ench = h264_fmtp_enc,
	.fmtp_cmph = h264_fmtp_cmp,
};
 
static int module_init(void)
{ 
	vidcodec_register(&h264);
 
	return 0;
}


static int module_close(void)
{
	vidcodec_unregister(&h264);
	
	return 0;
}


EXPORT_SYM const struct mod_export DECL_EXPORTS(h264packet) = {
	"h264packet",
	"codec",
	module_init,
	module_close
};
