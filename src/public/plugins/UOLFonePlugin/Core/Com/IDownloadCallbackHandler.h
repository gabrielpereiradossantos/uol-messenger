#pragma once

#include "../../_UOLFoneServerConfigProvider.h"

class IDownloadCallbackHandler
{
public:
	virtual ~IDownloadCallbackHandler()
	{
	}

	virtual void HandleDownloadComplete(IUOLFoneDownload* pDownload) = 0;
};