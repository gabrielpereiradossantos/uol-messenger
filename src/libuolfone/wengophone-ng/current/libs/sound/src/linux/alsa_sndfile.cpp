/*
 * ALSA sndfile
 * Copyright (C) 2007  Wengo
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/**
 * @author Mathieu Stute <mstute@gmail.com>
 */

#include "alsa_sndfile.h"

#include <sndfile.h>
#include <alsa/asoundlib.h>
#include <sys/time.h>

#include <util/Logger.h>

#define BUFFER_LEN (512)

static snd_pcm_t * alsa_open (const char * device, int channels, unsigned srate, int realtime);
static int alsa_write_float (snd_pcm_t *alsa_dev, float *data, int frames, int channels);
static int alsa_suspend(snd_pcm_t *handle);

void alsa_play_file(const char *filename, const char *device, int *stopper)
{
  static float buffer [BUFFER_LEN];
  SNDFILE *sndfile;
  SF_INFO sfinfo;
  snd_pcm_t * alsa_dev;
  int readcount, subformat;

  memset(&sfinfo, 0, sizeof (sfinfo));

  LOG_DEBUG("playing " + String(filename));
  if (!(sndfile = sf_open(filename, SFM_READ, &sfinfo)))
  {
    LOG_WARN("can't open file: " + String(filename));
    puts (sf_strerror(NULL));
    return;
  }

  if (sfinfo.channels < 1 || sfinfo.channels > 2)
  {
    LOG_WARN("error : channels = " + String::fromNumber(sfinfo.channels));
    return;
  }

  if ((alsa_dev = alsa_open(device, sfinfo.channels, (unsigned)sfinfo.samplerate, SF_FALSE)) == NULL)
  {
    LOG_WARN("can't open alsa device");
    return;
  }

  subformat = sfinfo.format & SF_FORMAT_SUBMASK;

  if (subformat == SF_FORMAT_FLOAT || subformat == SF_FORMAT_DOUBLE)
  {
    double scale;
    int m;

    sf_command(sndfile, SFC_CALC_SIGNAL_MAX, &scale, sizeof(scale));
    if (scale < 1e-10)
    {
      scale = 1.0;
    }
    else
    {
      scale = 32700.0 / scale;
    }

    while (((readcount = sf_read_float(sndfile, buffer, BUFFER_LEN))) && !(*stopper))
    {
      for (m = 0; m < readcount; m++)
      {
        buffer[m] *= scale;
      }
      alsa_write_float(alsa_dev, buffer, BUFFER_LEN / sfinfo.channels, sfinfo.channels);
    }
  }
  else
  {
    while (((readcount = sf_read_float(sndfile, buffer, BUFFER_LEN))) && !(*stopper))
    {
      alsa_write_float(alsa_dev, buffer, BUFFER_LEN / sfinfo.channels, sfinfo.channels);
    }
  }

  snd_pcm_drain(alsa_dev);
  snd_pcm_close(alsa_dev);

  sf_close(sndfile);

  return;
}

