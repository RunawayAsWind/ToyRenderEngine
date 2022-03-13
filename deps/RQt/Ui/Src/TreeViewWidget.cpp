#include "../Include/TreeViewWidget.h"

TreeViewWidget::TreeViewWidget(QWidget* parent) : QDockWidget(parent)
{
    RQt::Qt_SetColor(this, QColor(200, 200, 200));
    
    treeWidget = new QTreeWidget;
    RQt::Qt_SetColor(treeWidget, QColor(200, 200, 200));
    treeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    
    connect(treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)),
        this, SLOT(slotSelectChildren(QTreeWidgetItem*,int)));

    this->setWidget(treeWidget);

    InitTree();

}


TreeViewWidget::~TreeViewWidget()
{

}

void TreeViewWidget::InitTree()
{
    treeWidget->setColumnCount(2);
    rootList.append(new QTreeWidgetItem(static_cast<QTreeWidget*>(nullptr), QStringList(QString("Scene"))));
    
    for (int i = 0; i < 3; i++)
    {
        QTreeWidgetItem* node = RQt::AddChild(rootList.at(0), 0, QString("Node %1").arg(i));
        for (int j = 0; j < 3; j++)
        {
            RQt::AddChild(node, 0, QString("Child %1").arg(j));
        }
    }

    treeWidget->insertTopLevelItems(0, rootList);  //将结点插入部件中
    treeWidget->expandAll(); //全部展开
}

void TreeViewWidget::AddItem(QTreeWidgetItem* parItem, QString name)
{

}

void TreeViewWidget::slotSelectChildren(QTreeWidgetItem* item, int column)
{
    qDebug() << treeWidget->currentIndex();
    QList<QTreeWidgetItem*> selectedItemList = treeWidget->selectedItems();
    for (auto node : selectedItemList)
    {
        qDebug() << node->text(0);
    }
}