/**
 * @file alsa_play.c  ALSA sound driver - player
 *
 * Copyright (C) 2010 Creytiv.com
 */
#define _POSIX_SOURCE 1
#include <sys/types.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>
#include <alsa/asoundlib.h>
#include <pthread.h>
#include <re.h>
#include <rem.h>
#include <baresip.h>
#include "alsa.h"


struct auplay_st {
	struct auplay *ap;      /* inheritance */
	pthread_t thread;
	bool run;
	snd_pcm_t *write;
	struct mbuf *mbw;
	auplay_write_h *wh;
	void *arg;
	struct auplay_prm prm;
	char *device;
	bool le;					  /**< host endian is little flag      */
};


static void auplay_destructor(void *arg)
{
	struct auplay_st *st = arg;

	/* Wait for termination of other thread */
	if (st->run) {
		st->run = false;
		(void)pthread_join(st->thread, NULL);
	}

	if (st->write)
		snd_pcm_close(st->write);

	mem_deref(st->mbw);
	mem_deref(st->ap);
	mem_deref(st->device);
}


static void *write_thread(void *arg)
{
	struct auplay_st *st = arg;
	int n;
	int num_frames;
	size_t 	i, size;
	uint8_t temp;
	uint8_t* buf;
	
	num_frames = st->prm.srate * st->prm.ptime / 1000;

	while (st->run) {
		const int samples = num_frames;

		st->wh(st->mbw->buf, st->mbw->size, st->arg);

		buf = st->mbw->buf;
		size = st->mbw->size;
		
		if (!st->le){
			for(i=0; i<size-1; i+=2){
				temp 	= buf[i];
				buf[i] 	= buf[i+1];
				buf[i+1]= temp;
			}
		}
		
		n = snd_pcm_writei(st->write, st->mbw->buf, samples);
		if (-EPIPE == n) {
			snd_pcm_prepare(st->write);

			n = snd_pcm_writei(st->write, st->mbw->buf, samples);
			if (n != samples) {
				warning("alsa: write error: %s\n",
					snd_strerror(n));
			}
		}
		else if (n < 0) {
			warning("alsa: write error: %s\n", snd_strerror(n));
		}
		else if (n != samples) {
			warning("alsa: write: wrote %d of %d bytes\n",
				n, samples);
		}
	}

	return NULL;
}


int alsa_play_alloc(struct auplay_st **stp, struct auplay *ap,
		    struct auplay_prm *prm, const char *device,
		    auplay_write_h *wh, void *arg)
{
	struct auplay_st *st;
	uint32_t sampc;
	int num_frames;
	int err;
	const uint32_t endian_magic = 0x12345678;
	const uint8_t mg0 = ((uint8_t *)&endian_magic)[0];
	const uint8_t mg1 = ((uint8_t *)&endian_magic)[1];
	const uint8_t mg2 = ((uint8_t *)&endian_magic)[2];
	const uint8_t mg3 = ((uint8_t *)&endian_magic)[3];
	
	if (!stp || !ap || !prm || !wh)
		return EINVAL;
	if (prm->fmt != AUFMT_S16LE)
		return EINVAL;

	if (!str_isset(device))
		device = alsa_dev;

	st = mem_zalloc(sizeof(*st), auplay_destructor);
	if (!st)
		return ENOMEM;

	err = str_dup(&st->device, device);
	if (err)
		goto out;

	st->prm = *prm;
	st->ap  = mem_ref(ap);
	st->wh  = wh;
	st->arg = arg;

	sampc = prm->srate * prm->ch * prm->ptime / 1000;
	num_frames = st->prm.srate * st->prm.ptime / 1000;

	st->mbw = mbuf_alloc(2 * sampc);
	if (!st->mbw) {
		err = ENOMEM;
		goto out;
	}

	err = snd_pcm_open(&st->write, st->device, SND_PCM_STREAM_PLAYBACK, 0);
	if (err < 0) {
		warning("alsa: could not open auplay device '%s' (%s)\n",
			st->device, snd_strerror(err));
		goto out;
	}

	err = alsa_reset(st->write, st->prm.srate, st->prm.ch, st->prm.fmt,
			 num_frames);
	if (err) {
		warning("alsa: could not reset player '%s' (%s)\n",
			st->device, snd_strerror(err));
		goto out;
	}

	st->run = true;
	err = pthread_create(&st->thread, NULL, write_thread, st);
	if (err) {
		st->run = false;
		goto out;
	}

	if (0x12==mg0 && 0x34==mg1 && 0x56==mg2 && 0x78==mg3){
		st->le = false;
	}
	else if (0x12==mg3 && 0x34==mg2 && 0x56==mg1 && 0x78==mg0){
		st->le = true;
	}
	
 out:
	if (err)
		mem_deref(st);
	else
		*stp = st;

	return err;
}