static snd_pcm_t * alsa_open(const char *device, int channels, unsigned samplerate, int realtime)
{
  snd_pcm_t *alsa_dev = NULL;
  snd_pcm_hw_params_t *hw_params;
  snd_pcm_uframes_t buffer_size, xfer_align, start_threshold;
  snd_pcm_uframes_t alsa_period_size, alsa_buffer_frames;
  snd_pcm_sw_params_t *sw_params;

  int err;

  if (realtime)
  {
    alsa_period_size = 256;
    alsa_buffer_frames = 3 * alsa_period_size;
  }
  else
  {
    alsa_period_size = 1024;
    alsa_buffer_frames = 4 * alsa_period_size;
  }

  if ((err = snd_pcm_open(&alsa_dev, device, SND_PCM_STREAM_PLAYBACK, 0)) < 0)
  {
    LOG_WARN("cannot open audio device " + String(device) + ": " + String(snd_strerror(err)));
    goto catch_error;
  }

  snd_pcm_nonblock(alsa_dev, 0);

  if ((err = snd_pcm_hw_params_malloc(&hw_params)) < 0)
  {
    LOG_WARN("cannot allocate hardware parameter structure: " + String(snd_strerror(err)));
    goto catch_error;
  }

  if ((err = snd_pcm_hw_params_any(alsa_dev, hw_params)) < 0)
  {
    LOG_WARN("cannot initialize hardware parameter structure: " + String(snd_strerror(err)));
    goto catch_error;
  }

  if ((err = snd_pcm_hw_params_set_access (alsa_dev, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
  {
    LOG_WARN("cannot set access type: " + String(snd_strerror(err)));
    goto catch_error;
  }

  if ((err = snd_pcm_hw_params_set_format (alsa_dev, hw_params, SND_PCM_FORMAT_FLOAT)) < 0)
  {
    LOG_WARN("cannot set sample format: " + String(snd_strerror(err)));
    goto catch_error;
  }

  if ((err = snd_pcm_hw_params_set_rate_near (alsa_dev, hw_params, &samplerate, 0)) < 0)
  {
    LOG_WARN("cannot set sample rate: " + String(snd_strerror (err)));
    goto catch_error;
  }

  if ((err = snd_pcm_hw_params_set_channels (alsa_dev, hw_params, channels)) < 0)
  {
    LOG_WARN("cannot set channel count: " + String(snd_strerror (err)));
    goto catch_error;
  }

  if ((err = snd_pcm_hw_params_set_buffer_size_near (alsa_dev, hw_params, &alsa_buffer_frames)) < 0)
  {
    LOG_WARN("cannot set buffer size: " + String(snd_strerror (err)));
    goto catch_error;
  }

  if ((err = snd_pcm_hw_params_set_period_size_near (alsa_dev, hw_params, &alsa_period_size, 0)) < 0)
  {
    LOG_WARN("cannot set period size: " + String(snd_strerror(err)));
    goto catch_error;
  }

  if ((err = snd_pcm_hw_params (alsa_dev, hw_params)) < 0)
  {
    LOG_WARN("cannot set parameters: " + String(snd_strerror(err)));
    goto catch_error;
  }

  /* extra check: if we have only one period, this code won't work */
  snd_pcm_hw_params_get_period_size(hw_params, &alsa_period_size, 0);
  snd_pcm_hw_params_get_buffer_size(hw_params, &buffer_size);
  if (alsa_period_size == buffer_size)
  {
    LOG_WARN("Can't use period equal to buffer size");
    goto catch_error;
  }

  snd_pcm_hw_params_free(hw_params);

  if ((err = snd_pcm_sw_params_malloc (&sw_params)) != 0)
  {
    LOG_WARN("snd_pcm_sw_params_malloc: " + String(snd_strerror(err)));
    goto catch_error;
  }

  if ((err = snd_pcm_sw_params_current (alsa_dev, sw_params)) != 0)
  {
    LOG_WARN("snd_pcm_sw_params_current: " + String(snd_strerror(err)));
    goto catch_error;
  }

  /* note: set start threshold to delay start until the ring buffer is full */
  snd_pcm_sw_params_current (alsa_dev, sw_params);
  if ((err = snd_pcm_sw_params_get_xfer_align (sw_params, &xfer_align)) < 0)
  {
    LOG_WARN("cannot get xfer align: " + String(snd_strerror (err)));
    goto catch_error;
  }

  /* round up to closest transfer boundary */
  start_threshold = (buffer_size / xfer_align) * xfer_align;
  if (start_threshold < 1)
  {
    start_threshold = 1;
  }
  if ((err = snd_pcm_sw_params_set_start_threshold (alsa_dev, sw_params, start_threshold)) < 0)
  {
    LOG_WARN("cannot set start threshold: " + String(snd_strerror (err)));
    goto catch_error;
  }

  if ((err = snd_pcm_sw_params (alsa_dev, sw_params)) != 0)
  {
    LOG_WARN("snd_pcm_sw_params: " + String(snd_strerror (err)));
    goto catch_error;
  }

  snd_pcm_sw_params_free (sw_params);
  snd_pcm_reset (alsa_dev);

catch_error:

  if (err < 0 && alsa_dev != NULL)
  {
    snd_pcm_close(alsa_dev);
    return NULL;
  }

  return alsa_dev;
}

static int
alsa_write_float(snd_pcm_t *alsa_dev, float *data, int frames, int channels)
{
  int total = 0;
  int res;

  while(total < frames)
  {
    res = snd_pcm_writei(alsa_dev, data + total * channels, frames - total);
    if (res < 0)
    {
      if (res == -EAGAIN)
      {
        LOG_WARN("must wait: " + String(snd_strerror(res)));
        if (snd_pcm_wait(alsa_dev, 1000) < 0)
        {
          LOG_WARN("snd_pcm_wait failed: " + String(snd_strerror(res)));
          return 0;
        }
        continue;
      }
      else if (res == -EPIPE)
      {
        LOG_WARN("overrun: " + String(snd_strerror(res)));
        // PCM device reached overrun, let's recover and try again
        if (snd_pcm_prepare(alsa_dev) < 0)
        {
          LOG_WARN("snd_pcm_prepare failed: " + String(snd_strerror(res)));
          return 0;
        }
        continue;
      }
      else if (res == -ESTRPIPE)
      {
        // the system has suspended drivers, let's suspend and try again
        LOG_WARN("driver has been suspended: " + String(snd_strerror(res)));
        if (alsa_suspend(alsa_dev))
        {
          LOG_WARN("suspend failed");
          return 0;
        }
        continue;
      }
    }
    else
    {
      total += res;
    }
  }

  return total;
}

static int alsa_suspend(snd_pcm_t *handle)
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
    printf("failed restarting stream: %s\n", snd_strerror(res));
    if ((res = snd_pcm_prepare(handle)) < 0)
    {
      printf("suspend error: %s\n", snd_strerror(res));
      return 1;
    }
  }
  return 0;
}
