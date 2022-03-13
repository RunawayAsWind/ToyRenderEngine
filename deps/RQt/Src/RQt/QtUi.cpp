#include "../Include/RQt/QtUi.h"

namespace RQt
{
	void Qt_MoveWinToCenter(QWidget* wid, float w, float h, bool percent)
	{
		int nScreenWidth, nScreenHeight;
		nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
		nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

		int nWidth, nHeight;
		if (percent == false)
		{
			nWidth = w;
			nHeight = h;
		}
		else
		{
			nWidth = nScreenWidth * w;
			nHeight = nScreenHeight * h;
		}
		int nLefttopx = nScreenWidth / 2 - nWidth / 2;
		int nLefttopy = nScreenHeight / 2 - nHeight / 2;
		wid->setGeometry(nLefttopx, nLefttopy, nWidth, nHeight);
	}

	void Qt_SetColor(QWidget* wid, QColor color)
	{
		QPalette pal(wid->palette());
		pal.setColor(QPalette::Background, color);
		wid->setAutoFillBackground(true);
		wid->setPalette(pal);
	}

	//https://stackoverflow.com/questions/4272196/qt-remove-all-widgets-from-layout
	void ClearLayout(QLayout* layout, bool deleteWidgets)
	{
		while (QLayoutItem* item = layout->takeAt(0))
		{
			if (deleteWidgets)
			{
				if (QWidget* widget = item->widget())
					widget->deleteLater();
			}
			if (QLayout* childLayout = item->layout())
				ClearLayout(childLayout, deleteWidgets);
			delete item;
		}
	}

	QString TreeItemToFullPath(QTreeWidgetItem* treeItem, int dataColumn)
	{
		QString fullPath = treeItem->text(dataColumn);

		while (treeItem->parent() != NULL)
		{
			fullPath = treeItem->parent()->text(dataColumn) + "/" + fullPath;
			treeItem = treeItem->parent();
		}
		return fullPath;
	}

	QTreeWidgetItem* AddChild(QTreeWidgetItem* root, int dataColumn, const QString& atext)
	{
		QTreeWidgetItem* newNode = new QTreeWidgetItem();
		newNode->setText(dataColumn, atext);
		root->addChild(newNode);
		newNode->setToolTip(0, RQt::TreeItemToFullPath(newNode, dataColumn));
		return newNode;
	}
}