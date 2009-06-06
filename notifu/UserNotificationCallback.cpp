#include "stdafx.h"

#include "UserNotificationCallback.h"


STDMETHODIMP CUserNotificationCallback::QueryInterface(REFIID iid, void FAR* FAR* ppvObj)
{
	if (iid == IID_IUnknown || iid == IID_IUserNotificationCallback)
	{
		*ppvObj = this;
		AddRef();
		return NOERROR;
	}
	return ResultFromScode(E_NOINTERFACE);
}

ULONG CUserNotificationCallback::AddRef() 
{ 
	return 1; 
}

ULONG CUserNotificationCallback::Release() 
{ 
	return 0;
}

STDMETHODIMP CUserNotificationCallback::OnBalloonUserClick(POINT *)
{
	return S_OK;
}

STDMETHODIMP CUserNotificationCallback::OnContextMenu(POINT *)
{
	return S_OK;
}

STDMETHODIMP CUserNotificationCallback::OnLeftClick(POINT *)
{
	return S_OK;
}
