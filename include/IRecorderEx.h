#ifndef __IRECORDEREX_HEAD_FILE_IO34ERG75TF__
#define __IRECORDEREX_HEAD_FILE_IO34ERG75TF__
#include <libpcom.h>
#include <QtCore/QVariantMap>

//��ϸ�ӿ�˵���μ�doc\Interface\IRecorderEx.html

interface IRecorderEx : public IPComBase
{

	virtual int SetDevInfoEx(const int &nWindId, const int &nRecordType) = 0;

	virtual int FixExceptionalData() = 0;

	enum _emError{
		OK = 0,          //�ɹ�
		E_PARAMETER_ERROR, //�����������ȷ
		E_SYSTEM_FAILED,   //ϵͳ����
	};

};



#endif //__IRECORDEREX_HEAD_FILE_IO34ERG75TF__