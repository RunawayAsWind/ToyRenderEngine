#pragma once
#include "qwinhost.h"
#include <QtWidgets>
#include <functional>

/*
Imgui仅提供了Win32的消息处理函数WndProc，因此虽然可以在Qt里绘制，但无法进行交互（只能看到控件，无法点击拖拽）
为了使得Qt内可以使用交互性的Imgui，需要将Win32窗口和Win32的WndProc嵌入Qt
嵌入的关键是使用第三方代码QWinHost，可以通过继承来实现Win32和WndProc

该类Win32RenderWidget用于Dx12的渲染，由于内嵌了Win32窗口和WndProc，可以进行交互式的Imgui渲染

使用方法如下
1.声明Imgui的WnProc，不用定义，因为已经在Imgui代码中定义好了
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);   

2.在渲染窗口的父类窗口中添加成员，和show函数
class QtGuiZero00 : public QMainWindow
{
	Q_OBJECT

protect:
	Win32RenderWidget *renderWidget;
	RDx12::RenderThread<TestScene001_Base>* rdth;

	void showEvent(QShowEvent* event) override;  
};

3.在父类窗口的构造函数中传入Imgui的WndProc
QtGuiZero00::QtGuiZero00(QWidget *parent): QMainWindow(parent)
{
	auto imguiWndProc = std::function(ImGui_ImplWin32_WndProcHandler);
	renderWidget = new Win32RenderWidget(this);
	renderWidget->SetImguiWndProc(imguiWndProc);
	setCentralWidget(renderWidget);
}

4.由于QWinHost内部的createWindow需在QWidget.show()之后会调用生成Hwnd窗口句柄，因此，我们只能通过showEvent来初始化渲染
注意其中的renderWidget->window()即是获取Hwnd，


void QtGuiZero00::showEvent(QShowEvent* event)//此函数再show()后自动调用，renderWidget->window()的值已经初始化完毕
{
	rdth = new RDx12::RenderThread<TestScene001_Base>(GetModuleHandle(NULL), renderWidget->window());
	rdth->Load();
	rdth->Start();
}

但如果是再show之前获取，例如下面想在构造函数里面直接开启渲染
QtGuiZero00::QtGuiZero00(QWidget *parent): QMainWindow(parent)
{
	auto imguiWndProc = std::function(ImGui_ImplWin32_WndProcHandler);
	renderWidget = new Win32RenderWidget(this);
	renderWidget->SetImguiWndProc(imguiWndProc);
	setCentralWidget(renderWidget);

	rdth = new RDx12::RenderThread<TestScene001_Base>(GetModuleHandle(NULL), renderWidget->window()); //此时renderWidget->window()的值为0x000000(空值),会导致交换链创建失败
	rdth->Load();
	rdth->Start();
}
*/
class Win32RenderWidget : public QWinHost
{
    Q_OBJECT

public:
    Win32RenderWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);
    HWND createWindow(HWND parent, HINSTANCE instance)  override;
    void SetImguiWndProc(std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)> func);
protected:
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    static std::function <LRESULT(HWND, UINT, WPARAM, LPARAM)> imguiWndProc;
};