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
	�������sTypeList��Ϊ������������� (��ʽΪ"x;x;x" ������"0;1;2;3"�������������͵����)
	0��ʾ��ʱ¼��1��ʾ�ƶ�¼��2��ʾ����¼��3��ʾ�ֶ�¼��
	--*/
	virtual int searchVideoFileEx(const QString &sDevName,
								const QString& sDate,
								const QString& sTypeList) = 0;

	enum _emError{
		OK = 0,          //�ɹ�
		E_PARAMETER_ERROR, //�����������ȷ
		E_SYSTEM_FAILED,   //ϵͳ����
	};
};



#endif