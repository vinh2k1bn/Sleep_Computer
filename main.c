/* Visual Studio 2019 */

/*
 * The MIT License
 *
 * Copyright 2021 Vinh2k1bn (vinh2k1bn@gmail.com).
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <fcntl.h>		/* unicode */
#include <io.h>			/* for _setmode() */
#include <stdio.h>
#include <powrprof.h>	/* for SetSuspendState() */
#pragma comment(lib, "PowrProf.lib")

/* inline function */
#define ShowConsole()		ShowWindow(GetConsoleWindow(), SW_SHOW)
#define HideConsole()		ShowWindow(GetConsoleWindow(), SW_HIDE)

#define SleepAPI()			SetSuspendState(0,0,0)	/* does not work on my laptop */
#define HibernateAPI()		SetSuspendState(1,1,1)	/* You must enable hibernation */

/* virtual key codes */
#define VK_S    0x53
#define VK_U    0x55
#define VK_X    0x58

/* colors */
#define	COLOR_BLACK			0x00
#define	COLOR_NAVY			0x01
#define	COLOR_GREEN			0x02
#define	COLOR_TEAL			0x03
#define	COLOR_MAROON		0x04
#define	COLOR_PURPLE		0x05
#define	COLOR_OLIVE			0x06
#define	COLOR_SILVER		0x07
#define	COLOR_GRAY			0x08
#define	COLOR_BLUE			0x09
#define	COLOR_LIME			0x0A
#define	COLOR_AQUA			0x0B
#define	COLOR_RED			0x0C
#define	COLOR_FUCHSIA		0x0D
#define	COLOR_YELLOW		0x0E
#define	COLOR_WHITE			0x0F

#define NO_MODE				0
#define ABORT_MODE			1
#define SLEEP_MODE			2
#define	HIBERNATE_MODE		3

#define SEC_PER_MILISEC		1000
#define DEFAULT_TIME		9


void getHelp();
void shortcutSleepComputer();							/* Windows 10 or higher, English : Win + X > u > s */
BOOL abortSleep();										/* may cause system conflicts */
void showConsoleCursor(BOOL bshow);						/* if bshow is 0, console cursor will be hidden */
void waittingToSleep(const wchar_t* msg, long time);	/* display message in time */
void colorConsole(int colorCode);
long stringToSeconds(const wchar_t* str);
void formatTime(int sec);


/* tested cases (works as expected)
* sleepv
* sleepv /?
* sleepv -?
* sleepv -d
* sleepv -a
* sleepv -h
* sleepv -d -t 60
* sleepv -dt 60
* sleepv -dttt 60
* sleepv -dt 0:1
* sleepv -dt 60 -m "mess"
* sleepv -dt 60 -m "mess" -v 20
* sleepv -dt 60 -v 20 -m "mess"
*
*/

