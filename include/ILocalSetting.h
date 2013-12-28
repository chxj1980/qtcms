#ifndef __ILOCALSETTING_HEAD_FILE_ASDNVG8Y9ASDF__
#define __ILOCALSETTING_HEAD_FILE_ASDNVG8Y9ASDF__
#include <libpcom.h>
#include <QtCore/QString>

//��ϸ�ӿڶ���μ�doc\Interface\ILocalSetting.html

interface ILocalSetting : public IPComBase
{
	virtual int setLanguage(const QString & sLanguage) = 0;

	virtual QString getLanguage() = 0;
	
	virtual int setAutoPollingTime(int aptime) = 0;

	virtual int getAutoPollingTime() = 0;

	virtual int setSplitScreenMode(const QString & smode) = 0;

	virtual QString getSplitScreenMode() = 0;
	
	virtual int setAutoLogin(bool alogin);

	virtual bool getAutoLogin();

	virtual int setAutoSyncTime(bool synctime);

	virtual bool getAutoSyncTime();

	virtual int setAutoConnect(bool aconnect);

	virtual bool getAutoConnect();

	virtual int setAutoFullscreen(bool afullscreen);

	virtual bool getAutoFullscreen();

	virtual int setBootFromStart(bool bootstart);

	virtual bool getBootFromStart();

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
	id     name                value
		"misc_slanguage"       "en_GB"
		"misc_aptime"          "120"
		"misc_smode"           "div4_4"
		"misc_alogin"          "true"
		"misc_synctime"		   "true"
		"misc_aconnent"		   "false"
		"misc_bootstart"	   "false"
	����ʼvalue���ϣ�
	
	�¼�����
*/


#endif //__ILOCALSETTING_HEAD_FILE_ASDNVG8Y9ASDF__