
// H264DecodeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include <Windows.h>
#include "stdio.h"
#include "H264Decode.h"
#include "H264DecodeDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
int nalSize[4];
char *nalStartAddrs[4];
unsigned char frame_data[1024*1024*4];
int frame_len;
unsigned char* DisplayBuf = new unsigned char[10000000];
HI_HDL handle = NULL;
H264_LIBINFO_S    lib_info;
H264_DEC_ATTR_S   dec_attrbute;
H264_DEC_FRAME_S  dec_frame;
FILE *h264;
HI_U32 StrenthCoeff = 40;

//进行YUV->RGB转换以及显示视频的函数
//用来显示的全局变量
unsigned char *src[3];
BOOL PlayingVideo=FALSE;
unsigned char *clp = NULL;
unsigned char *clp1;
unsigned char bufRGB[16200000];

//进行YUV->RGB转换的函数
long int crv_tab[256];
long int cbu_tab[256];
long int cgu_tab[256];

long int cgv_tab[256];
long int tab_76309[256];

void init_dither_tab ()
{
	long int crv, cbu, cgu, cgv;
	int i;

	crv = 104597;
	cbu = 132201;                 /* fra matrise i global.h */
	cgu = 25675;
	cgv = 53279;

	for (i = 0; i < 256; i++)
	{
		crv_tab[i] = (i - 128) * crv;
		cbu_tab[i] = (i - 128) * cbu;
		cgu_tab[i] = (i - 128) * cgu;
		cgv_tab[i] = (i - 128) * cgv;
		tab_76309[i] = 76309 * (i - 16);
	}
	if (!(clp = (unsigned char *)malloc(sizeof(unsigned char)*1024)))
	{
		AfxMessageBox(_T("安排解码码表内存失败！"));
	}
	clp1 = clp;

	clp += 384;

	for (i = -384; i < 640; i++)
		clp[i] = (i < 0) ? 0 : ((i > 255) ? 255 : i);
}

void ConvertYUVtoRGB (unsigned char *src0, unsigned char *src1, unsigned char *src2,
	unsigned char *dst_ori,int width,int height)

{

	int y11, y21;
	int y12, y22;
	int y13, y23;
	int y14, y24;
	int u, v;
	int i, j;
	int c11, c21, c31, c41;
	int c12, c22, c32, c42;
	unsigned int DW;
	unsigned int *id1, *id2;
	unsigned char *py1, *py2, *pu, *pv;
	unsigned char *d1, *d2;

	d1 = dst_ori;
	d1 += width * height * 3 - width * 3;
	d2 = d1 - width * 3;

	py1 = src0;
	pu = src1;
	pv = src2;
	py2 = py1 + width;

	id1 = (unsigned int *) d1;
	id2 = (unsigned int *) d2;

	for (j = 0; j < height; j += 2)
	{
		/* line j + 0 */
		for (i = 0; i < width; i += 4)
		{
			u = *pu++;
			v = *pv++;
			c11 = crv_tab[v];
			c21 = cgu_tab[u];
			c31 = cgv_tab[v];
			c41 = cbu_tab[u];
			u = *pu++;
			v = *pv++;
			c12 = crv_tab[v];
			c22 = cgu_tab[u];
			c32 = cgv_tab[v];
			c42 = cbu_tab[u];

			y11 = tab_76309[*py1++];  /* (255/219)*65536 */
			y12 = tab_76309[*py1++];
			y13 = tab_76309[*py1++];  /* (255/219)*65536 */
			y14 = tab_76309[*py1++];

			y21 = tab_76309[*py2++];
			y22 = tab_76309[*py2++];
			y23 = tab_76309[*py2++];
			y24 = tab_76309[*py2++];

			/* RGBR */
			DW = ((clp[(y11 + c41) >> 16])) |
				((clp[(y11 - c21 - c31) >> 16]) << 8) |
				((clp[(y11 + c11) >> 16]) << 16) |
				((clp[(y12 + c41) >> 16]) << 24);
			*id1++ = DW;

			/* GBRG */
			DW = ((clp[(y12 - c21 - c31) >> 16])) |
				((clp[(y12 + c11) >> 16]) << 8) |
				((clp[(y13 + c42) >> 16]) << 16) |
				((clp[(y13 - c22 - c32) >> 16]) << 24);
			*id1++ = DW;

			/* BRGB */
			DW = ((clp[(y13 + c12) >> 16])) |
				((clp[(y14 + c42) >> 16]) << 8) |
				((clp[(y14 - c22 - c32) >> 16]) << 16) |
				((clp[(y14 + c12) >> 16]) << 24);
			*id1++ = DW;

			/* RGBR */
			DW = ((clp[(y21 + c41) >> 16])) |
				((clp[(y21 - c21 - c31) >> 16]) << 8) |
				((clp[(y21 + c11) >> 16]) << 16) |
				((clp[(y22 + c41) >> 16]) << 24);
			*id2++ = DW;

			/* GBRG */
			DW = ((clp[(y22 - c21 - c31) >> 16])) |
				((clp[(y22 + c11) >> 16]) << 8) |
				((clp[(y23 + c42) >> 16]) << 16) |
				((clp[(y23 - c22 - c32) >> 16]) << 24);
			*id2++ = DW;

			/* BRGB */
			DW = ((clp[(y23 + c12) >> 16])) |
				((clp[(y24 + c42) >> 16]) << 8) |
				((clp[(y24 - c22 - c32) >> 16]) << 16) |
				((clp[(y24 + c12) >> 16]) << 24);
			*id2++ = DW;
		}
		id1 -= (9 * width) >> 2;
		id2 -= (9 * width) >> 2;
		py1 += width;
		py2 += width;
	}
}

