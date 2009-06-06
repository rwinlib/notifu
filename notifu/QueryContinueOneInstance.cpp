#include "StdAfx.h"

#include <shobjidl.h>

#include "QueryContinueOneInstance.h"
#include "Serialize.h"
#include "trace.h"

CQueryContinueOneInstance::CQueryContinueOneInstance(DWORD d)
: CQueryContinue(d)
{
}


STDMETHODIMP CQueryContinueOneInstance::QueryContinue(VOID)
{
	HRESULT result = CQueryContinue::QueryContinue();

	if((result != S_FALSE) && ThreadWaiting())
	{
		TRACE(eINFO, _T("Notifu process %d exiting because another instance is being launched\n"), GetProcessId(GetCurrentProcess()));
		result = S_FALSE;
	}

	return result;
}
