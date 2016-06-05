#ifndef __PH_AUDIODRIVER_H__
#define __PH_AUDIODRIVER_H__
/*
  The phaudiodriver module implements abstract audio device interface for phapi
  Copyright (C) 2004  Vadim Lebedev  <vadim@mbdsys.com>

  this module is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  eXosip is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifdef __cplusplus
extern "C" {
#endif

struct ph_audio_stream;

typedef int (*ph_audio_cbk)(struct ph_audio_stream *as, void *recordbuf, int recbufsize, void *playbackbuf, int *playsize);

#define PH_SNDDRVR_REC_CALLBACK 1
#define PH_SNDDRVR_PLAY_CALLBACK 2
#define PH_SNDDRVR_FDS      4

struct ph_audio_driver
{
  const char *snd_driver_kind;
  int   snd_driver_flags;
  int   snd_driver_usage;

  void (*snd_stream_start)(struct ph_audio_stream *as);
  int  (*snd_stream_open)(struct ph_audio_stream *as, char *name, int rate, int framesize, ph_audio_cbk cbk);
  int  (*snd_stream_write)(struct ph_audio_stream *as, void *buf,  int len);
  int  (*snd_stream_read)(struct ph_audio_stream *as, void *buf,  int len);
  int  (*snd_stream_get_out_space)(struct ph_audio_stream *as, int *used);
  int  (*snd_stream_get_avail_data)(struct ph_audio_stream *as);
  void (*snd_stream_close)(struct ph_audio_stream *as);
  int  (*snd_stream_get_fds)(struct ph_audio_stream *as, int fds[2]);
  int  (*snd_driver_match)(struct ph_audio_driver *drv, const char *name);
};
extern struct ph_audio_driver ph_snd_driver;

#define audio_driver_has_rec_callback() (ph_snd_driver.snd_driver_flags & PH_SNDDRVR_REC_CALLBACK)
#define audio_driver_has_play_callback() (ph_snd_driver.snd_driver_flags & PH_SNDDRVR_PLAY_CALLBACK)
#define audio_driver_has_fds() (ph_snd_driver.snd_driver_flags & PH_SNDDRVR_FDS)

#define audio_stream_open(as, name, irate, framesize, cbk) ph_snd_driver.snd_stream_open(as, name, irate, framesize, cbk)
#define audio_stream_start(as) ph_snd_driver.snd_stream_start(as)
#define audio_stream_write(as, buf,  len) ph_snd_driver.snd_stream_write(as, buf,  len)
#define audio_stream_read(as, buf,  len) ph_snd_driver.snd_stream_read(as, buf,  len)
#define audio_stream_get_out_space(as, used) ph_snd_driver.snd_stream_get_out_space(as, used)
#define audio_stream_get_avail_data(as) ph_snd_driver.snd_stream_get_avail_data(as)
#define audio_stream_close(as) ph_snd_driver.snd_stream_close(as)
#define audio_stream_get_fds(as, fds) ph_snd_driver.snd_stream_get_fdse(as, fds)

#define PH_SNDDRVR_USE() ph_snd_driver.snd_driver_usage++
#define PH_SNDDRVR_UNUSE() ph_snd_driver.snd_driver_usage--
#define PH_SNDDRVR_USAGE ph_snd_driver.snd_driver_usage

void  ph_register_audio_driver(struct ph_audio_driver *);

enum { PH_SNDDRVR_BUSY = 1, PH_SNDDRVR_NOTFOUND = 2 };
int   ph_activate_audio_driver(const char* name);

#ifdef __cplusplus
}
#endif

#endif  //__PH_AUDIODRIVER_H__
