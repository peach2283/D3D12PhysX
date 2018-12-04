#include "stdafx.h"
#include "Timer.h"

////���� ���� ���� �� �ʱ�ȭ/////
double Timer::currTime = 0;
double Timer::prevTime = 0;
float  Timer::deltaTime = 0;
double Timer::freq = 0;

/////////�ʱ�ȭ  �Լ�///////////
bool Timer::init()
{

    INT64 countsPerSec;
    INT64 currentCounter;

    QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
    QueryPerformanceCounter((LARGE_INTEGER*)&currentCounter);

    Timer::freq = 1.0 / (double)countsPerSec;

    Timer::currTime = currentCounter * freq;  //���� ������ �ð�
    Timer::prevTime = currentCounter * freq;  //���� ������ ���� �ð�

    Timer::deltaTime = 0;

    return true;
}

/////////������Ʈ �Լ�//////////
void Timer::update()
{

    INT64 currentCounter;

    QueryPerformanceCounter((LARGE_INTEGER*)&currentCounter);

    Timer::currTime = currentCounter * freq;
    Timer::deltaTime = (float)(Timer::currTime - Timer::prevTime);

    Timer::prevTime = Timer::currTime;  //���� �ð��� ���� �������� �����ð�
}

////////��ġ ���� �Լ�//////////
void Timer::exit()
{
}