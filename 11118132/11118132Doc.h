
// 11118132Doc.h: CMy11118132Doc 类的接口
//


#pragma once

class CDib;
class CMy11118132Doc : public CDocument
{
protected: // 仅从序列化创建
	CMy11118132Doc() noexcept;
	DECLARE_DYNCREATE(CMy11118132Doc)

// 特性
public:

// 操作
public:

// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 实现
public:
	virtual ~CMy11118132Doc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 用于为搜索处理程序设置搜索内容的 Helper 函数
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	CDib* m_pDib;
	CDib* m_pBuffer;
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	afx_msg void OnRecover();
	afx_msg void OnInvert();
	afx_msg void OnRed();
	afx_msg void OnGreen();
	afx_msg void OnBlue();
	afx_msg void OnWindow();
	afx_msg void OnShowHistogram();
	afx_msg void OnHistogramEqualization();
	afx_msg void OnSmooth();
	afx_msg void OnLaplace();
	afx_msg void OnSobel();
	afx_msg void OnWhite();
	afx_msg void OnFFT();
	afx_msg void OnIdeal_LOW();
	afx_msg void OnIdeal_L_0();
	afx_msg void OnBW_L();
	afx_msg void OnBW_L_0();
	afx_msg void OnGauss_L_0();
	afx_msg void OnGauss_L();
	afx_msg void OnIdeal_H_0();
	afx_msg void OnIdeal_H();
	afx_msg void OnBW_H_0();
	afx_msg void OnBW_H();
	afx_msg void OnGauss_H_0();
	afx_msg void OnGauss_H();
	afx_msg void OnCenter_filter();
	afx_msg void OnDeterioration();
	afx_msg void OnA_filter();
	afx_msg void OnWeiner();
	afx_msg void OnFBP();
	afx_msg void OnLoadFile();
	afx_msg void OnBackground();
	afx_msg void OnR();
	afx_msg void OnG();
	afx_msg void OnB();
	afx_msg void OnH();
	afx_msg void OnS();
	afx_msg void OnI();
	afx_msg void OnLarge();
	afx_msg void OnSmall();
	afx_msg void Onbinary();
	afx_msg void Oncomplementary();
	afx_msg void OnErosion();
	afx_msg void Onexpand();
	afx_msg void Onfill();
	afx_msg void OnHaar();
	afx_msg void OnDaubechies();
	afx_msg void Onoutline();
	afx_msg void OnDrawline();
	afx_msg void Ontest();
};
