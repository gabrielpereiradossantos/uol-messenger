/*
 * The phmedia-alsa  module implements interface to ALSA audio devices for phapi
 *
 * Copyright (C) 2004  Vadim Lebedev  <vadim@mbdsys.com>
 * Copyright (C) 2006-2007 WENGO SAS
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2,
 * or (at your option) any later version.
 *
 * This is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with dpkg; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/** @see http://www.linuxjournal.com/article/6735 for an introduction to ALSA */

#ifdef ENABLE_ALSA

#define ALSA_PCM_NEW_HW_PARAMS_API
#include <alsa/asoundlib.h>

#include "phapi.h"
#include "phastream.h"
#include "phaudiodriver.h"
#include "phlog.h"

struct alsa_dev {
  snd_pcm_t *ain, *aout;
};

#define ADEV(x) ((struct alsa_dev *)((x)->drvinfo))

void alsa_stream_start(phastream_t *as);
int  alsa_stream_open(phastream_t *as, char *name, int rate, int framesize, ph_audio_cbk cbk);
int  alsa_stream_write(phastream_t *as, void *buf,  int len);
int  alsa_stream_read(phastream_t *as, void *buf,  int len);
int  alsa_stream_get_out_space(phastream_t *as, int *used);
int  alsa_stream_get_avail_data(phastream_t *as);
void alsa_stream_close(phastream_t *as);
int  alsa_stream_get_fds(phastream_t *as, int fds[2]);

void ph_alsa_driver_init(void);
snd_pcm_t *alsa_dev_open(char *name, int type, int wished_rate, int framesize, int latencymsecs, int *chosen_rate);
int alsa_dev_get_fd(snd_pcm_t *s);
int suspend(snd_pcm_t *handle);

struct ph_audio_driver ph_alsa_driver = {
  "alsa",
  PH_SNDDRVR_FDS,
  0,
  alsa_stream_start,
  alsa_stream_open,
  alsa_stream_write,
  alsa_stream_read,
  alsa_stream_get_out_space,
  alsa_stream_get_avail_data,
  alsa_stream_close,
  alsa_stream_get_fds,
  NULL
};

void ph_alsa_driver_init(void)
{
  ph_register_audio_driver(&ph_alsa_driver);
}

