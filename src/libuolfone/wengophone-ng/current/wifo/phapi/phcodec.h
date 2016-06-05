#ifndef __PHCODEC_H__
#define __PHCODEC_H__



#ifdef __cplusplus
extern "C"
{
#endif

enum phcodec_types 
{
  PH_CODEC_TYPE_AUDIO=0,
  PH_CODEC_TYPE_VIDEO,
  PH_CODEC_TYPE_OTHER
};

struct phcodec
{
  const char *mime;                  /** mime string describing the codec */
  int   clockrate; /** sampling rate of the decoded signal */
  int   encoded_framesize;
  int   decoded_framesize; /** size in bytes of a 20ms frame of samples */
  void  *(*encoder_init)(void *);
  void  *(*decoder_init)(void *);
  void  (*encoder_cleanup)(void *ctx);
  void  (*decoder_cleanup)(void *ctx);
  int   (*encode)(void *ctx, const void *src, int srcsize, void *dst, int dstsize);
  int   (*decode)(void *ctx, const void *src, int srcsize, void *dst, int dstsize);
  void (*tie)(void *enc, void *dec); 
/* <UOLFONE> */
  /*
   * This method should validate the local and remote attributes values and 
   * return a value whether the local attribute value should be changed.
   * Used for packetization attributes handling, like parameter "mode" from 
   * iLBC payload.
   *
   * @param attr_name         Name of local and remote SDP's field that will be compared.
   * @param local_attr_value  Value of local SDP field.
   * @param remote_attr_value Value of remote SDP field.
   *
   * @return NULL if only local value is informed or if it should be kept.
   *         A new string with the remote attribute value, or a calculation 
   *         from result obtained considering local and remote values.
   */
  char  *(*negotiate_attrib)(char *attr_name, char *local_attr_value, char *remote_attr_value);
/* </UOLFONE> */

  int   codec_type;
  int   priority;
  struct phcodec *next;
};

typedef struct phcodec phcodec_t;


extern phcodec_t *ph_codec_list;

void ph_media_codecs_init(const char *pluginpath);

/* <UOLFONE> */
void ph_media_codecs_terminate();
/* </UOLFONE> */

typedef   int (*ph_codec_plugin_init_type)(void (*codec_register_function)(phcodec_t *));

/*******************************************************************************************
 *										Codec MIME constants							   *
 *******************************************************************************************/

#ifdef USE_FLV1_OVER_H261
#define CODEC_FLV1_MIME_STRING "H261"
#else
#define CODEC_FLV1_MIME_STRING "H263-FLV1"
#endif

#define CODEC_H264_MIME_STRING "H264"
#define CODEC_H263_MIME_STRING "H263"
#define CODEC_H263P_MIME_STRING "H263-1998"

#ifdef __cplusplus
}
#endif

#endif
