#ifndef __UOLFONEAUTH_H__
#define __UOLFONEAUTH_H__

#define UOL_FONE_LIBRARY_BASE_PATH_MAX_LEN		244

int uolfone_auth_set_library_path(const char* library_path);

int uolfone_auth_get_cnonce(const char* input_buffer, char* cnonce, int* cnonce_len);


#endif /* __UOLFONEAUTH_H__ */