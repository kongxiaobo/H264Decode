// DisplayParaDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "H264Decode.h"
#include "DisplayParaDlg.h"
#include "afxdialogex.h"


// CDisplayParaDlg 对话框

IMPLEMENT_DYNAMIC(CDisplayParaDlg, CDialog)

CDisplayParaDlg::CDisplayParaDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDisplayParaDlg::IDD, pParent)
	, m_iWidth(2592)
	, m_iHeight(1952)
	, m_iFreq(20)
{

}

CDisplayParaDlg::~CDisplayParaDlg()
{
}

void CDisplayParaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_WIDTH, m_iWidth);
	DDX_Text(pDX, IDC_EDIT_HEIGHT, m_iHeight);
	DDX_Text(pDX, IDC_EDIT_FREQUENCE, m_iFreq);
}


BEGIN_MESSAGE_MAP(CDisplayParaDlg, CDialog)
END_MESSAGE_MAP()


// CDisplayParaDlg 消息处理程序
