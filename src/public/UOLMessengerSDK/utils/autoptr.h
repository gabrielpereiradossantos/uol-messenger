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
 
#ifndef _AUTOPTR_H_
#define _AUTOPTR_H_

#pragma once

// Boost smart pointer
#include <boost/smart_ptr.hpp>

template <typename T, typename Y>
T ap_static_cast(Y y)
{
	T t;    

	t.px = static_cast<T::pointer>(y.px);
	t.pn = y.pn;

    return t;
}

template <typename T, typename Y>
T ap_dynamic_cast(Y y)
{
	T t;    

	t.px = dynamic_cast<T::pointer>(y.px);
	t.pn = y.pn;

    return t;
}


template<class T, class Base = boost::shared_ptr<T> > 
class implict_shared_ptr : public Base
{
public:
	implict_shared_ptr() : Base() {}

	template<class Y>
	implict_shared_ptr(implict_shared_ptr<Y> const & r) : 
		Base() 
	{
		px = r.px;
		pn = r.pn;
	}	

	template<class Y>    
	implict_shared_ptr(Y* p) : Base(p) {}		

	template<class Y>
    implict_shared_ptr & operator=(implict_shared_ptr<Y> const & r)
    {
		px = r.px;
        pn = r.pn; // shared_count::op= doesn't throw
        
		return *this;
    }		
};

#ifndef MAKEAUTOPTR
#define MAKEAUTOPTR(X) typedef implict_shared_ptr<X> X##Ptr;
#endif

#endif