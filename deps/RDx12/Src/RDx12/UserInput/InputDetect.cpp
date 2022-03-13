#include "../Include/RDx12/UserInput/InputDetect.h"
#include <RWin32/Win32Debug.h>

namespace RDx12
{
    InputDetect::InputDetect(HINSTANCE hinst, HWND hWnd)
    {

        DirectInput8Create(hinst, 0x0800, IID_IDirectInput8, (void**)&dinput8, NULL);
        dinput8->CreateDevice(GUID_SysKeyboard, &keydev, NULL);
        dinput8->CreateDevice(GUID_SysMouse, &mousedev, NULL);

        keydev->SetDataFormat(&c_dfDIKeyboard);
        mousedev->SetDataFormat(&c_dfDIMouse);

        //设置协作模式，这里是前台+非独占
        keydev->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
        mousedev->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

        InitMouseDic();
        InitKeyDic();
    }

    void InputDetect::GetInput(float deltatime)
    {
        // 读取鼠标输入
        ::ZeroMemory(&mousestate, sizeof(mousestate));
        Device_Read(mousedev, (LPVOID)&mousestate, sizeof(mousestate));

        // 读取键盘输入
        ::ZeroMemory(keystate, sizeof(keystate));
        Device_Read(keydev, (LPVOID)keystate, sizeof(keystate));

        UpdateKeyDic(deltatime);
        UpdateMouseDic(deltatime);
    }

    BOOL InputDetect::Device_Read(IDirectInputDevice8* pDIDevice, void* pBuffer, long lSize)
    {
        HRESULT hr;
        while (true)
        {
            pDIDevice->Poll();              // 轮询设备
            pDIDevice->Acquire();          // 获取设备的控制权
            if (SUCCEEDED(hr = pDIDevice->GetDeviceState(lSize, pBuffer))) break;
            if (hr != DIERR_INPUTLOST || hr != DIERR_NOTACQUIRED) return FALSE;
            if (FAILED(pDIDevice->Acquire())) return FALSE;
        }
        return TRUE;
    }


    void InputDetect::InitMouseDic()
    {
        MouseDic[0] = L"X";
        MouseDic[1] = L"Y";
        MouseDic[2] = L"Z";
        MouseDic[3] = L"LEFT";
        MouseDic[4] = L"RIGHT";
        MouseDic[5] = L"MIDDLE";

        MouseData[0] = 0;
        MouseData[1] = 0;
        MouseData[2] = 0;
        MouseData[3] = -1;
        MouseData[4] = -1;
        MouseData[5] = -1;
    }

    void InputDetect::UpdateMouseDic(float deltatime)
    {
        MouseData[0] = mousestate.lX;
        MouseData[1] = mousestate.lY;
        MouseData[2] = mousestate.lZ;
        if (mousestate.rgbButtons[0] & 0x80)
        {
            if (MouseData[3] <= -1)
            {
                MouseData[3] = 0;
            }
            else
            {
                MouseData[3] += deltatime;
            }
        }
        else
        {
            if (MouseData[3] > 0)
            {
                MouseData[3] = -2;
            }
            else
            {
                MouseData[3] = -1;
            }
        }

        if (mousestate.rgbButtons[1] & 0x80)
        {
            if (MouseData[4] <= -1)
            {
                MouseData[4] = 0;
            }
            else
            {
                MouseData[4] += deltatime;
            }
        }
        else
        {
            if (MouseData[4] > 0)
            {
                MouseData[4] = -2;
            }
            else
            {
                MouseData[4] = -1;
            }
        }

        if (mousestate.rgbButtons[2] & 0x80)
        {
            if (MouseData[5] <= -1)
            {
                MouseData[5] = 0;
            }
            else
            {
                MouseData[5] += deltatime;
            }
        }
        else
        {
            if (MouseData[5] > 0)
            {
                MouseData[5] = -2;
            }
            else
            {
                MouseData[5] = -1;
            }
        }
    }