class CAboutDlg : public CDialogEx
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

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CH264DecodeDlg 对话框




CH264DecodeDlg::CH264DecodeDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CH264DecodeDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	hdib = NULL;
	hDecodeThread=NULL;
}

void CH264DecodeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CH264DecodeDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_DECODE, &CH264DecodeDlg::OnBnClickedButtonDecode)
	ON_MESSAGE(WM_DISPLAY_FRAME,OnDisplayFrameMsg)	
	ON_MESSAGE(WM_DECODE_COMPLETE,OnDecodeCompleteMsg)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// CH264DecodeDlg 消息处理程序

BOOL CH264DecodeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
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

	ShowWindow(SW_MINIMIZE);

	//初始化视频窗口
	// Adjust display windows	

	// For remote video display window
	wnd = this->GetDlgItem(IDC_REMOTEVIDEO);	// Video display window
	bwnd = this->GetDlgItem(IDC_REMOTEBORDER); // Border window...
	bwnd->GetWindowRect(brect);
	ScreenToClient(brect);

	remote_wnd_x=brect.TopLeft().x+(brect.Width()-IMAGE_WIDTH/2)/2;
	remote_wnd_y=brect.TopLeft().y+(brect.Height()-IMAGE_HEIGHT/2)/2;

	// Centre the remote video window
	wnd->SetWindowPos(&wndTop,remote_wnd_x-4,remote_wnd_y-4,IMAGE_WIDTH/2+9,IMAGE_HEIGHT/2+9,SWP_SHOWWINDOW | SWP_DRAWFRAME);

	// TODO: 在此添加额外的初始化代码
	m_bDecoding = 0;
	init_dither_tab();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CH264DecodeDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CH264DecodeDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
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
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CH264DecodeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CH264DecodeDlg::DisplayVideo(unsigned char* displaybuf)
{
	//ConvertYUVtoRGB (displaybuf, (displaybuf+IMAGE_HEIGHT*IMAGE_WIDTH), (displaybuf+(IMAGE_WIDTH*IMAGE_HEIGHT*5)/4), bufRGB, IMAGE_WIDTH, IMAGE_HEIGHT);
	ConvertYUVtoRGB (displaybuf, (displaybuf+displayparadlg.m_iHeight*displayparadlg.m_iWidth), (displaybuf+(displayparadlg.m_iWidth*displayparadlg.m_iHeight*5)/4), bufRGB, displayparadlg.m_iWidth, displayparadlg.m_iHeight);

	unsigned char* rgbdata = bufRGB;

	::DrawDibDraw(hdib,
		m_hdc,
		remote_wnd_x,		// dest : left pos
		remote_wnd_y,		// dest : top pos
		IMAGE_WIDTH/2,//-1,					 // don't zoom x
		IMAGE_HEIGHT/2,//-1,					 // don't zoom y
		&m_bmpinfo->bmiHeader,			 // bmp header info
		rgbdata,					 // bmp data
		0,					 // src :left
		0,					 // src :top
		displayparadlg.m_iWidth,				 // src : width
		displayparadlg.m_iHeight,				 // src : height
		DDF_SAME_DRAW			 // use prev params....
		);
}

