
// H264Decode.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CH264DecodeApp:
// �йش����ʵ�֣������ H264Decode.cpp
//

class CH264DecodeApp : public CWinApp
{
public:
	CH264DecodeApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CH264DecodeApp theApp;