    void InputDetect::InitKeyDic()
    {
        KeyDic[0x01] = L"ESCAPE";
        KeyDic[0x02] = L"1";
        KeyDic[0x03] = L"2";
        KeyDic[0x04] = L"3";
        KeyDic[0x05] = L"4";
        KeyDic[0x06] = L"5";
        KeyDic[0x07] = L"6";
        KeyDic[0x08] = L"7";
        KeyDic[0x09] = L"8";
        KeyDic[0x0A] = L"9";
        KeyDic[0x0B] = L"0";
        KeyDic[0x0C] = L"MINUS";
        KeyDic[0x0D] = L"EQUALS";
        KeyDic[0x0E] = L"BACK";
        KeyDic[0x0F] = L"TAB";
        KeyDic[0x10] = L"Q";
        KeyDic[0x11] = L"W";
        KeyDic[0x12] = L"E";
        KeyDic[0x13] = L"R";
        KeyDic[0x14] = L"T";
        KeyDic[0x15] = L"Y";
        KeyDic[0x16] = L"U";
        KeyDic[0x17] = L"I";
        KeyDic[0x18] = L"O";
        KeyDic[0x19] = L"P";
        KeyDic[0x1A] = L"LBRACKET";
        KeyDic[0x1B] = L"RBRACKET";
        KeyDic[0x1C] = L"RETURN";
        KeyDic[0x1D] = L"LCONTROL";
        KeyDic[0x1E] = L"A";
        KeyDic[0x1F] = L"S";
        KeyDic[0x20] = L"D";
        KeyDic[0x21] = L"F";
        KeyDic[0x22] = L"G";
        KeyDic[0x23] = L"H";
        KeyDic[0x24] = L"J";
        KeyDic[0x25] = L"K";
        KeyDic[0x26] = L"L";
        KeyDic[0x27] = L"SEMICOLON";
        KeyDic[0x28] = L"APOSTROPHE";
        KeyDic[0x29] = L"GRAVE";
        KeyDic[0x2A] = L"LSHIFT";
        KeyDic[0x2B] = L"BACKSLASH";
        KeyDic[0x2C] = L"Z";
        KeyDic[0x2D] = L"X";
        KeyDic[0x2E] = L"C";
        KeyDic[0x2F] = L"V";
        KeyDic[0x30] = L"B";
        KeyDic[0x31] = L"N";
        KeyDic[0x32] = L"M";
        KeyDic[0x33] = L"COMMA";
        KeyDic[0x34] = L"PERIOD";
        KeyDic[0x35] = L"SLASH";
        KeyDic[0x36] = L"RSHIFT";
        KeyDic[0x37] = L"MULTIPLY";
        KeyDic[0x38] = L"LMENU";
        KeyDic[0x39] = L"SPACE";
        KeyDic[0x3A] = L"CAPITAL";
        KeyDic[0x3B] = L"F1";
        KeyDic[0x3C] = L"F2";
        KeyDic[0x3D] = L"F3";
        KeyDic[0x3E] = L"F4";
        KeyDic[0x3F] = L"F5";
        KeyDic[0x40] = L"F6";
        KeyDic[0x41] = L"F7";
        KeyDic[0x42] = L"F8";
        KeyDic[0x43] = L"F9";
        KeyDic[0x44] = L"F10";
        KeyDic[0x45] = L"NUMLOCK";
        KeyDic[0x46] = L"SCROLL";
        KeyDic[0x47] = L"NUMPAD7";
        KeyDic[0x48] = L"NUMPAD8";
        KeyDic[0x49] = L"NUMPAD9";
        KeyDic[0x4A] = L"SUBTRACT";
        KeyDic[0x4B] = L"NUMPAD4";
        KeyDic[0x4C] = L"NUMPAD5";
        KeyDic[0x4D] = L"NUMPAD6";
        KeyDic[0x4E] = L"ADD";
        KeyDic[0x4F] = L"NUMPAD1";
        KeyDic[0x50] = L"NUMPAD2";
        KeyDic[0x51] = L"NUMPAD3";
        KeyDic[0x52] = L"NUMPAD0";
        KeyDic[0x53] = L"DECIMAL";
        KeyDic[0x56] = L"OEM_102";
        KeyDic[0x57] = L"F11";
        KeyDic[0x58] = L"F12";
        KeyDic[0x64] = L"F13";
        KeyDic[0x65] = L"F14";
        KeyDic[0x66] = L"F15";
        KeyDic[0x70] = L"KANA";
        KeyDic[0x73] = L"ABNT_C1";
        KeyDic[0x79] = L"CONVERT";
        KeyDic[0x7B] = L"NOCONVERT";
        KeyDic[0x7D] = L"YEN";
        KeyDic[0x7E] = L"ABNT_C2";
        KeyDic[0x8D] = L"NUMPADEQUALS";
        KeyDic[0x90] = L"PREVTRACK";
        KeyDic[0x91] = L"AT";
        KeyDic[0x92] = L"COLON";
        KeyDic[0x93] = L"UNDERLINE";
        KeyDic[0x94] = L"KANJI";
        KeyDic[0x95] = L"STOP";
        KeyDic[0x96] = L"AX";
        KeyDic[0x97] = L"UNLABELED";
        KeyDic[0x99] = L"NEXTTRACK";
        KeyDic[0x9C] = L"NUMPADENTER";
        KeyDic[0x9D] = L"RCONTROL";
        KeyDic[0xA0] = L"MUTE";
        KeyDic[0xA1] = L"CALCULATOR";
        KeyDic[0xA2] = L"PLAYPAUSE";
        KeyDic[0xA4] = L"MEDIASTOP";
        KeyDic[0xAE] = L"VOLUMEDOWN";
        KeyDic[0xB0] = L"VOLUMEUP";
        KeyDic[0xB2] = L"WEBHOME";
        KeyDic[0xB3] = L"NUMPADCOMMA";
        KeyDic[0xB5] = L"DIVIDE";
        KeyDic[0xB7] = L"SYSRQ";
        KeyDic[0xB8] = L"RMENU";
        KeyDic[0xC5] = L"PAUSE";
        KeyDic[0xC7] = L"HOME";
        KeyDic[0xC8] = L"UP";
        KeyDic[0xC9] = L"PRIOR";
        KeyDic[0xCB] = L"LEFT";
        KeyDic[0xCD] = L"RIGHT";
        KeyDic[0xCF] = L"END";
        KeyDic[0xD0] = L"DOWN";
        KeyDic[0xD1] = L"NEXT";
        KeyDic[0xD2] = L"INSERT";
        KeyDic[0xD3] = L"DELETE";
        KeyDic[0xDB] = L"LWIN";
        KeyDic[0xDC] = L"RWIN";
        KeyDic[0xDD] = L"APPS";
        KeyDic[0xDE] = L"POWER";
        KeyDic[0xDF] = L"SLEEP";
        KeyDic[0xE3] = L"WAKE";
        KeyDic[0xE5] = L"WEBSEARCH";
        KeyDic[0xE6] = L"WEBFAVORITES";
        KeyDic[0xE7] = L"WEBREFRESH";
        KeyDic[0xE8] = L"WEBSTOP";
        KeyDic[0xE9] = L"WEBFORWARD";
        KeyDic[0xEA] = L"WEBBACK";
        KeyDic[0xEB] = L"MYCOMPUTER";
        KeyDic[0xEC] = L"MAIL";
        KeyDic[0xED] = L"MEDIASELECT";

        for (int i = 0; i < 256; i++)
        {
            KeyData[i] = -1;
        }
    }

