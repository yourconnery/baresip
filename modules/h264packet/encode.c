/**
 * @file h264packet/encode.c  Packetize the H.264 video stream
 *
 * Copyright (C) 2010 - 2013 Creytiv.com
 */
#include <re.h>
#include <rem.h>
#include <baresip.h> 
#include "h26x.h"
#include "h264packet.h"
  

struct videnc_state { 
	struct videnc_param encprm;  
	union {
		struct {
			uint32_t packetization_mode;
			uint32_t profile_idc;
			uint32_t profile_iop;
			uint32_t level_idc;
			uint32_t max_fs;
			uint32_t max_smbps;
		} h264;
	} u;
};


static void destructor(void *arg)
{
	return;
}
  
int decode_sdpparam_h264(struct videnc_state *st, const struct pl *name,
			 const struct pl *val)
{
	if (0 == pl_strcasecmp(name, "packetization-mode")) {
		st->u.h264.packetization_mode = pl_u32(val);

		if (st->u.h264.packetization_mode != 0) {
			warning("h264packet: illegal packetization-mode %u\n",
				st->u.h264.packetization_mode);
			return EPROTO;
		}
	}
	else if (0 == pl_strcasecmp(name, "profile-level-id")) {
		struct pl prof = *val;
		if (prof.l != 6) {
			warning("h264packet: invalid profile-level-id (%r)\n",
				val);
			return EPROTO;
		}

		prof.l = 2;
		st->u.h264.profile_idc = pl_x32(&prof); prof.p += 2;
		st->u.h264.profile_iop = pl_x32(&prof); prof.p += 2;
		st->u.h264.level_idc   = pl_x32(&prof);
	}
	else if (0 == pl_strcasecmp(name, "max-fs")) {
		st->u.h264.max_fs = pl_u32(val);
	}
	else if (0 == pl_strcasecmp(name, "max-smbps")) {
		st->u.h264.max_smbps = pl_u32(val);
	}

	return 0;
}


static void param_handler(const struct pl *name, const struct pl *val,
			  void *arg)
{
	struct videnc_state *st = arg;
 
	(void)decode_sdpparam_h264(st, name, val);
}
 
int encode_update(struct videnc_state **vesp, const struct vidcodec *vc,
		  struct videnc_param *prm, const char *fmtp)
{
	struct videnc_state *st; 
	
	if (!vesp || !vc || !prm)
		return EINVAL;

	if (*vesp)
		return 0;

	st = mem_zalloc(sizeof(*st), destructor);
	if (!st)
		return ENOMEM;

	st->encprm = *prm;
 
	if (str_isset(fmtp)) {
		struct pl sdp_fmtp;

		pl_set_str(&sdp_fmtp, fmtp);

		fmt_param_apply(&sdp_fmtp, param_handler, st);
	}

	debug("h264packet: video encoder %s: %d fps, %d bit/s, pktsize=%u\n",
	      vc->name, prm->fps, prm->bitrate, prm->pktsize);
 
	*vesp = st;

	return 0;
}
 

int encode(struct videnc_state *st, bool update, const struct vidframe *frame,
	   videnc_packet_h *pkth, void *arg)
{
	int err;

	if (!st || !frame || !pkth)
		return EINVAL;
 
	err = h264_packetize(frame->data[0], frame->data[0]+frame->linesize[0], st->encprm.pktsize, pkth, arg);

	return err;
}
