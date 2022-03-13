#pragma once
#include <QtWidgets/QMainWindow>
#include <QCloseEvent>
#include "../GenCode/UiHeader/ui_QtGuiZero00.h"

#include "../../Global/GobalPath.h"
#include <RQt/QtUi.h>

#include <RDx12/Render/RenderThread.h>
#include "../../Include/FrameWork_Release/TestScene001_Base.h"
class QtGuiZero00 : public QMainWindow
{
	Q_OBJECT

public:
	QtGuiZero00(QWidget* parent = Q_NULLPTR);

private:
	Ui::QtGuiZero00 ui;
	Win32RenderWidget *renderWidget;
	FileViewWidget *projectWidget;
	TreeViewWidget *hierarchyWidget;
	QDockWidget *inspectorWidget;
	RDx12::RenderThread<TestScene001_Base>* rdth=nullptr;

protected:
	/*
	�¼�event������źźͲ�����������۲���ģʽ�����յ��¼����������ú�����Ӧ��û�а�һ˵
	Qt��ԭ������QWidget�ڲ��зḻ��Ĭ���¼����������ǿ���ͨ����д���������������еĲ�������show��close��update
	����Դ��Ĳ��裺��QWidget�Ҽ�-��ת�����塱-�����ؼ��֡�event�����Կ����ܶ��麯��
	*/
	void showEvent(QShowEvent* event) override;
	void hideEvent(QHideEvent* event) override;
	void closeEvent(QCloseEvent* event) override;
	void dragEnterEvent(QDragEnterEvent* event) override;
	void dropEvent(QDropEvent* event) override;
};
