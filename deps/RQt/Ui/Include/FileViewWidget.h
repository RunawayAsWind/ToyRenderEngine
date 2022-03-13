#pragma once
#include <QtWidgets>
#include "DragListWidget.h"

class FileViewWidget : public QDockWidget
{
    Q_OBJECT

public:
    FileViewWidget(QWidget* parent = Q_NULLPTR, QString assetsDir="./");
    ~FileViewWidget();
    DragListWidget *dragListWidget;
    QString rootPath;
    QString currentPath;
    QString projPath;
	QDir dir;

public slots:
	void ShowNowPathItem(); //���ݻ�ȡ��QFileInfoList��ʾ����Ŀ¼
    void DoubleClickItem(QListWidgetItem* item);   //��˫�����ļ�ȷ���µ�Ŀ¼
};
