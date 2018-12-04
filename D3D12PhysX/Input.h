#pragma once
#include <windows.h> 

////////////// Ű ����� ���콺 �Է� ���� ////////////////
enum KeyState { KEY_UP_REP = 0, KEY_DOWN = 1, KEY_DOWN_REP = 2, KEY_UP = 3 };

#define MAX_KEY  255    //�Է��� �˻��� Ű ����/���콺 ��ư ����

///////////// ���콺 ��ư ��ȣ //////////////////////////
enum MouseButton { LBUTTON = VK_LBUTTON, RBUTTON = VK_RBUTTON, MBUTTON = VK_MBUTTON };

class Input
{
private:

    static HWND hWnd;         //���콺 ��ǥ ��ȯ�� ���� ������ - ���콺 ��ǥ�� ȭ�� ��ǥ�� �ԷµǹǷ� ���� ȭ�� ��ǥ�� ��ȯ�ؾ���
    static int width;
    static int height;
    static int state[MAX_KEY];  //Ű����(���콺) ���� ���� �迭

public:

    //////////���콺 ��ġ�� �����ϴ� ���� - �� �����Ӹ��� ���콺 ��ǥ�� ���ؼ� �����Ѵ�//////////
    static  POINT mousePostion;   //POINT �� ������ API�� ��ǥ ���� ����ü

    //////////Ű ���� ���� �о���� �޼ҵ�//////////
    static bool getKey(int keyCode);
    static bool getKeyUp(int keyCode);
    static bool getKeyDown(int keyCode);

    //////////���콺 ���� �о���� �޼ҵ�//////////
    static bool getMouseButton(MouseButton btn);
    static bool getMouseButtonUp(MouseButton btn);
    static bool getMouseButtonDown(MouseButton btn);

    //////////�Է� ��ġ �ʱ�ȭ �� ������Ʈ �Լ�/////
    static bool  init(HWND hWnd, int width, int height);
    static void  update();
    static void  exit();
};