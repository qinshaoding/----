// InterNetCheckDlg.cpp : ʵ���ļ�
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


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CInterNetCheckDlg �Ի���



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


// CInterNetCheckDlg ��Ϣ�������

BOOL CInterNetCheckDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��\������...\���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	
	return TRUE;  // ���������˿ؼ��Ľ��㣬���򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CInterNetCheckDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ��������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
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


	//ΪPOST ��ʽ��0Ϊget,2Ϊupload
	nMethod = 1;

	// Sets the UseUtf8 property.��ʾ�Ƿ�ΪEncoding
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
		AfxMessageBox("ע��ɹ�!");

	m_objHttpClient.RemoveParam (_T("password"), 0) ;
	m_objHttpClient.RemoveParam (_T("username"), 0) ;

	OnOK();
}
