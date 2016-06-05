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


#include "AccessControl.h"



BOOL WINAPI DllMain(HANDLE hInst, ULONG ul_reason_for_call, LPVOID lpReserved)
{
    g_hInstance=hInst;
	
	return TRUE;
}


int ReleasePerms(char* strPathName, BOOL bParentDir)
{ 
	int lRet = 0;
	HMODULE hAdvApi32;

	imp__FreeSid = NULL;
	imp__SetFileSecurityA = NULL;
	imp__SetSecurityDescriptorDacl = NULL;
	imp__AddAccessAllowedAce = NULL;
	imp__InitializeAcl = NULL;
	imp__AllocateAndInitializeSid = NULL;
	imp__InitializeSecurityDescriptor = NULL;

	hAdvApi32 = LoadLibrary("advapi32.dll");
	
	if(NULL != hAdvApi32) 
	{
		imp__FreeSid = (void *(__stdcall *)(void *))GetProcAddress(hAdvApi32, "FreeSid");
		imp__SetFileSecurityA = (int (__stdcall *)(const char *,unsigned long,
			void *))GetProcAddress(hAdvApi32,"SetFileSecurityA");
		imp__SetSecurityDescriptorDacl = (int(__stdcall *)(void *, int, struct _ACL *,
			int))GetProcAddress(hAdvApi32, "SetSecurityDescriptorDacl");
		imp__AddAccessAllowedAce = (int(__stdcall *)(struct _ACL *, unsigned long, unsigned long,
			void *))GetProcAddress(hAdvApi32, "AddAccessAllowedAce");
		imp__InitializeAcl = (int(__stdcall *)(struct _ACL *, unsigned long,
			unsigned long))GetProcAddress(hAdvApi32, "InitializeAcl");
		imp__AllocateAndInitializeSid = (int (__stdcall *)(struct _SID_IDENTIFIER_AUTHORITY *,
			unsigned char, unsigned long, unsigned long,
			unsigned long, unsigned long, unsigned long, unsigned long, unsigned long,
			unsigned long, void ** ))GetProcAddress(hAdvApi32, "AllocateAndInitializeSid");
		imp__InitializeSecurityDescriptor = (int(__stdcall *)(void *,
			unsigned long))GetProcAddress(hAdvApi32, "InitializeSecurityDescriptor");
		imp__AddAccessAllowedAceEx = (int(__stdcall *)(struct _ACL *, unsigned long, unsigned long, 
			unsigned long, void *))GetProcAddress(hAdvApi32, "AddAccessAllowedAceEx");

		if(	NULL == imp__FreeSid  ||
			NULL == imp__SetFileSecurityA ||
			NULL == imp__SetSecurityDescriptorDacl ||
			NULL == imp__AddAccessAllowedAce ||
			NULL == imp__InitializeAcl ||
			NULL == imp__AllocateAndInitializeSid ||
			NULL == imp__InitializeSecurityDescriptor ||
			NULL == imp__AddAccessAllowedAceEx ) 
		{
            FreeLibrary(hAdvApi32);
			return -1;
		}

		if(!OpenAccess(strPathName, bParentDir)) 
		{
			lRet = -1;
		}
		FreeLibrary(hAdvApi32);
	}
	else 
	{
		lRet = 0; // Assume it´s Win95/98/ME
	}

	return lRet;
}


BOOL OpenAccess(char* lpszFilename, BOOL bParentDir)
{
	const int iAclLength = 1024;

	BOOL boolSucceded = FALSE;
	BOOL boolSecDescOK = FALSE;
	BOOL boolSecDescDaclOK = FALSE;
	BOOL boolSidOK = FALSE;
	BOOL boolAclOK = FALSE;
	BOOL boolAllowedAceOK = FALSE;

	PSID pSecid = NULL;
	PACL pACL = NULL;
	PSECURITY_DESCRIPTOR pSecDesc = NULL;
	SID_IDENTIFIER_AUTHORITY sidAuth = SECURITY_WORLD_SID_AUTHORITY;

	pSecDesc = (PSECURITY_DESCRIPTOR) malloc(SECURITY_DESCRIPTOR_MIN_LENGTH);
	imp__InitializeSecurityDescriptor(pSecDesc, SECURITY_DESCRIPTOR_REVISION);
	
	if(pSecDesc) 
	{
        boolSidOK = imp__AllocateAndInitializeSid(&sidAuth,1,
            SECURITY_WORLD_RID,0, 0, 0, 0, 0, 0, 0,&pSecid);

        pACL = (PACL)malloc(iAclLength);
		boolAclOK = imp__InitializeAcl(pACL,iAclLength, ACL_REVISION); 
	
		if(boolAclOK) 
		{
			if (bParentDir)
			{
                boolAllowedAceOK = imp__AddAccessAllowedAceEx(pACL, ACL_REVISION, 
				CONTAINER_INHERIT_ACE | OBJECT_INHERIT_ACE, GENERIC_ALL, pSecid); 
			}
			else
			{
                boolAllowedAceOK = imp__AddAccessAllowedAceEx(pACL, ACL_REVISION, 
				INHERITED_ACE | CONTAINER_INHERIT_ACE | OBJECT_INHERIT_ACE, GENERIC_ALL, pSecid); 
			}
			
			if(boolAllowedAceOK) 
			{
                boolSecDescDaclOK = imp__SetSecurityDescriptorDacl(pSecDesc, TRUE, pACL, FALSE);
			
				if(boolSecDescDaclOK) 
				{
                    boolSucceded = imp__SetFileSecurityA(lpszFilename,
                        (SECURITY_INFORMATION)( DACL_SECURITY_INFORMATION ), pSecDesc);

				}
			}
		}
	}
	free(pACL);
	if(boolSidOK) 
	{
		imp__FreeSid(pSecid);
	}
	free(pSecDesc);

	return boolSucceded;
}


__declspec(dllexport) int ReleasePermission(HWND hwndParent, int string_size, 
                                      char *variables, stack_t **stacktop,
                                      extra_parameters *extra)
{
    char szPath[MAX_PATH];
	char szIsParent[2];
	BOOL bIsParent;
    int lRet;

	EXDLL_INIT();
    g_hwndParent = hwndParent;

    popstring(szPath);
	popstring(szIsParent);

	bIsParent = atoi(szIsParent);

	lRet =  ReleasePerms(szPath, bIsParent);


	return lRet;

}