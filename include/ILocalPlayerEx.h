#ifndef __ILOCALPLAYEREX_HEAD_FILE_TG7RJ8H5YHTYF__
#define __ILOCALPLAYEREX_HEAD_FILE_TG7RJ8H5YHTYF__
#include <libpcom.h>
#include <QtCore/QVariantMap>

//��ϸ�ӿ�˵���μ�doc\Interface\ILocalPlayerEx.html

interface ILocalPlayerEx : public IPComBase
{

	virtual int AddFileIntoPlayGroupEx(const int & nWndId,
									const QWidget * pWnd,
									const QDate& date,
									const QTime & startTime,
									const QTime & endTime,
									const int & nTypes) = 0;

	enum _emError{
		OK = 0,          //�ɹ�
		E_PARAMETER_ERROR, //�����������ȷ
		E_SYSTEM_FAILED,   //ϵͳ����
	};

};



#endif //__ILOCALPLAYEREX_HEAD_FILE_TG7RJ8H5YHTYF__