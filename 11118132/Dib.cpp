//#include "StdAfx.h"
#include "pch.h"
#include "Dib.h"


CDib::CDib(void):m_pDibBits(NULL),m_pGrayValueCount(NULL)
{

}


CDib::CDib( CDib &Dib ):m_pDibBits(NULL),m_pGrayValueCount(NULL)
{
	if(&Dib == NULL)
	{
		return;
	}
	if(!IsNull())
	{
		Destroy();
	}
	//int n=Dib.GetBPP();
	Create(Dib.GetWidth(),Dib.GetHeight(),Dib.GetBPP(),0);
	m_nWidth = Dib.m_nWidth;
	m_nHeight = Dib.m_nHeight;
	//int nColors = Dib.GetMaxColorTableEntries();

	if(IsIndexed())
	{
		int nColors=Dib.GetMaxColorTableEntries();
		if(nColors > 0)
		{
			RGBQUAD *pal = new RGBQUAD[nColors];
			Dib.GetColorTable(0,nColors,pal);
			SetColorTable(0,nColors,pal);
			delete[] pal;
		} 
	}
	m_nWidthBytes =abs(GetPitch()) ;
	m_nBitCount = GetBPP();
	m_pDibBits = (unsigned char*)GetBits()+(m_nHeight-1)*GetPitch();
	memcpy(m_pDibBits,Dib.m_pDibBits,m_nHeight*m_nWidthBytes);

}

CDib::~CDib(void)
{
	m_pDibBits = NULL;
	if (m_pGrayValueCount != NULL)
	{
		delete []m_pGrayValueCount;
		m_pGrayValueCount = NULL;
	}
}

void CDib::LoadFile( LPCTSTR lpszPathName )
{
	Load(lpszPathName);
	m_nWidth = GetWidth();
	m_nHeight = GetHeight();
	m_nWidthBytes =abs(GetPitch()) ;
	m_nBitCount = GetBPP();
	m_pDibBits = (unsigned char*)GetBits()+(m_nHeight-1)*GetPitch();

}

void CDib::Invert()
{
	for (int i = 0; i < m_nHeight; i++)
	{
		for (int j = 0; j < m_nWidthBytes; j++)
		{
			*(m_pDibBits + i*m_nWidthBytes + j) = 255 - *(m_pDibBits + i*m_nWidthBytes + j);
		}
	}
}

long* CDib::GrayValueCount()
{
	long nColors = GetMaxColorTableEntries();
	if (nColors == 0)
	{
		return NULL;
	}
	long *pGrayValueCount = new long[nColors];
	memset(pGrayValueCount,0,nColors*sizeof(long));
	for (int i=0;i<m_nHeight;i++)
	{
		for (int j=0;j<m_nWidth;j++)
		{
			pGrayValueCount[*(m_pDibBits + i*m_nWidthBytes +j)]++;
		}
	}
	return pGrayValueCount;
}


