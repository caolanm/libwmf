// testView.cpp : implementation of the CTestView class
//


#include <Windows.h>




#include "stdafx.h"
#include "test.h"

#include "testDoc.h"
#include "testView.h"

#include "Resource.h"

#include <WINDOWS.H>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

typedef struct _SMALL_RECT { // srct 
    SHORT Left; 
    SHORT Top; 
    SHORT Right; 
    SHORT Bottom; 
} SMALL_RECT; 

#pragma pack( push )
#pragma pack( 2 )
typedef struct
{
	DWORD		dwKey;
	WORD		hmf;
	SMALL_RECT	bbox;
	WORD		wInch;
	DWORD		dwReserved;
	WORD		wCheckSum;
} APMHEADER, *PAPMHEADER;
#pragma pack( pop )




WORD CalculateAPMCheckSum( APMHEADER apmfh );
BOOL WriteEnhMetaFileAsPlaceable( HENHMETAFILE hMeta, LPTSTR szFileName );

//#pragma pack( pop )

/////////////////////////////////////////////////////////////////////////////
// CTestView

IMPLEMENT_DYNCREATE(CTestView, CView)

BEGIN_MESSAGE_MAP(CTestView, CView)
	//{{AFX_MSG_MAP(CTestView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestView construction/destruction

CTestView::CTestView()
{
	// TODO: add construction code here
	/*
	HDC met=CreateEnhMetafile("c:\test.wmf");
	Rectangle(met,100,100,100,100);
	CloseMetafile(met);
	*//*
	RECT rect;
	rect.top = 0;
	rect.right=100;
	rect.left=0;
	rect.bottom=100;
	CDC *dc = GetDC();
	dc->Rectangle(0,0,100,100);
	*/

}

CTestView::~CTestView()
{
}

BOOL CTestView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CTestView drawing LOGPEN

