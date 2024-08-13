
// TestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Test.h"
#include "TestDlg.h"
#include "afxdialogex.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include <shellapi.h>


// CTestDlg dialog




CTestDlg::CTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTestDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CTestDlg::OnBnClickedOk)
END_MESSAGE_MAP()
// CTestDlg message handlers

BOOL CTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTestDlg::OnBnClickedOk()
{
	// Opens Application
	//These paths need to reflect the location of the exe files.
		CWnd *video1 = GetDlgItem(IDC_MFCEDITBROWSE1);
		CString v1;
		video1->GetWindowText(v1);
	if(v1 != ""){
		TCHAR path[] = TEXT("C:\\Users\\CSRL\\Dropbox\\Capstone-ARI\\Work in Progress\\MFPlayer2\\MFPlayer\\Release\\MFPlayer.exe");
		ShellExecute(NULL, _T("open"), path, v1, NULL, SW_SHOWNORMAL);
	}
		CWnd *video2 = GetDlgItem(IDC_MFCEDITBROWSE2);
		CString v2;
		video2->GetWindowText(v2);

		CWnd *logName = GetDlgItem(IDC_EDIT1);
		CString ln;
		logName->GetWindowText(ln);
		v2+= " ";
		v2+=ln;
	if(v2 != "" && ln != ""){
		TCHAR path2[] = TEXT("C:\\Users\\CSRL\\Dropbox\\Capstone-ARI\\Work in Progress\\MFPlayer1\\MFPlayerOwen\\Release\\MFPlayer.exe");
		ShellExecute(NULL, _T("open"), path2, v2, NULL, SW_SHOWNORMAL);
	}
		
	//Open log with user's name
	CDialogEx::OnOK();
}
