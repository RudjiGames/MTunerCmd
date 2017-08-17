//--------------------------------------------------------------------------//
/// Copyright (c) 2017 Milos Tosic. All Rights Reserved.                   ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#include <MTunerCmd_pch.h>

#if RTM_PLATFORM_WINDOWS

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

int main(int argc, const char** /*argv*/)
{
	char* commandLine = rtm::WideToMulti(GetCommandLineW());

	char cmdLine[32768];
	char cmdLineUC[32768];

	// handle running from debugger
	char* exePos = strstr(commandLine, "MTunerCmd_debug.exe");

	strcpy(cmdLineUC, commandLine);
	_strupr(cmdLineUC);

	if (!exePos)
	{
		exePos = strstr(cmdLineUC, "MTUNER ");
		if (!exePos) exePos = strstr(cmdLineUC, "MTUNER.");
		if (!exePos) exePos = strstr(cmdLineUC, "MTUNER\t");
	}

	wchar_t exePath[512];
	GetModuleFileNameW(NULL, exePath, 512);

	strcpy(cmdLine, rtm::WideToMulti(exePath));
	char* com = strstr(cmdLine, ".com");
	if (com)
		strcpy(com, ".exe ");

	if (argc < 2)
	{
		rdebug::processRun(cmdLine);
		return 0;
	}

	char* argPos = 0;
	if (exePos)
	{
		exePos = &commandLine[exePos - cmdLineUC];
		argPos = strstr(exePos, " ");
		if (!argPos)
			argPos = strstr(exePos, "\t");
	}

	rtm::pathRemoveRelative(cmdLine);
	if (argPos)
		strcat(cmdLine, argPos);

	char* output = rdebug::processGetOutputOf(cmdLine, true);
	rdebug::processReleaseOutput(output);

	return 0;
}
#else

#include <rbase/inc/console.h>

int main(int /*argc*/, const char** /*argv*/)
{
	rtm::Console::error("Only Windows supported at the moment");
}

#endif // RTM_PLATFORM_WINDOWS
