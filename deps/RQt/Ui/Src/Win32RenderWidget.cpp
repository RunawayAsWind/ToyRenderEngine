#include "../Include/Win32RenderWidget.h"

std::function <LRESULT(HWND, UINT, WPARAM, LPARAM)> Win32RenderWidget::imguiWndProc;

Win32RenderWidget::Win32RenderWidget(QWidget* parent, Qt::WindowFlags f): QWinHost(parent, f)
{
    setFocusPolicy(Qt::StrongFocus);
}

HWND Win32RenderWidget::createWindow(HWND parent, HINSTANCE instance)
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

void Win32RenderWidget::SetImguiWndProc(std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)> func)
{
    imguiWndProc = func;
}

LRESULT CALLBACK Win32RenderWidget::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (imguiWndProc(hWnd, message, wParam, lParam))
        return true;
    QWidget* widget = QWidget::find((WId)GetParent(hWnd));
    Win32RenderWidget* window = qobject_cast<Win32RenderWidget*>(widget);
    if (window) switch (message) {
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}