void CTestView::OnDraw(CDC* pDC)
{
	CTestDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CDC *dc = GetDC();
	dc->SetBkColor(RGB(0x40,0x40,0x40));		
	
	dc->Rectangle(10,10,80,80);

	CPen cpen(PS_DASH,1,RGB(0xff,0x00,0x00));
	dc->SelectObject(&cpen);
	dc->Rectangle(90,10,160,80);
	CPen cpen2(PS_DOT,1,RGB(0x00,0xff,0x00));
	dc->SelectObject(&cpen2);
	dc->Rectangle(10,90,80,160);
	CPen cpen3(PS_DASHDOT,1,RGB(0x00,0x00,0xff));
	dc->SelectObject(&cpen3);
	dc->Rectangle(170,10,240,80);

	CPen cpen5(PS_SOLID,10,RGB(0xff,0xff,0x00));
	dc->SelectObject(&cpen5);
	dc->Rectangle(90,90,160,160);
	
	CPen cpen4(PS_DASHDOTDOT,1,RGB(0x00,0xff,0xff));
	dc->SelectObject(&cpen4);
	dc->Rectangle(170,90,240,160);

	CPen cpen6(PS_DASHDOTDOT,10,RGB(0xff,0x00,0xff));
	dc->SelectObject(&cpen6);
	dc->Rectangle(10,170,80,240);

	CBrush brush1(RGB(0xff,0xff,0x00));
	dc->SelectObject(&brush1);
	dc->Rectangle(90,170,160,240);

	dc->SelectObject(&cpen);
	dc->Rectangle(170,170,240,240);

	CPen cpen7(PS_INSIDEFRAME,10,RGB(0xff,0x00,0xff));
	dc->SelectObject(&cpen7);
	dc->Rectangle(250,170,320,240);

	CPen cpen8(PS_NULL,10,RGB(0xff,0x00,0xff));
	dc->SelectObject(cpen8);
	dc->Rectangle(250,90,320,160);

	CBrush brush2(HS_BDIAGONAL,RGB(0xff,0xff,0x00));
	dc->SelectObject(&brush2);
	dc->Rectangle(10,250,80,320);

	CBrush brush3(HS_CROSS,RGB(0xff,0xff,0x00));
	dc->SelectObject(&brush3);
	dc->Rectangle(90,250,160,320);
	

	CBitmap bits;
	bits.LoadOEMBitmap(32737);
	CBrush brush4(&bits);
	dc->SelectObject(&brush4);
	dc->Rectangle(170,250,240,320);

	dc->TextOut(0, 340, "I am a string", strlen("I am a string") );

	LOGFONT afont;
	afont.lfHeight=20;
	afont.lfWidth=0;
	afont.lfEscapement=0;
	afont.lfOrientation=0;
	afont.lfWeight=0; 
	afont.lfItalic=0; 
	afont.lfUnderline=0; 
	afont.lfStrikeOut=0; 
	afont.lfCharSet=ANSI_CHARSET; 
	afont.lfOutPrecision=OUT_DEFAULT_PRECIS; 
	afont.lfClipPrecision=CLIP_DEFAULT_PRECIS; 
	afont.lfQuality=DEFAULT_QUALITY; 
	afont.lfPitchAndFamily=DEFAULT_PITCH|FF_ROMAN;
	strcpy(afont.lfFaceName,"Garamond");
	HFONT cfont = CreateFontIndirect(&afont);

	dc->SelectObject(cfont);

	RECT rect;
	rect.left=0;
	rect.right=40;
	rect.top=360;
	rect.bottom=380;
	dc->ExtTextOut(0,360, ETO_CLIPPED, &rect, "Another Test",strlen("Another Test"), NULL );

	rect.left=0;
	rect.right=40;
	rect.top=380;
	rect.bottom=400;
	dc->SetBkColor(RGB(190,190,190));
	strcpy(afont.lfFaceName,"Arial");
	cfont = CreateFontIndirect(&afont);
	dc->SelectObject(cfont);
	dc->ExtTextOut(0,380, ETO_CLIPPED|ETO_OPAQUE, &rect, "Another Test",strlen("Another Test"), NULL );

	rect.left=0;
	rect.right=40;
	rect.top=400;
	rect.bottom=420;
	dc->SetBkColor(RGB(190,190,190));
	strcpy(afont.lfFaceName,"Arial Black");
	cfont = CreateFontIndirect(&afont);
	dc->SelectObject(cfont);
	dc->SetTextCharacterExtra(10);
	dc->ExtTextOut(0,400, ETO_OPAQUE, &rect, "Yet Another Test",strlen("Yet Another Test"), NULL );

	
	strcpy(afont.lfFaceName,"Arial Bold");
	cfont = CreateFontIndirect(&afont);
	dc->SelectObject(cfont);
	dc->SetTextCharacterExtra(0);
	dc->SetTextJustification(60,2);
	dc->ExtTextOut(0,420, ETO_OPAQUE, NULL, "This is yet Yet Another Test",strlen("This is yet yet Another Test"), NULL );

	dc->SelectObject(&brush1);
	dc->RoundRect(0,440,70,510,30,40);

	dc->SelectObject(&cpen7);
	dc->RoundRect(80,440,150,510,30,40);

	dc->SelectObject(&cpen6);
	dc->RoundRect(160,440,230,510,30,40);

	POINT apoints[10];
	apoints[0].x = 240;
	apoints[0].y = 440;
	apoints[1].x = 240;
	apoints[1].y = 500;
	apoints[2].x = 300;
	apoints[2].y = 500;
	apoints[3].x = 340;
	apoints[3].y = 400;
	dc->Polyline(apoints,4);


	apoints[0].x = 350;
	apoints[0].y = 440;
	apoints[1].x = 350;
	apoints[1].y = 500;
	apoints[2].x = 410;
	apoints[2].y = 500;
	apoints[3].x = 450;
	apoints[3].y = 400;
	dc->SelectObject(&cpen6);
	dc->Polygon(apoints,4);


	apoints[0].x = 350;
	apoints[0].y = 340;
	apoints[1].x = 350;
	apoints[1].y = 400;
	apoints[2].x = 410;
	apoints[2].y = 400;
	apoints[3].x = 450;
	apoints[3].y = 300;
	dc->SelectObject(&cpen7);
	dc->Polygon(apoints,4);


	int lists[2];
	lists[0] = 5;
	lists[1] = 4;

	apoints[0].x = 350;
	apoints[0].y = 10;
	apoints[1].x = 400;
	apoints[1].y = 10;
	apoints[2].x = 400;
	apoints[2].y = 80;
	apoints[3].x = 350;
	apoints[3].y = 80;
	apoints[4].x = 350;
	apoints[4].y = 10;

	apoints[5].x = 400;
	apoints[5].y = 10;
	apoints[6].x = 500;
	apoints[6].y = 10;
	apoints[7].x = 450;
	apoints[7].y = 80;
	apoints[8].x = 400;
	apoints[8].y = 10;

	dc->PolyPolygon(apoints,lists,2);

	
	dc->SelectObject(&cpen6);
	dc->Pie(350,80,500,160,10,10,500,500);

	dc->SelectObject(&cpen7);
	dc->Pie(450,80,600,160,110,10,600,500);

	dc->Ellipse(510,10,610,90);

	dc->SelectObject(&cpen6);
	dc->Ellipse(620,10,720,90);

	dc->SelectObject(&cpen6);
	dc->Arc(350,170,500,250,110,80,600,570);

	dc->SelectObject(&cpen7);
	dc->Arc(450,170,600,250,110,80,600,570);


	RECT arect;
	arect.left=570;
	arect.top=100;
	arect.right=670;
	arect.bottom=160;
	dc->DrawFocusRect(&arect);

	dc->SelectObject(&cpen6);
	dc->Chord(550,170,700,250,310,80,800,570);
	
	dc->SelectObject(&cpen7);
	dc->Chord(650,170,800,250,410,80,900,570);

/* DrawDr
	dc->SelectObject(&cpen6);
	dc->Draw3dRect(550,270,70,70,RGB(255,0,0),RGB(0,255,0));
		
	dc->SelectObject(&cpen7);
	dc->Draw3dRect(650,270,70,70,RGB(0,255,0),RGB(255,0,255));
*/

	dc->SelectObject(&cpen6);
	RECT brect;
	brect.left=550;
	brect.top=270;
	brect.right=620;
	brect.bottom=340;
	dc->FillRect(&brect,&brush1);
	RECT crect;
	crect.left=650;
	crect.top=270;
	crect.right=720;
	crect.bottom=340;
	dc->FrameRect(&crect,&brush1);

	RECT drect;
	drect.left=750;
	drect.top=270;
	drect.right=820;
	drect.bottom=340;



	dc->FillRect(&drect,&brush1);
	dc->InvertRect(&drect);

	CWinApp *myapp = AfxGetApp(); 

	HICON myicon = myapp->LoadStandardIcon(IDI_HAND);
	dc->DrawIcon(820,20,myicon);
	
	POINT mypoints[4];

	mypoints[0].x = 0;
	mypoints[0].y = 0;
	mypoints[1].x = 100;
	mypoints[1].y = 100;
	mypoints[2].x = 100;
	mypoints[2].y = 30;

	dc->PolyBezierTo(mypoints,3);

	
	afont.lfHeight=20;
	afont.lfWidth=0;
	afont.lfEscapement=90*10;
	afont.lfOrientation=0;
	afont.lfWeight=0; 
	afont.lfItalic=0; 
	afont.lfUnderline=1; 
	afont.lfStrikeOut=1; 
	afont.lfCharSet=ANSI_CHARSET; 
	afont.lfOutPrecision=OUT_DEFAULT_PRECIS; 
	afont.lfClipPrecision=CLIP_DEFAULT_PRECIS; 
	afont.lfQuality=DEFAULT_QUALITY; 
	afont.lfPitchAndFamily=DEFAULT_PITCH|FF_ROMAN;
	strcpy(afont.lfFaceName,"Garamond");
	HFONT dfont = CreateFontIndirect(&afont);

	dc->SelectObject(dfont);

	dc->TextOut(500, 500, "I am a string", strlen("I am a string") );

	dc->SetPixel(550,500,RGB(0x40,0x40,0x40));
	dc->SetPixel(551,501,RGB(0x40,0x40,0x40));
	dc->SetPixel(552,502,RGB(0x40,0x40,0x40));	
	dc->SetPixel(553,503,RGB(0x40,0x40,0x40));	
	dc->SetPixel(554,504,RGB(0x40,0x40,0x40));	
	dc->SetPixel(555,505,RGB(0x40,0x40,0x40));	

	LOGBRUSH lbrush2;
	lbrush2.lbStyle=BS_NULL;	
	/*lbrush.lbHatch;*/
	CBrush brush10;
	brush10.CreateBrushIndirect(&lbrush2);
	dc->SelectObject(brush10);
	CPen cpen10(PS_SOLID,1,RGB(0x80,0x90,0x20));
	dc->SelectObject(&cpen10);
	dc->Rectangle(560,510,570,570);

	dc->FloodFill(565,540,RGB(0x80,0x90,0x20));

	dc->PatBlt(565,400,80,80,BLACKNESS);

	dc->PatBlt(645,400,80,80,BLACKNESS);
	dc->PatBlt(645,400,80,80,WHITENESS);

	//dc->BitBlt(745,400,80,80,dc,0,0,SRCCOPY);
//	BitBlt(dc->GetSafeHdc(),745,400,80,80,dc->GetSafeHdc(),0,0,SRCCOPY);

	//dc->CreateRectRgn(645,400,645+80,400+80);		

	HENHMETAFILE mtf;

	//HDC met=CreateEnhMetaFile(NULL,"c:\\test.wmf",NULL,NULL);
	HDC met=CreateMetaFile("c:\\test.wmf");
	/*
	SetMapMode(met,MM_ANISOTROPIC);
	SetWindowExtEx(met,800,800,NULL);
	*/
	SetBkColor(met,RGB(0x40,0x40,0x40));		

	Rectangle(met,10,10,80,80);

	LOGPEN lpen;
	lpen.lopnStyle=PS_DASH;
	lpen.lopnWidth.x=1;
	lpen.lopnColor=RGB(0xff,0x00,0x00);
	
	HPEN pen=CreatePenIndirect(&lpen);
	SelectObject(met,pen);
	Rectangle(met,90,10,160,80);
	DeleteObject(pen);
	
	lpen.lopnStyle=PS_DOT;
	lpen.lopnColor=RGB(0x00,0xff,0x00);
	pen=CreatePenIndirect(&lpen);
	SelectObject(met,pen);
	Rectangle(met,10,90,80,160);
	DeleteObject(pen);

	lpen.lopnStyle=PS_DASHDOT;
	lpen.lopnColor=RGB(0x00,0x00,0xff);
	pen=CreatePenIndirect(&lpen);
	SelectObject(met,pen);
	Rectangle(met,170,10,240,80);
	DeleteObject(pen);

	lpen.lopnStyle=PS_SOLID;
	lpen.lopnColor=RGB(0xff,0xff,0x00);
	lpen.lopnWidth.x=10;
	pen=CreatePenIndirect(&lpen);
	SelectObject(met,pen);
	Rectangle(met,90,90,160,160);
	DeleteObject(pen);

	lpen.lopnStyle=PS_DASHDOTDOT;
	lpen.lopnWidth.x=1;
	lpen.lopnColor=RGB(0x00,0xff,0xff);
	pen=CreatePenIndirect(&lpen);
	SelectObject(met,pen);
	Rectangle(met,170,90,240,160);
	DeleteObject(pen);

	//lpen.lopnStyle= PS_INSIDEFRAME;
	lpen.lopnColor=RGB(0xff,0x00,0xff);
	lpen.lopnWidth.x=10;
	pen=CreatePenIndirect(&lpen);
	SelectObject(met,pen);
	Rectangle(met,10,170,80,240);
	

	LOGBRUSH lbrush;
	HBRUSH brush;
	lbrush.lbStyle=BS_SOLID;
	lbrush.lbColor=RGB(0xff,0xff,0x00);
	/*lbrush.lbHatch;*/
	brush=CreateBrushIndirect(&lbrush);
	SelectObject(met,brush);
	Rectangle(met,90,170,160,240);
	
	DeleteObject(pen);

	lpen.lopnStyle=PS_DASH;
	lpen.lopnWidth.x=1;
	lpen.lopnColor=RGB(0xff,0x00,0x00);
	pen=CreatePenIndirect(&lpen);
	SelectObject(met,pen);
	Rectangle(met,170,170,240,240);

	DeleteObject(pen);

	lpen.lopnStyle= PS_INSIDEFRAME;
	lpen.lopnWidth.x=10;
	lpen.lopnColor=RGB(0xff,0x99,0x00);
	pen=CreatePenIndirect(&lpen);
	SelectObject(met,pen);
	Rectangle(met,250,170,320,240);

	lpen.lopnStyle= PS_NULL;
	lpen.lopnWidth.x=1;
	lpen.lopnColor=RGB(0xff,0x99,0x00);
	pen=CreatePenIndirect(&lpen);
	SelectObject(met,pen);
	Rectangle(met,250,90,320,160);

	DeleteObject(brush);
	DeleteObject(pen);

	lpen.lopnStyle= PS_NULL;
	pen=CreatePenIndirect(&lpen);
	SelectObject(met,pen);

	lbrush.lbStyle=BS_HATCHED;
	lbrush.lbColor=RGB(0xff,0xff,0x00);
	lbrush.lbHatch=HS_BDIAGONAL;

	brush=CreateBrushIndirect(&lbrush);

	SelectObject(met,brush);
	Rectangle(met,10,250,80,320);

	DeleteObject(brush);

	lbrush.lbHatch=HS_CROSS;
	brush=CreateBrushIndirect(&lbrush);
	SelectObject(met,brush);
	Rectangle(met,90,250,160,320);


	DeleteObject(brush);
	
	/*CBitmap bits;
	bits.LoadOEMBitmap(32738);
	CBrush brush4(&bits);
	*/
	lbrush.lbStyle=BS_PATTERN;
	lbrush.lbHatch=(long)HBITMAP(bits);
	brush=CreateBrushIndirect(&lbrush);
	SelectObject(met,brush);
	Rectangle(met,170,250,240,320);


	TextOut(met,0, 340, "I am a string", strlen("I am a string") );

	rect.left=0;
	rect.right=40;
	rect.top=360;
	rect.bottom=380;
	ExtTextOut(met,0,360, ETO_CLIPPED, &rect, "Another Test",strlen("Another Test"), NULL );

	rect.left=0;
	rect.right=40;
	rect.top=380;
	rect.bottom=400;
	SetBkColor(met,RGB(190,190,190));
	ExtTextOut(met,0,380, ETO_CLIPPED|ETO_OPAQUE, &rect, "Another Test",strlen("Another Test"), NULL );

	rect.left=0;
	rect.right=0;
	rect.top=400;
	rect.bottom=900;
	SetBkColor(met,RGB(190,190,190));
	
	SetTextCharacterExtra(met,10);
	
	//ExtTextOut(met,0,400, ETO_OPAQUE, NULL, "Yet Another Test",strlen("Yet Another Test"), NULL );
	

	SelectObject(met,cfont);
	SetTextCharacterExtra(met,0);
	SetTextJustification(met,60,2);
	
	//ExtTextOut(met,0,420, ETO_OPAQUE, NULL, "This is yet Yet Another Test",strlen("This is yet yet Another Test"), NULL );


	SelectObject(met,dfont);
	TextOut(met,500, 500, "I am a string", strlen("I am a string") );

	lbrush.lbStyle=BS_SOLID;
	lbrush.lbColor=RGB(0xff,0xff,0x00);
	/*lbrush.lbHatch;*/
	brush=CreateBrushIndirect(&lbrush);
	
	SelectObject(met,brush);

	RoundRect(met,0,440,70,510,30,40);

	lpen.lopnStyle= PS_INSIDEFRAME;
	lpen.lopnWidth.x=10;
	lpen.lopnColor=RGB(0xff,0x99,0x00);
	pen=CreatePenIndirect(&lpen);
	SelectObject(met,pen);
	RoundRect(met,80,440,150,510,30,40);

	lpen.lopnStyle= PS_SOLID;
	pen=CreatePenIndirect(&lpen);
	SelectObject(met,pen);
	RoundRect(met,160,440,230,510,30,40);

	apoints[0].x = 240;
	apoints[0].y = 440;
	apoints[1].x = 240;
	apoints[1].y = 500;
	apoints[2].x = 300;
	apoints[2].y = 500;
	apoints[3].x = 340;
	apoints[3].y = 400;
	Polyline(met,apoints,4);
	

	apoints[0].x = 350;
	apoints[0].y = 440;
	apoints[1].x = 350;
	apoints[1].y = 500;
	apoints[2].x = 410;
	apoints[2].y = 500;
	apoints[3].x = 450;
	apoints[3].y = 400;
	Polygon(met,apoints,4);

	apoints[0].x = 350;
	apoints[0].y = 340;
	apoints[1].x = 350;
	apoints[1].y = 400;
	apoints[2].x = 410;
	apoints[2].y = 400;
	apoints[3].x = 450;
	apoints[3].y = 300;
	lpen.lopnStyle= PS_INSIDEFRAME;
	lpen.lopnWidth.x=10;
	lpen.lopnColor=RGB(0xff,0x99,0x00);
	pen=CreatePenIndirect(&lpen);
	SelectObject(met,pen);
	Polygon(met,apoints,4);

	//BOOL Polygon( LPPOINT lpPoints, int nCount );

	lists[0] = 5;
	lists[1] = 4;

	apoints[0].x = 350;
	apoints[0].y = 10;
	apoints[1].x = 400;
	apoints[1].y = 10;
	apoints[2].x = 400;
	apoints[2].y = 80;
	apoints[3].x = 350;
	apoints[3].y = 80;
	apoints[4].x = 350;
	apoints[4].y = 10;

	apoints[5].x = 400;
	apoints[5].y = 10;
	apoints[6].x = 500;
	apoints[6].y = 10;
	apoints[7].x = 450;
	apoints[7].y = 80;
	apoints[8].x = 400;
	apoints[8].y = 10;

	PolyPolygon(met,apoints,lists,2);

	lpen.lopnStyle= PS_SOLID;
	lpen.lopnWidth.x=10;
	lpen.lopnColor=RGB(0xff,0x99,0x00);
	pen=CreatePenIndirect(&lpen);
	SelectObject(met,pen);
	Pie(met,350,80,500,160,10,10,500,500);
	Arc(met,350,170,500,250,110,80,600,570);
	Chord(met,550,170,700,250,310,80,800,570);
	
	
	lpen.lopnStyle= PS_INSIDEFRAME;
	lpen.lopnWidth.x=10;
	lpen.lopnColor=RGB(0xff,0x99,0x00);
	pen=CreatePenIndirect(&lpen);
	SelectObject(met,pen);
	Pie(met,450,80,600,160,110,10,600,500);
	Arc(met,450,170,600,250,110,80,600,570);
	Chord(met,650,170,800,250,410,80,900,570);	
	
	

	Ellipse(met,510,10,610,90);

	lpen.lopnStyle= PS_SOLID;
	lpen.lopnWidth.x=10;
	lpen.lopnColor=RGB(0xff,0x99,0x00);
	pen=CreatePenIndirect(&lpen);
	SelectObject(met,pen);
	Ellipse(met,620,10,720,90);

	FillRect(met,&brect,brush);
	FrameRect(met,&crect,brush);
	
	FillRect(met,&drect,brush);
	InvertRect(met,&drect);

	DrawIcon(met,820,20,myicon);
	

/*
the same as void FrameRect( LPCRECT lpRect, CBrush* pBrush );
LOGBRUSH
*/
	DrawFocusRect(met,&arect);

	PolyBezierTo(met,mypoints,3);

	SetPixel(met,550,500,RGB(0x40,0x40,0x40));
	SetPixel(met,551,501,RGB(0x40,0x40,0x40));
	SetPixel(met,552,502,RGB(0x40,0x40,0x40));	
	SetPixel(met,553,503,RGB(0x40,0x40,0x40));	
	SetPixel(met,554,504,RGB(0x40,0x40,0x40));	
	SetPixel(met,555,505,RGB(0x40,0x40,0x40));	

//	HBRUSH hbrush10 = CreateBrushIndirect(&lbrush2);
//	SelectObject(met,hbrush10);
	lpen.lopnStyle= PS_SOLID;
	lpen.lopnWidth.x=1;
	lpen.lopnColor=RGB(0x80,0x90,0x20);
	HPEN pen4=CreatePenIndirect(&lpen);
	SelectObject(met,pen4);

	Rectangle(met,560,510,570,570);

	FloodFill(met,565,540,RGB(0x80,0x90,0x20));

	PatBlt(met,565,400,80,80,BLACKNESS);
	PatBlt(met,645,400,80,80,BLACKNESS);
	PatBlt(met,645,400,80,80,WHITENESS);

	
	BitBlt(met,0,0,80,80,NULL,10,20,BLACKNESS);

	Rectangle(met,90,0,170,80);
	BitBlt(met,90,0,80,80,NULL,0,0,DSTINVERT);

	BitBlt(met,180,0,80,80,NULL,0,0,PATCOPY);

	BitBlt(met,270,0,80,80,NULL,0,0,PATINVERT);

	BitBlt(met,360,0,80,80,NULL,0,0,WHITENESS);

	char *mybits = (char *)malloc(80*80);
	for(int i=0;i<80*80;i++)
		mybits[i] = 0xff;

	
	HBITMAP mytest = CreateCompatibleBitmap(met,80,80);
	
	HDC adc = CreateCompatibleDC(met);
	//if (adc == NULL)
	//	exit(-1);
	SelectObject(adc,mytest);
	//SetMapMode(adc,GetMapMode(met));

	//if (FALSE == BitBlt(met,450,0,80,80,adc,0,0,MERGECOPY))
		//exit(-1);

	HBITMAP mytest2 = CreateCompatibleBitmap(dc->GetSafeHdc(),80,80);
	HDC adc2 = CreateCompatibleDC(dc->GetSafeHdc());
	SelectObject(adc2,mytest2);

	BitBlt(adc2,0,0,80,80,dc->GetSafeHdc(),0,0,SRCCOPY);

	//SetMapMode(adc,GetMapMode(met));

	//if (FALSE == BitBlt(dc->GetSafeHdc(),0,0,80,80,adc2,0,0,SRCCOPY))
	if (FALSE == BitBlt(met,0,90,80,80,adc2,0,0,SRCCOPY))
		exit(-1);

	if (FALSE == BitBlt(met,90,90,80,80,adc2,0,0,SRCAND))
		exit(-1);

	if (FALSE == BitBlt(met,180,90,80,80,adc2,0,0,MERGECOPY))
		exit(-1);

	if (FALSE == BitBlt(met,270,90,80,80,adc2,0,0,NOTSRCCOPY))
		exit(-1);

	if (FALSE == BitBlt(met,360,90,80,80,adc2,0,0,NOTSRCERASE))
		exit(-1);

	if (FALSE == BitBlt(met,450,90,80,80,adc2,0,0,SRCERASE))
		exit(-1);

	if (FALSE == BitBlt(met,540,90,80,80,adc2,0,0,SRCINVERT))
		exit(-1);

	if (FALSE == BitBlt(met,0,180,80,80,adc2,0,0,SRCPAINT))
		exit(-1);

	if (FALSE == BitBlt(met,90,180,80,80,adc2,0,0,MERGEPAINT))
		exit(-1);

	if (FALSE == BitBlt(met,180,180,80,80,adc2,0,0,PATPAINT))
		exit(-1);
	/*
	BitBlt(met,745,400,80,80,NULL,0,0,BLACKNESS);
	*/
	

	HDC bltmet=CreateMetaFile("c:\\bitblt.wmf");
	//SetBkColor(bltmet,RGB(0x40,0x40,0x40));	

	lbrush.lbStyle=BS_SOLID;
	lbrush.lbColor=RGB(0xff,0xff,0x00);
	/*lbrush.lbHatch;*/
	brush=CreateBrushIndirect(&lbrush);
	
	SelectObject(bltmet,brush);

	Rectangle(bltmet,0,0,700,600);

	HBITMAP fromtest = CreateCompatibleBitmap(dc->GetSafeHdc(),80,80);
	HDC fromdc = CreateCompatibleDC(dc->GetSafeHdc());
	SelectObject(fromdc,fromtest);

	lbrush.lbStyle=BS_SOLID;
	lbrush.lbColor=RGB(0x45,0x45,0x45);
	/*lbrush.lbHatch;*/
	brush=CreateBrushIndirect(&lbrush);
	
	SelectObject(bltmet,brush);

	BitBlt(fromdc,0,0,80,80,dc->GetSafeHdc(),0,0,SRCCOPY);

	if (FALSE == BitBlt(bltmet,0,0,80,80,fromdc,0,0,SRCCOPY))
		exit(-1);

	if (FALSE == BitBlt(bltmet,90,0,80,80,adc2,0,0,SRCAND))
		exit(-1);

	if (FALSE == BitBlt(bltmet,180,0,80,80,adc2,0,0,MERGECOPY))
		exit(-1);

	if (FALSE == BitBlt(bltmet,270,0,80,80,adc2,0,0,NOTSRCCOPY))
		exit(-1);

	if (FALSE == BitBlt(bltmet,360,0,80,80,adc2,0,0,NOTSRCERASE))
		exit(-1);

	if (FALSE == BitBlt(bltmet,450,0,80,80,adc2,0,0,SRCERASE))
		exit(-1);

	if (FALSE == BitBlt(bltmet,540,0,80,80,adc2,0,0,SRCINVERT))
		exit(-1);

	if (FALSE == BitBlt(bltmet,0,90,80,80,adc2,0,0,SRCPAINT))
		exit(-1);

	if (FALSE == BitBlt(bltmet,90,90,80,80,adc2,0,0,MERGEPAINT))
		exit(-1);

	if (FALSE == BitBlt(bltmet,180,90,80,80,adc2,0,0,PATPAINT))
		exit(-1);

	if (FALSE == BitBlt(bltmet,270,90,80,80,adc2,0,0,BLACKNESS))
		exit(-1);

	if (FALSE == BitBlt(bltmet,360,90,80,80,adc2,0,0,DSTINVERT))
		exit(-1);

	if (FALSE == BitBlt(bltmet,450,90,80,80,adc2,0,0,PATCOPY))
		exit(-1);

	if (FALSE == BitBlt(bltmet,540,90,80,80,adc2,0,0,PATINVERT))
		exit(-1);

	if (FALSE == BitBlt(bltmet,0,180,80,80,adc2,0,0,WHITENESS))
		exit(-1);

	PatBlt(bltmet,90,180,80,80,PATCOPY);
	PatBlt(bltmet,180,180,80,80,PATINVERT);
	PatBlt(bltmet,270,180,80,80,DSTINVERT);
	PatBlt(bltmet,360,180,80,80,BLACKNESS);
	PatBlt(bltmet,450,180,80,80,WHITENESS);

	if (FALSE == BitBlt(bltmet,0,270,80,80,NULL,0,0,SRCCOPY))
		exit(-1);

	if (FALSE == BitBlt(bltmet,90,270,80,80,NULL,0,0,SRCAND))
		exit(-1);

	if (FALSE == BitBlt(bltmet,180,270,80,80,NULL,0,0,MERGECOPY))
		exit(-1);

	if (FALSE == BitBlt(bltmet,270,270,80,80,NULL,0,0,NOTSRCCOPY))
		exit(-1);

	if (FALSE == BitBlt(bltmet,360,270,80,80,NULL,0,0,NOTSRCERASE))
		exit(-1);

	if (FALSE == BitBlt(bltmet,450,270,80,80,NULL,0,0,SRCERASE))
		exit(-1);

	if (FALSE == BitBlt(bltmet,540,270,80,80,NULL,0,0,SRCINVERT))
		exit(-1);

	if (FALSE == BitBlt(bltmet,0,360,80,80,NULL,0,0,SRCPAINT))
		exit(-1);

	if (FALSE == BitBlt(bltmet,90,360,80,80,NULL,0,0,MERGEPAINT))
		exit(-1);

	if (FALSE == BitBlt(bltmet,180,360,80,80,NULL,0,0,PATPAINT))
		exit(-1);

	if (FALSE == BitBlt(bltmet,270,360,80,80,NULL,0,0,BLACKNESS))
		exit(-1);

	if (FALSE == BitBlt(bltmet,360,360,80,80,NULL,0,0,DSTINVERT))
		exit(-1);

	if (FALSE == BitBlt(bltmet,450,360,80,80,NULL,0,0,PATCOPY))
		exit(-1);

	if (FALSE == BitBlt(bltmet,540,360,80,80,NULL,0,0,PATINVERT))
		exit(-1);

	if (FALSE == BitBlt(bltmet,0,450,80,80,NULL,0,0,WHITENESS))
		exit(-1);

	if (FALSE == BitBlt(bltmet,90,450,60,60,adc2,0,0,SRCCOPY))
		exit(-1);

	if (FALSE == BitBlt(bltmet,180,450,60,60,adc2,10,20,SRCCOPY))
		exit(-1);


	

	WORD wBrickBits[] = 
	{0xff,0x0c,0x0c,0x0c,0xff,0xc0,0xc0};

	HBITMAP fromtest3 = CreateBitmap(8,8,1,1,(LPSTR)wBrickBits);

	HBRUSH myb = CreatePatternBrush(fromtest3);

	SelectObject(bltmet,myb);
	Rectangle(bltmet,100,100,200,200);


	HBITMAP fromtest2 = CreateCompatibleBitmap(dc->GetSafeHdc(),80,80);

	BITMAPINFO BitmapInfo;
	
	BitmapInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	BitmapInfo.bmiHeader.biWidth=8;
	BitmapInfo.bmiHeader.biHeight=8;
	BitmapInfo.bmiHeader.biPlanes=1;
	BitmapInfo.bmiHeader.biBitCount=1;
	BitmapInfo.bmiHeader.biCompression = BI_RGB;
	//BitmapInfo.bmiHeader.nSizeTable = 0;
	BitmapInfo.bmiHeader.biClrUsed = 0;

	void *lpBits = (void *)malloc(80*80*32);
	
	GetDIBits(bltmet, fromtest2, 0, 8, lpBits, &BitmapInfo, DIB_RGB_COLORS);
			
	if (0 == SetDIBitsToDevice(bltmet, 270, 450, 8, 
     8, 0, 0,0,8,wBrickBits,&BitmapInfo,DIB_RGB_COLORS))
		exit(-1);
	
		
	CloseMetaFile(bltmet);




	HRGN myreg = CreateRectRgn(645,400,645+80,400+80);
	SelectObject(met,myreg);
	PaintRgn(met,myreg);

#if 0
#endif 
	CloseMetaFile(met);

	//mtf = CloseEnhMetaFile(met);
	//mtf = GetEnhMetaFile("c:\\test.wmf");
	//WriteEnhMetaFileAsPlaceable(mtf,  "c:\\out.wmf");

	// TODO: add draw code for native data here
}

