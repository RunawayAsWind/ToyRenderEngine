#include "../Include/HostWindow.h"

HostWindow::HostWindow(QWidget* parent, Qt::WindowFlags f): QWinHost(parent, f)
{
    setFocusPolicy(Qt::StrongFocus);
}

HWND HostWindow::createWindow(HWND parent, HINSTANCE instance)
{
    static ATOM windowClass = 0;
    if (!windowClass) {
        WNDCLASSEX wcex;
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = (WNDPROC)WndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = instance;
        wcex.hIcon = NULL;
        wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)(COLOR_HIGHLIGHT + 1);
        wcex.lpszMenuName = NULL;
        wcex.lpszClassName = (LPCSTR)"qtest";
        wcex.hIconSm = NULL;

        windowClass = RegisterClassEx(&wcex);
    }

    HWND hwnd = CreateWindow((TCHAR*)windowClass, 0, WS_CHILD | WS_CLIPSIBLINGS | WS_TABSTOP,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, parent, NULL, instance, NULL);

    return hwnd;
}

void HostWindow::returnPressed()
{
    QMessageBox::information(topLevelWidget(), "Message from Qt", "Return pressed in QLineEdit!");
}

LRESULT CALLBACK HostWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    QWidget* widget = QWidget::find((WId)GetParent(hWnd));
    HostWindow* window = qobject_cast<HostWindow*>(widget);
    if (window) switch (message) {
    case WM_SETFOCUS:
        window->message("SetFocus for Win32 window!", 1000);
        break;
    case WM_KILLFOCUS:
        window->message("KillFocus for Win32 window!", 1000);
        break;
    case WM_MOUSEMOVE:
        window->message("Moving the mouse, aren't we?", 200);
        break;
    case WM_KEYDOWN:
        if (wParam != VK_TAB)
            window->message("Key Pressed!", 500);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}