snd_pcm_t *alsa_dev_open(char *name, int type, int wished_rate, int framesize, int latencymsecs, int *chosen_rate)
{
  snd_pcm_t *handle;
  snd_pcm_hw_params_t *params;
  snd_pcm_sw_params_t *sparams;
  unsigned int val;
  snd_pcm_uframes_t frames;
  int rc;
  char *nmend = 0;
  char *s = 0;
  *chosen_rate = 0;

#if ACTIVATE_DYNA_AUDIO_DRV_DBG
  int dir;
  unsigned int val2;
  snd_pcm_access_t access;
  snd_pcm_subformat_t format;
  snd_pcm_uframes_t uframe;
#endif

  if (name == NULL)
  {
    DBG_DYNA_AUDIO_DRV("alsa_dev_open: name == NULL!\n");
    return 0;
  }

  if (!strncasecmp(name, "alsa:", 5))
  {
    name += 5;
  }

  if (type == SND_PCM_STREAM_CAPTURE)
  {
    if ((s = strstr(name, "IN=")) != NULL)
    {
      nmend = strchr(name + 3, ' ');
      if (nmend)
      {
        *nmend = 0;
      }
      name = s + 3;
    }
  }
  else if (type == SND_PCM_STREAM_PLAYBACK)
  {
    if ((s = strstr(name, "OUT=")) != NULL)
    {
        nmend = strchr(name + 4, ' ');
        if (nmend)
        {
          *nmend = 0;
        }
        name = s + 4;
    }
  }

  if (type == SND_PCM_STREAM_PLAYBACK)
  {
    DBG_DYNA_AUDIO_DRV("alsa_dev_open: SND_PCM_STREAM_PLAYBACK (name: %s, rate: %d, framesize: %d)\n", name, wished_rate, framesize);
  }
  else if (type == SND_PCM_STREAM_CAPTURE)
  {
    DBG_DYNA_AUDIO_DRV("alsa_dev_open: SND_PCM_STREAM_CAPTURE (name: %s, rate: %d, framesize: %d)\n", name, wished_rate, framesize);
  }

  /* Open PCM device */
  rc = snd_pcm_open(&handle, name, type, 0);

  /* restore the overwritten space */
  if (nmend)
  {
    *nmend = ' ';
  }

  if (rc < 0)
  {
    DBG_DYNA_AUDIO_DRV("failed to open pcm device: %s\n", snd_strerror(rc));
    return 0;
  }
  else
  {
    DBG_DYNA_AUDIO_DRV("open pcm device: succes\n");
  }

  /* Allocate a hardware parameters object. */
  rc = snd_pcm_hw_params_malloc(&params);
  if (rc < 0)
  {
    DBG_DYNA_AUDIO_DRV("cannot allocate hardware parameter structure: %s\n", snd_strerror(rc));
    return 0;
  }

  /* Fill it in with default values. */
  snd_pcm_hw_params_any(handle, params);

  /* Set the desired hardware parameters. */
  /* One channel (mono) */
  rc = snd_pcm_hw_params_set_channels(handle, params, 1);
  if (rc < 0)
  {
    DBG_DYNA_AUDIO_DRV("unable to set hw parameters(channels=1): %s\n", snd_strerror(rc));
    /* two channel (stereo) */
#if 0
    rc = snd_pcm_hw_params_set_channels(handle, params, 2);
    if (rc < 0)
    {
      DBG_DYNA_AUDIO_DRV("unable to set hw parameters(channels=2): %s\n", snd_strerror(rc));
      goto err;
    }
#else
    goto err;
#endif
  }

  /* Interleaved mode */
  rc = snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
  if (rc < 0)
  {
    DBG_DYNA_AUDIO_DRV("unable to set hw parameters(SND_PCM_ACCESS_RW_INTERLEAVED): %s\n", snd_strerror(rc));
    goto err;
  }

  /* Signed 16-bit little-endian format */
  rc = snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);
  if (rc < 0)
  {
    DBG_DYNA_AUDIO_DRV("unable to set hw parameters(SND_PCM_FORMAT_S16_LE): %s\n", snd_strerror(rc));
    goto err;
  }

  /* Set sampling rate */
  val = wished_rate;
  rc = snd_pcm_hw_params_set_rate_near(handle, params, &val, 0);
  if (rc < 0)
  {
    DBG_DYNA_AUDIO_DRV("unable to set hw parameters(rate=%d dir=%d): %s\n", val, dir, snd_strerror(rc));
    goto err;
  }
  *chosen_rate = val;

  /* Set period size */
  frames = framesize;
  rc = snd_pcm_hw_params_set_period_size_near(handle, params, &frames, 0);
  if (rc < 0)
  {
    DBG_DYNA_AUDIO_DRV("unable to set hw parameters(period_size=%d): %s\n", (int) frames, snd_strerror(rc));
    goto err;
  }

  /* Write the parameters to the driver */
  rc = snd_pcm_hw_params(handle, params);
  if (rc < 0)
  {
    DBG_DYNA_AUDIO_DRV("unable to set hw parameters: %s\n", snd_strerror(rc));
    goto err;
  }

  /* Set the non-blocking mode */
  rc = snd_pcm_nonblock(handle, 1);
  if (rc < 0)
  {
    DBG_DYNA_AUDIO_DRV("unable to set noblocking mode:%s\n", snd_strerror(rc));
    goto err;
  }