BOOL CDib::FFT(unsigned char* pDIBBits, long nWidth, long nHeight)
{ 
	unsigned char*	lpSrc;							// 指向源图像的指针
	int y ;										// 循环控制变量
	int x ;										// 循环控制变量
	double dTmpOne ;								//存放临时变量
	double dTmpTwo ;								//存放临时变量
	int nTransWidth ;								// 傅立叶变换的宽度（2的整数次幂）
	int nTransHeight;								// 傅立叶变换的高度（2的整数次幂）
	double unchValue;								// 存贮图像各像素灰度的临时变量
	complex<double> * pCTData ;						// 指向时域数据的指针
	complex<double> * pCFData ;						// 指向频域数据的指针
	// 计算进行傅立叶变换的点数－横向	（2的整数次幂）
	dTmpOne = log(1.0*nWidth)/log(2.0);
	dTmpTwo = ceil(dTmpOne)	;
	dTmpTwo = pow(2,dTmpTwo);
	nTransWidth = (int) dTmpTwo;	
	// 计算进行傅立叶变换的点数－纵向 （2的整数次幂）
	dTmpOne = log(1.0*nHeight)/log(2.0);
	dTmpTwo = ceil(dTmpOne)	;
	dTmpTwo = pow(2,dTmpTwo);
	nTransHeight = (int) dTmpTwo;
	double dReal;									// 傅立叶变换的实部
	double dImag;									// 傅立叶变换的虚部
	
	pCTData=new complex<double>[nTransWidth * nTransHeight];	// 分配内存
	pCFData=new complex<double>[nTransWidth * nTransHeight];	// 分配内存
	// 图像数据的宽和高不一定是2的整数次幂，所以pCTData有一部分数据需要补0
	for(y=0; y<nTransHeight; y++)
	{
		for(x=0; x<nTransWidth; x++)
		{
			pCTData[y*nTransWidth + x]=complex<double>(0,0);		// 补零
		}
	}
	//把图像数据传给pCTData
	for(y=0; y<nHeight; y++)
	{
		for(x=0; x<nWidth; x++)
		{
			// 指向DIB第y行，第x个象素的指针
			lpSrc = (unsigned char*)pDIBBits + nWidth * (nHeight - 1 - y) + x;
			unchValue = (*lpSrc)*pow(-1.0,x+y);
			pCTData[y*nTransWidth + x]=complex<double>(unchValue,0);
		}
	}
	FFT_2D(pCTData, nWidth, nHeight, pCFData) ;				// 傅立叶正变换

	///////////////////////////////////////////////////////////////////////////
	// code needed to insert here
	//
	//
	//
	//
	///////////////////////////////////////////////////////////////////////////
	


	IFFT_2D(pCFData, pCTData,nHeight, nWidth); 				// 傅立叶反变换

	for(y=0; y<nHeight; y++)								// 反变换的数据传给lpDIBBits
	{
		for(x=0; x<nWidth; x++)
		{
			//需要考虑信号的正负问题以及实际所用的图象数据是灰度值还是原始数据
			dReal = pCTData[y*nTransWidth + x].real() ;		// 实部
			dImag = pCTData[y*nTransWidth + x].imag() ;		// 虚部
			unchValue = dReal*pow(-1.0,x+y);
			if (unchValue < 0)
			{
				unchValue = 0;
			}
			if (unchValue > 0xff)
			{
				unchValue = 0xff;
			}
			// 指向DIB第y行，第x个象素的指针
			lpSrc = (unsigned char*)pDIBBits + nWidth * (nHeight - 1 - y) + x;
			*lpSrc =(BYTE)unchValue ;
		}
	}	
	
	delete pCTData;										// 释放内存
	delete pCFData;										// 释放内存
	pCTData = NULL;
	pCFData = NULL;	
	return (true);										//返回结果
}

/*************************************************************************
 *
 * \函数名称：
 *   FFT_1D()
 *
 * \输入参数:
 *   complex<double> * pCTData	- 指向时域数据的指针，输入的需要变换的数据
 *   complex<double> * pCFData	- 指向频域数据的指针，输出的经过变换的数据
 *   nLevel						－傅立叶变换蝶形算法的级数，2的幂数，
 *
 * \返回值:
 *   无
 *
 * \说明:
 *   一维快速傅立叶变换。
 *
 *************************************************************************
 */
void CDib::FFT_1D(complex<double> * pCTData, complex<double> * pCFData, int nLevel)
{
		// 循环控制变量
	int		i;
	int     j;
	int     k;

	// 傅立叶变换点数
	int	nCount =0 ;

	// 计算傅立叶变换点数
	nCount =(int)pow(2.0,nLevel) ;
	
	// 某一级的长度
	int		nBtFlyLen;
	nBtFlyLen = 0 ;
	
	// 变换系数的角度 ＝2 * PI * i / nCount
	double	dAngle;
	
	complex<double> *pCW ;
	
	// 分配内存，存储傅立叶变化需要的系数表
	pCW  = new complex<double>[nCount / 2];

    // 计算傅立叶变换的系数
	for(i = 0; i < nCount / 2; i++)
	{
		dAngle = -2 * Pi * i / nCount;
		pCW[i] = complex<double> ( cos(dAngle), sin(dAngle) );
	}

	// 变换需要的工作空间
	complex<double> *pCWork1,*pCWork2; 
	
	// 分配工作空间
	pCWork1 = new complex<double>[nCount];

	pCWork2 = new complex<double>[nCount];

	
	// 临时变量
	complex<double> *pCTmp;
	
	// 初始化，写入数据
	memcpy(pCWork1, pCTData, sizeof(complex<double>) * nCount);

	// 临时变量
	int nInter; 
	nInter = 0;

	// 蝶形算法进行快速傅立叶变换
	for(k = 0; k < nLevel; k++)
	{
		for(j = 0; j < (int)pow(2.0,k); j++)
		{
			//计算长度
			nBtFlyLen = (int)pow( 2.0,(nLevel-k) );
			
			//倒序重排，加权计算
			for(i = 0; i < nBtFlyLen/2; i++)
			{
				nInter = j * nBtFlyLen;
				pCWork2[i + nInter] = 
					pCWork1[i + nInter] + pCWork1[i + nInter + nBtFlyLen / 2];
				pCWork2[i + nInter + nBtFlyLen / 2] =
					(pCWork1[i + nInter] - pCWork1[i + nInter + nBtFlyLen / 2]) 
					* pCW[(int)(i * pow(2.0,k))];
			}
		}

		// 交换 pCWork1和pCWork2的数据
		pCTmp   = pCWork1	;
		pCWork1 = pCWork2	;
		pCWork2 = pCTmp		;
	}
	
	// 重新排序
	for(j = 0; j < nCount; j++)
	{
		nInter = 0;
		for(i = 0; i < nLevel; i++)
		{
			if ( j&(1<<i) )
			{
				nInter += 1<<(nLevel-i-1);
			}
		}
		pCFData[j]=pCWork1[nInter];
	}
	
	// 释放内存空间
	delete pCW;
	delete pCWork1;
	delete pCWork2;
	pCW		=	NULL	;
	pCWork1 =	NULL	;
	pCWork2 =	NULL	;
}
/*************************************************************************
 *
 * \函数名称：
 *    IFFT_1D()
 *
 * \输入参数:
 *   complex<double> * pCTData	- 指向时域数据的指针，输入的需要反变换的数据
 *   complex<double> * pCFData	- 指向频域数据的指针，输出的经过反变换的数据
 *   nLevel						－傅立叶变换蝶形算法的级数，2的幂数，
 *
 * \返回值:
 *   无
 *
 * \说明:
 *   一维快速傅立叶反变换。
 *
 ************************************************************************
 */
