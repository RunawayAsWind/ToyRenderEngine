#include "../Include/RQt/QtDebug.h"

namespace RQt
{
	void QtDebug(const char* szFormat, ...)
	{
		va_list pArgList;
		va_start(pArgList, szFormat);
		qDebug() << QString().vsprintf(szFormat, pArgList);
		va_end(pArgList);
	}
}