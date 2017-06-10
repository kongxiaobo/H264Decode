#pragma once


// CDisplayParaDlg 对话框

class CDisplayParaDlg : public CDialog
{
	DECLARE_DYNAMIC(CDisplayParaDlg)

public:
	CDisplayParaDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDisplayParaDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_DISPLAYPARA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int m_iWidth;
	int m_iHeight;
	int m_iFreq;
};
