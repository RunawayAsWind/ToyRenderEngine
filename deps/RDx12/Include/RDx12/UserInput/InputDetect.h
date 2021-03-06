#pragma once
#include <DInput.h>
#include <unordered_map>
#pragma comment(lib, "dinput8.lib") 
#pragma comment(lib, "dxguid.lib")
namespace RDx12
{
    enum class InputKey
    {
        IK_ESCAPE = 0x01,
        IK_1 = 0x02,
        IK_2 = 0x03,
        IK_3 = 0x04,
        IK_4 = 0x05,
        IK_5 = 0x06,
        IK_6 = 0x07,
        IK_7 = 0x08,
        IK_8 = 0x09,
        IK_9 = 0x0A,
        IK_0 = 0x0B,
        IK_MINUS = 0x0C,
        IK_EQUALS = 0x0D,
        IK_BACK = 0x0E,
        IK_TAB = 0x0F,
        IK_Q = 0x10,
        IK_W = 0x11,
        IK_E = 0x12,
        IK_R = 0x13,
        IK_T = 0x14,
        IK_Y = 0x15,
        IK_U = 0x16,
        IK_I = 0x17,
        IK_O = 0x18,
        IK_P = 0x19,
        IK_LBRACKET = 0x1A,
        IK_RBRACKET = 0x1B,
        IK_RETURN = 0x1C,
        IK_LCONTROL = 0x1D,
        IK_A = 0x1E,
        IK_S = 0x1F,
        IK_D = 0x20,
        IK_F = 0x21,
        IK_G = 0x22,
        IK_H = 0x23,
        IK_J = 0x24,
        IK_K = 0x25,
        IK_L = 0x26,
        IK_SEMICOLON = 0x27,
        IK_APOSTROPHE = 0x28,
        IK_GRAVE = 0x29,
        IK_LSHIFT = 0x2A,
        IK_BACKSLASH = 0x2B,
        IK_Z = 0x2C,
        IK_X = 0x2D,
        IK_C = 0x2E,
        IK_V = 0x2F,
        IK_B = 0x30,
        IK_N = 0x31,
        IK_M = 0x32,
        IK_COMMA = 0x33,
        IK_PERIOD = 0x34,
        IK_SLASH = 0x35,
        IK_RSHIFT = 0x36,
        IK_MULTIPLY = 0x37,
        IK_LMENU = 0x38,
        IK_SPACE = 0x39,
        IK_CAPITAL = 0x3A,
        IK_F1 = 0x3B,
        IK_F2 = 0x3C,
        IK_F3 = 0x3D,
        IK_F4 = 0x3E,
        IK_F5 = 0x3F,
        IK_F6 = 0x40,
        IK_F7 = 0x41,
        IK_F8 = 0x42,
        IK_F9 = 0x43,
        IK_F10 = 0x44,
        IK_NUMLOCK = 0x45,
        IK_SCROLL = 0x46,
        IK_NUMPAD7 = 0x47,
        IK_NUMPAD8 = 0x48,
        IK_NUMPAD9 = 0x49,
        IK_SUBTRACT = 0x4A,
        IK_NUMPAD4 = 0x4B,
        IK_NUMPAD5 = 0x4C,
        IK_NUMPAD6 = 0x4D,
        IK_ADD = 0x4E,
        IK_NUMPAD1 = 0x4F,
        IK_NUMPAD2 = 0x50,
        IK_NUMPAD3 = 0x51,
        IK_NUMPAD0 = 0x52,
        IK_DECIMAL = 0x53,
        IK_OEM_102 = 0x56,
        IK_F11 = 0x57,
        IK_F12 = 0x58,
        IK_F13 = 0x64,
        IK_F14 = 0x65,
        IK_F15 = 0x66,
        IK_KANA = 0x70,
        IK_ABNT_C1 = 0x73,
        IK_CONVERT = 0x79,
        IK_NOCONVERT = 0x7B,
        IK_YEN = 0x7D,
        IK_ABNT_C2 = 0x7E,
        IK_NUMPADEQUALS = 0x8D,
        IK_PREVTRACK = 0x90,
        IK_AT = 0x91,
        IK_COLON = 0x92,
        IK_UNDERLINE = 0x93,
        IK_KANJI = 0x94,
        IK_STOP = 0x95,
        IK_AX = 0x96,
        IK_UNLABELED = 0x97,
        IK_NEXTTRACK = 0x99,
        IK_NUMPADENTER = 0x9C,
        IK_RCONTROL = 0x9D,
        IK_MUTE = 0xA0,
        IK_CALCULATOR = 0xA1,
        IK_PLAYPAUSE = 0xA2,
        IK_MEDIASTOP = 0xA4,
        IK_VOLUMEDOWN = 0xAE,
        IK_VOLUMEUP = 0xB0,
        IK_WEBHOME = 0xB2,
        IK_NUMPADCOMMA = 0xB3,
        IK_DIVIDE = 0xB5,
        IK_SYSRQ = 0xB7,
        IK_RMENU = 0xB8,
        IK_PAUSE = 0xC5,
        IK_HOME = 0xC7,
        IK_UP = 0xC8,
        IK_PRIOR = 0xC9,
        IK_LEFT = 0xCB,
        IK_RIGHT = 0xCD,
        IK_END = 0xCF,
        IK_DOWN = 0xD0,
        IK_NEXT = 0xD1,
        IK_INSERT = 0xD2,
        IK_DELETE = 0xD3,
        IK_LWIN = 0xDB,
        IK_RWIN = 0xDC,
        IK_APPS = 0xDD,
        IK_POWER = 0xDE,
        IK_SLEEP = 0xDF,
        IK_WAKE = 0xE3,
        IK_WEBSEARCH = 0xE5,
        IK_WEBFAVORITES = 0xE6,
        IK_WEBREFRESH = 0xE7,
        IK_WEBSTOP = 0xE8,
        IK_WEBFORWARD = 0xE9,
        IK_WEBBACK = 0xEA,
        IK_MYCOMPUTER = 0xEB,
        IK_MAIL = 0xEC,
        IK_MEDIASELECT = 0xED,
    };

    enum class InputMouse
    {
        IM_X = 0,
        IM_Y = 1,
        IM_Z = 2,
        IM_LEFT = 3,
        IM_RIGHT = 4,
        IM_MIDDLE = 5,
    };
    //https://docs.microsoft.com/en-us/previous-versions/windows/desktop/ee416842(v=vs.85)
    class InputDetect
    {

    public:
        InputDetect(HINSTANCE hinst, HWND hWnd);
        void GetInput(float deltatime);

        float GetMouseData(InputMouse key);
        bool MouseDown(InputMouse key);
        bool MouseDownContinued(InputMouse key);
        bool MouseUp(InputMouse key);

        float GetKeyData(InputKey key);
        bool KeyDown(InputKey key);
        bool KeyDownContinued(InputKey key);
        bool KeyUp(InputKey key);

    private:
        LPDIRECTINPUT8 dinput8 = NULL;
        LPDIRECTINPUTDEVICE8   keydev = NULL;
        LPDIRECTINPUTDEVICE8   mousedev = NULL;
        char keystate[256] = { 0 };
        DIMOUSESTATE mousestate = { 0 };
        float KeyData[256];
        float MouseData[256];
        std::wstring KeyDic[256];
        std::wstring MouseDic[256];

        void InitKeyDic();
        void InitMouseDic();
        void UpdateMouseDic(float deltatime);
        void UpdateKeyDic(float deltatime);
        BOOL Device_Read(IDirectInputDevice8* pDIDevice, void* pBuffer, long lSize);
    };


}