#pragma once


// CDisplayParaDlg �Ի���

class CDisplayParaDlg : public CDialog
{
	DECLARE_DYNAMIC(CDisplayParaDlg)

public:
	CDisplayParaDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDisplayParaDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_DISPLAYPARA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	int m_iWidth;
	int m_iHeight;
	int m_iFreq;
};