#if ACTIVATE_DYNA_AUDIO_DRV_DBG
  /* Display information about the PCM interface */
  DBG_DYNA_AUDIO_DRV("PCM handle name = '%s'\n", snd_pcm_name(handle));
  DBG_DYNA_AUDIO_DRV("PCM state = %s\n", snd_pcm_state_name(snd_pcm_state(handle)));
  snd_pcm_hw_params_get_access(params, &access);
  DBG_DYNA_AUDIO_DRV("access type = %s\n", snd_pcm_access_name(access));
  snd_pcm_hw_params_get_format(params, &dir);
  DBG_DYNA_AUDIO_DRV("format = '%s' (%s)\n", snd_pcm_format_name((snd_pcm_format_t)dir), snd_pcm_format_description((snd_pcm_format_t)dir));
  snd_pcm_hw_params_get_subformat(params, &format);
  DBG_DYNA_AUDIO_DRV("subformat = '%s' (%s)\n",
    snd_pcm_subformat_name(format),
    snd_pcm_subformat_description(format));
  snd_pcm_hw_params_get_channels(params, &val);
  DBG_DYNA_AUDIO_DRV("channels = %d\n", val);
  snd_pcm_hw_params_get_rate(params, &val, &dir);
  DBG_DYNA_AUDIO_DRV("rate = %d bps\n", val);
  snd_pcm_hw_params_get_period_time(params, &val, &dir);
  DBG_DYNA_AUDIO_DRV("period time = %d us\n", val);
  snd_pcm_hw_params_get_period_size(params, &frames, &dir);
  DBG_DYNA_AUDIO_DRV("period size = %d frames\n", (int)frames);
  snd_pcm_hw_params_get_buffer_time(params, &val, &dir);
  DBG_DYNA_AUDIO_DRV("buffer time = %d us\n", val);
  snd_pcm_hw_params_get_buffer_size(params, &uframe);
  DBG_DYNA_AUDIO_DRV("buffer size = %d frames\n", (int)uframe);
  snd_pcm_hw_params_get_periods(params, &val, &dir);
  DBG_DYNA_AUDIO_DRV("periods per buffer = %d frames\n", val);
  snd_pcm_hw_params_get_rate_numden(params, &val, &val2);
  DBG_DYNA_AUDIO_DRV("exact rate = %d/%d bps\n", val, val2);
  val = snd_pcm_hw_params_get_sbits(params);
  DBG_DYNA_AUDIO_DRV("significant bits = %d\n", val);
  snd_pcm_hw_params_get_tick_time(params, &val, &dir);
  DBG_DYNA_AUDIO_DRV("tick time = %d us\n", val);
  val = snd_pcm_hw_params_is_batch(params);
  DBG_DYNA_AUDIO_DRV("is batch = %d\n", val);
  val = snd_pcm_hw_params_is_block_transfer(params);
  DBG_DYNA_AUDIO_DRV("is block transfer = %d\n", val);
  val = snd_pcm_hw_params_is_double(params);
  DBG_DYNA_AUDIO_DRV("is double = %d\n", val);
  val = snd_pcm_hw_params_is_half_duplex(params);
  DBG_DYNA_AUDIO_DRV("is half duplex = %d\n", val);
  val = snd_pcm_hw_params_is_joint_duplex(params);
  DBG_DYNA_AUDIO_DRV("is joint duplex = %d\n", val);
  val = snd_pcm_hw_params_can_overrange(params);
  DBG_DYNA_AUDIO_DRV("can overrange = %d\n", val);
  val = snd_pcm_hw_params_can_mmap_sample_resolution(params);
  DBG_DYNA_AUDIO_DRV("can mmap = %d\n", val);
  val = snd_pcm_hw_params_can_pause(params);
  DBG_DYNA_AUDIO_DRV("can pause = %d\n", val);
  val = snd_pcm_hw_params_can_resume(params);
  DBG_DYNA_AUDIO_DRV("can resume = %d\n", val);
  val = snd_pcm_hw_params_can_sync_start(params);
  DBG_DYNA_AUDIO_DRV("can sync start = %d\n", val);
#endif

  if (!latencymsecs)
  {
    snd_pcm_hw_params_free(params);
    return handle;
  }

  snd_pcm_sw_params_alloca(&sparams);

  /* retrieve the parameters from the driver */
  rc = snd_pcm_sw_params_current(handle, sparams);
  if (rc < 0)
  {
    DBG_DYNA_AUDIO_DRV("unable to get sw parameters: %s\n", snd_strerror(rc));
    goto err;
  }

  frames = (*chosen_rate) / 1000 * latencymsecs;
  rc = snd_pcm_sw_params_set_start_threshold(handle, sparams, frames);
  if (rc < 0)
  {
    DBG_DYNA_AUDIO_DRV("unable to set start threshold: %s\n", snd_strerror(rc));
    goto err;
  }

  /* Write the parameters to the driver */
  rc = snd_pcm_sw_params(handle, sparams);
  if (rc < 0)
  {
    DBG_DYNA_AUDIO_DRV("unable to set sw parameters: %s\n", snd_strerror(rc));
    goto err;
  }

  snd_pcm_hw_params_free(params);
  //snd_pcm_sw_params_free(sparams);
  return handle;