void CDib::IFFT_1D(complex<double> * pCFData, complex<double> * pCTData, int nLevel)
{
	
	// 循环控制变量
	int		i;

	// 傅立叶反变换点数
	int nCount;

	// 计算傅立叶变换点数
	nCount = (int)pow(2.0,nLevel) ;
	
	// 变换需要的工作空间
	complex<double> *pCWork;	
	
	// 分配工作空间
	pCWork = new complex<double>[nCount];
	
	// 将需要反变换的数据写入工作空间pCWork
	memcpy(pCWork, pCFData, sizeof(complex<double>) * nCount);
	
	// 为了利用傅立叶正变换,可以把傅立叶频域的数据取共轭
	// 然后直接利用正变换，输出结果就是傅立叶反变换结果的共轭
	for(i = 0; i < nCount; i++)
	{
		pCWork[i] = complex<double> (pCWork[i].real(), -pCWork[i].imag());
	}
	
	// 调用快速傅立叶变换实现反变换，结果存储在pCTData中
	FFT_1D(pCWork, pCTData, nLevel);
	
	// 求时域点的共轭，求得最终结果
	// 根据傅立叶变换原理，利用这样的方法求得的结果和实际的时域数据
	// 相差一个系数nCount
	for(i = 0; i < nCount; i++)
	{
		pCTData[i] = 
			complex<double> (pCTData[i].real() / nCount, -pCTData[i].imag() / nCount);
	}
	
	// 释放内存
	delete pCWork;
	pCWork = NULL;
}

/*************************************************************************
 *
 * \函数名称：
 *   FFT_2D()
 *
 * \输入参数:
 *   complex<double> * pCTData	- 图像数据
 *   int    nWidth				- 数据宽度
 *   int    nHeight				- 数据高度
 *   complex<double> * pCFData	- 傅立叶变换后的结果
 *
 * \返回值:
 *   无
 *
 * \说明:
 *   二维傅立叶变换。
 *
 ************************************************************************
 */
void CDib::FFT_2D(complex<double> * pCTData, int nWidth, int nHeight, complex<double> * pCFData)
{
	
	// 循环控制变量
	int	x;
	int	y;
	
	// 临时变量
	double	dTmpOne;
	double  dTmpTwo;
	
	// 进行傅立叶变换的宽度和高度，（2的整数次幂）
	// 图像的宽度和高度不一定为2的整数次幂
	int		nTransWidth;
	int 	nTransHeight;

	// 计算进行傅立叶变换的宽度	（2的整数次幂）
	dTmpOne = log(1.0*nWidth)/log(2.0);
	dTmpTwo = ceil(dTmpOne)		   ;
	dTmpTwo = pow(2,dTmpTwo)	   ;
	nTransWidth = (int) dTmpTwo	   ;
	
	// 计算进行傅立叶变换的高度 （2的整数次幂）
	dTmpOne = log(1.0*nHeight)/log(2.0);
	dTmpTwo = ceil(dTmpOne)		   ;
	dTmpTwo = pow(2,dTmpTwo)	   ;
	nTransHeight = (int) dTmpTwo	   ;	
	
	// x，y（行列）方向上的迭代次数
	int		nXLev;
	int		nYLev;

	// 计算x，y（行列）方向上的迭代次数
	nXLev = (int) ( log(1.0*nTransWidth)/log(2.0) +  0.5 );
	nYLev = (int) ( log(1.0*nTransHeight)/log(2.0) + 0.5 );
	
	for(y = 0; y < nTransHeight; y++)
	{
		// x方向进行快速傅立叶变换
		FFT_1D(&pCTData[nTransWidth * y], &pCFData[nTransWidth * y], nXLev);
	}
	
	// pCFData中目前存储了pCTData经过行变换的结果
	// 为了直接利用FFT_1D，需要把pCFData的二维数据转置，再一次利用FFT_1D进行
	// 傅立叶行变换（实际上相当于对列进行傅立叶变换）
	for(y = 0; y < nTransHeight; y++)
	{
		for(x = 0; x < nTransWidth; x++)
		{
			pCTData[nTransHeight * x + y] = pCFData[nTransWidth * y + x];
		}
	}
	
	for(x = 0; x < nTransWidth; x++)
	{
		// 对x方向进行快速傅立叶变换，实际上相当于对原来的图象数据进行列方向的
		// 傅立叶变换
		FFT_1D(&pCTData[x * nTransHeight], &pCFData[x * nTransHeight], nYLev);
	}

	// pCFData中目前存储了pCTData经过二维傅立叶变换的结果，但是为了方便列方向
	// 的傅立叶变换，对其进行了转置，现在把结果转置回来
	for(y = 0; y < nTransHeight; y++)
	{
		for(x = 0; x < nTransWidth; x++)
		{
			pCTData[nTransWidth * y + x] = pCFData[nTransHeight * x + y];
		}
	}
}

