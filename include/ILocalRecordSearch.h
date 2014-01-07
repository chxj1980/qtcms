#ifndef __ILOCALPLAYSEARCH_HEAD_FILE_NS89VY9ASUD8__
#define __ILOCALPLAYSEARCH_HEAD_FILE_NS89VY9ASUD8__
#include <libpcom.h>
#include <QtCore/QString>

interface ILocalRecordSearch : public IPComBase
{
	/*++
	����sdevname�豸¼������ڣ�����������¼�����ʽ�׳���
	����ֵ_emError����
	OK�����м�����E_PARAMETER_ERROR�������������ȷ���������Ųο�������Ŷ��塱��

	--*/
	virtual int searchDateByDeviceName(const QString& sdevname) = 0;

	/*++
	����������������Ƶ�ļ�������������¼�����ʽ�׳���
	�������sdevname��Ϊ�豸��
	�������sdate��ΪҪ���������� (��ʽ"yyyy-MM-dd")
	�������sbegintime��Ϊ��ʼʱ�� (��ʽΪ"hh:mm:ss")
	�������sendtime��Ϊ����ʱ�� (��ʽΪ"hh:mm:ss")
	�������schannellist��Ϊ������ͨ����� (��ʽΪ"x;xx;x" ������"1;13;3"��ȡ1��13��3 ͨ���ĵ����)
	--*/
	virtual int searchVideoFile(const QString& sdevname,
								const QString& sdate,
								const QString& sbegintime,
								const QString& sendtime,
								const QString& schannellist) = 0;

	enum _emError{
		OK = 0,          //�ɹ�
		E_PARAMETER_ERROR, //�����������ȷ
		E_SYSTEM_FAILED,   //ϵͳ����
	};
};

/*
��ע�����������ӿ�Ϊ������ʵ��
	  ��ȡ���������ע���¼����õ�
Event:
@1	name: "GetRecordDate" �ýӿ�searchDateByDeviceName����ʼ
	parameters:
		"devname":�豸��(QString)
		"date":¼������(QDateTime)
	
@2	name: "GetRecordFile" �ýӿ�searchVideoFile����ʼ
	parameters:
		"filename":�ļ��� (QString)
		"filepath":�ļ�·�� (QString)(ע���ļ�·����'/'���ָ�)
		"filesize":�ļ���С (int) (��λ(MB))
		"channelnum":ͨ����
		"startTime":��ʼʱ��(QDateTime)
		"stopTime":����ʱ��(QDateTime)


@3	name: "SearchStop"
	parameters:
		"stopevent":��Ӧ�¼���������(QString) �����ò��� GetRecordDate ��GetRecordFile)
*/


#endif