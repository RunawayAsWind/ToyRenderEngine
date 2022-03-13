#pragma once
#include "../../Ui/Include/Win32RenderWidget.h"
#include "../../Ui/Include/FileViewWidget.h"
#include "../../Ui/Include/TreeViewWidget.h"
#include <QtWidgets>

namespace RQt
{
	void Qt_MoveWinToCenter(QWidget* wid, float w, float h, bool percent);

	void Qt_SetColor(QWidget* wid, QColor color);

	void ClearLayout(QLayout* layout, bool deleteWidgets = true);

	QString TreeItemToFullPath(QTreeWidgetItem* treeItem, int dataColumn);

	QTreeWidgetItem* AddChild(QTreeWidgetItem* root, int dataColumn, const QString& atext);
}