    void InputDetect::UpdateKeyDic(float deltatime)
    {
        for (int i = 0; i < 256; i++)
        {
            if (keystate[i] & 0x80)
            {
                if (KeyData[i] <= -1)
                {
                    KeyData[i] = 0;
                }
                else
                {
                    KeyData[i] += deltatime;
                }
            }
            else
            {
                if (KeyData[i] > 0)
                {
                    KeyData[i] = -2;
                }
                else
                {
                    KeyData[i] = -1;
                }
            }
        }
    }


    float InputDetect::GetMouseData(InputMouse key)
    {
        return MouseData[(int)key];
    }

    bool InputDetect::MouseDown(InputMouse key)
    {
        if (GetMouseData(key) == 0)
        {
            WIN32DEBUG(L"鼠标 %s 键 按下\n", MouseDic[(int)key].c_str());
            return true;
        }
        else
        {
            return false;
        }
    }

    bool InputDetect::MouseDownContinued(InputMouse key)
    {
        if (GetMouseData(key) > 0)
        {
            //WIN32DEBUG(L"鼠标 %s 键 持续按下\n",  MouseDic[(int)key].c_str());
            return true;
        }
        else
        {
            return false;
        }
    }

    bool InputDetect::MouseUp(InputMouse key)
    {
        if (GetMouseData(key) == -2)
        {
            WIN32DEBUG(L"鼠标 %s 键 松开\n", MouseDic[(int)key].c_str());
            return true;
        }
        else
        {
            return false;
        }
    }

    float InputDetect::GetKeyData(InputKey key)
    {
        return KeyData[(int)key];
    }

    bool InputDetect::KeyDown(InputKey key)
    {
        if (GetKeyData(key) == 0)
        {
            WIN32DEBUG(L"键盘 %s 键 按下\n", KeyDic[int(key)].c_str());
            return true;
        }
        else
        {
            return false;
        }
    }

    bool InputDetect::KeyDownContinued(InputKey key)
    {
        if (GetKeyData(key) > 0)
        {
            //WIN32DEBUG(L"键盘 %s 键 持续按下\n", KeyDic[int(key)].c_str());
            return true;
        }
        else
        {
            return false;
        }
    }

    bool InputDetect::KeyUp(InputKey key)
    {
        if (GetKeyData(key) == -2)
        {
            WIN32DEBUG(L"键盘 %s 键 松开\n", KeyDic[int(key)].c_str());
            return true;
        }
        else
        {
            return false;
        }
    }
}


