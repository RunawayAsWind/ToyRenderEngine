#pragma once

#include <QListWidget>
#include <QMouseEvent>
#include <functional>
/*
Goal QObject must has
GoalQObject::GoalQObject(QWidget *parent)
{
	// ����Ҫ���ܷ������ݵĲ����ϵ���setAcceptDrops()������ʹ�ò����ܽ����Ϸ��¼�!!!!!!
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
	
	QString fname = event->mimeData()->text();   // ȡ�÷���itemЯ�������ݣ��������ļ���
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