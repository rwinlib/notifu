// notifu.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "CommandLine.h"
#include "NotifyUser.h"
#include "RegistryFix.h"
#include "Serialize.h"
#include "returnvalue.h"
#include "QueryContinueOneInstance.h"
#include "UserNotificationCallback.h"
#include "Trace.h"

CAppModule _Module;

/* -------------------------------------------------------------------------- */
/** @brief The entry point of the program (it is command line).
* 
* This function serves as a demonstration on how to use the NotifyUser function.
* You must somehow fill a NOTIFU_PARAM structure and pass it to NotifyUser. 
* You will most probably throw everything else away, and that's ok. 
*
* The SerializeEnter and SerializeLeave are there to provide the "ballon 
* replacement" feature of the command line utility.
*
* Good luck !
*
* @param hInstance This module's instance
* @param hPrevInstance Legacy parameter, always 0
* @param lpCmdLine The command line parameters. They will be sent to the command
*                  line parser object
* @param nCmdShow  Recommemded way of showing a window that does not exist in
*                  this program. It is not used.        
* 
* @return One of the error codes documeneted in the enum above.
*/
/* ---------------------------------------------------------------------------- */
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	int errorlevel = eUnknown;

	TRACE(eINFO, _T("Notifu process %d starting.\n"), GetProcessId(GetCurrentProcess()));
			    
	gCommandLine.Setup();

	gCommandLine.Parse(lpCmdLine);

	if(gCommandLine.Fix)
	{
		if(EnableBalloonTips() == ERROR_SUCCESS)
		{
			if(gCommandLine.HasErrors())
			{
				//just wanted to fix the registry, that's ok
				errorlevel = eSuccess;
				gCommandLine.Show(_T("Balloon tips were enabled in the registry.\n\nThe key EnableBalloonTips=1 was added under HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced"));
				goto end;
			}
		}
		else
		{
			gCommandLine.Show(_T("Unable to enable balloon tips in the registry.\n\nPlease add EnableBalloonTips=1 under HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced"));
		}
	}

	if(gCommandLine.Continue())
	{
		HRESULT result;
		CoInitialize(0);

		NOTIFU_PARAM params;

		gCommandLine.CopyCommandLineToParams(params);

		CQueryContinueOneInstance mqc(params.mDelay);
      CUserNotificationCallback unc;

		SerializeEnter();

		result = NotifyUser(params, &mqc, &unc);

		SerializeLeave();

		DestroyIcon(params.mIcon);

		switch(result)
		{
		case 0x800704C7 : errorlevel = eClosedBallon; break;
		case S_OK :	errorlevel = eClickedBallon; break;
		case S_FALSE :	
			if(mqc.TimeoutReached())
			{
				errorlevel = eTimedOut; 
			}
			else
			{
				errorlevel = unc.GetResultCode();
			}
			break;
		case E_NOINTERFACE:
		case REGDB_E_CLASSNOTREG:
			errorlevel = eNotSupported; 
			gCommandLine.Show(_T("IUserNotification is not supported on this version of Windows."));
			break;
		}

		CoUninitialize();
	}
	else
	{
		errorlevel = eBadArgs;
	}

end:

	return errorlevel;
}

