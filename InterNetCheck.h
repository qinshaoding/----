// InterNetCheck.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error �ڰ������� PCH �Ĵ��ļ�֮ǰ������stdafx.h��
#endif

#include "resource.h"		// ������


// CInterNetCheckApp:
// �йش����ʵ�֣������ InterNetCheck.cpp
//

class CInterNetCheckApp : public CWinApp
{
public:
	CInterNetCheckApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CInterNetCheckApp theApp;
