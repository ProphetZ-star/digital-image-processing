
// 11118132View.cpp: CMy11118132View 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "11118132.h"
#endif

#include "11118132Doc.h"
#include "11118132View.h"
#include "Dib.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMy11118132View

IMPLEMENT_DYNCREATE(CMy11118132View, CView)

BEGIN_MESSAGE_MAP(CMy11118132View, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CMy11118132View 构造/析构

CMy11118132View::CMy11118132View() noexcept
{
	// TODO: 在此处添加构造代码

}

CMy11118132View::~CMy11118132View()
{
}

BOOL CMy11118132View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CMy11118132View 绘图

void CMy11118132View::OnDraw(CDC* pDC)
{
	CMy11118132Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	if (pDoc->m_pDib != NULL) {
		pDoc->m_pDib->Draw(pDC->m_hDC,0,0);
	}
	// TODO: 在此处为本机数据添加绘制代码
}


// CMy11118132View 打印

BOOL CMy11118132View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CMy11118132View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CMy11118132View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}


// CMy11118132View 诊断

#ifdef _DEBUG
void CMy11118132View::AssertValid() const
{
	CView::AssertValid();
}

void CMy11118132View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMy11118132Doc* CMy11118132View::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMy11118132Doc)));
	return (CMy11118132Doc*)m_pDocument;
}
#endif //_DEBUG


// CMy11118132View 消息处理程序
