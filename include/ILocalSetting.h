#ifndef __ILOCALSETTING_HEAD_FILE_ASDNVG8Y9ASDF__
#define __ILOCALSETTING_HEAD_FILE_ASDNVG8Y9ASDF__
#include <libpcom.h>
#include <QtCore/QString>

interface ILocalSetting : public IPComBase
{
	virtual int setLauguage(const QString & sLauguage) = 0;

	virtual QString getLauguage() = 0;
	
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
	�¼�����
*/


#endif //__ILOCALSETTING_HEAD_FILE_ASDNVG8Y9ASDF__