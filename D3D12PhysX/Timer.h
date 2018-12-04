#pragma once

class Timer
{
private:
    static double currTime;
    static double prevTime;
    static double freq;

public:

    static float deltaTime;

    static bool init();
    static void update();
    static void exit();
};