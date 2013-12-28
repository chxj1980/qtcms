#ifndef __IDISKSETTING_HEAD_FILE_ASDNVG8Y9ASDF__
#define __IDISKSETTING_HEAD_FILE_ASDNVG8Y9ASDF__
#include <libpcom.h>
#include <QtCore/QString>

//�ӿڶ���μ�doc\Interface\IDisksSetting.htm

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
	���ݿ�洢��system.db
	��general_setting
	�ֶΣ�                    
	id     name                       value
		  "storage_usedisks"          "D:"
	      "storage_cover"             "true"
		  "storage_filesize"          "128"
		  "storage_reservedsize"      "1024"
	����ʼvalue���ϣ�
	�¼�����
*/


#endif //__IDISKSETTING_HEAD_FILE_ASDNVG8Y9ASDF__