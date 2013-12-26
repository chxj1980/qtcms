#ifndef __IDISKSETTING_HEAD_FILE_ASDNVG8Y9ASDF__
#define __IDISKSETTING_HEAD_FILE_ASDNVG8Y9ASDF__
#include <libpcom.h>
#include <QtCore/QString>

interface IDisksSetting : public IPComBase
{
	/*++
	����¼��ʹ�õĴ���(��ʽΪ"X:\X:\X:\..."  XΪD E F....)
	����ֵ_emError����
	S_OK�����óɹ�
	E_SYSTEM_FAILED����������ʧ��
	--*/
	virtual int setUseDisks(const QString & sDisks) = 0;
	/*++
	��ȡ¼��ʹ�õĴ���(��ʽΪ"X:\X:\X:\..."  XΪD E F....)
	����ֵ_emError����
	S_OK����ȡ�ɹ�
	E_SYSTEM_FAILED����ȡ����ʧ��
	--*/
	virtual int getUseDisks(QString & sDisks) = 0;
	/*++
	��ȡϵͳ���õĴ��̷���(��ʽΪ"X:\X:\X:\..."  XΪD E F....)
	����ֵ_emError����
	S_OK����ȡ�ɹ�
	E_SYSTEM_FAILED����������ʧ��
	--*/
	virtual int getEnableDisks(QString & sDisks) = 0;

	/*++
	����¼���ļ�����С(��λm)
	����ֵ_emError����
	S_OK�����óɹ�
	E_PARAMETER_ERROR ��������ȷ
	--*/
	virtual int setFilePackageSize(const unsigned int filesize) = 0;
	/*++
	��ȡ¼���ļ�����С(��λm)
	����ֵ_emError����
	S_OK����ȡ�ɹ�
	E_SYSTEM_FAILED����ȡ����ʧ��
	--*/
	virtual int getFilePackageSize(unsigned int& filesize) = 0;
	/*++
	�����Ƿ�ѭ��¼��
	����ֵ_emError����
	S_OK�����óɹ�
	E_SYSTEM_FAILED����������ʧ��
	--*/
	virtual int setLoopRecording(bool loop) = 0;
	/*++
	��ȡ�Ƿ�ѭ��¼��
	����ֵbool  
	--*/
	virtual bool getLoopRecording() = 0;
	/*++
	���ô���Ԥ���ռ�(��λm)
	����ֵ_emError����
	S_OK�����óɹ�
	E_PARAMETER_ERROR ��������ȷ
	--*/
	virtual int setDiskSpaceReservedSize(const unsigned int spacereservedsize) = 0;
	/*++
	��ȡ����ʣ��ռ�(��λm)
	����ֵ_emError����
	S_OK����ȡ�ɹ�
	E_SYSTEM_FAILED����ȡ����ʧ��
	--*/
	virtual int getDiskSpaceReservedSize(unsigned int& spacereservedsize) = 0;
	
	enum _emError{
		S_OK = 0,          //�ɹ�
		//E_DISK_NOT_FOUND,  //����δ�ҵ�	
		E_PARAMETER_ERROR, //�����������ȷ
		E_SYSTEM_FAILED,   //ϵͳ����
	};

};

/*
	�¼�����
*/


#endif //__IDISKSETTING_HEAD_FILE_ASDNVG8Y9ASDF__