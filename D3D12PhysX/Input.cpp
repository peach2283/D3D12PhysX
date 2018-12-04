#include "stdafx.h"
#include "input.h"

////////// Input Ŭ������ ����(static) ��� ���� �� �ʱ�ȭ //////////
HWND Input::hWnd = NULL;

int Input::width  = 0;
int Input::height = 0;
int Input::state[MAX_KEY] = { 0, };

POINT Input::mousePostion = { 0,0 };

bool Input::init(HWND hWnd, int width, int height)
{
    ///////////���� �������� Ŭ���� ���� ������ ������////////////
    Input::width = width;
    Input::height = height;
    Input::hWnd = hWnd;

    ///////////Ű����(���콺 ��ư) ���� �迭�� �ʱ�ȭ��////
    for (int i = 0; i <MAX_KEY; i++)
    {
        state[i] = KEY_UP_REP;  //Ű���� �ʱ� ���� - ������ ����
    }

    return true;
}

void Input::update()
{
    //////////////////////���콺 �����͸� �а�, ������ �������� ��ǥ�� ��ȯ�Ѵ�//////////
    POINT point;

    if (GetCursorPos(&point) == TRUE)
    {
        ScreenToClient(Input::hWnd, &point);  //ȭ�� ��ǥ�� ������ ��ǥ�� ��ȯ�Ѵ�.

        point.x = min(max(0, point.x), (INT)width - 1);
        point.y = min(max(0, point.y), (INT)height - 1);

        Input::mousePostion.x = point.x;    //���콺 ��ǥ�� ���� ������ �����Ѵ�.
        Input::mousePostion.y = point.y;     //���콺 ��ǥ�� �ʿ��� ��쿡�� �� �������� ���� �о ����Ѵ�.
    }

    /////////////////////Ű����� ���콺 �Է� ���� ����///////////
    for (int i = 1; i < MAX_KEY; i++)  //��� Ű������ Ű�� �˻���
    {
        int key = GetAsyncKeyState(i); //i ��° Ű�� �о��

        if (Input::state[i] == KEY_UP_REP)  //Ű�� ������ ���� ����
        {
            if ((key & 0x8000) == 0x8000) //Ű�� ����
            {
                Input::state[i] = KEY_DOWN;  //Ű�� ����(�ѹ�)
            }
            else {
                Input::state[i] = KEY_UP_REP; //������ ���� ����
            }
        }
        else if (Input::state[i] == KEY_DOWN) { //Ű�� �ѹ� ����

            if ((key & 0x8000) == 0x8000) //Ű�� ����
            {
                Input::state[i] = KEY_DOWN_REP;  //Ű�� ���� ����

            }
            else {

                Input::state[i] = KEY_UP;        //Ű�� ���ȴٰ� ������ ���� 
            }
        }
        else if (Input::state[i] == KEY_DOWN_REP) {  //Ű�� ������ ����

            if ((key & 0x8000) == 0x8000) //Ű�� ����
            {
                Input::state[i] = KEY_DOWN_REP;  //Ű�� ���� ����

            }
            else {

                Input::state[i] = KEY_UP;        //Ű�� ���ȴٰ� ������ ���� 
            }
        }
        else if (Input::state[i] == KEY_UP) {  //Ű�� ���ȴ� ������ (�ѹ�)

            if ((key & 0x8000) == 0x8000) //Ű�� ����
            {
                Input::state[i] = KEY_DOWN;  //Ű�� ���� �ѹ�
            }
            else {
                Input::state[i] = KEY_UP_REP;  //Ű�� ���ȴٰ� ������ ���� 
            }
        }
    }
}

void Input::exit()
{
    //Ű����(���콺) ��ġ ����� Ư���� ������ ������ ����
}

/////////////// Ű(���콺)�Է� �Լ� ����� ����////////////
bool Input::getKey(int keycode)  //�����ؼ� �����ִ��� �˻�
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

/////////////////���콺 ��ư ���� �о� ����//////////////////////// 
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