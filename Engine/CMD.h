#include <Windows.h>
#include <assert.h>
#ifndef __CONSOLE_H__
#define __CONSOLE_H__
//This code is directly copied from stackoverflow
#include <fcntl.h>
#include <io.h>
#include <iostream>

#ifndef _USE_OLD_IOSTREAMS
using namespace std;
#endif

void RedirectIOToConsole()
{
	AllocConsole();

	HANDLE hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
	int iSystemOut = _open_osfhandle(intptr_t(hConsoleOut), _O_WTEXT);
	assert(iSystemOut);
	FILE *SystemOut = _wfdopen(iSystemOut, L"w");
	assert(SystemOut);


	HANDLE hConsoleIn = GetStdHandle(STD_INPUT_HANDLE);
	int iSystemIn = _open_osfhandle(intptr_t(hConsoleIn), _O_WTEXT);
	assert(iSystemIn);
	FILE *SystemIn = _wfdopen(iSystemIn, L"r");
	assert(SystemIn);

	HANDLE hConsoleErr = GetStdHandle(STD_ERROR_HANDLE);
	int iSystemErr = _open_osfhandle(intptr_t(hConsoleErr), _O_WTEXT);
	assert(iSystemErr);
	FILE *SystemErr = _wfdopen(intptr_t(iSystemErr), L"w");
	assert(SystemErr);

	ios::sync_with_stdio();

	_wfreopen_s(&SystemOut, L"CONOUT$", L"w", stdout);
	_wfreopen_s(&SystemIn, L"CONIN$", L"r", stdin);
	_wfreopen_s(&SystemErr, L"CONOUT$", L"w", stderr);

	wcout.clear();
	cout.clear();
	wcerr.clear();
	cerr.clear();
	wcin.clear();
	cin.clear();

};
#endif