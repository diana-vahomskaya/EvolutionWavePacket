
// WavePacket.h : ������� ���� ��������� ��� ���������� PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�������� stdafx.h �� ��������� ����� ����� � PCH"
#endif

#include "resource.h"		// �������� �������


// CWavePacketApp:
// � ���������� ������� ������ ��. WavePacket.cpp
//

class CWavePacketApp : public CWinApp
{
public:
	CWavePacketApp();

// ���������������
public:
	virtual BOOL InitInstance();

// ����������

	DECLARE_MESSAGE_MAP()
};

extern CWavePacketApp theApp;
