
// H264DecodeDlg.h : ͷ�ļ�
#include "hi_config.h"
#include "hi_h264api.h"
#include "DisplayParaDlg.h"

#include <vfw.h>
#pragma comment(lib,"vfw32")
//
#define		MAX_LENGTH			(512<<20)		//512Mb
#define		WM_DISPLAY_FRAME	WM_USER+10
#define		WM_DECODE_COMPLETE	WM_USER+11

#define		IMAGE_WIDTH			2592//2048
#define		IMAGE_HEIGHT		1952//1536

#pragma once

// CH264DecodeDlg �Ի���
class CH264DecodeDlg : public CDialogEx
{
// ����
public:
	CH264DecodeDlg(CWnd* pParent = NULL);	// ��׼���캯��
	void DisplayVideo(unsigned char* displaybuf);

	int m_bDecoding;//0��δ���� 1����ʼ���� 2:��ͣ����
	CString	FileName;

	HDRAWDIB hdib;
	PBITMAPINFO m_bmpinfo;
	BITMAPINFO compbmp;
	HDC m_hdc;

	int remote_wnd_x , remote_wnd_y;
	CWnd *wnd,*bwnd;
	CRect rect,brect;

	CDisplayParaDlg displayparadlg;

	//�����̺߳���
	static DWORD DecodeThread(void *pDlg);
	bool AddFrame(LPBITMAPINFOHEADER alpbi,LPSTR lpdata);
	HANDLE hDecodeThread;
	DWORD dwThreadID;

// �Ի�������
	enum { IDD = IDD_H264DECODE_DIALOG };

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
	afx_msg LONG OnDisplayFrameMsg(WPARAM wParam,LPARAM lParam);
	afx_msg LONG OnDecodeCompleteMsg(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonDecode();
	afx_msg void OnDestroy();
};
