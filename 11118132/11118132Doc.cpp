
// 11118132Doc.cpp: CMy11118132Doc 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "11118132.h"
#endif

#include "11118132Doc.h"
#include "Dib.h"
#include"Histogram.h"
#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMy11118132Doc

IMPLEMENT_DYNCREATE(CMy11118132Doc, CDocument)

BEGIN_MESSAGE_MAP(CMy11118132Doc, CDocument)
	ON_COMMAND(ID_RECOVER, &CMy11118132Doc::OnRecover)
	ON_COMMAND(ID_32771, &CMy11118132Doc::OnInvert)
	ON_COMMAND(ID_32772, &CMy11118132Doc::OnRed)
	ON_COMMAND(ID_32773, &CMy11118132Doc::OnGreen)
	ON_COMMAND(ID_32774, &CMy11118132Doc::OnBlue)
	ON_COMMAND(ID_32780, &CMy11118132Doc::OnWindow)
	ON_COMMAND(ID_32775, &CMy11118132Doc::OnShowHistogram)
	ON_COMMAND(ID_32776, &CMy11118132Doc::OnHistogramEqualization)
	ON_COMMAND(ID_32777, &CMy11118132Doc::OnSmooth)
	ON_COMMAND(ID_Menu, &CMy11118132Doc::OnLaplace)
	ON_COMMAND(ID_32781, &CMy11118132Doc::OnSobel)
	ON_COMMAND(ID_32783, &CMy11118132Doc::OnWhite)
	ON_COMMAND(ID_32784, &CMy11118132Doc::OnFFT)
	//ON_COMMAND(ID_FILE_OPEN, &CMy11118132Doc::OnFileOpen)
	ON_COMMAND(ID_32786, &CMy11118132Doc::OnIdeal_LOW)
	ON_COMMAND(ID_32785, &CMy11118132Doc::OnIdeal_L_0)
	ON_COMMAND(ID_32787, &CMy11118132Doc::OnBW_L)
	ON_COMMAND(ID_32788, &CMy11118132Doc::OnBW_L_0)
	ON_COMMAND(ID_32789, &CMy11118132Doc::OnGauss_L_0)
	ON_COMMAND(ID_32790, &CMy11118132Doc::OnGauss_L)
	ON_COMMAND(ID_32791, &CMy11118132Doc::OnIdeal_H_0)
	ON_COMMAND(ID_32792, &CMy11118132Doc::OnIdeal_H)
	ON_COMMAND(ID_32793, &CMy11118132Doc::OnBW_H_0)
	ON_COMMAND(ID_32794, &CMy11118132Doc::OnBW_H)
	ON_COMMAND(ID_32795, &CMy11118132Doc::OnGauss_H_0)
	ON_COMMAND(ID_32796, &CMy11118132Doc::OnGauss_H)
	ON_COMMAND(ID_32797, &CMy11118132Doc::OnCenter_filter)
	ON_COMMAND(ID_32798, &CMy11118132Doc::OnDeterioration)
	ON_COMMAND(ID_32799, &CMy11118132Doc::OnA_filter)
	ON_COMMAND(ID_32800, &CMy11118132Doc::OnWeiner)
	ON_COMMAND(ID_32801, &CMy11118132Doc::OnFBP)
	ON_COMMAND(ID_32804, &CMy11118132Doc::OnLoadFile)
	ON_COMMAND(ID_32808, &CMy11118132Doc::OnBackground)
	ON_COMMAND(ID_32809, &CMy11118132Doc::OnR)
	ON_COMMAND(ID_32810, &CMy11118132Doc::OnG)
	ON_COMMAND(ID_32811, &CMy11118132Doc::OnB)
	ON_COMMAND(ID_32812, &CMy11118132Doc::OnH)
	ON_COMMAND(ID_32813, &CMy11118132Doc::OnS)
	ON_COMMAND(ID_32814, &CMy11118132Doc::OnI)
	ON_COMMAND(ID_32802, &CMy11118132Doc::OnLarge)
	ON_COMMAND(ID_32803, &CMy11118132Doc::OnSmall)
	ON_COMMAND(ID_32815, &CMy11118132Doc::Onbinary)
	ON_COMMAND(ID_32816, &CMy11118132Doc::Oncomplementary)
	ON_COMMAND(ID_32817, &CMy11118132Doc::OnErosion)
	ON_COMMAND(ID_32818, &CMy11118132Doc::Onexpand)
	ON_COMMAND(ID_32819, &CMy11118132Doc::Onfill)
	ON_COMMAND(ID_32822, &CMy11118132Doc::OnHaar)
	ON_COMMAND(ID_32823, &CMy11118132Doc::OnDaubechies)
	ON_COMMAND(ID_32820, &CMy11118132Doc::Onoutline)
	ON_COMMAND(ID_32821, &CMy11118132Doc::OnDrawline)
	ON_COMMAND(ID_32824, &CMy11118132Doc::Ontest)
