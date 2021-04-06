
// 11118132View.h: CMy11118132View 类的接口
//

#pragma once


class CMy11118132View : public CView
{
protected: // 仅从序列化创建
	CMy11118132View() noexcept;
	DECLARE_DYNCREATE(CMy11118132View)

// 特性
public:
	CMy11118132Doc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CMy11118132View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // 11118132View.cpp 中的调试版本
inline CMy11118132Doc* CMy11118132View::GetDocument() const
   { return reinterpret_cast<CMy11118132Doc*>(m_pDocument); }
#endif

