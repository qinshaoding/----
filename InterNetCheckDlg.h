// InterNetCheckDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

using namespace Ryeol ;


// CInterNetCheckDlg 对话框
class CInterNetCheckDlg : public CDialog
{
// 构造
public:
	CInterNetCheckDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_INTERNETCHECK_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