END_MESSAGE_MAP()


// CMy11118132Doc 构造/析构

CMy11118132Doc::CMy11118132Doc() noexcept
{
	// TODO: 在此添加一次性构造代码
	m_pDib = NULL;
	m_pBuffer = NULL;
}

CMy11118132Doc::~CMy11118132Doc()
{
	if (m_pDib != NULL) {
		delete m_pDib;
		m_pDib = NULL;
	}
	if (m_pBuffer != NULL) {
		delete m_pBuffer;
		m_pBuffer = NULL;
	}
}

BOOL CMy11118132Doc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CMy11118132Doc 序列化

void CMy11118132Doc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}

#ifdef SHARED_HANDLERS

// 缩略图的支持
void CMy11118132Doc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改此代码以绘制文档数据
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 搜索处理程序的支持
void CMy11118132Doc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:     strSearchContent = _T("point;rectangle;circle;ole object;")；
	SetSearchContent(strSearchContent);
}

void CMy11118132Doc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CMy11118132Doc 诊断

#ifdef _DEBUG
void CMy11118132Doc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMy11118132Doc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CMy11118132Doc 命令


BOOL CMy11118132Doc::OnOpenDocument(LPCTSTR lpszPathName)
{

	if (!CDocument::OnOpenDocument(lpszPathName))
		

		return FALSE;

	// TODO:  在此添加您专用的创建代码
	if (m_pDib != NULL) {
		delete m_pDib;
		m_pDib = NULL;
	}
	if (m_pBuffer != NULL)
	{
		delete m_pBuffer;
		m_pBuffer = NULL;
	}
	m_pDib = new CDib;
	m_pDib->LoadFile(lpszPathName);
	m_pBuffer = new CDib(*m_pDib);
	UpdateAllViews(NULL);
	return TRUE;
}


void CMy11118132Doc::OnRecover()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL)
	{
		delete m_pDib;
		m_pDib = NULL;
	}
	if (m_pBuffer != NULL)
	{
		m_pDib = new CDib(*m_pBuffer);
	}
	UpdateAllViews(NULL);
}


void CMy11118132Doc::OnInvert()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		OnRecover();
		m_pDib->Invert();
		UpdateAllViews(NULL);
	}
}


void CMy11118132Doc::OnRed()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		OnRecover();
		m_pDib->Red();
		UpdateAllViews(NULL);
	}
}


void CMy11118132Doc::OnGreen()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL)
	{
		OnRecover();
		m_pDib->Green();
		UpdateAllViews(NULL);
	}
}


void CMy11118132Doc::OnBlue()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL)
	{
		OnRecover();
		m_pDib->Blue();
		UpdateAllViews(NULL);
	}
}


void CMy11118132Doc::OnWindow()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL)
	{
		m_pDib->Window(100, 200);
		UpdateAllViews(NULL);
	}
}


void CMy11118132Doc::OnShowHistogram()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		Histogram dlg(m_pDib);
		dlg.DoModal();
	}
}


