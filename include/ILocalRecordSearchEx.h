#ifndef __ILOCALPLAYSEARCHEX_HEAD_FILE_FSJ48J8KY4YU5D__
#define __ILOCALPLAYSEARCHEX_HEAD_FILE_FSJ48J8KY4YU5D__
#include <libpcom.h>
#include <QtCore/QString>

interface ILocalRecordSearchEx : public IPComBase
{
	/*++
	����������������Ƶ�ļ�������������¼�����ʽ�׳���
	�������sDevName��Ϊ�豸��
	�������sDate��ΪҪ���������� (��ʽ"yyyy-MM-dd")
	�������nTypes��Ϊ������������� (��λ���㣬��0λ��ʾ��ʱ¼�񣬵�1λ��ʾ�ƶ����¼�񣬵�2λ��ʾ����¼�񣬵�3λ��ʾ�ֶ�¼������"15"����ʾ�����������͵����)
	--*/
	virtual int searchVideoFileEx(const QString &sDevName,
								const QString& sDate,
								const int& nTypes) = 0;
	/*++
	����������������Ƶ�ļ�������������¼�����ʽ�׳���
	�������nWndId��Ϊ�豸��
	�������sDate��ΪҪ���������� (��ʽ"yyyy-MM-dd")
	�������sStartTime��Ϊ��ʼ����ʱ�䣨��ʽ"hh:mm:ss")
	�������sEndTime��Ϊ��������ʱ�䣨��ʽ"hh:mm:ss")
	�������nTypes��Ϊ������������� (��λ���㣬��0λ��ʾ��ʱ¼�񣬵�1λ��ʾ�ƶ����¼�񣬵�2λ��ʾ����¼�񣬵�3λ��ʾ�ֶ�¼������"15"����ʾ�����������͵����)
	--*/
	virtual int searchVideoFileEx(const int & nWndId,
								const QString & sDate,
								const QString & sStartTime,
								const QString & sEndTime,
								const int & nTypes) = 0;

	enum _emError{
		OK = 0,          //�ɹ�
		E_PARAMETER_ERROR, //�����������ȷ
		E_SYSTEM_FAILED,   //ϵͳ����
	};
};



#endif