#include "../Include/QtGuiZero00.h"
#include<RImgui/ImMain.h>
#include<RTmpl/STLHelp.h>
#include <functional>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

QtGuiZero00::QtGuiZero00(QWidget *parent): QMainWindow(parent)
{
	ui.setupUi(this);
	RQt::Qt_MoveWinToCenter(this,0.7,0.7,true);
	
	auto imguiWndProc = std::function(ImGui_ImplWin32_WndProcHandler);
	renderWidget = new Win32RenderWidget(this);
	renderWidget->SetImguiWndProc(imguiWndProc);
	setCentralWidget(renderWidget);

	projectWidget = new FileViewWidget(this,QString(GobalPath().AssetsPath.c_str()));
	addDockWidget(Qt::BottomDockWidgetArea, projectWidget);
	resizeDocks({ projectWidget }, { int(this->height() * 0.2) }, Qt::Vertical);
	
	//hierarchyWidget = new TreeViewWidget(this);
	//addDockWidget(Qt::LeftDockWidgetArea, hierarchyWidget);
	//resizeDocks({ hierarchyWidget }, { int(this->width() * 0.2) }, Qt::Horizontal);

	//inspectorWidget = new QDockWidget(this);
	//inspectorWidget->setAttribute(Qt::WA_DeleteOnClose);
	//addDockWidget(Qt::RightDockWidgetArea, inspectorWidget);
	//RQt::Qt_SetColor(inspectorWidget, Qt::gray);
	//resizeDocks({ inspectorWidget }, { int(this->width() * 0.2) }, Qt::Horizontal);

	//recogWidget = new RecogWidget(this);
	//addDockWidget(Qt::LeftDockWidgetArea, recogWidget);
	//resizeDocks({ recogWidget }, { int(this->width() * 0.8) }, Qt::Horizontal);

	std::cout << renderWidget->window() << std::endl;

	// 在需要接受放下数据的部件上调用setAcceptDrops()函数以使该部件能接受拖放事件!!!!!!
	setAcceptDrops(true);
}

void QtGuiZero00::showEvent(QShowEvent* event)
{
	if (rdth == nullptr)
	{
		rdth = new RDx12::RenderThread<TestScene001_Base>(GetModuleHandle(NULL), renderWidget->window());
		//recogWidget->rdth = rdth;
		rdth->Load();
		rdth->Start();
	}
	else
	{
		rdth->Resume();
	}
}

void QtGuiZero00::hideEvent(QHideEvent* event)
{
	rdth->Stop();
}

void QtGuiZero00::closeEvent(QCloseEvent* event)
{
	rdth->Quit();
}

void QtGuiZero00::dragEnterEvent(QDragEnterEvent* event)
{
	if (event->source() == this)
		event->ignore();
	else
		event->accept();
}

void QtGuiZero00::dropEvent(QDropEvent* event)
{
	auto src = event->source();

	event->pos();

	event->setDropAction(Qt::LinkAction);
	event->accept();

	event->mimeData();

	QString fname = event->mimeData()->text();   // 取得放置item携带的数据，这里是文件名


	std::string dragFilePath = std::string((const char*)fname.toLocal8Bit());
	rdth->fw.messageQue.push(
		[dragFilePath](TestScene001_Base* base) {
			base->Ui_SetDragFilePath(dragFilePath);
		}
	);

	/*qDebug() << "*** X:" << event->pos().x();

	qDebug() << "*** Y:" << event->pos().y();*/

	qDebug() << "DragFile: "<<fname;

}

