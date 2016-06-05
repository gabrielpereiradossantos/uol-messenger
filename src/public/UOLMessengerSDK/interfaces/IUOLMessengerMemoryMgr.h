#pragma once

struct IUOLMessengerMemoryMgr
{
	virtual ~IUOLMessengerMemoryMgr() {}

#ifdef _DEBUG
	virtual void* Allocate( size_t nBytes, const char * szFileName, int nLine) throw() = 0;	
#else	
	virtual void* Allocate( size_t nBytes ) throw() = 0;	
#endif
	
	virtual void Free( void* p ) throw() = 0;

#ifdef _DEBUG
	virtual void* Reallocate( void* p, size_t nBytes, const char * szFileName, int nLine) throw() = 0;
#else	
	virtual void* Reallocate( void* p, size_t nBytes ) throw() = 0;
#endif

	virtual size_t GetSize( void* p ) throw() = 0;
};

