#pragma once
#include "QWinHost.h"
#include <QtWidgets>

class HostWindow : public QWinHost
{
    Q_OBJECT
public:
    HostWindow(QWidget* parent = 0, Qt::WindowFlags f = 0);
    HWND createWindow(HWND parent, HINSTANCE instance);
signals:
    void message(const QString& msg, int timeout);

public slots:
    void returnPressed();
protected:

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};