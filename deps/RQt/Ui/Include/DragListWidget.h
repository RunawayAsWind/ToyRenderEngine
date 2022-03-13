#pragma once

#include <QListWidget>
#include <QMouseEvent>
#include <functional>
/*
Goal QObject must has
GoalQObject::GoalQObject(QWidget *parent)
{
	// 在需要接受放下数据的部件上调用setAcceptDrops()函数以使该部件能接受拖放事件!!!!!!
	setAcceptDrops(true);
}

void GoalQObject::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->source() == this)
        event->ignore();
    else
        event->accept();
}

void GoalQObject::dropEvent(QDropEvent* event)
{
	auto src = event->source();
	event->pos();
	event->setDropAction(Qt::LinkAction);
	event->accept();
	event->mimeData();
	
	QString fname = event->mimeData()->text();   // 取得放置item携带的数据，这里是文件名
	std::string dragFilePath = std::string((const char*)fname.toLocal8Bit());
	
	//Do something with dragFilePath.......

	qDebug() << "  X:" << event->pos().x()<< "  Y:" << event->pos().y()<< "  DragFile: " << fname;
}
*/
class DragListWidget : public QListWidget
{
    Q_OBJECT

public:
    DragListWidget(QWidget* parent = nullptr);
    ~DragListWidget();
protected:
    void mousePressEvent(QMouseEvent* event) override;
};