void CMy11118132Doc::OnHistogramEqualization()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->HistogramEqualization();
		UpdateAllViews(NULL);
	}
}


void CMy11118132Doc::OnSmooth()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->smooth();
		UpdateAllViews(NULL);
	}
}



void CMy11118132Doc::OnLaplace()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->Laplace();
		UpdateAllViews(NULL);
	}
}


void CMy11118132Doc::OnSobel()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->Sobel();
		UpdateAllViews(NULL);
	}
}


void CMy11118132Doc::OnWhite()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL)
	{
		delete m_pDib;
		m_pDib = NULL;
	}
	if (m_pBuffer != NULL) {
		delete m_pBuffer;
		m_pBuffer = NULL;
	}
	//delete m_pDib;
	m_pDib = new CDib;
	m_pDib->White();
	m_pBuffer = new CDib(*m_pDib);
	UpdateAllViews(NULL);
}


void CMy11118132Doc::OnFFT()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL){
		m_pDib->FFT_VIEW();
		UpdateAllViews(NULL);
	}
}

void CMy11118132Doc::OnIdeal_LOW()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->Filtering(m_pDib->L_Ideal,false);
		UpdateAllViews(NULL);
	}
}


void CMy11118132Doc::OnIdeal_L_0()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->Filtering(m_pDib->L_Ideal, true);
		UpdateAllViews(NULL);
	}
}



void CMy11118132Doc::OnBW_L()
{
	// TODO: 在此添加命令处理程序代码
	//补零
	if (m_pDib != NULL) {
		m_pDib->Filtering(m_pDib->L_BW, true);
		UpdateAllViews(NULL);
	}
}


void CMy11118132Doc::OnBW_L_0()
{
	// TODO: 在此添加命令处理程序代码
	//不补零，函数命名写错了
	if (m_pDib != NULL) {
		m_pDib->Filtering(m_pDib->L_BW, false);
		UpdateAllViews(NULL);
	}
}



void CMy11118132Doc::OnGauss_L_0()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->Filtering(m_pDib->L_Gauss, true);
		UpdateAllViews(NULL);
	}
}


void CMy11118132Doc::OnGauss_L()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->Filtering(m_pDib->L_Gauss, false);
		UpdateAllViews(NULL);
	}
}


void CMy11118132Doc::OnIdeal_H_0()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->Filtering(m_pDib->H_Ideal, true);
		UpdateAllViews(NULL);
	}
}


void CMy11118132Doc::OnIdeal_H()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->Filtering(m_pDib->H_Ideal, false);
		UpdateAllViews(NULL);
	}
}


void CMy11118132Doc::OnBW_H_0()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->Filtering(m_pDib->H_BW, true);
		UpdateAllViews(NULL);
	}
}


void CMy11118132Doc::OnBW_H()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->Filtering(m_pDib->H_BW, false);
		UpdateAllViews(NULL);
	}
}


void CMy11118132Doc::OnGauss_H_0()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->Filtering(m_pDib->H_Gauss, true);
		UpdateAllViews(NULL);
	}
}


void CMy11118132Doc::OnGauss_H()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->Filtering(m_pDib->H_Gauss, false);
		UpdateAllViews(NULL);
	}
}


void CMy11118132Doc::OnCenter_filter()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->Center_filter();
		UpdateAllViews(NULL);
	}
}


void CMy11118132Doc::OnDeterioration()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->Filtering(m_pDib->Tur_D, false);
		UpdateAllViews(NULL);
	}
}


void CMy11118132Doc::OnA_filter()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->Filtering(m_pDib->Anti_F, false);
		UpdateAllViews(NULL);
	}

}


void CMy11118132Doc::OnWeiner()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->Filtering(m_pDib->Wiener, false);
		UpdateAllViews(NULL);
	}
}