BOOL WriteEnhMetaFileAsPlaceable( HENHMETAFILE hMeta, LPTSTR szFileName )
{
	APMHEADER		APMHeader;
	ENHMETAHEADER	emh;
	HDC				hDC;
	DWORD			dwSize, dwBytes;
	LPBYTE			pBits;
	HANDLE			hFile;

	// Initialize the header
	ZeroMemory( &emh, sizeof(ENHMETAHEADER) );
	emh.nSize = sizeof(ENHMETAHEADER);
	// Fill in the enhanced metafile header
	if( GetEnhMetaFileHeader( hMeta, sizeof( ENHMETAHEADER ), &emh ) == 0 )
		return FALSE;

	// Fill in the Aldus Placeable Header
	APMHeader.dwKey = 0x9ac6cdd7l;
	APMHeader.hmf = 0;
	APMHeader.bbox.Top = 1000 * emh.rclFrame.top/2540;
	APMHeader.bbox.Left = 1000 * emh.rclFrame.left/2540;
	APMHeader.bbox.Right = 1000 * emh.rclFrame.right/2540;
	APMHeader.bbox.Bottom = 1000 * emh.rclFrame.bottom/2540;
	APMHeader.wInch = 1000;
	APMHeader.dwReserved = 0;
	APMHeader.wCheckSum = CalculateAPMCheckSum( APMHeader );

	// We need a reference DC
	hDC = GetDC( NULL );
	// How big are the windows metafile bits going to be?
	dwSize = GetWinMetaFileBits( hMeta, 0, NULL, MM_ANISOTROPIC, hDC  );
	// Allocate that much memory
	pBits = (unsigned char*) malloc( dwSize );
	// Let Windows convert our enhanced metafile to a regular windows metafile
	GetWinMetaFileBits( hMeta, dwSize, pBits, MM_ANISOTROPIC, hDC );
	// Release the reference DC
	ReleaseDC( NULL, hDC );

	// Open the file
	hFile = CreateFile( szFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	// Write the Aldus Placeable Header
	WriteFile( hFile, &APMHeader, sizeof(APMHEADER), &dwBytes, NULL );
	// Write the metafile bits
	WriteFile( hFile, pBits, dwSize, &dwBytes, NULL );

	// Clean up
	free( pBits );
	CloseHandle( hFile );

	return TRUE;
}

WORD CalculateAPMCheckSum( APMHEADER apmfh )
{
	LPWORD	lpWord;
	WORD	wResult, i;
	
	// Start with the first word
	wResult = *(lpWord = (LPWORD)(&apmfh));
	// XOR in each of the other 9 words
	for(i=1;i<=9;i++)
	{
		wResult ^= lpWord[i];
	}
	return wResult;
}
// End CalculateAPMCheckSum

/////////////////////////////////////////////////////////////////////////////
// CTestView diagnostics

#ifdef _DEBUG
void CTestView::AssertValid() const
{
	CView::AssertValid();
}

void CTestView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTestDoc* CTestView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTestDoc)));
	return (CTestDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTestView message handlers
