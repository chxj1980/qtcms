#include "StdAfx.h"
#include "AudioWaveDevice.h"

void CALLBACK waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	((CAudioWaveDevice *)dwInstance)->waveOutProc(hwo,uMsg,dwParam1,dwParam2);
	return;
}

DWORD WINAPI AudioPlayThread(LPVOID pParam)
{
	return (((CAudioWaveDevice *)pParam)->AudioPlayThread());
}

CAudioWaveDevice::CAudioWaveDevice()
{
	m_hWaveout = NULL;
	m_nBufferCount = 20;
	InitializeCriticalSection(&m_csBufferCount);
	InitializeCriticalSection(&m_csBufList);
	m_hPlayWnd = NULL;
	HANDLE hThread;
	DWORD dwThreadId;                  
	m_pBuffer = allocateBlocks(WAVEDEV_BLOCK_SIZE,WAVEDEV_BLOCK_COUNT);
	hThread = CreateThread(NULL,0,::AudioPlayThread,this,CREATE_SUSPENDED,&dwThreadId);
	if (NULL != hThread)
	{
		m_bThreadEnd = false;
		ResumeThread(hThread);
	}
}

CAudioWaveDevice::~CAudioWaveDevice()
{
	m_bThreadEnd = true;
	DWORD dwCount = ::GetTickCount();
	while (m_bThreadRunning && ::GetTickCount() - dwCount < 3000)
	{
		Sleep(10);
	}
	freeBlocks(m_pBuffer);
}

int CAudioWaveDevice::SetMainWnd(HWND hWnd)
{
	m_hPlayWnd = hWnd;
	return 0;
}

int CAudioWaveDevice::SetAudioParam(int nChannel,int nSampleRate,int nSampleWidth)
{
	CloseHwo();

	WAVEFORMATEX wfx;
	MMRESULT hr;
	wfx.wBitsPerSample = 16;
	wfx.nSamplesPerSec = 8000;
	wfx.nChannels = 1;
	wfx.cbSize = 0;
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * (wfx.wBitsPerSample / 8);
	wfx.nBlockAlign = wfx.wBitsPerSample / 8 * wfx.nChannels;
	
	hr = waveOutOpen(&m_hWaveout, WAVE_MAPPER, &wfx, (DWORD)::waveOutProc, (DWORD)this, CALLBACK_FUNCTION);

	if (MMSYSERR_NOERROR != hr)
	{
		return hr;
	}

	return 0;
}

int CAudioWaveDevice::SetVolume(int nPersent)
{
	return 0;
}

int CAudioWaveDevice::PlayBuffer(char *pBuffer,int nSize)
{
	BufferListNode node;
	node.pBuffer = (char *)malloc(nSize);
	node.nBufferSize = nSize;
	memcpy(node.pBuffer,pBuffer,nSize);
	EnterCriticalSection(&m_csBufList);
	m_BufList.push_back(node);
	LeaveCriticalSection(&m_csBufList);

	waveOutRestart(m_hWaveout);

	return 0;
}

int CAudioWaveDevice::Stop()
{
	waveOutPause(m_hWaveout);
	return 0;
}

void CAudioWaveDevice::CloseHwo()
{
	if (NULL != m_hWaveout)
	{
		waveOutClose(m_hWaveout);
		m_hWaveout = NULL;
	}
}

void CAudioWaveDevice::waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD dwParam1, DWORD dwParam2)
{
	switch(uMsg)
	{
	case WOM_DONE:
		{
			if (m_hWaveout == hwo)
			{
				EnterCriticalSection(&m_csBufferCount);
				m_nBufferCount ++;
				LeaveCriticalSection(&m_csBufferCount);
			}
		}
		break;
	default:
		break;
	}
}

WAVEHDR* CAudioWaveDevice::allocateBlocks(int size, int count)
{
	unsigned char* buffer;
	int i;
	WAVEHDR* blocks;
	DWORD totalBufferSize = (size + sizeof(WAVEHDR)) * count;

	buffer = (unsigned char *)malloc(totalBufferSize);

	memset(buffer,0,totalBufferSize);

	blocks = (WAVEHDR*)buffer;
	buffer += sizeof(WAVEHDR) * count;
	for(i = 0; i < count; i++) {
		blocks[i].dwBufferLength = size;
		blocks[i].lpData = (char *)buffer;
		buffer += size;
	}
	return blocks;
}