/*************************************************************************
 *
 * \函数名称：
 *   IFFT_2D()
 *
 * \输入参数:
 *   complex<double> * pCFData	- 频域数据
 *   complex<double> * pCTData	- 时域数据
 *   int    nWidth				- 图像数据宽度
 *   int    nHeight				- 图像数据高度
 *
 * \返回值:
 *   无
 *
 * \说明:
 *   二维傅立叶反变换。
 *
 ************************************************************************
 */
void CDib::IFFT_2D(complex<double> * pCFData, complex<double> * pCTData, int nWidth, int nHeight)
{
	// 循环控制变量
	int	x;
	int	y;
	
	// 临时变量
	double	dTmpOne;
	double  dTmpTwo;
	
	// 进行傅立叶变换的宽度和高度，（2的整数次幂）
	// 图像的宽度和高度不一定为2的整数次幂
	int		nTransWidth;
	int 	nTransHeight;

	// 计算进行傅立叶变换的宽度	（2的整数次幂）
	dTmpOne = log(1.0*nWidth)/log(2.0);
	dTmpTwo = ceil(dTmpOne)		   ;
	dTmpTwo = pow(2,dTmpTwo)	   ;
	nTransWidth = (int) dTmpTwo	   ;
	
	// 计算进行傅立叶变换的高度 （2的整数次幂）
	dTmpOne = log(1.0*nHeight)/log(2.0);
	dTmpTwo = ceil(dTmpOne)		   ;
	dTmpTwo = pow(2,dTmpTwo)	   ;
	nTransHeight = (int) dTmpTwo	   ;
	
	// 分配工作需要的内存空间
	complex<double> *pCWork= new complex<double>[nTransWidth * nTransHeight];

	//临时变量
	complex<double> *pCTmp ;
	
	// 为了利用傅立叶正变换,可以把傅立叶频域的数据取共轭
	// 然后直接利用正变换，输出结果就是傅立叶反变换结果的共轭
	for(y = 0; y < nTransHeight; y++)
	{
		for(x = 0; x < nTransWidth; x++)
		{
			pCTmp = &pCFData[nTransWidth * y + x] ;
			pCWork[nTransWidth * y + x] = complex<double>( pCTmp->real() , -pCTmp->imag() );
		}
	}

	// 调用傅立叶正变换
	FFT_2D(pCWork, nWidth, nHeight, pCTData) ;
	
	// 求时域点的共轭，求得最终结果
	// 根据傅立叶变换原理，利用这样的方法求得的结果和实际的时域数据
	// 相差一个系数
	for(y = 0; y < nTransHeight; y++)
	{
		for(x = 0; x < nTransWidth; x++)
		{
			pCTmp = &pCTData[nTransWidth * y + x] ;
			pCTData[nTransWidth * y + x] = 
				complex<double>( pCTmp->real()/(nTransWidth*nTransHeight),
								 -pCTmp->imag()/(nTransWidth*nTransHeight) );
		}
	}
	delete pCWork ;
	pCWork = NULL ;
}


 void CDib::Red()
 {
	 // TODO: 在此处添加实现代码.
	 if (IsIndexed())
	 {
		 int nColors = GetMaxColorTableEntries();
		 if (nColors > 0)
		 {
			 RGBQUAD *pal = new RGBQUAD[nColors];
			 GetColorTable(0, nColors, pal);
			 for (int i = 0; i < nColors; ++i) {
				 pal[i].rgbBlue = 0;
				 pal[i].rgbGreen = 0;
			 }
			 SetColorTable(0, nColors, pal);
			 delete[] pal;
		 }
	 }
 }


 void CDib::Green()
 {
	 // TODO: 在此处添加实现代码.
	 if (IsIndexed())
	 {
		 int nColors = GetMaxColorTableEntries();
		 if (nColors > 0)
		 {
			 RGBQUAD *pal = new RGBQUAD[nColors];
			 GetColorTable(0, nColors, pal);
			 for (int i = 0; i < nColors; ++i) {
				 pal[i].rgbBlue = 0;
				 pal[i].rgbRed = 0;
			 }
			 SetColorTable(0, nColors, pal);
			 delete[] pal;
		 }
	 }
 }


 void CDib::Blue()
 {
	 // TODO: 在此处添加实现代码.
	 if (IsIndexed())
	 {
		 int nColors = GetMaxColorTableEntries();
		 if (nColors > 0)
		 {
			 RGBQUAD *pal = new RGBQUAD[nColors];
			 GetColorTable(0, nColors, pal);
			 for (int i = 0; i < nColors; ++i) {
				 pal[i].rgbRed = 0;
				 pal[i].rgbGreen = 0;
			 }
			 SetColorTable(0, nColors, pal);
			 delete[] pal;
		 }
	 }
 }


 void CDib::Window(int min, int max)
 {
	 // TODO: 在此处添加实现代码.
	 if (min < max - 5) {
		 for (int i = 0; i < m_nHeight; i++)
		 {
			 for (int j = 0; j < m_nWidthBytes; j++)
			 {
				 if (*(m_pDibBits + i * m_nWidthBytes + j) > max) {
					 *(m_pDibBits + i * m_nWidthBytes + j) = 255;
				 }
				 if (*(m_pDibBits + i * m_nWidthBytes + j) < min) {
					 *(m_pDibBits + i * m_nWidthBytes + j) = 0;
				 }
				 else {
					 *(m_pDibBits + i * m_nWidthBytes + j) = 255 / (max - min)*(*(m_pDibBits + i * m_nWidthBytes + j) - min);
				 }
			 }
		 }
	 }
 }


 void CDib::HistogramEqualization()
 {
	 // TODO: 在此处添加实现代码.
	 long* GrayCount = GrayValueCount();
	 int nColor = GetMaxColorTableEntries();
	 long N = 0;//直方图像素总数
	 long* Sk = new long[nColor];
	 for (int i = 0; i < nColor; ++i) {
		 Sk[i] = 0;//初始化Sk
		 N += GrayCount[i];
	 }
	 for (int i = 0; i < nColor; ++i) {
		 for (int j = 0; j <= i; ++j) {
			 Sk[i] = Sk[i] + GrayCount[j];
		 }
	 }
	 for (int i = 0; i < m_nHeight; i++)
	 {
		 for (int j = 0; j < m_nWidthBytes; j++)
		 {
			 if (long(nColor*(Sk[(*(m_pDibBits + i * m_nWidthBytes + j))]) / N) <= 0)
				 *(m_pDibBits + i * m_nWidthBytes + j) = 0;
			 else
				 *(m_pDibBits + i * m_nWidthBytes + j) = (nColor*(Sk[(*(m_pDibBits + i * m_nWidthBytes + j))]) / N) -1;
			 //*(m_pDibBits + i * m_nWidthBytes + j) = 255;
		 }
	 }
	 delete[] GrayCount;
	 delete[] Sk;
 }



 void CDib::smooth()
 {
	 // TODO: 在此处添加实现代码.

	 for (int i = 0; i < m_nHeight; i++){
		 *(m_pDibBits + i * m_nWidthBytes + 0) = (*(m_pDibBits + i * m_nWidthBytes + 0) + *(m_pDibBits + i * m_nWidthBytes + 0) + *(m_pDibBits + i * m_nWidthBytes + 1)) / 3;
		 for (int j = 1; j < (m_nWidthBytes-1); j++) {
			 *(m_pDibBits + i * m_nWidthBytes + j) = (*(m_pDibBits + i * m_nWidthBytes + j - 1) + *(m_pDibBits + i * m_nWidthBytes + j + 1) + *(m_pDibBits + i * m_nWidthBytes + j)) / 3;
		 }
		 *(m_pDibBits + i * m_nWidthBytes + (m_nWidthBytes - 1)) = (*(m_pDibBits + i * m_nWidthBytes + (m_nWidthBytes - 1)) + *(m_pDibBits + i * m_nWidthBytes + (m_nWidthBytes - 1)) + *(m_pDibBits + i * m_nWidthBytes + (m_nWidthBytes - 2))) / 3;
	 }
	 for (int j = 0; j < m_nWidthBytes; j++) {
		 *(m_pDibBits + 0 * m_nWidthBytes + j) = (*(m_pDibBits + 0 * m_nWidthBytes + j) + *(m_pDibBits + 0 * m_nWidthBytes + j) + *(m_pDibBits + 1 * m_nWidthBytes + j)) / 3;
		 for (int i = 1; i < (m_nHeight - 1); i++) {
			 *(m_pDibBits + i * m_nWidthBytes + j) = (*(m_pDibBits + (i + 1) * m_nWidthBytes + j) + *(m_pDibBits + i * m_nWidthBytes + j) + *(m_pDibBits + (i - 1) * m_nWidthBytes + j)) / 3;
		 }
		 *(m_pDibBits + (m_nHeight - 1) * m_nWidthBytes + j) = (*(m_pDibBits + (m_nHeight - 1) * m_nWidthBytes + j) + *(m_pDibBits + (m_nHeight - 1) * m_nWidthBytes + j) + *(m_pDibBits + (m_nHeight - 2) * m_nWidthBytes + j)) / 3;
	 }	
 }



 void CDib::Sobel()
 {
	 // TODO: 在此处添加实现代码.
	 int *pixel = new int[(m_nHeight+2)*(m_nWidthBytes+2)];
	 for (int i = 0; i <= (m_nHeight+1); i++)
	 {
		 for (int j = 0; j <= (m_nWidthBytes+1); j++)
		 {
			 if (i == 0) {
				 if (j == 0) {
					 pixel[0] = *(m_pDibBits);
				 }
				 else {
					 if (j == (m_nWidthBytes + 1)) {
						 pixel[(m_nWidthBytes + 1)] = *(m_pDibBits + j - 2);
					 }
					 else
					 {
						 pixel[j] = *(m_pDibBits + j - 1);
					 }
				 }
			 }
			 if (i == m_nHeight + 1) {
				 if (j == 0)
					 pixel[i*(m_nWidthBytes + 2) + j] = *(m_pDibBits + (i - 2) * m_nWidthBytes + j);
				 else {
					 if (j == m_nWidthBytes + 1)
						 pixel[i*(m_nWidthBytes + 2) + j] = *(m_pDibBits + (i - 2) * m_nWidthBytes + j - 2);
					 else {
						 pixel[i*(m_nWidthBytes + 2) + j] = *(m_pDibBits + (i - 2)*m_nWidthBytes + j - 1);
					 }
				 }
			 }
			 else {
				 if (j == 0)
					 pixel[i*(m_nWidthBytes + 2) + j] = *(m_pDibBits + (i - 1)*m_nWidthBytes);
				 else {
					 if (j == m_nWidthBytes + 1)
						 pixel[i*(m_nWidthBytes + 2) + j] = *(m_pDibBits + (i - 1)*m_nWidthBytes + j - 2);
					 else
						 pixel[i*(m_nWidthBytes + 2) + j] = *(m_pDibBits + (i - 1)*m_nWidthBytes + j - 1);
				 }
			 }
		 }
	 }
	 for (int i = 0; i < m_nHeight; ++i) {
		 for (int j = 0; j < m_nWidthBytes; ++j) {
			 *(m_pDibBits + i * m_nWidthBytes + j) = abs(pixel[(i + 2)*(m_nWidthBytes + 2) + j ] + 2 * pixel[(i + 2)*(m_nWidthBytes + 2) + j + 1] + pixel[(i + 2)*(m_nWidthBytes + 2) + j + 2] - pixel[(i + 0)*(m_nWidthBytes + 2) + j + 0] - 2 * pixel[(i + 0)*(m_nWidthBytes + 2) + j + 1] - pixel[(i + 0)*(m_nWidthBytes + 2) + j + 2]) +
				 abs(pixel[(i + 0)*(m_nWidthBytes + 2) + j + 2] + 2 * pixel[(i + 1)*(m_nWidthBytes + 2) + j + 2] + pixel[(i + 2)*(m_nWidthBytes + 2) + j + 2] - pixel[(i + 0)*(m_nWidthBytes + 2) + j + 0] + 2 * pixel[(i + 1)*(m_nWidthBytes + 2) + j + 0] + pixel[(i + 2)*(m_nWidthBytes + 2) + j + 0]);
		 }
	 }
	 delete[] pixel;
 }


 void CDib::Laplace()
 {
	 // TODO: 在此处添加实现代码.
	 int *pixel = new int[m_nHeight*m_nWidthBytes];
	 pixel[0] = (*(m_pDibBits + 0 * m_nWidthBytes + 0) * 2 + *(m_pDibBits + 0 * m_nWidthBytes + 1) + *(m_pDibBits + 1 * m_nWidthBytes + 0) - 4 * (*(m_pDibBits + 0 * m_nWidthBytes + 0)));
	 for (int j = 1; j < (m_nWidthBytes - 1); ++j) {
		 pixel[j] = (*(m_pDibBits + 0 * m_nWidthBytes + j+1) + *(m_pDibBits + 0 * m_nWidthBytes +j- 1) + *(m_pDibBits + 0 * m_nWidthBytes + j) + *(m_pDibBits + (j+1) * m_nWidthBytes + j) - 4 * (*(m_pDibBits + 0 * m_nWidthBytes + j)));
	 }
	 pixel[m_nWidthBytes-1] = (*(m_pDibBits + m_nWidthBytes - 1) * 2 + *(m_pDibBits + m_nWidthBytes - 2) + *(m_pDibBits + (1) * m_nWidthBytes + m_nWidthBytes - 1) - 4 * (*(m_pDibBits + m_nWidthBytes - 1)));
	 for (int i = 1; i < (m_nHeight-1); i++) {
		 pixel[i*m_nWidthBytes] = (*(m_pDibBits + m_nWidthBytes * i) + *(m_pDibBits + i * m_nWidthBytes + 1) + *(m_pDibBits + (i - 1) * m_nWidthBytes) + *(m_pDibBits + (i + 1) * m_nWidthBytes) - 4 * (*(m_pDibBits + m_nWidthBytes * i)));
		 for (int j = 1; j < (m_nWidthBytes - 1); j++) {
			 pixel[i * m_nWidthBytes + j] = (*(m_pDibBits + i * m_nWidthBytes + j - 1) + *(m_pDibBits + i * m_nWidthBytes + j + 1) + *(m_pDibBits + i * m_nWidthBytes + j)) / 3;
		 }
		 pixel[(i+1)*m_nWidthBytes-1] = (*(m_pDibBits + (i + 1)*m_nWidthBytes - 1) + *(m_pDibBits + (i + 1)*m_nWidthBytes - 2) + *(m_pDibBits + (i + 2)*m_nWidthBytes - 1) + *(m_pDibBits + (i + 0)*m_nWidthBytes - 1) - 4 * (*(m_pDibBits + (i + 1)*m_nWidthBytes - 1)));
	 }
	 pixel[(m_nHeight-1)*m_nWidthBytes] = (*(m_pDibBits + (m_nHeight - 1)*m_nWidthBytes) * 2 + *(m_pDibBits + (m_nHeight - 1)*m_nWidthBytes+1) + *(m_pDibBits + (m_nHeight - 2)*m_nWidthBytes) - 4 * (*(m_pDibBits + (m_nHeight - 1)*m_nWidthBytes)));
	 for (int j = 1; j < (m_nWidthBytes - 1); ++j) {
		 pixel[(m_nHeight - 1)*m_nWidthBytes+j] = (*(m_pDibBits + (m_nHeight - 1)*m_nWidthBytes + j + 1) + *(m_pDibBits + (m_nHeight - 1)*m_nWidthBytes + j - 1) + *(m_pDibBits + (m_nHeight - 1)*m_nWidthBytes + j) + *(m_pDibBits + (m_nHeight - 2)*m_nWidthBytes + j) - 4 * (*(m_pDibBits + (m_nHeight - 1)*m_nWidthBytes + j)));
	 }
	 pixel[m_nHeight*m_nWidthBytes - 1] = (*(m_pDibBits + m_nHeight * m_nWidthBytes - 1) * 2 + *(m_pDibBits + m_nHeight * m_nWidthBytes - 2) + *(m_pDibBits + (m_nHeight-1) * m_nWidthBytes - 1) - 4 * (*(m_pDibBits + m_nHeight * m_nWidthBytes - 1)));
	 for (int i = 0; i < m_nHeight; i++)
	 {
		 for (int j = 0; j < m_nWidthBytes; j++)
		 {
			 *(m_pDibBits + i * m_nWidthBytes + j) = *(m_pDibBits + i * m_nWidthBytes + j) - pixel[i * m_nWidthBytes + j];
		 }
	 }
	 delete[] pixel;
 }


 void CDib::White()
 {
	 // TODO: 在此处添加实现代码.
	 if (!IsNull())
	 {
		 Destroy();
	 }
	 Create(512, 512, 8, 0);
	 m_nWidth = 512;
	 m_nHeight = 512;
	 if (IsIndexed())
	 {
		 int nColors = 256;
		 RGBQUAD *pal = new RGBQUAD[nColors];
		 for (int i = 0; i < nColors; ++i) {
			 pal[i].rgbBlue = i;
			 pal[i].rgbRed = i;
			 pal[i].rgbGreen = i;
		 }
		 //GetColorTable(0, nColors, pal);
		 SetColorTable(0, nColors, pal);
		 delete[] pal;
	 }
	 m_nWidthBytes = 512;
	 m_nBitCount = 8;
	 m_pDibBits = (unsigned char*)GetBits() + (m_nHeight - 1)*GetPitch();
	 //memcpy(m_pDibBits, Dib.m_pDibBits, m_nHeight*m_nWidthBytes);
	 int W = 50, H = 50;
	 for (int i = 0; i < W; ++i) {
		 for (int j = 0; j < H; ++j) {
			 *(m_pDibBits + (i+ m_nHeight/2 -H/2) * m_nWidthBytes + (j+m_nWidthBytes/2-W/2)) = 255;
		 }
	 }
 }


 void CDib::FFT_VIEW()
 {
	 // TODO: 在此处添加实现代码.
	 unsigned char*	lpSrc;							// 指向源图像的指针
	 int y;										// 循环控制变量
	 int x;										// 循环控制变量
	 double dTmpOne;								//存放临时变量
	 double dTmpTwo;								//存放临时变量
	 int nTransWidth;								// 傅立叶变换的宽度（2的整数次幂）
	 int nTransHeight;								// 傅立叶变换的高度（2的整数次幂）
	 double unchValue;								// 存贮图像各像素灰度的临时变量
	 complex<double> * pCTData;						// 指向时域数据的指针
	 complex<double> * pCFData;						// 指向频域数据的指针
	 // 计算进行傅立叶变换的点数－横向	（2的整数次幂）
	 dTmpOne = log(1.0*m_nWidth) / log(2.0);
	 dTmpTwo = ceil(dTmpOne);
	 dTmpTwo = pow(2, dTmpTwo);
	 nTransWidth = (int)dTmpTwo;
	 // 计算进行傅立叶变换的点数－纵向 （2的整数次幂）
	 dTmpOne = log(1.0*m_nHeight) / log(2.0);
	 dTmpTwo = ceil(dTmpOne);
	 dTmpTwo = pow(2, dTmpTwo);
	 nTransHeight = (int)dTmpTwo;
	 double dReal;									// 傅立叶变换的实部
	 double dImag;									// 傅立叶变换的虚部

	 pCTData = new complex<double>[nTransWidth * nTransHeight];	// 分配内存
	 pCFData = new complex<double>[nTransWidth * nTransHeight];	// 分配内存
	 // 图像数据的宽和高不一定是2的整数次幂，所以pCTData有一部分数据需要补0
	 for (y = 0; y < nTransHeight; y++)
	 {
		 for (x = 0; x < nTransWidth; x++)
		 {
			 pCTData[y*nTransWidth + x] = complex<double>(0, 0);		// 补零
		 }
	 }
	 //把图像数据传给pCTData
	 for (y = 0; y < m_nHeight; y++)
	 {
		 for (x = 0; x < m_nWidth; x++)
		 {
			 // 指向DIB第y行，第x个象素的指针
			 lpSrc = (unsigned char*)m_pDibBits + m_nWidth * (m_nHeight - 1 - y) + x;
			 unchValue = (*lpSrc)*pow(-1.0, x + y);
			 pCTData[y*nTransWidth + x] = complex<double>(unchValue, 0);
		 }
	 }
	 FFT_2D(pCTData, m_nWidth, m_nHeight, pCFData);				// 傅立叶正变换
	 int *pixel=new int[m_nWidth*m_nHeight];
	 for (y = 0; y < m_nHeight; y++)								// 反变换的数据传给lpDIBBits
	 {
		 for (x = 0; x < m_nWidth; x++)
		 {
			 //需要考虑信号的正负问题以及实际所用的图象数据是灰度值还是原始数据
			 dReal = pCTData[y*nTransWidth + x].real();		// 实部
			 dImag = pCTData[y*nTransWidth + x].imag();		// 虚部
			 
			 if (unchValue < 0)
			 {
				 unchValue = 0;
			 }
			 if (unchValue > 0xff)
			 {
				 unchValue = 0xff;
			 }
			 pixel[y*m_nWidth+x] = sqrt(dReal*dReal + dImag* dImag);
			 // 指向DIB第y行，第x个象素的指针
			 lpSrc = (unsigned char*)m_pDibBits + m_nWidth * (m_nHeight - 1 - y) + x;
			 *lpSrc = (BYTE)unchValue;
		 }
	 }
	 
	 delete pCTData;										// 释放内存
	 delete pCFData;										// 释放内存
	 pCTData = NULL;
	 pCFData = NULL;
 }
