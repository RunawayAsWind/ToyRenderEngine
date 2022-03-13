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
	事件event相对于信号和槽来讲，更像观察者模式，即收到事件，立即调用函数相应，没有绑定一说
	Qt的原生类如QWidget内部有丰富的默认事件函数，我们可以通过重写来进行生命周期中的操作，如show，close，update
	查找源码的步骤：对QWidget右键-“转到定义”-搜索关键字“event”可以看到很多虚函数
	*/
	void showEvent(QShowEvent* event) override;
	void hideEvent(QHideEvent* event) override;
	void closeEvent(QCloseEvent* event) override;
	void dragEnterEvent(QDragEnterEvent* event) override;
	void dropEvent(QDropEvent* event) override;
};