int wmain(int argc, wchar_t** argv)
{

	int inResult = _setmode(_fileno(stdin), _O_U16TEXT);
	int outResult = _setmode(_fileno(stdout), _O_U16TEXT);
	if (inResult == -1 || outResult == -1)
		perror("Cannot set mode");

	if (argc == 1)
	{
		getHelp();
		return 0;
	}

	/*wprintf(L"Số lượng argument = %d\n", argc);
	wprintf(L"Ds argument\n");
	for (int i = 0; i < argc; i++)
	{
		wprintf(L"%ls\n", argv[i]);
	}
	wprintf(L"\n");*/

	int offMode = NO_MODE, posCountdown = -1, posDisplayTime = -2, posMessage = -3;
	int countdown = -1, displayTime = -1;
	BOOL bcorrectSyntax = FALSE;
	wchar_t* message = L"";

	const wchar_t language[][63] =
	{
		L"Máy tính đươc lập lịch Sleep trong",
		L"Máy tính được lập lịch Hibernate trong",
		L"Loại bỏ lập lịch tắt máy tính",
		L"thành công",
		L"thất bại",
		L"Hãy thử lại !",
		L"Đảm bảo chế độ Hibernate được bật : ",
		L"Mở PowerShell/Terminal với quyền Admin",
		L"Nhập \"powercfg.exe /h on\" > Enter",
		L"Bạn chỉ cần nhập lệnh này trong lần đầu sử dụng",
	};

	for (int n = 1; n < argc; n++)
		if (argv[n][0] == '-' || argv[n][0] == '/')
			for (int i = 1; i < (int)wcslen(argv[n]); i++)
			{
				switch (towlower(argv[n][i]))
				{
				case 'a':
					if (offMode == NO_MODE)
					{
						offMode = ABORT_MODE;
						bcorrectSyntax = TRUE;
					}
					else
						bcorrectSyntax = FALSE;
					break;
				case 'd':
					if (offMode == NO_MODE)
					{
						offMode = SLEEP_MODE;
						bcorrectSyntax = TRUE;
					}
					else
						bcorrectSyntax = FALSE;
					break;
				case 'h':
					if (offMode == NO_MODE)
					{
						offMode = HIBERNATE_MODE;
						bcorrectSyntax = TRUE;
					}
					else
						bcorrectSyntax = FALSE;
					break;
				case 'm':
					posMessage = n + 1;
					break;
				case 't':
					posCountdown = n + 1;
					break;
				case 'v':
					posDisplayTime = n + 1;
					break;
				default:
					bcorrectSyntax = FALSE;
					break;
				}
			}

	/*wprintf(L"posCountdown = %d\n", posCountdown);
	wprintf(L"posDisplayTime = %d\n", posDisplayTime);
	wprintf(L"posMessage = %d\n", posMessage);*/

	if (posCountdown == posDisplayTime || posCountdown == posMessage || posDisplayTime == posMessage)
		bcorrectSyntax = FALSE;
	else
	{
		if (posCountdown < 0)
			countdown = DEFAULT_TIME;
		else if (posCountdown < argc)
			countdown = stringToSeconds(argv[posCountdown]);

		if (posDisplayTime < 0)
			displayTime = 0;
		else if (posDisplayTime < argc)
			displayTime = stringToSeconds(argv[posDisplayTime]);
	}

	if (posMessage >= argc)
		bcorrectSyntax = FALSE;
	else if (posMessage > 0)
		message = argv[posMessage];

	/*wprintf(L"offMode = %d\n", offMode);
	wprintf(L"bcorrectSyntax = %d\n", bcorrectSyntax);
	wprintf(L"countdown = %d\n", countdown);
	wprintf(L"displayTime = %d\n", displayTime);*/

	/* check correct syntax */
	if (bcorrectSyntax && offMode == ABORT_MODE)
	{
		if (abortSleep())
		{
			wprintf(L"%ls %ls.", language[2], language[3]);
			return 0;
		}

		wprintf(L"%ls %ls. %ls", language[2], language[4], language[5]);
		return 1;
	}

	if (bcorrectSyntax && offMode == HIBERNATE_MODE)
	{
		wprintf(L"%ls\n%ls\n%ls\n%ls\n", language[6], language[7], language[8],language[9]);
	}

	if (bcorrectSyntax && countdown >= 0 && displayTime >= 0)
	{
		//wprintf(L"Correct syntax\n");
		wprintf(L"%ls ", (offMode == SLEEP_MODE) ? language[0] : language[1]);
		formatTime(countdown);
		wprintf(L".\n");

		if (countdown - displayTime >= 0)
		{
			HideConsole();
			Sleep((countdown - displayTime) * SEC_PER_MILISEC);
			ShowConsole();
			showConsoleCursor(0);
			waittingToSleep(message, displayTime * SEC_PER_MILISEC);
		}
		else
		{
			ShowConsole();
			showConsoleCursor(0);
			waittingToSleep(message, countdown * SEC_PER_MILISEC);
		}
		/* sleep / hibernate computer */
		(offMode == SLEEP_MODE) ? shortcutSleepComputer() : HibernateAPI();
	}
	else
		getHelp();

	return 0;
}

