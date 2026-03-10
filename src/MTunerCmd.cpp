//--------------------------------------------------------------------------//
/// Copyright 2025 Milos Tosic. All Rights Reserved.                       ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#include <MTunerCmd_pch.h>

#if RTM_PLATFORM_WINDOWS
#include <shellapi.h>

static const char* g_banner = "Copyright (c) 2024 by Milos Tosic. All rights reserved.\n";

void err(const char* _message)
{
	rtm::Console::info(g_banner);
	rtm::Console::error(_message);
	exit(EXIT_FAILURE);
}

const char* findMTunerExe(const char* _string)
{
	uint32_t len = (uint32_t)strlen(_string);

	// Search from the end of the string to find the executable filename,
	// not a directory that happens to contain "mtuner" in its name.
	// First find the last path separator.
	const char* lastSep = nullptr;
	for (uint32_t i = 0; i < len; ++i)
	{
		if (_string[i] == '/' || _string[i] == '\\')
			lastSep = &_string[i];
	}

	// Search only within the filename portion
	const char* searchStart = lastSep ? lastSep + 1 : _string;
	uint32_t searchLen = (uint32_t)strlen(searchStart);

	const char*		exePos = rtm::strStr<rtm::charToUpper>(searchStart, searchLen, "MTUNERCMD_DEBUG");	// handle running from debugger
	if (!exePos)	exePos = rtm::strStr<rtm::charToUpper>(searchStart, searchLen, "MTUNER ");
	if (!exePos)	exePos = rtm::strStr<rtm::charToUpper>(searchStart, searchLen, "MTUNER.");
	if (!exePos)	exePos = rtm::strStr<rtm::charToUpper>(searchStart, searchLen, "MTUNER\t");
	return exePos;
}

int main(int /*argc*/, const char** /*argv*/)
{
	wchar_t** argv;
	int argc;

	const wchar_t* cmdLine = GetCommandLineW();

	argv = CommandLineToArgvW(cmdLine, &argc);
	if (!argv)
		err("Could not retrieve command line!");


	wchar_t exePathWide[512];
	DWORD pathLen = GetModuleFileNameW(NULL, exePathWide, 512);
	if (pathLen == 0 || pathLen >= 512)
		err("Could not retrieve executable path!");

	rtm::WideToMulti exePath(exePathWide);

	rtm::pathCanonicalize(exePath);

	const char* exePos = findMTunerExe(exePath);
	if (!exePos)
		err("Could not locate executable!");

	// Ensure the target "MTuner.exe" (10 chars + null) fits within the
	// remaining buffer from exePos to the end of the WideToMulti storage.
	uint32_t usedBytes = (uint32_t)(exePos - exePath.m_ptr);
	uint32_t remainingBuffer = exePath.size() + 1 - usedBytes;
	RTM_ASSERT(remainingBuffer > 10, "Needs sufficient space to copy MTuner.exe");
	rtm::strlCpy(const_cast<char*>(exePos), remainingBuffer, "MTuner.exe");

	if (argc < 2)
	{
		// Just run MTuner GUI
		rdebug::processRun(exePath);
		LocalFree(argv);
		return 0;
	}

	// Forward arguments
	const wchar_t* argsStart = wcsstr(cmdLine, argv[1]);
	if (!argsStart)
	{
		LocalFree(argv);
		err("Could not parse command line arguments!");
	}

	rtm::WideToMulti argsMulti(argsStart);

	char commandLine[32768];
	size_t exeLen = rtm::strLen(exePath);
	size_t argsLen = rtm::strLen(argsMulti);

	// 3 = opening quote + closing quote + space
	if (exeLen + argsLen + 3 >= sizeof(commandLine))
	{
		LocalFree(argv);
		err("Command line too long!");
	}

	strcpy(commandLine, "\"");
	strcat(commandLine, exePath);
	strcat(commandLine, "\" ");
	strcat(commandLine, argsMulti);

	char* output = rdebug::processGetOutputOf(commandLine, true);
	if (output)
	{
		rtm::Console::print(output);
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
	return 1;
}

#endif // RTM_PLATFORM_WINDOWS
