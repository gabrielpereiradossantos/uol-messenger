#include "StdAfx.h"
#include ".\notifypopupimages.h"

CNotifyPopUpImages::CNotifyPopUpImages()
{
}

CNotifyPopUpImages::~CNotifyPopUpImages()
{
}

void CNotifyPopUpImages::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	__super::SetParameters(pMapParams, pElementTable);

	try
	{
		CString strOnline;
		GetParameterValue(strOnline, pMapParams, "online_image");
		
		m_pOnline = UOLMessengerImage::CImageBuilder::LoadImage(strOnline);
	}
	catch(const CSkinException&)
	{
	}
	
	try
	{
		CString strOffline;
		GetParameterValue(strOffline, pMapParams, "offline_image");
		
		m_pOffline = UOLMessengerImage::CImageBuilder::LoadImage(strOffline);
	}
	catch(const CSkinException&)
	{
	}

	try
	{
		CString strDefault;
		GetParameterValue(strDefault, pMapParams, "default_image");
		
		m_pDefault = UOLMessengerImage::CImageBuilder::LoadImage(strDefault);
	}
	catch(const CSkinException&)
	{
	}

	try
	{
		CString strNewMessage;
		GetParameterValue(strNewMessage, pMapParams, "new_message_image");
		
		m_pNewMessage = UOLMessengerImage::CImageBuilder::LoadImage(strNewMessage);
	}
	catch(const CSkinException&)
	{
	}
}

CString CNotifyPopUpImages::GetName() const
{
	return "__NotifyPopUpImagesInstance";
}

IUOLMessengerImagePtr CNotifyPopUpImages::GetOnlineImage()
{
	return m_pOnline;
}

IUOLMessengerImagePtr CNotifyPopUpImages::GetOfflineImage()
{
	return m_pOffline;
}

IUOLMessengerImagePtr CNotifyPopUpImages::GetDefaultImage()
{
	return m_pDefault;
}

IUOLMessengerImagePtr CNotifyPopUpImages::GetNewMessageImage()
{
	return m_pNewMessage;
}
