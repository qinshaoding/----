// InterNetCheckDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

using namespace Ryeol ;


// CInterNetCheckDlg �Ի���
class CInterNetCheckDlg : public CDialog
{
// ����
public:
	CInterNetCheckDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_INTERNETCHECK_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CString m_strSvrURL;
	enum {
		OPERATING				= 0x00000001
		, OPERATION_BEGINNING	= 0x00000002
		, OPERATION_CANCELING	= 0x00000004
		, TERMINATING			= 0x00000008
	} ;

	CHttpClient			m_objHttpClient ;
	DWORD				m_nOperationStat ;
	CString strCheckBackValue;
};
