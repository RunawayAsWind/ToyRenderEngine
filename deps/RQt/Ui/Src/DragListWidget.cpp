#include <RQt/QtUi.h>
#include <QDrag>
#include <QMimeData>
#include "../Include/DragListWidget.h"

DragListWidget::DragListWidget(QWidget* parent)
{
	setViewMode(QListView::IconMode);	//图标方式显示（类似于windows快捷方式，图标+name）
	setFlow(QListView::LeftToRight);	//从左到右显示Item
	setWrapping(true);					//Item满格后自动换行
	setIconSize(QSize(60, 60));
	setUniformItemSizes(true);
}
DragListWidget::~DragListWidget()
{

}
void DragListWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        QListWidget::mousePressEvent(event);//调用基类函数,防止覆写掉基本的点击功能
        QListWidgetItem* curItem = currentItem();

        if (curItem != nullptr)//如果点击了Item
        {
            QDrag* drag = new QDrag(this);
            QString fname = curItem->data(4).toString(); //  获取Item携带的路径数据，在FileViewWidget.cpp代码中可知数据存于data(4)

            QMimeData* data = new QMimeData;
            data->setText(fname);

            drag->setMimeData(data);
            drag->exec(Qt::MoveAction);
        }
    }
}

