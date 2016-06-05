#pragma once

#include "../../_UOLFoneClient.h"

class ICallbackHandler
{
public:
	virtual ~ICallbackHandler() {}

	virtual void PhoneCallStateChanged(IUOLFoneClientPhoneCall* pPhoneCall, EnumPhoneCallState phoneCallState, IUOLFoneClientExtraResponseData2* pExtraResponseData) = 0;
	virtual void PhoneLineStateChanged(IUOLFoneClientPhoneLine* pPhoneLine, EnumPhoneLineState phoneLineState, IUOLFoneClientExtraResponseData2* pExtraResponseData) = 0;
	virtual void MessageEvent(IUOLFoneClientMessageEvent* pMessageEvent, IUOLFoneClientExtraResponseData2* pExtraResponseData) = 0;
	virtual void PhoneCallStreamError(IUOLFoneClientPhoneCall* pPhoneCall, EnumPhoneCallStreamError phoneCallStreamError) = 0;
	virtual void PhoneLineConnectionTimeout() = 0;
	virtual void OsipDebugEvent(IUOLFoneClientOsipDebugInfo* pOsipDebugInfo) = 0;
};