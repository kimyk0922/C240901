#include <stdio.h>
#include <process.h>
#include <windows.h>


#pragma lib, timepi

int Sum = 0;

int Sum2 = 0;

CRITICAL_SECTION &SumCriticalSection1;
CRITICAL_SECTION& SumCriticalSection2;


//Dead Lock -> 
//LOCK 프로그래밍
unsigned __stdcall Increase(void* Arg)
{
	
	
	for (int i = 0; i < 100000; ++i)
	{

		EnterCriticalSection(&SumCriticalSection1);
		EnterCriticalSection(&SumCriticalSection2);

		Sum2--;


		Sum++;

		LeaveCriticalSection(&SumCriticalSection2);
		LeaveCriticalSection(&SumCriticalSection1);

	}	

	return 0;

}

unsigned __stdcall Decrease(void* Arg)
{

	EnterCriticalSection(&SumCriticalSection2);
	EnterCriticalSection(&SumCriticalSection1);

	for (int i = 0; i < 100000; ++i)
	{

		Sum--;

//		EnterCriticalSection(&SumCriticalSection);

		Sum2++;

//		LeaveCriticalSection(&SumCriticalSection);

	}

	LeaveCriticalSection(&SumCriticalSection1);
	LeaveCriticalSection(&SumCriticalSection2);

	return 0;

}

//main thread
int main()
{
	HANDLE ThreadHandles[2] = { 0, };

	InitializeCriticalSection(&SumCriticalSection);

	ThreadHandles[0] = (HANDLE)_beginthreadex(
		nullptr,
		0,
		Increase,
		nullptr,
		0,
		nullptr
	);


	ThreadHandles[0] = (HANDLE)_beginthreadex(
		nullptr,
		0,
		Decrease,
		nullptr,
		0,
		nullptr
	);


//	WaitForSingleObject(ThreadHandle1, INFINITE);
//	WaitForSingleObject(ThreadHandle2, INFINITE);

	WaitForMultipleObjects(2, ThreadHandles, TRUE, INFINITE);
	
//	Sleep(10000);

	DWORD EndTime = timeGetTime();


	printf("%d", EndTinme - StartTime);
		
	DeleteCriticalSection(&SumCiriticalSection);


	return 0;
}