void CAudioWaveDevice::freeBlocks(WAVEHDR *pBlocks)
{
	if (NULL != pBlocks)
	{
		free(pBlocks);
	}
}

DWORD CAudioWaveDevice::AudioPlayThread()
{
	TRACE("[Treadtrace] 1\r\n");
	BufferList::const_iterator buffer;
	BufferListNode node;
	bool           bReadBuf = false;
	int            nCurrentBlock = 0;
	m_bThreadRunning = true;
	while (!m_bThreadEnd)
	{
		bReadBuf = false;
		EnterCriticalSection(&m_csBufList);
		if (m_BufList.size() > 0)
		{
			buffer = m_BufList.begin();
			node = *buffer;
			m_BufList.pop_front();
			bReadBuf = true;
		}
		LeaveCriticalSection(&m_csBufList);

		if (bReadBuf)
		{
			WAVEHDR * current;
			int remain;
			current = &m_pBuffer[nCurrentBlock];
			int nSize= node.nBufferSize;
			char * pData = node.pBuffer;
			while (nSize > 0)
			{
				if (current->dwFlags & WHDR_PREPARED)
				{
					waveOutUnprepareHeader(m_hWaveout,current,sizeof(WAVEHDR));
				}

				// ������д�뵱ǰ��
				if(nSize < (int)(WAVEDEV_BLOCK_SIZE - current->dwUser))
				{
					memcpy(current->lpData + current->dwUser, pData, nSize);
					current->dwUser += nSize;
					// ��δд�������˳�
					break;
				}

				// ��д��
				remain = WAVEDEV_BLOCK_SIZE - current->dwUser;
				memcpy(current->lpData + current->dwUser, pData, remain);
				nSize -= remain;
				pData += remain;
				current->dwBufferLength = WAVEDEV_BLOCK_SIZE;

				// �������Ƶ����豸
				waveOutPrepareHeader(m_hWaveout, current, sizeof(WAVEHDR));
				waveOutWrite(m_hWaveout, current, sizeof(WAVEHDR));

				// ����Block��-1
				EnterCriticalSection(&m_csBufferCount);
				m_nBufferCount--;
				LeaveCriticalSection(&m_csBufferCount);

				while(!m_nBufferCount)
					Sleep(10);

				// ��ǰ��ָ��ƫ��
				nCurrentBlock++;
				nCurrentBlock %= WAVEDEV_BLOCK_COUNT;

				// ���õ�ǰ����û�����(��������)
				current = &m_pBuffer[nCurrentBlock];
				current->dwUser = 0;
			}
			delete node.pBuffer;
			node.pBuffer = NULL;
		}
		else
		{
			Sleep(10);
		}
	}

	// �ȴ����п����
	while(m_nBufferCount < WAVEDEV_BLOCK_COUNT)
		Sleep(10);

	// ж��ͷ
	for(int i = 0; i < m_nBufferCount; i++)
	{
		if(m_pBuffer[i].dwFlags & WHDR_PREPARED)
		{
			waveOutUnprepareHeader(m_hWaveout, &m_pBuffer[i], sizeof(WAVEHDR));
		}
	}

	// �������Buffer
	EnterCriticalSection(&m_csBufList);
	while (m_BufList.size() > 0)
	{
		buffer = m_BufList.begin();
		node = *buffer;
		delete node.pBuffer;
		node.pBuffer = NULL;
		m_BufList.pop_front();
	}
	LeaveCriticalSection(&m_csBufList);

	m_bThreadRunning = false;
	return 0;
}

