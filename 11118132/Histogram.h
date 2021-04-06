#pragma once

#include"Dib.h"
// Histogram 对话框

class Histogram : public CDialogEx
{
	DECLARE_DYNAMIC(Histogram)

public:
	Histogram(CDib* pDib,CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~Histogram();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = HISTOGRAM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	long* m_pGrayValue;
	long m_GrayCount;
	afx_msg void OnPaint();
};
