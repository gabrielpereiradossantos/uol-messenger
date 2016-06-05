#pragma once

#include "../skin/Element.h"
#include "../image/image.h"
#include "../Image/imageBuilder.h"

class CNotifyPopUpImages :
	public CElement
{
public:
	DECLARE_ELEMENT_NAME("NotifyPopUpImages");
	CNotifyPopUpImages();
	~CNotifyPopUpImages();

	void SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable);	
	CString GetName() const;

	IUOLMessengerImagePtr GetOnlineImage();
	IUOLMessengerImagePtr GetOfflineImage();
	IUOLMessengerImagePtr GetDefaultImage();
	IUOLMessengerImagePtr GetNewMessageImage();
private:
	IUOLMessengerImagePtr m_pOnline, m_pDefault, m_pOffline, m_pNewMessage;
};

MAKEAUTOPTR(CNotifyPopUpImages);