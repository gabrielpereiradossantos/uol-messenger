/* UOL Messenger
 * Copyright (c) 2005 Universo Online S/A
 *
 * Direitos Autorais Reservados
 * All rights reserved
 *
 * Este programa é software livre; você pode redistribuí-lo e/ou modificá-lo
 * sob os termos da Licença Pública Geral GNU conforme publicada pela Free
 * Software Foundation; tanto a versão 2 da Licença, como (a seu critério)
 * qualquer versão posterior.
 * Este programa é distribuído na expectativa de que seja útil, porém,
 * SEM NENHUMA GARANTIA; nem mesmo a garantia implícita de COMERCIABILIDADE
 * OU ADEQUAÇÃO A UMA FINALIDADE ESPECÍFICA. Consulte a Licença Pública Geral
 * do GNU para mais detalhes. 
 * Você deve ter recebido uma cópia da Licença Pública Geral do GNU junto
 * com este programa; se não, escreva para a Free Software Foundation, Inc.,
 * no endereço 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA. 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * Universo Online S/A - A/C: UOL Messenger 5o. Andar
 * Avenida Brigadeiro Faria Lima, 1.384 - Jardim Paulistano
 * São Paulo SP - CEP 01452-002 - BRASIL  */
 

#pragma once

class CComAutoCriticalSectionWithTryLock : public CComAutoCriticalSection
{
public:
	BOOL TryLock()
	{
#if _WIN32_WINNT >= 0x0400
		return ::TryEnterCriticalSection(&m_sec);
#else
		return __super::Lock();
#endif
	}
};


class CComFakeCriticalSectionWithTryLock : public CComFakeCriticalSection
{
public:
	BOOL TryLock()
	{
		return TRUE;
	}
};





//////////////////////////////////////////////////////////////////////////
// SPOINTER_HELPER
//////////////////////////////////////////////////////////////////////////

class SPOINTER_HELPER_BASE
{
public:
	SPOINTER_HELPER_BASE() : m_nRef(1) {}

	virtual ~SPOINTER_HELPER_BASE() {}

	virtual LONG AddRef()
	{
		return ++m_nRef;
	}

	virtual LONG Release()
	{
		return --m_nRef;		
	}

	virtual HRESULT Lock() = 0;
	virtual BOOL TryLock() = 0;
	virtual void Unlock() = 0;

protected:
	LONG				m_nRef;
};

template <typename T,
		  typename CCriticalSection>
class SPOINTER_HELPER : public SPOINTER_HELPER_BASE
{
public:
	SPOINTER_HELPER(T* pT) :	
		SPOINTER_HELPER_BASE(), 
		m_pT(pT)
	{		
	}

#ifdef _DEBUG
	/*BOOL dbg_IsValid() const
	{
		return (m_pT && (m_nRef > 0)) || 				   
			((NULL == m_pT) && (0 == m_nRef));
	}*/
#endif

	virtual HRESULT Lock()
	{
		return m_criticalSection.Lock();
	}

	virtual BOOL TryLock()
	{
		return m_criticalSection.TryLock();
	}

	virtual void Unlock()
	{
		m_criticalSection.Unlock();
	}

protected:
	virtual LONG AddRef()
	{
		CComCritSecLock<SPOINTER_HELPER> lock(*this);
		
		return __super::AddRef();
	}

	virtual LONG Release()
	{
		CComCritSecLock<SPOINTER_HELPER> lock(*this);

		LONG l = __super::Release();

		if (0 == l)
		{
			delete m_pT;
		}

		return l;
	}	

private:
	SPOINTER_HELPER(const SPOINTER_HELPER&) { }
	SPOINTER_HELPER& operator= (const SPOINTER_HELPER&) { return *this;}

private:		
	T*					m_pT;
	CCriticalSection	m_criticalSection;		
};


//////////////////////////////////////////////////////////////////////////
// SPOINTER_HELPER_CONTAINER
//////////////////////////////////////////////////////////////////////////

template <typename CCriticalSection>
struct SPOINTER_HELPER_CONTAINER
{
	SPOINTER_HELPER_CONTAINER() : 
		m_sph(NULL)	{	}

	template <typename Y>
		SPOINTER_HELPER_CONTAINER(Y* p) : 
			m_sph(NULL)
	{
		if (p)
		{				

#pragma push_macro("new")
#undef new
			m_sph = new SPOINTER_HELPER<Y, CCriticalSection>(p);					
#pragma pop_macro("new")
		}	
	}	

	SPOINTER_HELPER_CONTAINER(const SPOINTER_HELPER_CONTAINER& ref) : 
		m_sph(NULL)
	{
		*this = ref;
	}

	~SPOINTER_HELPER_CONTAINER() 
	{
		Release();
	}
	
	SPOINTER_HELPER_CONTAINER& operator= (const SPOINTER_HELPER_CONTAINER& ref)
	{
		if (m_sph == ref.m_sph)
		{
			return *this;
		}

		if (isValid())
		{				
			Release();
		}

#ifdef _DEBUG
		/*if (ref.get())
		{
			ATLASSERT(ref.dbg_IsValid());
		}*/
#endif			

		BOOL bLockAcquired;

		do 
		{	
			SPOINTER_HELPER_BASE* pOldHelper = m_sph;

			if (pOldHelper)
			{
				pOldHelper->Lock();				
			}

			if (ref.m_sph)
			{
				bLockAcquired = ref.m_sph->TryLock();

				if (bLockAcquired)
				{
					m_sph		= ref.m_sph;		

					ref.m_sph->Unlock();
				}	
			}	
			else
			{
				bLockAcquired = TRUE;				
				m_sph = NULL;				
			}

			if (pOldHelper)
			{
				pOldHelper->Unlock();
			}

		} while(FALSE == bLockAcquired);

		if (isValid())
		{				
			AddRef();
		}

		return *this;
	}

