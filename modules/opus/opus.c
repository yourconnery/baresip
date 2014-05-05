/**
 * @file opus.c Opus Audio Codec
 *
 * Copyright (C) 2010 Creytiv.com
 */

#include <re.h>
#include <baresip.h>
#include <opus/opus.h>
#include "opus.h"


/**
 * @defgroup opus opus
 *
 * The OPUS audio codec
 *
 * Latest supported version: libopus 1.0.0
 *
 * References:
 *
 *    draft-ietf-codec-opus-10
 *    draft-spittka-payload-rtp-opus-00
 *
 *    http://opus-codec.org/downloads/
 */


static struct aucodec opus = {
	.name      = "opus",
	.srate     = 48000,
	.ch        = 2,
	.fmtp      = "stereo=1;sprop-stereo=1",
	.encupdh   = opus_encode_update,
	.ench      = opus_encode_frm,
	.decupdh   = opus_decode_update,
	.dech      = opus_decode_frm,
	.plch      = opus_decode_pkloss,
};


static int module_init(void)
{
	aucodec_register(&opus);

	return 0;
}


static int module_close(void)
{
	aucodec_unregister(&opus);

	return 0;
}


EXPORT_SYM const struct mod_export DECL_EXPORTS(opus) = {
	"opus",
	"audio codec",
	module_init,
	module_close,
};