void CMy11118132Doc::OnFBP()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		long* result = m_pDib->FBP();
		int r[256 * 256] = { 0 };
		for (int i = 0; i < 256 * 256; ++i) {
			r[i]=result[i];
			//result[i] = result[i] / 5;
		}
		delete m_pDib;
		m_pDib = NULL;
		m_pDib = new CDib;
		m_pDib->blank(result);
		//m_pBuffer = new CDib(*m_pDib);
		UpdateAllViews(NULL);
		delete result;
	}
}


void CMy11118132Doc::OnLoadFile()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL)
	{
		delete m_pDib;
		m_pDib = NULL;
	}
	if (m_pBuffer != NULL) {
		delete m_pBuffer;
		m_pBuffer = NULL;
	}
	m_pDib = new CDib;
	m_pDib->LoadFromTxtFile();
	m_pBuffer = new CDib(*m_pDib);
	UpdateAllViews(NULL);
}


void CMy11118132Doc::OnBackground()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL)
	{
		delete m_pDib;
	    m_pDib = NULL;
	}
	if (m_pBuffer != NULL) {
		delete m_pBuffer;
		m_pBuffer = NULL;
	}
	m_pDib = new CDib;
	m_pDib->background();
	m_pBuffer = new CDib(*m_pDib);
	UpdateAllViews(NULL);
}


void CMy11118132Doc::OnR()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->R();
		UpdateAllViews(NULL);
	}
}


void CMy11118132Doc::OnG()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->G();
		UpdateAllViews(NULL);
	}
}


void CMy11118132Doc::OnB()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->B();
		UpdateAllViews(NULL);
	}
}


void CMy11118132Doc::OnH()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->H();
		UpdateAllViews(NULL);
	}
}


void CMy11118132Doc::OnS()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->S();
		UpdateAllViews(NULL);
	}
}


void CMy11118132Doc::OnI()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->I();
		UpdateAllViews(NULL);
	}
}


void CMy11118132Doc::OnLarge()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		long* result=m_pDib->Large();
		delete m_pDib;
		m_pDib = new CDib;
		m_pDib->init(614,614,result);
		UpdateAllViews(NULL);
	}
}


void CMy11118132Doc::OnSmall()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		long* result = m_pDib->Small();
		delete m_pDib;
		m_pDib = new CDib;
		m_pDib->init(409, 409, result);
		UpdateAllViews(NULL);
	}
}


void CMy11118132Doc::Onbinary()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->binaryzation();
		UpdateAllViews(NULL);
	}
}


void CMy11118132Doc::Oncomplementary()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->complementary();
		UpdateAllViews(NULL);
	}
}


void CMy11118132Doc::OnErosion()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->Erosion();
		UpdateAllViews(NULL);
	}
}


void CMy11118132Doc::Onexpand()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->expand();
		UpdateAllViews(NULL);
	}
}


void CMy11118132Doc::Onfill()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->fill();
		UpdateAllViews(NULL);
	}
}


void CMy11118132Doc::OnHaar()
{
	// TODO: 在此添加命令处理程序代码
	//未完成
	if (m_pDib != NULL) {
		m_pDib->Haar();
		UpdateAllViews(NULL);
	}
}


void CMy11118132Doc::OnDaubechies()
{
	// TODO: 在此添加命令处理程序代码
	//未完成
}


void CMy11118132Doc::Onoutline()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		m_pDib->outline();
		UpdateAllViews(NULL);
	}
}


void CMy11118132Doc::OnDrawline()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		int *line = m_pDib->DrawLine();
		delete m_pDib;
		m_pDib = NULL;
		if (m_pBuffer != NULL)
		{
			m_pDib = new CDib(*m_pBuffer);
		}
		m_pDib->DrawLine2(line);
		UpdateAllViews(NULL);
		delete line;
	}
}


void CMy11118132Doc::Ontest()
{
	// TODO: 在此添加命令处理程序代码
	if (m_pDib != NULL) {
		double a[8] = { 1,1,1,1,1,1,0,0 };
		double *d = a;
		double c[8];
		UpdateAllViews(NULL);
	}
}