void getHelp()
{
	//wprintf(L"GET HELP\n");
	wchar_t* usage =
		L"SleepV v1.0 - Lên Lịch Sleep, Hibernate tự động cho Windows\
	\nCopyright (C) 2021 Vinh2k1bn\
	\n\
	\nCách dùng:\
	\nSleepV -a|-d|-h [-t nn|h:m] [-v nn|h:m] [-m \"message\"]\
	\n-a\tLoại bỏ tắt máy (chỉ có thể khi đang đếm ngược)\
	\n-d\tChế độ Sleep\
	\n-h\tChế độ Hibernate\
	\n-m\tTin nhắn hiển thị trước khi tắt máy\
	\n-t\tChỉ định thời gian (giây) cho đến khi tắt máy (mặc định là 9)\
	\n\thoặc thời gian tắt máy (theo ký hiệu 24 giờ)\
	\n-v\tHiển thị tin nhắn trong số giây được chỉ định trước khi\
	\n\ttắt máy. Nếu bạn bỏ qua tham số này,\
	\n\ttin nhắn hiển thị và chỉ định giá trị 0\
	";
	wprintf(L"\n%ls", usage);
}

void showConsoleCursor(BOOL showFlag)
{
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO     cursorInfo;

	GetConsoleCursorInfo(out, &cursorInfo);
	cursorInfo.bVisible = showFlag; // set the cursor visibility
	SetConsoleCursorInfo(out, &cursorInfo);
}

void colorConsole(int colorCode)
{
	HANDLE hcolor = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hcolor, colorCode);
}

void waittingToSleep(const wchar_t* msg, long miliSeconds)
{
	colorConsole(COLOR_OLIVE);
	wprintf(L"%ls\n", msg);

	/* print progress bar */
	const wchar_t* Pbstr = L"||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||";
	const int PbWidth = 60;
	const long Delay = (long)(miliSeconds / 60);
	colorConsole(COLOR_SILVER);
	for (int i = 1; i <= PbWidth; i++)
	{
		Sleep(Delay);
		wprintf(L"\r[%.*ls%*ls]", i, Pbstr, PbWidth - i, L"");
		fflush(stdout);
	}

}

void shortcutSleepComputer()
{
	//wprintf(L"OK Sleep\n");
	INPUT inputs[4] = { 0 };
	ZeroMemory(inputs, sizeof(inputs));

	//Sleep(2000);
	//printf("Sending 'Win-X'\r\n");

	inputs[0].type = INPUT_KEYBOARD;
	inputs[0].ki.wVk = VK_LWIN;

	inputs[1].type = INPUT_KEYBOARD;
	inputs[1].ki.wVk = VK_X;

	inputs[2].type = INPUT_KEYBOARD;
	inputs[2].ki.wVk = VK_X;
	inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;

	inputs[3].type = INPUT_KEYBOARD;
	inputs[3].ki.wVk = VK_LWIN;
	inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;

	SendInput(4, inputs, sizeof(INPUT));
	Sleep(50);

	inputs[0].ki.wVk = VK_MENU;			/* tránh xung đột gõ tiếng Việt */

	inputs[1].type = INPUT_KEYBOARD;
	inputs[1].ki.wVk = VK_U;

	inputs[2].type = INPUT_KEYBOARD;
	inputs[2].ki.wVk = VK_U;
	inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;

	SendInput(3, inputs, sizeof(INPUT));
	Sleep(50);

	inputs[0].type = INPUT_KEYBOARD;
	inputs[0].ki.wVk = VK_S;

	inputs[1].type = INPUT_KEYBOARD;
	inputs[1].ki.wVk = VK_S;
	inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;

	inputs[2].type = INPUT_KEYBOARD;
	inputs[2].ki.wVk = VK_MENU;
	inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;

	SendInput(3, inputs, sizeof(INPUT));
}

BOOL abortSleep()
{
	/* may cause system conflicts */
	//wprintf(L"Abort Sleep\n");
	system("taskkill /f /im SleepV.exe > nul");
	return TRUE;
	//return FALSE;
}

long stringToSeconds(const wchar_t* str)
{
	long sec = -1;
	int h, m, s = 0;
	if (swscanf(str, L"%d:%d:%d", &h, &m, &s) >= 2)	/* case h:m:s */
	{
		wprintf(L"%d %d %d\n", h, m, s);
		sec = h * 3600 + m * 60 + s;
	}
	else if (swscanf(str, L"%d", &s) == 1)			/* case nn */
		sec = s;
	return sec;
}

void formatTime(int sec)
{
	wchar_t* buffer = L"";
	int h = sec / 3600;
	sec %= 3600;
	int m = sec / 60;
	int s = sec % 60;

	wprintf(L"%02d:%02d:%02d", h, m, s);
}
