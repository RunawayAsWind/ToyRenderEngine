#pragma once

#include <QtWidgets>
#include <QDebug.h>
#include <RQt/QtUi.h>

class TreeViewWidget : public QDockWidget
{
    Q_OBJECT

public:
    TreeViewWidget(QWidget* parent = Q_NULLPTR);
    ~TreeViewWidget();


public slots:
    void InitTree();
    void AddItem(QTreeWidgetItem* parItem, QString name);//添加一个目录节点
    void slotSelectChildren(QTreeWidgetItem* item, int column);

private:
    QTreeWidget* treeWidget;
    QList<QTreeWidgetItem*> rootList;
};
