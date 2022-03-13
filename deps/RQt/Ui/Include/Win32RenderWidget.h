#pragma once
#include "qwinhost.h"
#include <QtWidgets>
#include <functional>

/*
Imgui���ṩ��Win32����Ϣ������WndProc�������Ȼ������Qt����ƣ����޷����н�����ֻ�ܿ����ؼ����޷������ק��
Ϊ��ʹ��Qt�ڿ���ʹ�ý����Ե�Imgui����Ҫ��Win32���ں�Win32��WndProcǶ��Qt
Ƕ��Ĺؼ���ʹ�õ���������QWinHost������ͨ���̳���ʵ��Win32��WndProc

����Win32RenderWidget����Dx12����Ⱦ��������Ƕ��Win32���ں�WndProc�����Խ��н���ʽ��Imgui��Ⱦ

ʹ�÷�������
1.����Imgui��WnProc�����ö��壬��Ϊ�Ѿ���Imgui�����ж������
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);   

2.����Ⱦ���ڵĸ��ര������ӳ�Ա����show����
class QtGuiZero00 : public QMainWindow
{
	Q_OBJECT

protect:
	Win32RenderWidget *renderWidget;
	RDx12::RenderThread<TestScene001_Base>* rdth;

	void showEvent(QShowEvent* event) override;  
};

3.�ڸ��ര�ڵĹ��캯���д���Imgui��WndProc
QtGuiZero00::QtGuiZero00(QWidget *parent): QMainWindow(parent)
{
	auto imguiWndProc = std::function(ImGui_ImplWin32_WndProcHandler);
	renderWidget = new Win32RenderWidget(this);
	renderWidget->SetImguiWndProc(imguiWndProc);
	setCentralWidget(renderWidget);
}

4.����QWinHost�ڲ���createWindow����QWidget.show()֮����������Hwnd���ھ������ˣ�����ֻ��ͨ��showEvent����ʼ����Ⱦ
ע�����е�renderWidget->window()���ǻ�ȡHwnd��


void QtGuiZero00::showEvent(QShowEvent* event)//�˺�����show()���Զ����ã�renderWidget->window()��ֵ�Ѿ���ʼ�����
{
	rdth = new RDx12::RenderThread<TestScene001_Base>(GetModuleHandle(NULL), renderWidget->window());
	rdth->Load();
	rdth->Start();
}

���������show֮ǰ��ȡ�������������ڹ��캯������ֱ�ӿ�����Ⱦ
QtGuiZero00::QtGuiZero00(QWidget *parent): QMainWindow(parent)
{
	auto imguiWndProc = std::function(ImGui_ImplWin32_WndProcHandler);
	renderWidget = new Win32RenderWidget(this);
	renderWidget->SetImguiWndProc(imguiWndProc);
	setCentralWidget(renderWidget);

	rdth = new RDx12::RenderThread<TestScene001_Base>(GetModuleHandle(NULL), renderWidget->window()); //��ʱrenderWidget->window()��ֵΪ0x000000(��ֵ),�ᵼ�½���������ʧ��
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