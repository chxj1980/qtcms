#ifndef __IDEVICESEARCH_HEAD_FILE_HVPAPdsfdsfG87WDHFVC8SA__
#define __IDEVICESEARCH_HEAD_FILE_HVPAPdsfdsfG87WDHFVC8SA__
#include <libpcom.h>
#include <IEventRegister.h>

interface IOnvifRemoteInfo : public IPComBase
{
	virtual void setOnvifDeviceInfo(const QString &sIp, const QString &sPort, const QString &sUserName, const QString &sPassword) = 0;
	
	//����ֵ��ʽΪ xml ���ַ���
	/* 
	<OnvifDeviceInfo manufacturer="" devname="" model="" firmware="" sn="" hwid="" sw_builddate="" sw_version="" hw_version="" />
	manufacturer���������ң�devname���豸���ƣ�model���ͺţ�firmware���̼���hwid��Ӳ��ID��sw_builddate���������ʱ�䣬sw_version������汾��hw_version��Ӳ���汾
	*/
	virtual QString getOnvifDeviceInfo() = 0;

	virtual bool getOnvifDeviceNetworkInfo(QString &sMac,QString &sGateway,QString &sMask,QString &sDns)=0;
	
	virtual bool setOnvifDeviceNetWorkInfo(QString sSetIp,QString sSetMac,QString sSetGateway,QString sSetMask,QString sSetDns)=0;
	
	//����ֵ��ʽΪ xml ��ʽ�� �ַ���
	/*
	<OnvifStreamEncoderInfo itemNum="">
		<StreamItem index="" width="" height="" enc_fps="" enc_bps="" codeFormat="" enc_interval="" enc_profile="">
			<EncodeOption>
				<enc_quality min="" max=""/>
				<enc_fps min="" max=""/>
				<enc_bps min="" max=""/>
				<enc_gov min="" max=""/>
				<enc_interval min="" max=""/>
				<resolution itemNum="">
					<item width="" height=""/>
					<item width="" height=""/>
				</resolution>
				<enc_profile itemNum="">
					<item profile=""/>
					<item profile=""/>
				</enc_profile>
			</EncodeOption>
		</StreamItem>
	<OnvifStreamEncoderInfo>
	index:����������0������������width��������height�������ߣ�enc_fps��֡�ʣ�enc_bps�����ʣ�codeFormat�������ʽ��enc_interval��I֡���
	*/
	virtual QString getOnvifDeviceEncoderInfo() = 0;

	virtual bool setOnvifDeviceEncoderInfo(int nIndex,int nWidth,int nHeight,QString sEnc_fps,QString sEnc_bps,QString sCodeFormat,QString sEncInterval,QString sEncProfile)=0;
};

#endif