err:
  snd_pcm_hw_params_free(params);
  //snd_pcm_sw_params_free(sparams);
  snd_pcm_close(handle);
  return 0;
}

int alsa_dev_get_fd(snd_pcm_t *s)
{
  int err;
  struct pollfd ufd;

  if ((err = snd_pcm_poll_descriptors(s, &ufd, 1)) < 0)
  {
    DBG_DYNA_AUDIO_DRV( "Unable to obtain poll descriptors for device: %s\n", snd_strerror(err));
    return -1;
  }

  return ufd.fd;
}

int alsa_stream_open(phastream_t *as, char *name, int wished_rate, int framesize, ph_audio_cbk cbk)
{
  struct alsa_dev *ad = 0;
  int mic_chosen_rate = 0;
  int spk_chosen_rate = 0;

  DBG_DYNA_AUDIO_DRV("alsa_stream_open: (name: %s, rate: %d, framesize: %d)\n", name, wished_rate, framesize);
  ad = calloc(sizeof(*ad), 1);
  if (!ad)
  {
    return -PH_NORESOURCES;
  }

  // 200ms may satisfied every sound card
  ad->aout = alsa_dev_open(name, SND_PCM_STREAM_PLAYBACK, wished_rate, framesize / 2, 200, &spk_chosen_rate);
  if (!ad->aout)
  {
    free(ad);
    return -PH_NORESOURCES;
  }

  ad->ain = alsa_dev_open(name, SND_PCM_STREAM_CAPTURE, wished_rate, framesize / 2, 0, &mic_chosen_rate);
  if (!ad->ain)
  {
    // close the playback device if we fail to open capture device
    snd_pcm_close(ad->aout);
    free(ad);
    return -PH_NORESOURCES;
  }

  // TODO: refactor the actual_rate across all phaudio_driver backends
  //       it should be an OUT parameter in the dev_open prototype
  // here, hopefully, both MIC and SPK devices have been opened with the same rate
  // we use only the MIC rate
  as->actual_rate = mic_chosen_rate;
  DBG_DYNA_AUDIO_DRV("alsa_stream_open: chosen rate (freq)=(%d)\n",
    as->actual_rate);

  as->drvinfo = ad;
  PH_SNDDRVR_USE();

  return 0;
}

void alsa_stream_close(phastream_t *as)
{
  DBG_DYNA_AUDIO_DRV("alsa_stream_close\n");

  if (!as->drvinfo)
  {
    DBG_DYNA_AUDIO_DRV("alsa stream already closed\n");
    return;
  }

  snd_pcm_drop(ADEV(as)->ain);
  snd_pcm_drop(ADEV(as)->aout);
  snd_pcm_close(ADEV(as)->ain);
  snd_pcm_close(ADEV(as)->aout);

  free(as->drvinfo);
  as->drvinfo = 0;

  PH_SNDDRVR_UNUSE();
}

void alsa_stream_start(phastream_t *as)
{
  snd_pcm_start(ADEV(as)->ain);
  snd_pcm_start(ADEV(as)->aout);
}

int suspend(snd_pcm_t *handle)
{
  int res = 0;

  // wait until suspend flag is released
  while ((res = snd_pcm_resume(handle)) == -EAGAIN)
  {
    sleep(1);
  }
  if (res < 0)
  {
    // failed to restart stream, let's try to repair
    DBG_DYNA_AUDIO_DRV("failed restarting stream: %s\n", snd_strerror(res));
    if ((res = snd_pcm_prepare(handle)) < 0)
    {
      DBG_DYNA_AUDIO_DRV("suspend error: %s\n", snd_strerror(res));
      return 1;
    }
  }
  return 0;
}

