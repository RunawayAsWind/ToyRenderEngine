#include <RQt/QtUi.h>
#include <QDrag>
#include <QMimeData>
#include "../Include/DragListWidget.h"

DragListWidget::DragListWidget(QWidget* parent)
{
	setViewMode(QListView::IconMode);	//ͼ�귽ʽ��ʾ��������windows��ݷ�ʽ��ͼ��+name��
	setFlow(QListView::LeftToRight);	//��������ʾItem
	setWrapping(true);					//Item������Զ�����
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
        QListWidget::mousePressEvent(event);//���û��ຯ��,��ֹ��д�������ĵ������
        QListWidgetItem* curItem = currentItem();

        if (curItem != nullptr)//��������Item
        {
            QDrag* drag = new QDrag(this);
            QString fname = curItem->data(4).toString(); //  ��ȡItemЯ����·�����ݣ���FileViewWidget.cpp�����п�֪���ݴ���data(4)

            QMimeData* data = new QMimeData;
            data->setText(fname);

            drag->setMimeData(data);
            drag->exec(Qt::MoveAction);
        }
    }
}

