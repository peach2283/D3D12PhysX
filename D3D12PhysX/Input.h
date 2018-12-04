#pragma once
#include <windows.h> 

////////////// 키 보드와 마우스 입력 상태 ////////////////
enum KeyState { KEY_UP_REP = 0, KEY_DOWN = 1, KEY_DOWN_REP = 2, KEY_UP = 3 };

#define MAX_KEY  255    //입력을 검사할 키 보드/마우스 버튼 갯수

///////////// 마우스 버튼 번호 //////////////////////////
enum MouseButton { LBUTTON = VK_LBUTTON, RBUTTON = VK_RBUTTON, MBUTTON = VK_MBUTTON };

class Input
{
private:

    static HWND hWnd;         //마우스 좌표 변환을 위한 윈도우 - 마우스 좌표는 화면 좌표로 입력되므로 게임 화면 좌표로 변환해야함
    static int width;
    static int height;
    static int state[MAX_KEY];  //키보드(마우스) 상태 저장 배열

public:

    //////////마우스 위치를 저장하는 변수 - 매 프레임마다 마우스 좌표를 구해서 저장한다//////////
    static  POINT mousePostion;   //POINT 는 윈도우 API의 좌표 저장 구조체

    //////////키 보드 상태 읽어오기 메소드//////////
    static bool getKey(int keyCode);
    static bool getKeyUp(int keyCode);
    static bool getKeyDown(int keyCode);

    //////////마우스 상태 읽어오기 메소드//////////
    static bool getMouseButton(MouseButton btn);
    static bool getMouseButtonUp(MouseButton btn);
    static bool getMouseButtonDown(MouseButton btn);

    //////////입력 장치 초기화 및 업데이트 함수/////
    static bool  init(HWND hWnd, int width, int height);
    static void  update();
    static void  exit();
};