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
	void ShowNowPathItem(); //根据获取的QFileInfoList显示本地目录
    void DoubleClickItem(QListWidgetItem* item);   //由双击的文件确定新的目录
};
