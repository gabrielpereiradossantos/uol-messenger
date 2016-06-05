#include "uolfoneauth.h"

#ifdef WIN32
#include <windows.h>

typedef int (__cdecl* UolfoneEncryptFunction)(const char*, char*, int*);

#endif

#define UOL_FONE_CRYPTO_LIB_NAME		"UOLFoneCrypt"
#define UOL_FONE_CRYPTO_LIB_NAME_LEN	12
#define LIBRARY_PATH_MAX_LEN			UOL_FONE_LIBRARY_BASE_PATH_MAX_LEN + UOL_FONE_CRYPTO_LIB_NAME_LEN
#ifdef WIN32
#define DIR_SEPARATOR					'\\'
#else
#define DIR_SEPARATOR					'/'
#endif

char library_base_path[LIBRARY_PATH_MAX_LEN + 1] = {'\0',};


int uolfone_auth_set_library_path(const char* library_path)
{
	int len_base_path = 0;

	if (!library_path)
	{
		return -1;
	}

	memset(library_base_path, 0, LIBRARY_PATH_MAX_LEN + 1);
	strncpy(library_base_path, library_path, UOL_FONE_LIBRARY_BASE_PATH_MAX_LEN);

	library_base_path[UOL_FONE_LIBRARY_BASE_PATH_MAX_LEN] = '\0';

	len_base_path = (int) strlen(library_base_path);
	/* len_base_path <= UOL_FONE_LIBRARY_BASE_PATH_MAX_LEN */

	if ((len_base_path >= 1) && (library_base_path[len_base_path - 1] != DIR_SEPARATOR))
	{
		library_base_path[len_base_path] = DIR_SEPARATOR;
		library_base_path[len_base_path + 1] = '\0';
	}

	strncat(library_base_path, UOL_FONE_CRYPTO_LIB_NAME, min(UOL_FONE_CRYPTO_LIB_NAME_LEN, LIBRARY_PATH_MAX_LEN - strlen(library_base_path)));

	return 0;
}



int uolfone_auth_get_cnonce(const char* input_buffer, char* cnonce, int* cnonce_len)
{
#ifdef WIN32
	int ret = -1;
	HINSTANCE hModule;
#endif

	if (!input_buffer || !cnonce || !cnonce_len)
	{
		return -1;
	}

	if (0 == *cnonce_len)
	{
		return -1;
	}
	
#ifndef WIN32
	/* TODO: supply non-win32 implementation */
	return -1;

#else
	hModule = LoadLibrary(library_base_path);

	if (hModule)
	{

		UolfoneEncryptFunction encrypt_function = (UolfoneEncryptFunction) GetProcAddress(hModule, "uolfone_encrypt_password");

		if (encrypt_function)
		{
			ret = ! (*encrypt_function)(input_buffer, cnonce, cnonce_len);
		}

		FreeLibrary(hModule);
	}

	return ret;
#endif
}