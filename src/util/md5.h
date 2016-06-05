/*
* RFC 1321 compliant MD5 implementation,
* by Christophe Devine <devine@cr0.net>;
* this program is licensed under the GPL.
*/
 
#pragma once

#ifdef  __cplusplus
extern "C" {
#endif


#define uint8  unsigned char
#define uint32 unsigned long int

struct md5_context
{
    uint32 total[2];
    uint32 state[4];
    uint8 buffer[64];
};


void md5_starts( struct md5_context *ctx );
void md5_update( struct md5_context *ctx, uint8 *input, uint32 length );
void md5_finish( struct md5_context *ctx, uint8 digest[16] );

#ifdef  __cplusplus
}
#endif


class CMD5
{
public:
	CMD5();
	CMD5(PUCHAR pBuffer, UINT nBufferLength);
	CMD5(const CString& strMD5);

	BOOL operator == (const CMD5& ref) const;
	BOOL operator != (const CMD5& ref) const;

	CString ToString() const;

	static CMD5 CalcMD5(PUCHAR pBuffer, UINT nBufferLength);
	static CMD5 CalcMD5(const CString& strFileName);

private:
	int GetCharValue(TCHAR ch);

private:
	UCHAR md5sum[16];
};



	