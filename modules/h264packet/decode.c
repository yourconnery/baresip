/**
 * @file h264packet/decode.c  discard all the receiving data
 *
 * Copyright (C) 2010 - 2013 Creytiv.com
 */
#include <re.h>
#include <rem.h>
#include <baresip.h>
#include "h264packet.h"


struct viddec_state {
	struct mbuf *mb;
	bool got_keyframe;
};


static void destructor(void *arg)
{
	return;
}
 

int decode_update(struct viddec_state **vdsp, const struct vidcodec *vc,
		  const char *fmtp)
{
	struct viddec_state *st;
	int err = 0;

	if (!vdsp || !vc)
		return EINVAL;

	if (*vdsp)
		return 0;

	(void)fmtp;

	st = mem_zalloc(sizeof(*st), destructor);
	if (!st)
		return ENOMEM;
 
	debug("h264packet: video decoder %s (%s)\n", vc->name, fmtp);

 out:
	if (err)
		mem_deref(st);
	else
		*vdsp = st;

	return err;
} 


int decode_h264(struct viddec_state *st, struct vidframe *frame,
		bool eof, uint16_t seq, struct mbuf *src)
{
	int err;

	(void)seq;

	if (!src)
		return 0; 
		
	if (err)
		return err;

	return 0; 
}

 
