#include "stdafx.h"
#include "input.h"

////////// Input 클래스의 정적(static) 멤버 정의 및 초기화 //////////
HWND Input::hWnd = NULL;

int Input::width  = 0;
int Input::height = 0;
int Input::state[MAX_KEY] = { 0, };

POINT Input::mousePostion = { 0,0 };

bool Input::init(HWND hWnd, int width, int height)
{
    ///////////전달 변수값을 클래스 정적 변수에 저장함////////////
    Input::width = width;
    Input::height = height;
    Input::hWnd = hWnd;

    ///////////키보드(마우스 버튼) 상태 배열을 초기화함////
    for (int i = 0; i <MAX_KEY; i++)
    {
        state[i] = KEY_UP_REP;  //키들의 초기 상태 - 놓여져 있음
    }

    return true;
}

void Input::update()
{
    //////////////////////마우스 포인터를 읽고, 윈도우 영역으로 좌표를 변환한다//////////
    POINT point;

    if (GetCursorPos(&point) == TRUE)
    {
        ScreenToClient(Input::hWnd, &point);  //화면 좌표를 윈도우 좌표로 변환한다.

        point.x = min(max(0, point.x), (INT)width - 1);
        point.y = min(max(0, point.y), (INT)height - 1);

        Input::mousePostion.x = point.x;    //마우스 좌표를 정적 변수에 저장한다.
        Input::mousePostion.y = point.y;     //마우스 좌표가 필요한 경우에는 이 변수에서 값을 읽어서 사용한다.
    }

    /////////////////////키보드와 마우스 입력 상태 갱신///////////
    for (int i = 1; i < MAX_KEY; i++)  //모든 키보드의 키를 검사함
    {
        int key = GetAsyncKeyState(i); //i 번째 키를 읽어옴

        if (Input::state[i] == KEY_UP_REP)  //키가 놓여진 상태 지속
        {
            if ((key & 0x8000) == 0x8000) //키가 눌림
            {
                Input::state[i] = KEY_DOWN;  //키가 눌림(한번)
            }
            else {
                Input::state[i] = KEY_UP_REP; //놓여진 상태 지속
            }
        }
        else if (Input::state[i] == KEY_DOWN) { //키가 한번 눌림

            if ((key & 0x8000) == 0x8000) //키가 눌림
            {
                Input::state[i] = KEY_DOWN_REP;  //키가 눌림 지속

            }
            else {

                Input::state[i] = KEY_UP;        //키가 눌렸다가 놓여진 상태 
            }
        }
        else if (Input::state[i] == KEY_DOWN_REP) {  //키가 눌려서 지속

            if ((key & 0x8000) == 0x8000) //키가 눌림
            {
                Input::state[i] = KEY_DOWN_REP;  //키가 눌림 지속

            }
            else {

                Input::state[i] = KEY_UP;        //키가 눌렸다가 놓여진 상태 
            }
        }
        else if (Input::state[i] == KEY_UP) {  //키가 눌렸다 놓여짐 (한번)

            if ((key & 0x8000) == 0x8000) //키가 눌림
            {
                Input::state[i] = KEY_DOWN;  //키가 눌림 한번
            }
            else {
                Input::state[i] = KEY_UP_REP;  //키가 눌렸다가 놓여진 상태 
            }
        }
    }
}

void Input::exit()
{
    //키보드(마우스) 장치 종료는 특별히 수행할 내용이 없음
}

/////////////// 키(마우스)입력 함수 만들어 보기////////////
bool Input::getKey(int keycode)  //지속해서 눌려있는지 검사
{
    if (Input::state[keycode] == KEY_DOWN || Input::state[keycode] == KEY_DOWN_REP)
    {
        return true;
    }
    else {
        return false;

    }
}

bool Input::getKeyDown(int keycode)
{
    if (Input::state[keycode] == KEY_DOWN)
    {
        return true;
    }
    else {

        return false;
    }
}

bool Input::getKeyUp(int keycode)
{

    if (Input::state[keycode] == KEY_UP)
    {
        return true;
    }
    else {
        return false;
    }
}

/////////////////마우스 버튼 상태 읽어 오기//////////////////////// 
bool Input::getMouseButton(MouseButton btn)
{
    return Input::getKey(btn);
}

bool Input::getMouseButtonUp(MouseButton btn)
{
    return Input::getKeyUp(btn);
}

bool Input::getMouseButtonDown(MouseButton btn)
{

    return Input::getKeyDown(btn);
}