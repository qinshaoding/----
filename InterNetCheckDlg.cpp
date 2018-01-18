// InterNetCheckDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "InterNetCheck.h"
#include "InterNetCheckDlg.h"
#include ".\internetcheckdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#pragma warning (disable: 4290)	// avoids 'C++ Exception Specification ignored' message

using namespace Ryeol ;


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CInterNetCheckDlg 对话框



CInterNetCheckDlg::CInterNetCheckDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInterNetCheckDlg::IDD, pParent)
	, m_strSvrURL(_T(""))
	, m_nOperationStat(0)
	, strCheckBackValue(_T(""))

{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CInterNetCheckDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CInterNetCheckDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CInterNetCheckDlg 消息处理程序

BOOL CInterNetCheckDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将\“关于...\”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	
	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

void CInterNetCheckDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CInterNetCheckDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CInterNetCheckDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

static inline void CheckOperationStat (CInterNetCheckDlg * pobjDlg) throw (DWORD)
{
	if ( pobjDlg->m_nOperationStat & (CInterNetCheckDlg::OPERATION_CANCELING | CInterNetCheckDlg::TERMINATING) )
		throw (pobjDlg->m_nOperationStat) ;
}
static DWORD WINAPI HttpProc (LPVOID lpParameter)
{
	_ASSERTE ( lpParameter != NULL ) ;

	CInterNetCheckDlg *		pobjDlg = static_cast<CInterNetCheckDlg *> (lpParameter) ;
	CHttpClient *				pobjHttp = &pobjDlg->m_objHttpClient ;
	CHttpPostStat				objPostStat ;
	CHttpResponse *				pobjRes = NULL ;
	const DWORD					cbStep = 1024 ;
	int							nMethod ;


	//为POST 方式，0为get,2为upload
	nMethod = 1;

	// Sets the UseUtf8 property.表示是否为Encoding
	pobjHttp->SetUseUtf8 (static_cast<BOOL> (0)) ;

	try {
		pobjDlg->m_nOperationStat |= CInterNetCheckDlg::OPERATION_BEGINNING ;

		CheckOperationStat (pobjDlg) ;

		if ( nMethod == 0 )		
			pobjRes = pobjHttp->RequestGet (pobjDlg->m_strSvrURL) ;

		else if ( nMethod == 1 )
			pobjHttp->BeginPost (pobjDlg->m_strSvrURL) ;

        else
			pobjHttp->BeginUpload (pobjDlg->m_strSvrURL) ;


		while ( pobjRes == NULL ) {
			CheckOperationStat (pobjDlg) ;

			pobjHttp->Query (objPostStat) ;
//			pobjDlg->UpdateProgressStat (objPostStat) ;
			pobjRes = pobjHttp->Proceed (cbStep) ;
		}


		CString		str ;
		str.Format (_T ("%u %s\r\n"), pobjRes->GetStatus (), pobjRes->GetStatusText ()) ;
//		pobjDlg->SetTextMsg (str) ;


		static LPCTSTR		szHeaders[] = 
		{ _T ("Server"), _T ("Date"), _T ("X-Powered-By"), _T ("Content-Length"), _T ("Set-Cookie")
		, _T ("Expires"), _T ("Cache-control"), _T ("Connection"), _T ("Transfer-Encoding")
		, _T ("Content-Type"), _T ("") } ;

		LPCTSTR		szHeader ;

		for (size_t i = 0; i < sizeof (szHeaders) / sizeof (LPCTSTR); i++) {
			CheckOperationStat (pobjDlg) ;

			if ( szHeader = pobjRes->GetHeader (szHeaders[i]) ) {
				str.Format (_T ("%s: %s\r\n"), szHeaders[i], szHeader) ;
//				pobjDlg->SetTextMsg (str) ;
			}
		}

//		pobjDlg->SetTextMsg (_T ("\r\n")) ;

		BOOL		bIsText = FALSE ;
		if ( szHeader = pobjRes->GetHeader (_T ("Content-Type")) )
			bIsText = (0 == ::_tcsncicmp (szHeader, _T ("text/"), 5)) ;

		DWORD		dwContSize ;
		if ( !pobjRes->GetContentLength (dwContSize) )
			dwContSize = 0 ;

//		pobjDlg->SetCurrProgress (_T ("Reading HTTP response..."), 0, dwContSize) ;

		const DWORD		cbBuff = 1024 * 10 ;
		BYTE			byBuff[cbBuff] ;
		DWORD			dwRead ;
		size_t			cbSize = 0 ;

		while ( dwRead = pobjRes->ReadContent (byBuff, cbBuff - 1) ) {
			CheckOperationStat (pobjDlg) ;
			cbSize += dwRead ;
//			pobjDlg->SetCurrProgress (_T ("Reading HTTP response..."), cbSize, dwContSize) ;

			if ( bIsText ) {
				byBuff[dwRead] = '\0' ;			
				str = reinterpret_cast<LPCSTR> (byBuff) ;
				pobjDlg->strCheckBackValue +=str; 
			}
		}


		if ( !bIsText ) {
			str.Format (_T ("%u bytes skipped..\r\n"), cbSize) ;
		}


	} catch (DWORD & nOpStat) {
		delete pobjRes ;
		pobjHttp->Cancel () ;

		// If the program is going to terminate,
		if ( nOpStat & CInterNetCheckDlg::TERMINATING )
		{
			return 0 ;
		}

		// canceled by user
		return 0 ;

	} catch (CHttpClient::Exception & e) {
		delete pobjRes ;
		pobjHttp->Cancel () ;

		// If the program is going to terminate,
		if ( pobjDlg->m_nOperationStat & CInterNetCheckDlg::TERMINATING )
		{
			return 0 ;
		}

//		pobjDlg->ReportHttpClientError (e) ;
//		::PostMessage (pobjDlg->GetSafeHwnd (), WM_THREADFINISHED, 0, 0) ;
		return 0 ;
	}

	delete pobjRes ;
//	::PostMessage (pobjDlg->GetSafeHwnd (), WM_THREADFINISHED, 0, 0) ;
	return 0 ;
}




void CInterNetCheckDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	HANDLE				m_hThread ;
	CString m_UserName,m_Password;
	strCheckBackValue = _T("");
	GetDlgItem(IDC_NAME)->GetWindowText(m_UserName);
	GetDlgItem(IDC_PASS)->GetWindowText(m_Password);

	m_strSvrURL=(_T("http://10.1.41.173/softreg/getinfo.asp"));
	m_objHttpClient.AddParam (_T("username"), m_UserName, 0) ;
	m_objHttpClient.AddParam (_T("password"), m_Password, 0) ;

	if ( NULL == (m_hThread = (HANDLE) ::_beginthreadex (
			NULL, 0
			, (unsigned (_stdcall *) (void *)) HttpProc
			, (void *) this, 0, NULL)) ) {
			AfxMessageBox(_T ("Thread creation failed")) ;
		// Restores the state.
		m_nOperationStat &= ~OPERATING ;
		return ;
	}
	WaitForSingleObject(m_hThread, INFINITE);

	if(strCheckBackValue == "123456789")
		AfxMessageBox("注册成功!");

	m_objHttpClient.RemoveParam (_T("password"), 0) ;
	m_objHttpClient.RemoveParam (_T("username"), 0) ;

	OnOK();
}
