#pragma once

#define		CALLBACK_EXECUTION_FAILURE		-1978

struct __declspec(novtable) IAppThreadCallback
{
	virtual ~IAppThreadCallback() {}
	virtual void operator()(UINT) = 0;
	virtual LRESULT GetResult() { return CALLBACK_EXECUTION_FAILURE; };
};
