#include <vfw.h>

#define TEXT_HEIGHT	20
#define AVIIF_KEYFRAME	0x00000010L // this frame is a key frame.
#define BUFSIZE 260

class CAVIFile : public CObject
{
public:
	bool AddFrame(LPBITMAPINFOHEADER alpbi,LPSTR lpdata);
	CAVIFile(LPCTSTR lpszFileName, int xdim = -1, int ydim = -1);
	virtual ~CAVIFile();

	virtual bool AddFrame(CBitmap& bmp);
	virtual bool AddFrame(LPSTR lpframe);
	CString GetFName() const             {return FName;};
	virtual bool IsOK() const            {return bOK;};
	int timeinterval;
private:
	CString FName;
	int xDim;
	int yDim;

	AVISTREAMINFO strhdr;
	PAVIFILE pfile;
	PAVISTREAM ps;
	PAVISTREAM psCompressed;
	PAVISTREAM psText;
	AVICOMPRESSOPTIONS opts;
	AVICOMPRESSOPTIONS FAR * aopts[1];
	DWORD dwTextFormat;
	char szText[BUFSIZE];
	int nFrames;
	bool bOK;
};