LONG CH264DecodeDlg::OnDisplayFrameMsg(WPARAM wParam,LPARAM lParam)
{
	unsigned char* DisplayBuf = (unsigned char*)wParam;
	int length = (int)lParam;

	int i;
	for(i=0;i<length/((displayparadlg.m_iHeight*displayparadlg.m_iWidth*3)/2);i++)
	{
		DisplayVideo(DisplayBuf+i*(displayparadlg.m_iHeight*displayparadlg.m_iWidth*3)/2);
	}

	return 1;
}

LONG CH264DecodeDlg::OnDecodeCompleteMsg(WPARAM wParam,LPARAM lParam)
{
	SetDlgItemText(IDC_BUTTON_DECODE, _T("解码"));
	//停止编码线程
	if(hDecodeThread)
	{
		TerminateThread(hDecodeThread, 0);
		CloseHandle(hDecodeThread);
		hDecodeThread=NULL;
	}
	m_bDecoding = 0;

	OnOK();

	return 1;
}

#define INPUT_BUFFER_SIZE      0x200000

HI_U8 inputData[INPUT_BUFFER_SIZE];
HI_U32 prevtime = 0;
HI_U32 fileOffset = 0;
//解码线程
DWORD CH264DecodeDlg::DecodeThread(void *pDlg)
{
	CH264DecodeDlg* pDialog=(CH264DecodeDlg*)pDlg;

	int numNalUnits;
	frame_len = 0;
	char name[256] = {0};
	wchar_t* pawstr = NULL;
	pawstr = pDialog->FileName.GetBuffer(pDialog->FileName.GetLength()+1);
	wcstombs(name, pawstr, pDialog->FileName.GetLength()+1);

	h264 = fopen(name, "rb");
	HI_S32 end = 0;
	int ImageEnhanceEnable = 0;

	long p = 0;

	while (!end)
	{
		/* read bitstream from "h264" */
		HI_U32  len = (HI_U32)fread(inputData, 1, sizeof(inputData), h264);
		HI_U32  flags = (len>0)?0:1;
		HI_S32 result = 0;

		result = Hi264DecFrame(handle, inputData, len, 0, &dec_frame,  flags);

		while(HI_H264DEC_NEED_MORE_BITS  !=  result)
		{
			if(HI_H264DEC_NO_PICTURE ==  result)   //flush over and all the remain picture are output
			{
				end = 1;
				break;
			}

			if(HI_H264DEC_OK == result)   //get a picture
			{
				if (ImageEnhanceEnable)    //image enhance
				{
					Hi264DecImageEnhance(handle, &dec_frame, StrenthCoeff);
				}

				const HI_U8 *pY = dec_frame.pY;
				const HI_U8 *pU = dec_frame.pU;
				const HI_U8 *pV = dec_frame.pV;
				HI_U32 width    = dec_frame.uWidth;
				HI_U32 height   = dec_frame.uHeight - dec_frame.uCroppingBottomOffset;
				HI_U32 yStride  = dec_frame.uYStride;
				HI_U32 uvStride = dec_frame.uUVStride;

				/*fwrite(pY, 1, height* yStride, yuv);
				fwrite(pU, 1, height* uvStride/2, yuv);
				fwrite(pV, 1, height* uvStride/2, yuv);*/

				p = 0;

				memcpy(DisplayBuf+p, pY, height* yStride);
				p += height* yStride;

				memcpy(DisplayBuf+p, pU, height* uvStride/2);
				p += height* uvStride/2;

				memcpy(DisplayBuf+p, pV, height* uvStride/2);
				p += height* uvStride/2;

				pDialog->SendMessage(WM_DISPLAY_FRAME, (WPARAM)DisplayBuf, (LPARAM)p);

				if ((GetTickCount() - prevtime) < ((int)(1000/(pDialog->displayparadlg.m_iFreq+1))))
				{
					Sleep((int)(1000/(pDialog->displayparadlg.m_iFreq+1)) - (GetTickCount() - prevtime));
					prevtime = GetTickCount();
				}

				fileOffset += len;
			}

			/* continue decoding the remaining bitstream */
			result = Hi264DecFrame(handle, NULL,  0, 0, &dec_frame,  flags);
		}
	}

	//fclose(pFileyuv);
	pDialog->SendMessage(WM_DECODE_COMPLETE,(WPARAM)NULL,(LPARAM)NULL);
	return 0;
}

