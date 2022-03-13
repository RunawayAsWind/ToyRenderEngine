#include "../Include/FileViewWidget.h"
#include <QDir>
#include <QDebug.h>
#include <RQt/QtUi.h>
#include <QFileInfoList>

FileViewWidget::FileViewWidget(QWidget* parent, QString assetsDir) : QDockWidget(parent),rootPath(assetsDir)
{
	dragListWidget = new DragListWidget;
	this->setWidget(dragListWidget);

    connect(dragListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
        this, SLOT(DoubleClickItem(QListWidgetItem*)));
	
	dir.setPath(dir.absolutePath()+"/"+ rootPath);
	dir.setSorting(QDir::Type);
	currentPath = dir.absolutePath();
	rootPath = dir.absolutePath();
	projPath = dir.currentPath();
    ShowNowPathItem();
}


FileViewWidget::~FileViewWidget()
{

}

void FileViewWidget::ShowNowPathItem()
{ 
	dragListWidget->clear();
	QFileInfoList list = dir.entryInfoList();//获取当前path下的文件
   
    for (int i = 0; i < list.count(); i++)
    {
        QFileInfo tmpFileInfo = list.at(i);
        bool needAdd = false;
        QIcon icon;
        if (tmpFileInfo.isDir())    //目录Item
        {
			if (tmpFileInfo.fileName() == ".." && dir.path() == rootPath)//如果path是根目录，Item不添加父目录".."，防止用户通过点击Item超出作用域
			{
			}
			else
			{
				icon.addFile(rootPath + "/Icon/folder.png");
				needAdd = true;
			}
        }
        else if (tmpFileInfo.isFile())    //文件Item
        {
            if (tmpFileInfo.suffix() == "dds" || tmpFileInfo.suffix() == "jpg" || tmpFileInfo.suffix() == "png")
            {
                icon.addFile(rootPath + "/Icon/picture.png");
                needAdd = true;
            }
            else if (tmpFileInfo.suffix() == "hlsl")
            {
                icon.addFile(rootPath + "/Icon/document.png");
                needAdd = true;
            }
        }

        if (needAdd == true)
        {
            QString fileName = tmpFileInfo.fileName();
            QListWidgetItem* tmp = new QListWidgetItem(icon, fileName);
            tmp->setSizeHint(QSize(100, 100));

			QString relPath = tmpFileInfo.filePath().replace(QRegExp(projPath+"/"), "");

            tmp->setData(4, relPath);     // 储存Item携带的数据,相对路径
            dragListWidget->addItem(tmp);
        }
    }
}

//鼠标双击触发，接受item参数
void FileViewWidget::DoubleClickItem(QListWidgetItem* item)
{
	QString itemName = item->text();
	dir.setPath(currentPath);
	if (dir.cd(itemName))        
	{
		currentPath = dir.absolutePath();
		qDebug() << currentPath<< endl;
		ShowNowPathItem();	
	/*	qDebug() << dir.currentPath() << endl;
		qDebug() << dir.absolutePath() << endl;
		qDebug() << dir.canonicalPath() << endl;
		qDebug() << dir.path() << endl;
		qDebug() << dir.homePath() << endl;
		qDebug() << dir.rootPath() << endl;*/
	}
	else //如果进入失败，则为普通文件，创建新的进程打开对应文件
	{
		QStringList arguments;
		QString start_path = dir.absolutePath() + "/" + itemName;
		arguments << "/c" << start_path;

		QProcess* process = new QProcess;

		qDebug() << start_path << endl;
		process->start("cmd.exe", arguments);  //开启新进程打开文件
	}
}