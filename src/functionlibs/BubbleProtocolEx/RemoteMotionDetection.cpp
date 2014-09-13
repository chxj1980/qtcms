#include "bubbleprotocolex.h"

void BubbleProtocolEx::mdSocketStateChange(QAbstractSocket::SocketState sockState)
{

}

void BubbleProtocolEx::mdSocketConnetced()
{

}

void BubbleProtocolEx::mdSocketDisconnected()
{

}

void BubbleProtocolEx::mdSocketError(QAbstractSocket::SocketError socketError)
{

}

int BubbleProtocolEx::startMotionDetection()
{
	m_sockMD.connectToHost(m_tDeviceInfo.tIpAddr,m_tDeviceInfo.tPorts["http"].toInt(),QIODevice::ReadWrite);
	if (!m_sockMD.waitForConnected(5000))
	{
		qDebug("[%s:%d] %s:%s",__FILE__,__LINE__,__FUNCTION__,m_sockMD.errorString());
		return -1;
	}

	// keep alive
	m_sockMD.setSocketOption(QAbstractSocket::KeepAliveOption,QVariant(1));

	// nodelay
	m_sockMD.setSocketOption(QAbstractSocket::LowDelayOption,QVariant(1));

	// timer & work object thread
	m_MdWorkObj.moveToThread(&m_MdThread);
	m_MdTimer.moveToThread(&m_MdThread);
	m_MdThread.start();

	connect(&m_MdTimer,SIGNAL(timeout()),&m_MdWorkObj,SLOT(motionDetectionEcho()));
	// echo everytime
	m_MdTimer.setInterval(1000);
	m_MdTimer.start();

	m_bQuitMd = false;

	return 0;
}

int BubbleProtocolEx::stopMotionDetection()
{
	// �Ͽ�socket
	m_sockMD.disconnectFromHost();
	m_sockMD.close();
	
	// �رռ�ʱ��
	m_MdTimer.stop();

	// �ȴ��߳��˳�
	m_bQuitMd = true;
	m_MdThread.quit();
	m_MdThread.wait();
	return 0;
}

void BubbleProtocolEx::motionDetectionEcho()
{
	// ����echo����
	m_csMdEcho.lock();
	char sRecvBuffer[1024]; // ���ܻ���
	QString sPack;
	QString sLR;
	QString sContent;

	// �����û�������
	QString sUserandPwd;
	sUserandPwd = m_tDeviceInfo.sUserName + QString(":") + m_tDeviceInfo.sPassword;
	QString sAuthorization(sUserandPwd.toAscii().toBase64());

	// �����ַ���
	QString sReq;
	sReq.clear();
	sReq += QString("GET /NetSDK/Video/motionDetection/channel/1/status HTTP/1.1\r\n");
	sReq += QString("Host: ") + m_tDeviceInfo.tIpAddr.toString() + QString("\r\n");
	sReq += QString("Connection: keep-alive\r\n");
	sReq += QString("Authorization: Basic ") + sAuthorization + QString("\r\n");
	sReq += QString("Cookie: juanipcam_lang=zh-cn\r\n\r\n");

	// ��������
	qint64 nTotleWrite = 0;
	do 
	{
		if (m_bQuitMd)
		{
			// �˳�Mdѭ��
			m_sockMD.disconnectFromHost();
			m_sockMD.close();

			m_csMdEcho.unlock();
			return;
		}

		qint64 nWrite = m_sockMD.write(sReq.toAscii().data() + nTotleWrite,sReq.length());
		if (-1 == nWrite)
		{
			// дʧ��,�ر�socket,�˳�����echo
			m_sockMD.disconnectFromHost();
			m_sockMD.close();

			// ������������-- to be modified

			m_csMdEcho.unlock();
			return;
		}
		nTotleWrite += nWrite;

		if ( ! m_sockMD.waitForBytesWritten(1000) )
		{
			// д��ʱ,�ر�socket,�˳�����echo
			m_sockMD.disconnectFromHost();
			m_sockMD.close();

			// ������������-- to be modified
			m_csMdEcho.unlock();
			return;
		}
	} while (nTotleWrite < sReq.length());

	// ��������
	while(1) // to be modified ��Ҫ����ѭ��ǿ���˳�����
	{
		if (m_bQuitMd)
		{
			// �˳�Mdѭ��
			m_sockMD.disconnectFromHost();
			m_sockMD.close();

			m_csMdEcho.unlock();
			return;
		}

		if (m_sockMD.waitForReadyRead(50))
		{
			qint64 nRead = m_sockMD.read(sRecvBuffer,sizeof(sRecvBuffer) - 1); // Ԥ��������λ�ö�ȡ
			if (-1 == nRead)
			{
				// ��ʧ�ܣ��ر�socket���˳�����echo
				m_sockMD.disconnectFromHost();
				m_sockMD.close();

				// ������������ -- to be modified
				m_csMdEcho.unlock();
				return;
			}

			sRecvBuffer[nRead] = 0;
			sPack += QString(sRecvBuffer);

			// ���httpͷ�Ƿ��������,\r\n\r\n����\n\n,δ�������򷵻�ѭ���������ܣ��ѽ����������ִ�к�������
			sLR = QString("\r\n");
			if (!sPack.contains(QString("\r\n\r\n")))
			{
				// δ�ҵ�\r\n����β
				sLR = QString("\n");
				if (!sPack.contains(QString("\n\n")))
				{
					// δ�ҵ�\n����β��ͷδ����
					// �¸�ѭ����������
					continue;
				}
			}

			// ��ȡcontent-length�ֶ�
			int nContentLength = 0;
			if (sPack.contains(QString("content-length"),Qt::CaseInsensitive))
			{
				int nContentLengthStart = sPack.indexOf(QString("content-length"),0,Qt::CaseInsensitive);
				int nContentLengthEnd = sPack.indexOf(sLR,nContentLengthStart);
				QString sContentLength = sPack.mid(nContentLengthStart,nContentLengthEnd = nContentLengthStart);
				sContentLength.remove(QChar(' '));
				int nContentLength = sContentLength.mid(sContentLength.indexOf(QChar(':')) + 1).toInt();
			}

			if (0 != nContentLength)
			{
				// �����д�����
				sContent = sPack.mid(sPack.indexOf(sLR + sLR) + QString(sLR + sLR).length());
				if (sContent.length() == nContentLength)
				{
					// �������
					// �˳�����ѭ��
					break;
				}
				else if (sContent.length() > nContentLength)
				{
					// ���ݲ���ȷ
					m_sockMD.disconnectFromHost();
					m_sockMD.close();

					// ������������ -- to be modified
					m_csMdEcho.unlock();
					return;
				}
				else
				{
					// ��Ҫ��������
					continue;
				}
			}
			else
			{
				// ��content���������
				// �˳�����ѭ��
				break;
			}
		}
	}

	// �������ݽ���
	QString sFirstLine = sPack.left(sPack.indexOf(sLR));
	if (sFirstLine.contains(QString("200")))
	{
		// 200 OK
		if (sContent == "true")
		{
			QVariantMap eparam;
			eparam.insert("signal",QVariant(true));
			eventProcCall("MDSignal",eparam);
		}
		else if (sContent == "false")
		{
			QVariantMap eparam;
			eparam.insert("signal",QVariant(false));
			eventProcCall("MDSignal",eparam);
		}
	}
	else if (sFirstLine.contains(QString("404")))
	{
		// 404 page not found
	}


	m_csMdEcho.unlock();
}