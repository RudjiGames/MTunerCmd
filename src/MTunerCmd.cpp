//--------------------------------------------------------------------------//
/// Copyright (c) 2017 Milos Tosic. All Rights Reserved.                   ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#include <MTunerCmd_pch.h>

#if RTM_PLATFORM_WINDOWS

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>

static const char* g_banner = "Copyright (c) 2017 by Milos Tosic. All rights reserved.\n";

void err(const char* _message)
{
	rtm::Console::error(_message);
	exit(EXIT_FAILURE);
}

const char* findMTunerExe(const char* _string)
{
	uint32_t len = (uint32_t)strlen(_string);
	const char*		exePos = rtm::strStr<rtm::toUpper>(_string, len, "MTUNERCMD_DEBUG");	// handle running from debugger
	if (!exePos)	exePos = rtm::strStr<rtm::toUpper>(_string, len, "MTUNER ");
	if (!exePos)	exePos = rtm::strStr<rtm::toUpper>(_string, len, "MTUNER.");
	if (!exePos)	exePos = rtm::strStr<rtm::toUpper>(_string, len, "MTUNER\t");
	return exePos;
}

int main(int /*argc*/, const char** /*argv*/)
{
	rtm::Console::info(g_banner);

	wchar_t** argv;
	int argc;

	argv = CommandLineToArgvW(GetCommandLineW(), &argc);
	if (!argv)
		err("Could not retrieve command line!");

	wchar_t exePathWide[512];
	GetModuleFileNameW(NULL, exePathWide, 512); 
	rtm::WideToMulti exePath(exePathWide);

	rtm::pathRemoveRelative(exePath);

	const char* exePos = findMTunerExe(exePath);
	if (!exePos)
		err("Could not locate executable!");

	strcpy(const_cast<char*>(exePos), "MTuner.exe");

	if (argc < 2)
	{
		// Just run MTuner GUI
		rdebug::processRun(exePath);
		LocalFree(argv);
		return 0;
	}

	// Forward arguments
	char commandLine[32768];
	strcpy(commandLine, exePath);

	for (int i=1; i<argc; ++i)
	{
		strcat(commandLine, " ");
		rtm::WideToMulti argvM(argv[i]);
		strcat(commandLine, argvM);
	}

	char* output = rdebug::processGetOutputOf(commandLine, true);
	if (output)
	{
		rtm::Console::info(output);
		rdebug::processReleaseOutput(output);
	}

	LocalFree(argv);

	return 0;
}
#else

#include <rbase/inc/console.h>

int main(int /*argc*/, const char** /*argv*/)
{
	rtm::Console::error("Only Windows supported at the moment");
}

#endif // RTM_PLATFORM_WINDOWS
