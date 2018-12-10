// CPUHangDetector.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>

#include <Windows.h>
#include <sysinfoapi.h>
#include <conio.h>
#include <stdio.h>

#define MS_TO_SLEEP 500
#define MICROSECONDS_TO_SLEEP (MS_TO_SLEEP * 1000)

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cout << "Expected usage: CPUHangDetector <path to log file>\n";
		return 1;
	}

	char stringBuffer[260];
	std::ofstream logFile;
	logFile.open(argv[1], std::ios::out | std::ios::app);

	SYSTEMTIME systemTime;
	GetSystemTime(&systemTime);

	snprintf(stringBuffer, sizeof(stringBuffer), "Starting hang detection: %d/%02d/%02d %02d:%02d:%02d.%03dZ\n", 
		systemTime.wYear, systemTime.wMonth, systemTime.wDay, systemTime.wHour, systemTime.wMinute, 
		systemTime.wSecond, systemTime.wMilliseconds);
	logFile << stringBuffer;

	logFile.flush();

	//ULONGLONG previousTickCount = GetTickCount64();
	//ULONGLONG newTickCount;
	LARGE_INTEGER previousTime;
	LARGE_INTEGER currentTime;
	LARGE_INTEGER qpcFrequency;
	LARGE_INTEGER elapsedTime;

	QueryPerformanceFrequency(&qpcFrequency);
	QueryPerformanceCounter(&previousTime);

	while (!_kbhit())
	{
		Sleep(MS_TO_SLEEP);
		QueryPerformanceCounter(&currentTime);

		elapsedTime.QuadPart = currentTime.QuadPart - previousTime.QuadPart;
		elapsedTime.QuadPart *= 1000000; // Convert to microseconds
		elapsedTime.QuadPart /= qpcFrequency.QuadPart;

		if ((elapsedTime.QuadPart - MICROSECONDS_TO_SLEEP) > (MICROSECONDS_TO_SLEEP / 2))
		{
			GetSystemTime(&systemTime);

			snprintf(stringBuffer, sizeof(stringBuffer), "HANG DETECTED: %d/%02d/%02d %02d:%02d:%02d.%03dZ\n",
				systemTime.wYear, systemTime.wMonth, systemTime.wDay, systemTime.wHour, systemTime.wMinute,
				systemTime.wSecond, systemTime.wMilliseconds);
			logFile << stringBuffer;
			logFile << "DURATION: " << (elapsedTime.QuadPart / 1000) << "ms\n\n";

			logFile.flush();

			std::cout << stringBuffer;
			std::cout << "DURATION: " << (elapsedTime.QuadPart / 1000) << "ms\n\n";
		}

		previousTime.QuadPart = currentTime.QuadPart;
	}

	GetSystemTime(&systemTime);

	snprintf(stringBuffer, sizeof(stringBuffer), "Closing hang detection: %d/%02d/%02d %02d:%02d:%02d.%03dZ\n",
		systemTime.wYear, systemTime.wMonth, systemTime.wDay, systemTime.wHour, systemTime.wMinute,
		systemTime.wSecond, systemTime.wMilliseconds);
	logFile << stringBuffer << "----------------------------------------------------\n";

	logFile.close();

	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
