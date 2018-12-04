#include "stdafx.h"
#include "Timer.h"

////정적 변수 정의 및 초기화/////
double Timer::currTime = 0;
double Timer::prevTime = 0;
float  Timer::deltaTime = 0;
double Timer::freq = 0;

/////////초기화  함수///////////
bool Timer::init()
{

    INT64 countsPerSec;
    INT64 currentCounter;

    QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
    QueryPerformanceCounter((LARGE_INTEGER*)&currentCounter);

    Timer::freq = 1.0 / (double)countsPerSec;

    Timer::currTime = currentCounter * freq;  //현재 측정한 시간
    Timer::prevTime = currentCounter * freq;  //이전 프레임 측정 시간

    Timer::deltaTime = 0;

    return true;
}

/////////업데이트 함수//////////
void Timer::update()
{

    INT64 currentCounter;

    QueryPerformanceCounter((LARGE_INTEGER*)&currentCounter);

    Timer::currTime = currentCounter * freq;
    Timer::deltaTime = (float)(Timer::currTime - Timer::prevTime);

    Timer::prevTime = Timer::currTime;  //현재 시간은 다음 프레임의 이전시간
}

////////장치 종료 함수//////////
void Timer::exit()
{
}