/*void writeAudio(HWAVEOUT hWaveOut, LPSTR data, int size)
{
	WAVEHDR* current;
	int remain;
	current = &waveBlocks[waveCurrentBlock];
	while(size > 0) {

		// Buffer ������˳�
		if(current->dwFlags & WHDR_PREPARED)
			waveOutUnprepareHeader(hWaveOut, current, sizeof(WAVEHDR));

		// ������д�뵱ǰ��
		if(size < (int)(BLOCK_SIZE - current->dwUser)) {
			memcpy(current->lpData + current->dwUser, data, size);
			current->dwUser += size;
			// ��δд�������˳�
			break;
		}

		// ��д��
		remain = BLOCK_SIZE - current->dwUser;
		memcpy(current->lpData + current->dwUser, data, remain);
		size -= remain;
		data += remain;
		current->dwBufferLength = BLOCK_SIZE;

		// �������Ƶ����豸
		waveOutPrepareHeader(hWaveOut, current, sizeof(WAVEHDR));
		waveOutWrite(hWaveOut, current, sizeof(WAVEHDR));

		// ����Block��-1
		EnterCriticalSection(&waveCriticalSection);
		waveFreeBlockCount--;
		LeaveCriticalSection(&waveCriticalSection);


		// ���û�п���Block����ȴ�
		while(!waveFreeBlockCount)
			Sleep(10);

		// ��ǰ��ָ��ƫ��
		waveCurrentBlock++;
		waveCurrentBlock %= BLOCK_COUNT;


		// ���õ�ǰ����û�����(��������)
		current = &waveBlocks[waveCurrentBlock];
		current->dwUser = 0;
	}
}*/
/*
{
	HWAVEOUT hWaveOut; 
	HANDLEhFile;
	WAVEFORMATEX wfx; 
	char buffer[1024]; 
	int i;

	// Ϊ�����ռ�
	waveBlocks = allocateBlocks(BLOCK_SIZE, BLOCK_COUNT);

	// �������
	waveFreeBlockCount = BLOCK_COUNT;

	// ��ʼ����ǰ���ָ��
	waveCurrentBlock= 0;
	InitializeCriticalSection(&waveCriticalSection);

	// ���ļ�
	if((hFile = CreateFile(
		argv[1],
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		0,
		NULL
		)) == INVALID_HANDLE_VALUE)
	{
		fprintf(stderr, "%s: unable to open file '%s'\n", argv[0], argv[1]);
		ExitProcess(1);
	}

	// ��waveout�豸
	wfx.nSamplesPerSec = 44100; 
	wfx.wBitsPerSample = 16; 
	wfx.nChannels= 2; 
	wfx.cbSize = 0; 
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nBlockAlign = (wfx.wBitsPerSample * wfx.nChannels) >> 3;
	wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;
	if(waveOutOpen(
		&hWaveOut,
		WAVE_MAPPER,
		&wfx,
		(DWORD_PTR)waveOutProc,
		(DWORD_PTR)&waveFreeBlockCount,
		CALLBACK_FUNCTION
		) != MMSYSERR_NOERROR)
	{
		fprintf(stderr, "%s: unable to open wave mapper device\n", argv[0]);
		ExitProcess(1);
	}

	
	// ��ѭ��
	while(1) {
		DWORD readBytes;

		// ������
		if(!ReadFile(hFile, buffer, sizeof(buffer), &readBytes, NULL))
			break;

		// �ļ�������
		if(readBytes == 0)
			break;

		// �������ݲ��������0
		if(readBytes < sizeof(buffer)) {
			printf("at end of buffer\n");
			memset(buffer + readBytes, 0, sizeof(buffer) - readBytes);
			printf("after memcpy\n");
		}

		// д���豸������
		writeAudio(hWaveOut, buffer, sizeof(buffer));
	}

	// �ȴ��鲥�����
	while(waveFreeBlockCount < BLOCK_COUNT)
		Sleep(10);

	// ж��ͷ
	for(i = 0; i < waveFreeBlockCount; i++)
	{
		if(waveBlocks[i].dwFlags & WHDR_PREPARED)
		{
			waveOutUnprepareHeader(hWaveOut, &waveBlocks[i], sizeof(WAVEHDR));
		}
	}

	// �����ͷ�
	DeleteCriticalSection(&waveCriticalSection);
	freeBlocks(waveBlocks);
	waveOutClose(hWaveOut);
	CloseHandle(hFile);
	return 0;
}*/