void CH264DecodeDlg::OnBnClickedButtonDecode()
{
	// TODO: 在此添加控件通知处理程序代码
	if (UpdateData(TRUE))
	{
		if (m_bDecoding != 1)
		{
			//判断是继续播放还是从头开始播放
			if (fileOffset == 0)
			{
				if (displayparadlg.DoModal() == IDOK)
				{
					if(hdib!=NULL)
					{
						::DrawDibEnd(hdib);
						::DrawDibClose(hdib);
					}

					// Initialize DIB for drawing...
					// Setup bmpinfo structure yourself
					m_bmpinfo=new BITMAPINFO;
					m_bmpinfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
					m_bmpinfo->bmiHeader.biWidth = displayparadlg.m_iWidth;
					m_bmpinfo->bmiHeader.biHeight = displayparadlg.m_iHeight;
					m_bmpinfo->bmiHeader.biPlanes = 1;
					m_bmpinfo->bmiHeader.biBitCount = 24;
					m_bmpinfo->bmiHeader.biCompression = 0;
					m_bmpinfo->bmiHeader.biSizeImage = 0;
					m_bmpinfo->bmiHeader.biXPelsPerMeter = 0;
					m_bmpinfo->bmiHeader.biYPelsPerMeter = 0;
					m_bmpinfo->bmiHeader.biClrUsed = 0;
					m_bmpinfo->bmiHeader.biClrImportant = 0;
					// Get Dialog DC
					m_hdc=this->GetDC()->m_hDC;
					hdib=::DrawDibOpen();
					if(hdib!=NULL)
					{

						::DrawDibBegin(hdib,
							m_hdc,
							-1,				// don't stretch
							-1,				// don't stretch
							&m_bmpinfo->bmiHeader,
							displayparadlg.m_iWidth,         // width of image
							displayparadlg.m_iHeight,        // height of image
							0				
							);

					}

					//获取输入文件和输出文件名称
					CFileDialog FileDlg(TRUE,NULL,NULL,OFN_HIDEREADONLY,TEXT("264 Files(*.*)|*.*||"));
					//FileDlg1.m_pOFN->lpstrTitle="请选择需要解码的文件";
					if(FileDlg.DoModal()==IDOK)
					{
						FileName=FileDlg.GetPathName();

						if ( 0 == Hi264DecGetInfo(&lib_info) )
						{
							fprintf(stderr, "Version: %s\nCopyright: %s\n\n", lib_info.sVersion, lib_info.sCopyRight);
							fprintf(stderr, "FunctionSet: 0x%x\n", lib_info.uFunctionSet);
						}

						dec_attrbute.uBufNum        = 16;     // reference frames number: 16
						dec_attrbute.uPicHeightInMB = 122;     // D1(720x576)
						dec_attrbute.uPicWidthInMB  = 162;
						dec_attrbute.uStreamInType  = 0x00;   // bitstream begin with "00 00 01" or "00 00 00 01"
						dec_attrbute.uWorkMode |= 0x20;
						handle = Hi264DecCreate(&dec_attrbute);

						if(hDecodeThread)
						{
							TerminateThread(hDecodeThread,0);
							CloseHandle(hDecodeThread);
							hDecodeThread=NULL;
						}

						//开始解码线程
						m_bDecoding = 1;
						SetDlgItemText(IDC_BUTTON_DECODE,_T("停止解码"));
						hDecodeThread=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)DecodeThread,(void *)this,0,&dwThreadID);
					}
					else
					{
						return;
					}
				}
			}
			else
			{
				//开始解码线程
				m_bDecoding = 1;
				SetDlgItemText(IDC_BUTTON_DECODE,_T("停止解码"));
			}
		}
		else
		{
			SetDlgItemText(IDC_BUTTON_DECODE,_T("继续解码"));
			m_bDecoding = 2;
		}
	}
}


void CH264DecodeDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	Hi264DecDestroy(handle);

	if (NULL != h264)
	{
		fclose(h264);
	}

	if(hDecodeThread)
	{
		TerminateThread(hDecodeThread,0);
		CloseHandle(hDecodeThread);
		hDecodeThread=NULL;
	}

	if(m_bmpinfo)
	{
		delete m_bmpinfo;
	}
	// Close graphics....
	if(hdib!=NULL)
	{
		::DrawDibEnd(hdib);
		::DrawDibClose(hdib);
	}

	delete DisplayBuf;
	DisplayBuf = NULL;

	free(clp1);
}
