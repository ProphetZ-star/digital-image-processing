
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
