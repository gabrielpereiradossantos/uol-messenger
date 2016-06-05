#pragma once

class CDIB
{
public:
	class IDIBReleaseMemoryStrategy
	{
	public:
		virtual ~IDIBReleaseMemoryStrategy() {}
		virtual void Dispose(CDIB* pDIB) = 0;
	};
	MAKEAUTOPTR(IDIBReleaseMemoryStrategy); 

	class CDIBReleaseMemoryStrategy : public IDIBReleaseMemoryStrategy
	{
	protected:
		virtual void Dispose(CDIB*) { };
	};



	class CDIBReleaseBitmapInfoStrategy : virtual public IDIBReleaseMemoryStrategy
	{
	protected:
		virtual void Dispose(CDIB* pDIB)
		{
			delete[] pDIB->m_pBMI;
			pDIB->m_pBMI = NULL;
		}
	};

	class CDIBReleaseBitsInfoStrategy  : virtual public IDIBReleaseMemoryStrategy
	{
	protected:
		virtual void Dispose(CDIB* pDIB)
		{
			delete[] pDIB->m_pBits;
			pDIB->m_pBits = NULL;
		}
	};

	class CDIBReleaseBitmapInfoAndBitsInfoStrategy : public CDIBReleaseBitmapInfoStrategy, 
													 public CDIBReleaseBitsInfoStrategy
	{
	private:
		virtual void Dispose(CDIB* pDIB)
		{
			CDIBReleaseBitmapInfoStrategy::Dispose(pDIB);
			CDIBReleaseBitsInfoStrategy::Dispose(pDIB);
		}
	};

	CDIB(void);
	~CDIB(void);

	BOOL Attach(BITMAPINFO* pDIB, BYTE* pBits, IDIBReleaseMemoryStrategyPtr pDIBReleaseMemoryStrategy = new CDIBReleaseMemoryStrategy());
	BOOL LoadFile(const TCHAR * pFileName);
	BOOL LoadBitmap(HMODULE hModule, LPCTSTR lpszBitmapName);

	void Dettach();

	int DrawDIB(HDC hDC, int dx, int dy, int dw, int dh, int sx, int sy, int sw, int sh, DWORD rop) const;

	int  GetWidth(void) const;
	int  GetHeight(void) const;	

	const void* GetBitmapInfo() const;
	const BYTE* GetBitmapBits() const;

private:
	BITMAPINFO* m_pBMI;	
	BYTE*		m_pBits;	

	RGBTRIPLE*	m_pRGBTRIPLE;
	RGBQUAD*	m_pRGBQUAD;
	DWORD*		m_pBitFields;
	
	int			m_nClrUsed;
	int 		m_nWidth;    
	int 		m_nHeight;   
	int 		m_nPlanes;   
	int 		m_nBitCount; 
	int 		m_nImageSize;

	IDIBReleaseMemoryStrategyPtr m_pDIBReleaseMemoryStrategy;	
};

MAKEAUTOPTR(CDIB);
