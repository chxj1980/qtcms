#ifndef __IDISKSETTING_HEAD_FILE_ASDNVG8Y9ASDF__
#define __IDISKSETTING_HEAD_FILE_ASDNVG8Y9ASDF__
#include <libpcom.h>
#include <QtCore/QString>

interface IDisksSetting : public IPComBase
{

	virtual int setUseDisks(const QString & sDisks) = 0;

	virtual int getUseDisks(QString & sDisks) = 0;

	virtual int getEnableDisks(QString & sDisks) = 0;

	virtual int setFilePackageSize(const int filesize) = 0;

	virtual int getFilePackageSize(int& filesize) = 0;

	virtual int setLoopRecording(bool bcover) = 0;

	virtual bool getLoopRecording() = 0;

	virtual int setDiskSpaceReservedSize(const int spacereservedsize) = 0;

	virtual int getDiskSpaceReservedSize(int& spacereservedsize) = 0;
	
	enum _emError{
		S_OK = 0,          //�ɹ�	
		E_PARAMETER_ERROR, //�����������ȷ
		E_SYSTEM_FAILED,   //ϵͳ����
	};

};

/*
	���ݿ�洢��
	system.db
	�ֶ�id     name           ��ʼ value
			  "use_disks"          "D;"
	          "b_cover"            "true"
			  "file_size"          "128"
			  "reserved_size"      "1024"
	�¼�����
*/


#endif //__IDISKSETTING_HEAD_FILE_ASDNVG8Y9ASDF__