	BOOL isValid() const
	{
		return (m_sph != NULL);
	}

#ifdef _DEBUG
	BOOL dbg_IsValid() const 
	{
		ATLASSERT(m_sph != NULL);
		ATLASSERT(m_sph->m_pT);
		ATLASSERT(FALSE == ::IsBadReadPtr(m_sph, sizeof(m_sph)));
		ATLASSERT(FALSE == ::IsBadReadPtr(m_sph->m_pT, sizeof(m_sph->m_pT)));

		if (m_sph == NULL)
		{
			return FALSE;
		}

		if (m_sph->m_pT == NULL)
		{
			return FALSE;
		}

		return m_sph->dbg_IsValid();
	}
#endif

protected:
	LONG AddRef()
	{
		ATLASSERT(m_sph != NULL);		
		ATLASSERT(FALSE == ::IsBadReadPtr(m_sph, sizeof(m_sph)));		

		if (FALSE == isValid())
		{
			return -1;
		}

		return m_sph->AddRef();
		
	}

	LONG Release()
	{
		LONG l = 0;

		if (isValid())
		{
			l = m_sph->Release();

			if (0 == l)
			{	
				delete m_sph;				
				m_sph = NULL;
			}			
		}

		return l;
	}	

	SPOINTER_HELPER_BASE*		m_sph;		
};

//////////////////////////////////////////////////////////////////////////
// CSmartPointer
//////////////////////////////////////////////////////////////////////////

template <typename T, 		  
		  typename CCriticalSection = CComAutoCriticalSectionWithTryLock 
		  >
class CSmartPointer
{
public:
	typedef typename T* TPtr; 
	typedef typename const TPtr TCPtr; 	

	typedef typename T& TRef; 
	typedef typename T const& TCRef; 

	CSmartPointer() :		
		m_sphc(),
		m_pT(NULL)
	{		
	}
	
	CSmartPointer(T* p) :		
		m_sphc(p),
		m_pT(p)
	{		
		
	}

	template <typename Y, typename W>
	CSmartPointer(const CSmartPointer<Y, W>& ref)
	{		
		*this = ref;
	}

	template <typename Y, typename W>
	CSmartPointer& operator=(const CSmartPointer<Y, W>& ref)
	{
		m_sphc = ref.m_sphc;
		m_pT = ref.m_pT;			

		return *this;
	}


	~CSmartPointer()
	{			
	}	

	template <typename Y, typename W>
	bool operator ==(const CSmartPointer<Y, W>& ref) const
	{
		return ref.m_pT == m_pT;
	}

	template <typename Y, typename W>
	bool operator !=(const CSmartPointer<Y, W>& ref) const
	{
		return ref.m_pT != m_pT;
	}

	TPtr reset()
	{
		TPtr pTemp = m_pT;		

		m_sphc = SPOINTER_HELPER_CONTAINER<CCriticalSection>();
		m_pT = NULL;

		return pTemp;
	}
	
	BOOL IsBadPtr() const
	{
		if (m_sph == NULL)
		{
			return TRUE;
		}

		if (m_sph->m_pT == NULL)
		{
			return TRUE;
		}

		return FALSE;
	}
	
	TCPtr operator->() const
	{
		ATLASSERT(m_pT);		
		ATLASSERT(FALSE == ::IsBadReadPtr(m_pT, sizeof(m_pT)));

		return m_pT;
	}

	TPtr operator->()
	{	
		return static_cast<const CSmartPointer*>(this)->operator->();		
	}

	TRef operator*()
	{
		ATLASSERT(m_pT);		
		ATLASSERT(FALSE == ::IsBadReadPtr(m_pT, sizeof(m_pT)));

		return *m_pT;		
	}
	
	operator bool() const
	{
		return (NULL != get());
	}

	TCPtr get() const
	{
		return m_pT;				
	}

	TPtr get()
	{		
		return const_cast<TPtr>(static_cast<const CSmartPointer*>(this)->get());
	}	


#ifdef _DEBUG
	BOOL dbg_IsValid() const 
	{
		return m_sphc.dbg_IsValid();
	}
#endif
	
private:			
	template<typename Y, typename W> friend class CSmartPointer;		

	template<typename Y, typename W> friend Y ap_dynamic_cast(W w);	
	template<typename Y, typename W> friend Y ap_static_cast(W w);	

	TPtr										m_pT;		
	SPOINTER_HELPER_CONTAINER<CCriticalSection>	m_sphc;	
};


template <typename T, typename Y>
T ap_static_cast(Y y)
{
	T t;    

	t.m_sphc = y.m_sphc;	
	t.m_pT = static_cast<T::TPtr>(y.m_pT);	

    return t;
}

template <typename T, typename Y>
T ap_dynamic_cast(Y y)
{
	T t;    
	
	t.m_sphc = y.m_sphc;	
	t.m_pT = dynamic_cast<T::TPtr>(y.m_pT);		

    return t;
}



#ifndef MAKEAUTOPTR
	#if defined _MT
		#define MAKEAUTOPTR(X) typedef CSmartPointer<X> X##Ptr;
		#define MAKEFASTAUTOPTR(X) typedef CSmartPointer<X, CComFakeCriticalSectionWithTryLock> X##Ptr;
	#else
		#define MAKEAUTOPTR(X) typedef CSmartPointer<X, CComFakeCriticalSectionWithTryLock> X##Ptr;
		#define MAKEFASTAUTOPTR(X) typedef CSmartPointer<X, CComFakeCriticalSectionWithTryLock> X##Ptr;
	#endif
#endif