int alsa_stream_write(phastream_t *as, void *buf,  int len)
{
  int res = 0;
  int total = 0;

  while(total < len / 2)
  {
    res = snd_pcm_writei(ADEV(as)->aout, buf, len / 2);
    if (res < 0)
    {
      if (res == -EAGAIN)
      {
        DBG_DYNA_AUDIO_DRV("must wait: %s\n", snd_strerror(res));
        if (snd_pcm_wait(ADEV(as)->aout, 1000) < 0)
        {
          DBG_DYNA_AUDIO_DRV("snd_pcm_wait failed: %s\n", snd_strerror(res));
          return 0;
        }
        continue;
      }
      else if (res == -EPIPE)
      {
        DBG_DYNA_AUDIO_DRV("overrun: %s\n", snd_strerror(res));
        // PCM device reached overrun, let's recover and try again
        if (snd_pcm_prepare(ADEV(as)->aout) < 0)
        {
          DBG_DYNA_AUDIO_DRV("snd_pcm_prepare failed: %s\n", snd_strerror(res));
          return 0;
        }
        continue;
      }
      else if (res == -ESTRPIPE)
      {
        // the system has suspended drivers, let's suspend and try again
        DBG_DYNA_AUDIO_DRV("driver has been suspended: %s\n", snd_strerror(res));
        if (suspend(ADEV(as)->aout))
        {
          DBG_DYNA_AUDIO_DRV("suspend failed\n");
          return 0;
        }
        continue;
      }
    }
    else
    {
#if ACTIVATE_DYNA_AUDIO_DRV_DBG
      if (res != len / 2)
      {
        // failed to write all data
        DBG_DYNA_AUDIO_DRV("%d samples has been written instead of %d\n", res, len / 2);
      }
#endif
      total += res;
    }
  }

  return total * 2;
}

int alsa_stream_read(phastream_t *as, void *buf,  int len)
{
  int res = 0;
  int total = 0;

  while(total < len / 2)
  {
    res = snd_pcm_readi(ADEV(as)->ain, buf, len / 2);
    if (res < 0)
    {
      if (res == -EAGAIN)
      {
        DBG_DYNA_AUDIO_DRV("must wait: %s\n", snd_strerror(res));
        if (snd_pcm_wait(ADEV(as)->ain, 1000) < 0)
        {
          DBG_DYNA_AUDIO_DRV("snd_pcm_wait failed: %s\n", snd_strerror(res));
          return 0;
        }
        continue;
      }
      else if (res == -EPIPE)
      {
        // PCM device reached underrun, let's recover and try again
        DBG_DYNA_AUDIO_DRV("underrun: %s\n", snd_strerror(res));
        if (snd_pcm_prepare(ADEV(as)->ain) < 0)
        {
          DBG_DYNA_AUDIO_DRV("snd_pcm_prepare failed: %s\n", snd_strerror(res));
          return 0;
        }
        continue;
      }
      else if (res == -ESTRPIPE)
      {
        // the system has suspended drivers, let's suspend and try again
        DBG_DYNA_AUDIO_DRV("driver has been suspended: %s\n", snd_strerror(res));
        if (suspend(ADEV(as)->ain))
        {
          DBG_DYNA_AUDIO_DRV("suspend failed\n");
          return 0;
        }
        continue;
      }
    }
    else
    {
#if ACTIVATE_DYNA_AUDIO_DRV_DBG
      if (res != len / 2)
      {
        // failed to write all data
        DBG_DYNA_AUDIO_DRV("%d samples has been written instead of %d\n", res, len / 2);
      }
#endif
      total += res;
      break;
    }
  }

  return res * 2;
}

int alsa_stream_get_out_space(phastream_t *as, int *used)
{
  snd_pcm_status_t *st;
  snd_pcm_status_alloca(&st);

  if (snd_pcm_status(ADEV(as)->aout, st) < 0)
  {
    *used = 0;
    return 320;
  }

  *used = 2 * (int) snd_pcm_status_get_delay(st);
  return 2 * (int) snd_pcm_status_get_avail(st);
}

int alsa_stream_get_avail_data(phastream_t *as)
{
  snd_pcm_status_t *st;
  snd_pcm_status_alloca(&st);

  if (snd_pcm_status(ADEV(as)->ain, st) < 0)
  {
    return 0;
  }

  return 2 * (int) snd_pcm_status_get_avail(st);
}

int alsa_stream_get_fds(phastream_t *as, int fds[2])
{
  fds[0] = fds[1] = -1;

  fds[0] = alsa_dev_get_fd(ADEV(as)->ain);
  fds[1] = alsa_dev_get_fd(ADEV(as)->aout);

  return 0;
}

#endif //ENABLE_ALSA
