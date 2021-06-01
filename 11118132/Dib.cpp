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
	Create(Dib.GetWidth(),Dib.GetHeight(),Dib.GetBPP(),0);
	m_nWidth = Dib.m_nWidth;
	m_nHeight = Dib.m_nHeight;

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
	//if (m_pDibBits != NULL)
	//	delete m_pDibBits;
	//m_pDibBits = NULL;
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
	long *pGrayValueCount;
	long nColors;
	int i = 0; int j = 0;
	if (m_nWidth == m_nWidthBytes) {
		nColors = GetMaxColorTableEntries();
		if (nColors == 0)
		{
			return NULL;
		}
		pGrayValueCount = new long[nColors];
		memset(pGrayValueCount, 0, nColors * sizeof(long));
		for (i = 0; i < m_nHeight; i++)
		{
			for (j = 0; j < m_nWidth; j++)
			{
				pGrayValueCount[*(m_pDibBits + i * m_nWidthBytes + j)]++;
			}
		}
	}
	else {
		long I;
		nColors = 256;
		RGBTRIPLE* pRGB;
		pGrayValueCount = new long[nColors];
		memset(pGrayValueCount, 0, nColors * sizeof(long));
		for (i = 0; i < m_nWidth; ++i) {
			for (j = 0; j < m_nHeight; ++j) {
				pRGB = (RGBTRIPLE*)(m_pDibBits + (j)* m_nWidthBytes + i * 3);
				I = int(pRGB->rgbtBlue) + int(pRGB->rgbtRed) + int(pRGB->rgbtGreen);
				I = I / 3;
				pGrayValueCount[I]++;
			}
		}
	}
	return pGrayValueCount;
}

BOOL CDib::FFT(unsigned char* pDIBBits, long nWidth, long nHeight)
{ 
	unsigned char*	lpSrc;							// ָ��Դͼ���ָ��
	int y ;										// ѭ�����Ʊ���
	int x ;										// ѭ�����Ʊ���
	double dTmpOne ;								//�����ʱ����
	double dTmpTwo ;								//�����ʱ����
	int nTransWidth ;								// ����Ҷ�任�Ŀ�ȣ�2���������ݣ�
	int nTransHeight;								// ����Ҷ�任�ĸ߶ȣ�2���������ݣ�
	double unchValue;								// ����ͼ������ػҶȵ���ʱ����
	complex<double> * pCTData ;						// ָ��ʱ�����ݵ�ָ��
	complex<double> * pCFData ;						// ָ��Ƶ�����ݵ�ָ��
	// ������и���Ҷ�任�ĵ���������	��2���������ݣ�
	dTmpOne = log(1.0*nWidth)/log(2.0);
	dTmpTwo = ceil(dTmpOne)	;
	dTmpTwo = pow(2,dTmpTwo);
	nTransWidth = (int) dTmpTwo;	
	// ������и���Ҷ�任�ĵ��������� ��2���������ݣ�
	dTmpOne = log(1.0*nHeight)/log(2.0);
	dTmpTwo = ceil(dTmpOne)	;
	dTmpTwo = pow(2,dTmpTwo);
	nTransHeight = (int) dTmpTwo;
	double dReal;									// ����Ҷ�任��ʵ��
	double dImag;									// ����Ҷ�任���鲿
	
	pCTData=new complex<double>[nTransWidth * nTransHeight];	// �����ڴ�
	pCFData=new complex<double>[nTransWidth * nTransHeight];	// �����ڴ�
	// ͼ�����ݵĿ�͸߲�һ����2���������ݣ�����pCTData��һ����������Ҫ��0
	for(y=0; y<nTransHeight; y++)
	{
		for(x=0; x<nTransWidth; x++)
		{
			pCTData[y*nTransWidth + x]=complex<double>(0,0);		// ����
		}
	}
	//��ͼ�����ݴ���pCTData
	for(y=0; y<nHeight; y++)
	{
		for(x=0; x<nWidth; x++)
		{
			// ָ��DIB��y�У���x�����ص�ָ��
			lpSrc = (unsigned char*)pDIBBits + nWidth * (nHeight - 1 - y) + x;
			unchValue = (*lpSrc)*pow(-1.0,x+y);
			pCTData[y*nTransWidth + x]=complex<double>(unchValue,0);
		}
	}
	FFT_2D(pCTData, nWidth, nHeight, pCFData) ;				// ����Ҷ���任

	///////////////////////////////////////////////////////////////////////////
	// code needed to insert here
	//
	//
	//
	//
	///////////////////////////////////////////////////////////////////////////
	


	IFFT_2D(pCFData, pCTData,nHeight, nWidth); 				// ����Ҷ���任

	for(y=0; y<nHeight; y++)								// ���任�����ݴ���lpDIBBits
	{
		for(x=0; x<nWidth; x++)
		{
			//��Ҫ�����źŵ����������Լ�ʵ�����õ�ͼ�������ǻҶ�ֵ����ԭʼ����
			dReal = pCTData[y*nTransWidth + x].real() ;		// ʵ��
			dImag = pCTData[y*nTransWidth + x].imag() ;		// �鲿
			unchValue = dReal*pow(-1.0,x+y);
			if (unchValue < 0)
			{
				unchValue = 0;
			}
			if (unchValue > 0xff)
			{
				unchValue = 0xff;
			}
			// ָ��DIB��y�У���x�����ص�ָ��
			lpSrc = (unsigned char*)pDIBBits + nWidth * (nHeight - 1 - y) + x;
			*lpSrc =(BYTE)unchValue ;
		}
	}	
	
	delete pCTData;										// �ͷ��ڴ�
	delete pCFData;										// �ͷ��ڴ�
	pCTData = NULL;
	pCFData = NULL;	
	return (true);										//���ؽ��
}

/*************************************************************************
 *
 * \�������ƣ�
 *   FFT_1D()
 *
 * \�������:
 *   complex<double> * pCTData	- ָ��ʱ�����ݵ�ָ�룬�������Ҫ�任������
 *   complex<double> * pCFData	- ָ��Ƶ�����ݵ�ָ�룬����ľ����任������
 *   nLevel						������Ҷ�任�����㷨�ļ�����2��������
 *
 * \����ֵ:
 *   ��
 *
 * \˵��:
 *   һά���ٸ���Ҷ�任��
 *
 *************************************************************************
 */
void CDib::FFT_1D(complex<double> * pCTData, complex<double> * pCFData, int nLevel)
{
		// ѭ�����Ʊ���
	int		i;
	int     j;
	int     k;

	// ����Ҷ�任����
	int	nCount =0 ;

	// ���㸵��Ҷ�任����
	nCount =(int)pow(2.0,nLevel) ;
	
	// ĳһ���ĳ���
	int		nBtFlyLen;
	nBtFlyLen = 0 ;
	
	// �任ϵ���ĽǶ� ��2 * PI * i / nCount
	double	dAngle;
	
	complex<double> *pCW ;
	
	// �����ڴ棬�洢����Ҷ�仯��Ҫ��ϵ����
	pCW  = new complex<double>[nCount / 2];

    // ���㸵��Ҷ�任��ϵ��
	for(i = 0; i < nCount / 2; i++)
	{
		dAngle = -2 * Pi * i / nCount;
		pCW[i] = complex<double> ( cos(dAngle), sin(dAngle) );
	}

	// �任��Ҫ�Ĺ����ռ�
	complex<double> *pCWork1,*pCWork2; 
	
	// ���乤���ռ�
	pCWork1 = new complex<double>[nCount];

	pCWork2 = new complex<double>[nCount];

	
	// ��ʱ����
	complex<double> *pCTmp;
	
	// ��ʼ����д������
	memcpy(pCWork1, pCTData, sizeof(complex<double>) * nCount);

	// ��ʱ����
	int nInter; 
	nInter = 0;

	// �����㷨���п��ٸ���Ҷ�任
	for(k = 0; k < nLevel; k++)
	{
		for(j = 0; j < (int)pow(2.0,k); j++)
		{
			//���㳤��
			nBtFlyLen = (int)pow( 2.0,(nLevel-k) );
			
			//�������ţ���Ȩ����
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

		// ���� pCWork1��pCWork2������
		pCTmp   = pCWork1	;
		pCWork1 = pCWork2	;
		pCWork2 = pCTmp		;
	}
	
	// ��������
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
	
	// �ͷ��ڴ�ռ�
	delete pCW;
	delete pCWork1;
	delete pCWork2;
	pCW		=	NULL	;
	pCWork1 =	NULL	;
	pCWork2 =	NULL	;
}
/*************************************************************************
 *
 * \�������ƣ�
 *    IFFT_1D()
 *
 * \�������:
 *   complex<double> * pCTData	- ָ��ʱ�����ݵ�ָ�룬�������Ҫ���任������
 *   complex<double> * pCFData	- ָ��Ƶ�����ݵ�ָ�룬����ľ������任������
 *   nLevel						������Ҷ�任�����㷨�ļ�����2��������
 *
 * \����ֵ:
 *   ��
 *
 * \˵��:
 *   һά���ٸ���Ҷ���任��
 *
 ************************************************************************
 */
void CDib::IFFT_1D(complex<double> * pCFData, complex<double> * pCTData, int nLevel)
{
	
	// ѭ�����Ʊ���
	int		i;

	// ����Ҷ���任����
	int nCount;

	// ���㸵��Ҷ�任����
	nCount = (int)pow(2.0,nLevel) ;
	
	// �任��Ҫ�Ĺ����ռ�
	complex<double> *pCWork;	
	
	// ���乤���ռ�
	pCWork = new complex<double>[nCount];
	
	// ����Ҫ���任������д�빤���ռ�pCWork
	memcpy(pCWork, pCFData, sizeof(complex<double>) * nCount);
	
	// Ϊ�����ø���Ҷ���任,���԰Ѹ���ҶƵ�������ȡ����
	// Ȼ��ֱ���������任�����������Ǹ���Ҷ���任����Ĺ���
	for(i = 0; i < nCount; i++)
	{
		pCWork[i] = complex<double> (pCWork[i].real(), -pCWork[i].imag());
	}
	
	// ���ÿ��ٸ���Ҷ�任ʵ�ַ��任������洢��pCTData��
	FFT_1D(pCWork, pCTData, nLevel);
	
	// ��ʱ���Ĺ��������ս��
	// ���ݸ���Ҷ�任ԭ�����������ķ�����õĽ����ʵ�ʵ�ʱ������
	// ���һ��ϵ��nCount
	for(i = 0; i < nCount; i++)
	{
		pCTData[i] = 
			complex<double> (pCTData[i].real() / nCount, -pCTData[i].imag() / nCount);
	}
	
	// �ͷ��ڴ�
	delete pCWork;
	pCWork = NULL;
}

/*************************************************************************
 *
 * \�������ƣ�
 *   FFT_2D()
 *
 * \�������:
 *   complex<double> * pCTData	- ͼ������
 *   int    nWidth				- ���ݿ��
 *   int    nHeight				- ���ݸ߶�
 *   complex<double> * pCFData	- ����Ҷ�任��Ľ��
 *
 * \����ֵ:
 *   ��
 *
 * \˵��:
 *   ��ά����Ҷ�任��
 *
 ************************************************************************
 */
void CDib::FFT_2D(complex<double> * pCTData, int nWidth, int nHeight, complex<double> * pCFData)
{
	
	// ѭ�����Ʊ���
	int	x;
	int	y;
	
	// ��ʱ����
	double	dTmpOne;
	double  dTmpTwo;
	
	// ���и���Ҷ�任�Ŀ�Ⱥ͸߶ȣ���2���������ݣ�
	// ͼ��Ŀ�Ⱥ͸߶Ȳ�һ��Ϊ2����������
	int		nTransWidth;
	int 	nTransHeight;

	// ������и���Ҷ�任�Ŀ��	��2���������ݣ�
	dTmpOne = log(1.0*nWidth)/log(2.0);
	dTmpTwo = ceil(dTmpOne)		   ;
	dTmpTwo = pow(2,dTmpTwo)	   ;
	nTransWidth = (int) dTmpTwo	   ;
	
	// ������и���Ҷ�任�ĸ߶� ��2���������ݣ�
	dTmpOne = log(1.0*nHeight)/log(2.0);
	dTmpTwo = ceil(dTmpOne)		   ;
	dTmpTwo = pow(2,dTmpTwo)	   ;
	nTransHeight = (int) dTmpTwo	   ;	
	
	// x��y�����У������ϵĵ�������
	int		nXLev;
	int		nYLev;

	// ����x��y�����У������ϵĵ�������
	nXLev = (int) ( log(1.0*nTransWidth)/log(2.0) +  0.5 );
	nYLev = (int) ( log(1.0*nTransHeight)/log(2.0) + 0.5 );
	
	for(y = 0; y < nTransHeight; y++)
	{
		// x������п��ٸ���Ҷ�任
		FFT_1D(&pCTData[nTransWidth * y], &pCFData[nTransWidth * y], nXLev);
	}
	
	// pCFData��Ŀǰ�洢��pCTData�����б任�Ľ��
	// Ϊ��ֱ������FFT_1D����Ҫ��pCFData�Ķ�ά����ת�ã���һ������FFT_1D����
	// ����Ҷ�б任��ʵ�����൱�ڶ��н��и���Ҷ�任��
	for(y = 0; y < nTransHeight; y++)
	{
		for(x = 0; x < nTransWidth; x++)
		{
			pCTData[nTransHeight * x + y] = pCFData[nTransWidth * y + x];
		}
	}
	
	for(x = 0; x < nTransWidth; x++)
	{
		// ��x������п��ٸ���Ҷ�任��ʵ�����൱�ڶ�ԭ����ͼ�����ݽ����з����
		// ����Ҷ�任
		FFT_1D(&pCTData[x * nTransHeight], &pCFData[x * nTransHeight], nYLev);
	}

	// pCFData��Ŀǰ�洢��pCTData������ά����Ҷ�任�Ľ��������Ϊ�˷����з���
	// �ĸ���Ҷ�任�����������ת�ã����ڰѽ��ת�û���
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
 * \�������ƣ�
 *   IFFT_2D()
 *
 * \�������:
 *   complex<double> * pCFData	- Ƶ������
 *   complex<double> * pCTData	- ʱ������
 *   int    nWidth				- ͼ�����ݿ��
 *   int    nHeight				- ͼ�����ݸ߶�
 *
 * \����ֵ:
 *   ��
 *
 * \˵��:
 *   ��ά����Ҷ���任��
 *
 ************************************************************************
 */
void CDib::IFFT_2D(complex<double> * pCFData, complex<double> * pCTData, int nWidth, int nHeight)
{
	// ѭ�����Ʊ���
	int	x;
	int	y;
	
	// ��ʱ����
	double	dTmpOne;
	double  dTmpTwo;
	
	// ���и���Ҷ�任�Ŀ�Ⱥ͸߶ȣ���2���������ݣ�
	// ͼ��Ŀ�Ⱥ͸߶Ȳ�һ��Ϊ2����������
	int		nTransWidth;
	int 	nTransHeight;

	// ������и���Ҷ�任�Ŀ��	��2���������ݣ�
	dTmpOne = log(1.0*nWidth)/log(2.0);
	dTmpTwo = ceil(dTmpOne)		   ;
	dTmpTwo = pow(2,dTmpTwo)	   ;
	nTransWidth = (int) dTmpTwo	   ;
	
	// ������и���Ҷ�任�ĸ߶� ��2���������ݣ�
	dTmpOne = log(1.0*nHeight)/log(2.0);
	dTmpTwo = ceil(dTmpOne)		   ;
	dTmpTwo = pow(2,dTmpTwo)	   ;
	nTransHeight = (int) dTmpTwo	   ;
	
	// ���乤����Ҫ���ڴ�ռ�
	complex<double> *pCWork= new complex<double>[nTransWidth * nTransHeight];

	//��ʱ����
	complex<double> *pCTmp ;
	
	// Ϊ�����ø���Ҷ���任,���԰Ѹ���ҶƵ�������ȡ����
	// Ȼ��ֱ���������任�����������Ǹ���Ҷ���任����Ĺ���
	for(y = 0; y < nTransHeight; y++)
	{
		for(x = 0; x < nTransWidth; x++)
		{
			pCTmp = &pCFData[nTransWidth * y + x] ;
			pCWork[nTransWidth * y + x] = complex<double>( pCTmp->real() , -pCTmp->imag() );
		}
	}

	// ���ø���Ҷ���任
	FFT_2D(pCWork, nWidth, nHeight, pCTData) ;
	
	// ��ʱ���Ĺ��������ս��
	// ���ݸ���Ҷ�任ԭ�����������ķ�����õĽ����ʵ�ʵ�ʱ������
	// ���һ��ϵ��
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
	 // TODO: �ڴ˴����ʵ�ִ���.
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
	 // TODO: �ڴ˴����ʵ�ִ���.
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
	 // TODO: �ڴ˴����ʵ�ִ���.
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
	 // TODO: �ڴ˴����ʵ�ִ���.
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
	 // TODO: �ڴ˴����ʵ�ִ���.
	 long* GrayCount = GrayValueCount();
	 int nColor;
	 if(m_nWidth==m_nWidthBytes)
		nColor = GetMaxColorTableEntries();
	 else nColor = 256;
	 long N = 0;//ֱ��ͼ��������
	 long* Sk = new long[nColor];
	 for (int i = 0; i < nColor; ++i) {
		 Sk[i] = 0;//��ʼ��Sk
		 N += GrayCount[i];
	 }
	 for (int i = 0; i < nColor; ++i) {
		 for (int j = 0; j <= i; ++j) {
			 Sk[i] = Sk[i] + GrayCount[j];
		 }
	 }
	 if (m_nWidth == m_nWidthBytes) {
		 for (int i = 0; i < m_nHeight; i++)
		 {
			 for (int j = 0; j < m_nWidth; j++)
			 {
				 if (long(nColor*(Sk[(*(m_pDibBits + i * m_nWidthBytes + j))]) / N) <= 0)
					 *(m_pDibBits + i * m_nWidthBytes + j) = 0;
				 else
					 *(m_pDibBits + i * m_nWidthBytes + j) = (nColor*(Sk[(*(m_pDibBits + i * m_nWidthBytes + j))]) / N);
				 
			 }
		 }
	 }
	 else
	 {
		 RGBTRIPLE* pRGB;
		 double r, g, b, temp1, temp2;
		 double H;
		 for (int i = 0; i < m_nWidth; ++i) {
			 for (int j = 0; j < m_nHeight; ++j) {
				 RGBTRIPLE* pRGB = (RGBTRIPLE*)(m_pDibBits + (j)* m_nWidthBytes + i * 3);
				 r = double(pRGB->rgbtRed) / 255;
				 g = double(pRGB->rgbtGreen) / 255;
				 b = double(pRGB->rgbtBlue) / 255;
				 temp1 = (2 * r - g - b) / 2;
				 temp2 = (r - g)*(r - g) + (r - b)*(g - b);
				 temp2 = pow(temp2, 0.5);
				 if (temp2 != 0)
					 temp1 = acos(temp1 / temp2);
				 else
					 temp1 = 0;
				 if (b > g)
					 H = (2 * Pi - temp1);
				 else
					 H = (temp1);

				 int min = 500;
				 int tp[3] = { int(pRGB->rgbtRed) ,int(pRGB->rgbtGreen) ,int(pRGB->rgbtBlue) };
				 for (int t = 0; t < 3; ++t) {
					 if (min > tp[t])
						 min = tp[t];
				 }

				 double S = min * 255;
				 long I = int(pRGB->rgbtBlue) + int(pRGB->rgbtRed) + int(pRGB->rgbtGreen);
				 I = I / 3;
				 //temp = temp / (int(pRGB->rgbtBlue) + int(pRGB->rgbtRed) + int(pRGB->rgbtGreen));
				 if (I == 0)
					 S = 0;
				 else
					 S = 255 - S / I;
				 S = S / 255;
				 int newI = (nColor*(Sk[(I)]) / N);
				 //newI = I;
				 int t1= newI * (1 - S);
				 if (t1 > 255)
					 t1 = 255;
				 if (H>=0&&H<2*Pi/3) {
					 pRGB->rgbtBlue = t1;
					 int t2 = newI * (1 + S * cos(H) / cos(Pi / 3 - H));
					 if (t2 > 255)
						 t2 = 255;
					 pRGB->rgbtRed = t2;
					 int t3 = newI * 3 - (pRGB->rgbtBlue + pRGB->rgbtRed);
					 if (t3 > 255)
						 t3 = 255;
					 pRGB->rgbtGreen = t3;
				 }
				 else if (H >= 2 * Pi / 3&&H<4*Pi/3) {
					 H = H - 2 * Pi / 3;
					 int t2 = newI * (1 + S * cos(H) / cos(Pi / 3 - H));
					 if (t2 > 255)
						 t2 = 255;
					 pRGB->rgbtRed = t1;
					 pRGB->rgbtGreen = t2;
					 int t3 = newI * 3 - (pRGB->rgbtGreen + pRGB->rgbtRed);
					 if (t3 > 255)
						 t3 = 255;
					 pRGB->rgbtBlue = t3;
				 }
				 else if(H>4*Pi/3&&H<2*Pi){
					 H = H - 4 * Pi / 3;
					 int t2 = newI * (1 + S * cos(H) / cos(Pi / 3 - H));
					 if (t2 > 255)
						 t2 = 255;
					 pRGB->rgbtBlue = t2;
					 pRGB->rgbtGreen = t1;
					 int t3 = newI * 3 - (pRGB->rgbtGreen + pRGB->rgbtBlue);
					 if (t3 > 255)
						 t3 = 255;
					 pRGB->rgbtRed = t3;
				 }
			 }
		 }
	 }
	 delete[] GrayCount;
	 delete[] Sk;
 }

 void CDib::smooth()
 {
	 // TODO: �ڴ˴����ʵ�ִ���.3*3
	 int** pixel;
	 int** result;
	 int laplace[9] = { 1,1,1,1,1,1,1,1,1 };
	 if (m_nWidthBytes == m_nWidth) {
		 pixel = AddZeros(0);
		 //int** result;
		 

		 result = Conv(pixel, laplace);

		 for (int i = 0; i < m_nHeight; ++i) {
			 for (int j = 0; j < m_nWidthBytes; ++j) {
				 *(m_pDibBits + i * m_nWidthBytes + j) = result[i][j] / 9;

			 }
			 delete[] pixel[i];
			 delete[] result[i];
		 }
	 }
	 else
	 {
		 RGBTRIPLE* pRGB;
		 pixel = AddZeros(1);
		 result = Conv(pixel, laplace);
		 for (int i = 0; i < m_nHeight; ++i) {
			 for (int j = 0; j < m_nWidth; ++j) {
				 pRGB = (RGBTRIPLE*)(m_pDibBits + i * m_nWidthBytes + j * 3);
				 pRGB->rgbtRed = result[i][j] / 9;
			 }
			 delete[] pixel[i];
			 delete[] result[i];
		 }
		 delete[] pixel;
		 delete[] result;
		 pixel = AddZeros(2);
		 result = Conv(pixel, laplace);
		 for (int i = 0; i < m_nHeight; ++i) {
			 for (int j = 0; j < m_nWidth; ++j) {
				 pRGB = (RGBTRIPLE*)(m_pDibBits + i * m_nWidthBytes + j * 3);
				 pRGB->rgbtGreen = result[i][j] / 9;
			 }
			 delete[] pixel[i];
			 delete[] result[i];
		 }
		 delete[] pixel;
		 delete[] result;
		 pixel = AddZeros(3);
		 result = Conv(pixel, laplace);
		 for (int i = 0; i < m_nHeight; ++i) {
			 for (int j = 0; j < m_nWidth; ++j) {
				 pRGB = (RGBTRIPLE*)(m_pDibBits + i * m_nWidthBytes + j * 3);
				 pRGB->rgbtBlue = result[i][j] / 9;
			 }
			 delete[] pixel[i];
			 delete[] result[i];
		 }
	 }
	 delete[] pixel;
	 delete[] result;
 }

 void CDib::Sobel()
 {
     int** pixel;
	 int** result1;
	 int** result2;
	 int Sobel1[9] = { -1,-2,-1,0,0,0,1,2,1 };
	 int Sobel2[9] = { -1,0,1,-2,0,2,-1,0,1 };
	 if (m_nWidth == m_nWidthBytes) {
		 pixel = AddZeros(0);
		 result1 = Conv(pixel, Sobel1);
		 result2 = Conv(pixel, Sobel2);

		 for (int i = 0; i < m_nHeight; ++i) {
			 for (int j = 0; j < m_nWidth; ++j) {
				 *(m_pDibBits + i * m_nWidthBytes + j) = abs(result1[i][j]) + abs(result2[i][j]);
			 }
			 delete[] result1[i];
			 delete[] result2[i];
		 }
		 for (int i = 0; i < m_nHeight + 2; ++i) {
			 delete[] pixel[i];
		 }
	 }
	 else {
		 RGBTRIPLE* pRGB;

		 pixel = AddZeros(1);
		 result1 = Conv(pixel, Sobel1);
		 result2 = Conv(pixel, Sobel2);

		 for (int i = 0; i < m_nHeight; ++i) {
			 for (int j = 0; j < m_nWidth; ++j) {
				 pRGB = (RGBTRIPLE*)(m_pDibBits + i * m_nWidthBytes + j * 3);
				 pRGB->rgbtRed = abs(result1[i][j]) + abs(result2[i][j]);
			 }
			 delete[] result1[i];
			 delete[] result2[i];
		 }
		 for (int i = 0; i < m_nHeight + 2; ++i) {
			 delete[] pixel[i];
		 }
		 delete[] pixel;
		 delete[] result1;
		 delete[] result2;
		 pixel = AddZeros(2);
		 result1 = Conv(pixel, Sobel1);
		 result2 = Conv(pixel, Sobel2);

		 for (int i = 0; i < m_nHeight; ++i) {
			 for (int j = 0; j < m_nWidth; ++j) {
				 pRGB = (RGBTRIPLE*)(m_pDibBits + i * m_nWidthBytes + j * 3);
				 pRGB->rgbtGreen = abs(result1[i][j]) + abs(result2[i][j]);
			 }
			 delete[] result1[i];
			 delete[] result2[i];
		 }
		 for (int i = 0; i < m_nHeight + 2; ++i) {
			 delete[] pixel[i];
		 }
		 delete[] pixel;
		 delete[] result1;
		 delete[] result2;
		 pixel = AddZeros(3);
		 result1 = Conv(pixel, Sobel1);
		 result2 = Conv(pixel, Sobel2);

		 for (int i = 0; i < m_nHeight; ++i) {
			 for (int j = 0; j < m_nWidth; ++j) {
				 pRGB = (RGBTRIPLE*)(m_pDibBits + i * m_nWidthBytes + j * 3);
				 pRGB->rgbtBlue = abs(result1[i][j]) + abs(result2[i][j]);
			 }
			 delete[] result1[i];
			 delete[] result2[i];
		 }
		 for (int i = 0; i < m_nHeight + 2; ++i) {
			 delete[] pixel[i];
		 }
	 }
	 delete[] pixel;
	 delete[] result1;
	 delete[] result2;
 }

 void CDib::Laplace()
 {
	 int** pixel;
	 int** result;
	 int laplace[9] = { 0,-1,0,-1,4,-1,0,-1,0 };
	 if (m_nWidth == m_nWidthBytes) {
		 pixel = AddZeros(0);
		 result = Conv(pixel, laplace);
		 for (int i = 0; i < m_nHeight; ++i) {
			 for (int j = 0; j < m_nWidthBytes; ++j) {
				 if (result[i][j] < 0)
					 result[i][j] = 0;
				 if (result[i][j] >255)
					 result[i][j] = 255;
				 *(m_pDibBits + i * m_nWidthBytes + j) = result[i][j];
			 }
			 delete[] result[i];
		 }
		 for (int i = 0; i < m_nHeight + 2; ++i) {
			 delete[] pixel[i];
		 }
	 }
	 else {
		 RGBTRIPLE* pRGB;
		 pixel = AddZeros(1);
		 result = Conv(pixel, laplace);
		 for (int i = 0; i < m_nHeight; ++i) {
			 for (int j = 0; j < m_nWidth; ++j) {
				 pRGB = (RGBTRIPLE*)(m_pDibBits + i * m_nWidthBytes + j * 3);
				 if (result[i][j] < 0)
					 result[i][j] = 0;
				 if (result[i][j] > 255)
					 result[i][j] = 255;
				 pRGB->rgbtRed = result[i][j];
			 }
			 delete[] result[i];
		 }
		 for (int i = 0; i < m_nHeight + 2; ++i) {
			 delete[] pixel[i];
		 }
		 delete[] pixel;
		 delete[] result;

		 pixel = AddZeros(2);
		 result = Conv(pixel, laplace);
		 for (int i = 0; i < m_nHeight; ++i) {
			 for (int j = 0; j < m_nWidth; ++j) {
				 pRGB = (RGBTRIPLE*)(m_pDibBits + i * m_nWidthBytes + j * 3);
				 if (result[i][j] < 0)
					 result[i][j] = 0;
				 if (result[i][j] > 255)
					 result[i][j] = 255;
				 pRGB->rgbtGreen = result[i][j];
			 }
			 delete[] result[i];
		 }
		 for (int i = 0; i < m_nHeight + 2; ++i) {
			 delete[] pixel[i];
		 }
		 delete[] pixel;
		 delete[] result;

		 pixel = AddZeros(3);
		 result = Conv(pixel, laplace);
		 for (int i = 0; i < m_nHeight; ++i) {
			 for (int j = 0; j < m_nWidth; ++j) {
				 pRGB = (RGBTRIPLE*)(m_pDibBits + i * m_nWidthBytes + j * 3);
				 if (result[i][j] < 0)
					 result[i][j] = 0;
				 if (result[i][j] > 255)
					 result[i][j] = 255;
				 pRGB->rgbtBlue = result[i][j];
			 }
			 delete[] result[i];
		 }
		 for (int i = 0; i < m_nHeight + 2; ++i) {
			 delete[] pixel[i];
		 }
	 }
	 delete[] pixel;
	 delete[] result;
 }

 void CDib::White()
 {
	 // TODO: �ڴ˴����ʵ�ִ���.
	 if (!IsNull())
	 {
		 Destroy();
	 }
	 Create(512, 512, 8, 0);
	 m_nWidth = GetWidth();
	 m_nHeight = GetHeight();
	 m_nWidthBytes = abs(GetPitch());
	 m_nBitCount = GetBPP();
	 m_pDibBits = (unsigned char*)GetBits() + (m_nHeight - 1)*GetPitch();
	 m_pGrayValueCount = NULL;
	 if (IsIndexed())
	 {
		 int nColors = 256;
		 RGBQUAD *pal = new RGBQUAD[nColors];
		 for (int i = 0; i < nColors; ++i) {
			 pal[i].rgbBlue = i;
			 pal[i].rgbRed = i;
			 pal[i].rgbGreen = i;
		 }
		 SetColorTable(0, nColors, pal);
		 delete[] pal;
	 }
	 int W = 50, H = 512;
	 for (int i = 0; i < W; ++i) {
		 for (int j = 0; j < H; ++j) {
			 *(m_pDibBits + (i+ (m_nHeight+1)/2 -W/2) * m_nWidthBytes + (j+(m_nWidthBytes+1)/2-H/2)) = 255;
		 }
	 }
 }

 void CDib::FFT_VIEW()
 {
	 // TODO: �ڴ˴����ʵ�ִ���.
	 unsigned char*	lpSrc;							// ָ��Դͼ���ָ��
	 int y;										// ѭ�����Ʊ���
	 int x;										// ѭ�����Ʊ���
	 double dTmpOne;								//�����ʱ����
	 double dTmpTwo;								//�����ʱ����
	 int nTransWidth;								// ����Ҷ�任�Ŀ�ȣ�2���������ݣ�
	 int nTransHeight;								// ����Ҷ�任�ĸ߶ȣ�2���������ݣ�
	 double unchValue;								// ����ͼ������ػҶȵ���ʱ����
	 complex<double> * pCTData;						// ָ��ʱ�����ݵ�ָ��
	 complex<double> * pCFData;						// ָ��Ƶ�����ݵ�ָ��
	 // ������и���Ҷ�任�ĵ���������	��2���������ݣ�
	 dTmpOne = log(1.0*m_nWidth) / log(2.0);
	 dTmpTwo = ceil(dTmpOne);
	 dTmpTwo = pow(2, dTmpTwo);
	 nTransWidth = (int)dTmpTwo;
	 // ������и���Ҷ�任�ĵ��������� ��2���������ݣ�
	 dTmpOne = log(1.0*m_nHeight) / log(2.0);
	 dTmpTwo = ceil(dTmpOne);
	 dTmpTwo = pow(2, dTmpTwo);
	 nTransHeight = (int)dTmpTwo;
	 double dReal;									// ����Ҷ�任��ʵ��
	 double dImag;									// ����Ҷ�任���鲿

	 pCTData = new complex<double>[nTransWidth * nTransHeight];	// �����ڴ�
	 pCFData = new complex<double>[nTransWidth * nTransHeight];	// �����ڴ�
	 // ͼ�����ݵĿ�͸߲�һ����2���������ݣ�����pCTData��һ����������Ҫ��0
	 for (y = 0; y < nTransHeight; y++)
	 {
		 for (x = 0; x < nTransWidth; x++)
		 {
			 pCTData[y*nTransWidth + x] = complex<double>(0, 0);		// ����
		 }
	 }
	 //��ͼ�����ݴ���pCTData
	 for (y = 0; y < m_nHeight; y++)
	 {
		 for (x = 0; x < m_nWidth; x++)
		 {
			 // ָ��DIB��y�У���x�����ص�ָ��
			 lpSrc = (unsigned char*)m_pDibBits + m_nWidth * (m_nHeight - 1 - y) + x;
			 unchValue = (*lpSrc)*pow(-1.0, x + y);
			 pCTData[y*nTransWidth + x] = complex<double>(unchValue, 0);
		 }
	 }
	 FFT_2D(pCTData, m_nWidth, m_nHeight, pCFData);				// ����Ҷ���任
	 double *pixel=new double[m_nWidth*m_nHeight];
	 //long max = 0;
	 for (y = 0; y < m_nHeight; y++)								// ���任�����ݴ���lpDIBBits
	 {
		 for (x = 0; x < m_nWidth; x++)
		 {
			 //��Ҫ�����źŵ����������Լ�ʵ�����õ�ͼ�������ǻҶ�ֵ����ԭʼ����
			 dReal = pCFData[y*nTransWidth + x].real();		// ʵ��
			 dImag = pCFData[y*nTransWidth + x].imag();		// �鲿
			 pixel[y*m_nWidth + x] = log2(1 + sqrt(dReal*dReal + dImag * dImag)) / log2(1.06);
			 //if (max < sqrt(dReal*dReal + dImag * dImag))
			//	 max = sqrt(dReal*dReal + dImag * dImag);
		 }
	 }
	 for (int i = 0; i < m_nHeight; ++i) {
		 for (int j = 0; j < m_nWidthBytes; ++j) {
			 *(m_pDibBits + (i)* m_nWidthBytes + (j)) = pixel[i*m_nWidthBytes + j];
		 }
	 }
	 delete pCTData;										// �ͷ��ڴ�
	 delete pCFData;										// �ͷ��ڴ�
	 pCTData = NULL;
	 pCFData = NULL;
	 delete[] pixel;
 }

 int** CDib::Conv(int** pixel, int kernel[9])
 {
	 // TODO: �ڴ˴����ʵ�ִ���.
	 int **result = new int*[(m_nHeight)];
	 for (int i = 0; i < (m_nHeight); ++i) {
		 result[i] = new int[m_nWidthBytes];
	 }
	 int j = 0;
	 for (int i = 0;i< m_nHeight;++i) {
		 for (j = 0; j < m_nWidthBytes; ++j) {
			 result[i][j] = pixel[i][j]*kernel[0] + pixel[i][j+1]* kernel[1] + pixel[i][j+2]* kernel[2]
				 + pixel[i+1][j]* kernel[3] + pixel[i+1][j+1]* kernel[4] + pixel[i+1][j+2]* kernel[5]
				 + pixel[i+2][j]* kernel[6] + pixel[i+2][j+1]* kernel[7] + pixel[i+2][j+2]* kernel[8];
		 }

	 }
	 return result;
 }

 int** CDib::AddZeros(int type)
 {
	 // TODO: �ڴ˴����ʵ�ִ���.
	 //type,0:�Ҷȣ�1��R��2��G��3��B
	 int **pixel = new int*[(m_nHeight + 2)];
	 for (int i = 0; i < (m_nHeight + 2); ++i) {
		 pixel[i] = new int[m_nWidthBytes + 2];
	 }
	 if (type == 0) {
		 for (int i = 0; i < (m_nHeight + 2); ++i) {
			 for (int j = 0; j < (m_nWidthBytes + 2); ++j) {
				 if (i<1 || i>(m_nWidthBytes - 2) || j<1 || j>(m_nHeight - 2))
					 pixel[i][j] = 0;
				 else {
					 pixel[i][j] = *(m_pDibBits + (i - 1)*m_nWidthBytes + (j - 1));
				 }
			 }
		 }
	 }
	 else  {
		 RGBTRIPLE* pRGB;
		 int temp;
		 for (int i = 0; i < (m_nHeight + 2); ++i) {
			 for (int j = 0; j < (m_nWidth + 2); ++j) {
				 if (i<1 || i>(m_nWidth - 2) || j<1 || j>(m_nHeight - 2))
					 pixel[i][j] = 0;
				 else {
					 pRGB = (RGBTRIPLE*)(m_pDibBits + (i-1) * m_nWidthBytes + (j-1) * 3);
					 if (type == 1) {
						 temp = pRGB->rgbtRed;
					 }
					 if (type == 2) {
						 temp = pRGB->rgbtGreen;
					 }
					 if (type == 3) {
						 temp = pRGB->rgbtBlue;
					 }
					 pixel[i][j] = temp;
				 }
			 }
		 }
	 }
	 return pixel;
 }

 void CDib::Filtering(enum type t, bool pad)
 {
	 // TODO: �ڴ˴����ʵ�ִ���.
	 unsigned char*	lpSrc;							// ָ��Դͼ���ָ��
	 int y;										// ѭ�����Ʊ���
	 int x;										// ѭ�����Ʊ���
	 double dTmpOne;								//�����ʱ����
	 double dTmpTwo;								//�����ʱ����
	 int nTransWidth;								// ����Ҷ�任�Ŀ�ȣ�2���������ݣ�
	 int nTransHeight;								// ����Ҷ�任�ĸ߶ȣ�2���������ݣ�
	 double unchValue;								// ����ͼ������ػҶȵ���ʱ����
	 complex<double> * pCTData;						// ָ��ʱ�����ݵ�ָ��
	 complex<double> * pCFData;						// ָ��Ƶ�����ݵ�ָ��
	 // ������и���Ҷ�任�ĵ���������	��2���������ݣ�
	 if (pad == false) {
		 //int m_nWidth2 = m_nWidth;
		 dTmpOne = log(1.0*m_nWidth) / log(2.0);
		 dTmpTwo = ceil(dTmpOne);
		 dTmpTwo = pow(2, dTmpTwo);
		 nTransWidth = (int)dTmpTwo;
	 }
	 else {
		 dTmpOne = log(2.0*m_nWidth) / log(2.0);
		 dTmpTwo = ceil(dTmpOne);
		 dTmpTwo = pow(2, dTmpTwo);
		 nTransWidth = (int)dTmpTwo;
	 }
	 // ������и���Ҷ�任�ĵ��������� ��2���������ݣ�
	 if (pad == false) {
		 //int m_nHeight2 = m_nHeight;
		 dTmpOne = log(1.0*m_nHeight) / log(2.0);
		 dTmpTwo = ceil(dTmpOne);
		 dTmpTwo = pow(2, dTmpTwo);
		 nTransHeight = (int)dTmpTwo;
	 }
	 else {
		 //int m_nHeight2 = m_nHeight * 2;
		 dTmpOne = log(2.0*m_nHeight) / log(2.0);
		 dTmpTwo = ceil(dTmpOne);
		 dTmpTwo = pow(2, dTmpTwo);
		 nTransHeight = (int)dTmpTwo;
	 }
	 double dReal;									// ����Ҷ�任��ʵ��
	 double dImag;									// ����Ҷ�任���鲿
	 pCTData = new complex<double>[nTransWidth * nTransHeight];	// �����ڴ�
	 pCFData = new complex<double>[nTransWidth * nTransHeight];	// �����ڴ�
	 // ͼ�����ݵĿ�͸߲�һ����2���������ݣ�����pCTData��һ����������Ҫ��0
	 for (y = 0; y < nTransHeight; y++)
	 {
		 for (x = 0; x < nTransWidth; x++)
		 {
			 pCTData[y*nTransWidth + x] = complex<double>(0, 0);		// ����
		 }
	 }
	 //��ͼ�����ݴ���pCTData
	 for (y = 0; y < m_nHeight; y++)
	 {
		 for (x = 0; x < m_nWidth; x++)
		 {
			 // ָ��DIB��y�У���x�����ص�ָ��
			 lpSrc = (unsigned char*)m_pDibBits + m_nWidth * (m_nHeight - 1 - y) + x;
			 unchValue = (*lpSrc)*pow(-1.0, x + y);
			 pCTData[y*nTransWidth + x] = complex<double>(unchValue, 0);
		 }
	 }
	 FFT_2D(pCTData, nTransWidth, nTransHeight, pCFData);				// ����Ҷ���任
	 //int *pixel = new int[m_nWidth*m_nHeight];
	 int type1, type2;
	 switch (t)
	 {
	 case CDib::L_Ideal:
		 type1 = 0, type2 = 0;
		 break;
	 case CDib::L_BW:
		 type1 = 0, type2 = 1;
		 break;
	 case CDib::L_Gauss:
		 type1 = 0, type2 = 2;
		 break;
	 case CDib::H_Ideal:
		 type1 = 1, type2 = 0;
		 break;
	 case CDib::H_BW:
		 type1 = 1, type2 = 1;
		 break;
	 case CDib::H_Gauss:
		 type1 = 1, type2 = 2;
		 break;
	 case CDib::Tur_D:
		 type1 = 2, type2 = 0;
		 break;
	 case CDib::Anti_F:
		 type1 = 2, type2 = 1;
		 break;
	 case CDib::Wiener:
		 type1 = 2, type2 = 2;
		 break;
	 default:
		 type1 = -1, type2 = -1;
		 break;
	 }
	 double F;
	 for (y = 0; y < nTransHeight; y++)
	 {
		 for (x = 0; x < nTransWidth; x++)
		 {
			 F = Filter(type1, type2, nTransWidth, nTransHeight, x, y);
			 pCFData[y*nTransWidth + x] *= F;
		 }
	 }
	 IFFT_2D(pCFData, pCTData, nTransHeight, nTransWidth); 				// ����Ҷ���任
	 //int max=0, min=256;
	 for (y = 0; y < m_nHeight; y++)								// ���任�����ݴ���lpDIBBits
	 {
		 for (x = 0; x < m_nWidth; x++)
		 {
			 //��Ҫ�����źŵ����������Լ�ʵ�����õ�ͼ�������ǻҶ�ֵ����ԭʼ����
			 dReal = pCTData[y*nTransWidth + x].real();		// ʵ��
			 unchValue = dReal * pow(-1.0, x + y);
			 if (unchValue < 0)
			 {
				 unchValue = 0;
			 }
			 if (unchValue > 0xff)
			 {
				 unchValue = 0xff;
			 }
			 lpSrc = (unsigned char*)m_pDibBits + m_nWidth * (m_nHeight - 1 - y) + x;
			 *lpSrc = (BYTE)unchValue;
		 }
	 }
	 delete pCTData;										// �ͷ��ڴ�
	 delete pCFData;										// �ͷ��ڴ�
	 pCTData = NULL;
	 pCFData = NULL; 
 }

 double CDib::Filter(int type1, int type2, int p, int q,int x,int y)
 {
	 // TODO: �ڴ˴����ʵ�ִ���.
	 //p����q��
	 double filter=0;
	 double D0 = 50;
	 double D = sqrt((q / 2 - y)*(q / 2 - y) + (p / 2 - x)*(p / 2 - x));
	 if (type1 == 0||type1==1) {
		 //��ͨ�˲���:0,��ͨ:1
		 if (type2 == 0) {
			 //����	  
			 if (D <= D0) {
				 filter = 1 - type1;
			 }
			 else
				 filter = type1;
			 return filter;
		 }
		 else if (type2 == 1) {
			 //Butter Worth
			 //p����q��
			 int n2 = 4 * pow(-1, type1); 
			 filter = 1 / (1 + pow(D / D0, n2));
			 return filter;
		 }
		 else if (type2 == 2) {
			 //Gauss
			 int n = pow(-1,type1);
			 double n2 = -pow(D, 2) / (2 * pow(D0, 2));
			 filter = 1 - n + n * pow(Ei, n2);
			 return filter;
		 }
		 else
			 return 0;
	 }
	 else if(type1==2)
	 {   //�����˻������˲�
		 double k = 0.001; double t = 5;
		 t = t / 6;
		 double temp= (q / 2 - y)*(q / 2 - y) + (p / 2 - x)*(p / 2 - x);
		 temp = -k * pow(temp, t);
		 filter = pow(Ei,temp);
		 if (type2 == 0) {
			 return filter;
		 }
		 else if (type2 == 1) {
			 if (D <= 110) {
				 return 1 / filter;
			 }
			 else
				 return 0;
		 }
		 else if (type2 == 2) {
			 //ά���˲�
			 int K = 0.002;
			 filter = filter / (filter*filter + K);
			 return filter;
		 }
	 }
	 else
		 return 0;
 }

 void CDib::Center_filter()
 {
	 // TODO: �ڴ˴����ʵ�ִ���.
	 int **pixel = new int*[(m_nHeight )];
     int **result = new int*[(m_nHeight)];
	 RGBTRIPLE* pRGB;
	 int temp;
	 int i, j;
	 for (int i = 0; i < (m_nHeight ); ++i) {
		 pixel[i] = new int[m_nWidthBytes ];
		 result[i] = new int[m_nWidthBytes];
	 }
	 if (m_nWidth != m_nWidthBytes) {
		 for (int color = 0; color < 3; ++color) {
			 for (i = 0; i < (m_nHeight); ++i) {
				 for (j = 0; j < (m_nWidth); ++j) {
					 pRGB = (RGBTRIPLE*)(m_pDibBits + (i)* m_nWidthBytes + j * 3);
					 if (color == 0) {
						 temp = pRGB->rgbtBlue;
					 }
					 if (color == 1) {
						 temp = pRGB->rgbtGreen;
					 }
					 if (color == 2) {
						 temp = pRGB->rgbtRed;
					 }
					 pixel[i][j] = temp;
					 result[i][j] = pixel[i][j];
				 }
			 }

			 for (i = 1; i < m_nHeight - 2; ++i) {
				 for (j = 1; j < m_nWidth - 2; ++j) {
					 int temp[9] = { pixel[i][j] , pixel[i][j + 1] , pixel[i][j + 2],
						 pixel[i + 1][j] , pixel[i + 1][j + 1] , pixel[i + 1][j + 2]
						 , pixel[i + 2][j] , pixel[i + 2][j + 1] , pixel[i + 2][j + 2] };
					 result[i][j] = Sort_center(temp);
				 }
			 }
			 for (i = 0; i < m_nHeight; ++i) {
				 for (j = 0; j < m_nWidth; ++j) {
					 pRGB = (RGBTRIPLE*)(m_pDibBits + (i)* m_nWidthBytes + j * 3);
					 if (color == 0) {
						 pRGB->rgbtBlue = result[i][j];
					 }
					 if (color == 1) {
						 pRGB->rgbtGreen = result[i][j];
					 }
					 if (color == 2) {
						 pRGB->rgbtRed = result[i][j];
					 }
					 //pRGB->rgbtGreen = result[i][j];
				 }

			 }
		 }
	 }
	 else {
		 for (i = 0; i < (m_nHeight); ++i) {
			 for (j = 0; j < (m_nWidth); ++j) {
				 //pRGB = (RGBTRIPLE*)(m_pDibBits + (i)* m_nWidthBytes + j * 3);
				 pixel[i][j] = *(m_pDibBits + (i)* m_nWidthBytes + j);
				 result[i][j] = pixel[i][j];
			 }
		 }

		 for (i = 1; i < m_nHeight - 2; ++i) {
			 for (j = 1; j < m_nWidth - 2; ++j) {
				 int temp[9] = { pixel[i][j] , pixel[i][j + 1] , pixel[i][j + 2],
					 pixel[i + 1][j] , pixel[i + 1][j + 1] , pixel[i + 1][j + 2]
					 , pixel[i + 2][j] , pixel[i + 2][j + 1] , pixel[i + 2][j + 2] };
				 result[i][j] = Sort_center(temp);
			 }
		 }
		 for (i = 0; i < m_nHeight; ++i) {
			 for (j = 0; j < m_nWidth; ++j) {
				 //pRGB = (RGBTRIPLE*)(m_pDibBits + (i)* m_nWidthBytes + j * 3);
				 *(m_pDibBits + (i)* m_nWidthBytes + j) = result[i][j];
			 }

		 }
	 }
	 for (i = 0; i < m_nHeight; ++i) {
		 delete[] result[i];
		 delete[] pixel[i];
	 }
	 delete[] pixel;
	 delete[] result;
 }

 int CDib::Sort_center(int x[9])
 {
	 // TODO: �ڴ˴����ʵ�ִ���.
	 int max[5] = {0};
	 int temp;
	 for (int i = 0; i < 5; ++i) {
		 for (int j = 0; j < 9; ++j) {
			 if (max[i]<x[j]) {
				 max[i] = x[j];
				 temp = j;
			 }
		 }
		 for (int j = 0; j < 9; ++j) {
			 if (x[j]==max[i]) {
				 x[j]=0;
				 break;
			 }
		 }
	 }
	 return max[4];
 }

 long* CDib::FBP()
 {
	 long* result=new long[256*256];
	 unsigned char*	lpSrc;							// ָ��Դͼ���ָ��
	 int y;										// ѭ�����Ʊ���
	 int x;										// ѭ�����Ʊ���
	 double dTmpOne;								//�����ʱ����
	 double dTmpTwo;								//�����ʱ����
	 int nTransWidth=m_nWidth;								// ����Ҷ�任�Ŀ�ȣ�2���������ݣ�
	 int nTransHeight;								// ����Ҷ�任�ĸ߶ȣ�2���������ݣ�
	 double unchValue;								// ����ͼ������ػҶȵ���ʱ����
	 complex<double> * pCTData;						// ָ��ʱ�����ݵ�ָ��
	 complex<double> * pCFData;						// ָ��Ƶ�����ݵ�ָ��
	 // ������и���Ҷ�任�ĵ��������� ��2���������ݣ�
	 dTmpOne = log(1.0*m_nHeight) / log(2.0);
	 dTmpTwo = ceil(dTmpOne);
	 dTmpTwo = pow(2, dTmpTwo);
	 nTransHeight = (int)dTmpTwo;
	 double dReal;									// ����Ҷ�任��ʵ��
	 double dImag;									// ����Ҷ�任���鲿

	 pCTData = new complex<double>[nTransWidth * nTransHeight];	// �����ڴ�
	 pCFData = new complex<double>[nTransWidth * nTransHeight];	// �����ڴ�
		// x��y�����У������ϵĵ�������
	 int		nYLev;

	 // ����x��y�����У������ϵĵ�������
	 nYLev = (int)(log(1.0*nTransHeight) / log(2.0) + 0.5);
	 // ͼ�����ݵĿ�͸߲�һ����2���������ݣ�����pCTData��һ����������Ҫ��0
	 for (y = 0; y < nTransHeight; y++)
	 {
		 for (x = 0; x < nTransWidth; x++)
		 {
			 pCTData[y*nTransWidth + x] = complex<double>(0, 0);		// ����
		 }
	 }
	 for (int y = 0; y < 256 * 256; y++)
		 result[y] = 0;
	 //��ͼ�����ݴ���pCTData
	 for (y = 0; y < m_nHeight; y++)
	 {
		 for (x = 0; x < m_nWidth; x++)
		 {
			 // ָ��DIB��y�У���x�����ص�ָ��
			 lpSrc = (unsigned char*)m_pDibBits + m_nWidth * (y) + x;
			 unchValue = (*lpSrc);
			 pCTData[x*nTransHeight + y] = complex<double>(unchValue, 0);
		 }
	 }
	 // ����Ҷ���任
	 for (x = 0; x < m_nWidth; ++x) {
		 FFT_1D(&pCTData[nTransHeight * x], &pCFData[nTransHeight * x], nYLev);
	 }

	 for (x = 0; x < m_nWidth; ++x) {
		 for (y = 0; y < nTransHeight; ++y) {
			 pCFData[x*nTransHeight + y] *= double(abs(nTransHeight / 2 - y));// / double(nTransHeight / 2);
		 }
		 
	 }
	 //IFFT_2D(pCFData, pCTData, m_nHeight, m_nWidth); 				// ����Ҷ���任
	 for (x = 0; x < m_nWidth; ++x) {
		 IFFT_1D(&pCFData[nTransHeight * x], &pCTData[nTransHeight * x], nYLev);
	 }

	 int t;
	 int min = 4000, max = 0;
	 double pixel = 0;
	 for (y = 0; y < 256; y++) {
		 for (x = 0; x < 256; x++) {
			 pixel = 0;
			 for (int w = 0; w < 180; w++) {
				 t = round((x-128) * cos(w) + (y-128) * sin(w));
				 dReal = pCTData[nTransHeight * w + int(t + m_nHeight / 2)].real();
				  pixel+= dReal;
			 }
			 pixel = pixel / 180;
			 result[y * 256 + x] = pixel;
			 if (pixel > max)
				 max = pixel;
			 if (pixel < min)
				 min = pixel;
		 }
	 }


	 delete pCTData;										// �ͷ��ڴ�
	 delete pCFData;										// �ͷ��ڴ�
	 pCTData = NULL;
	 pCFData = NULL;
	 return result;										//���ؽ��

 }

 void CDib::LoadFromTxtFile()
 {
	 // TODO: �ڴ˴����ʵ�ִ���.
	 streamsize strcount = 16;
	 char buffer[16];
	 string m;
	 double ndata, maxdata = 0;
	 ifstream out("C:\\Users\\ProZH\\Desktop\\�γ�\\ͼ����\\HW8\\Radon.txt");
	 //ifstream out("C:\\Users\\ProZH\\Desktop\\�γ�\\ͼ����\\HW8\\shepp-logan.txt");
	 if (out.is_open())
	 {
		 ////��ȡ���ݸ߶ȺͿ��
		 //out.seekg(0, ios::beg);
		 getline(out, m);
		 int linewords = m.length();
		 out.seekg(0, ios::end);
		 int totalwords = out.tellg();
		 int height = totalwords / linewords;
		 long width = m.length() / strcount;

		 ////����һ���ڴ��Ŷ���ĸ�����
		 double* m_pRawdata;
		 m_pRawdata = new double[width*height];
		 memset(m_pRawdata, 0.0, width*height);
		 int j = 0;
		 out.seekg(0, ios::beg);
		 while (getline(out, m))
		 {
			 for (int i = 0; i < width; i++)
			 {
				 m.copy(buffer, strcount, i * strcount);
				 ndata = atof(buffer);
				 *(m_pRawdata + j * width + i) = ndata;
				 if (maxdata < ndata)
				 {
					 maxdata = ndata;
				 }
			 }
			 j++;
		 }
		 out.close();

		 ////������ת������ʾ����
		 long BitCount = 8;
		 BOOL Issuccess = Create(width, height, BitCount, 0);
		 if (!Issuccess)
		 {
			 return;
		 }
		 if (IsIndexed())
		 {
			 int nColors = GetMaxColorTableEntries();
			 if (nColors > 0)
			 {
				 RGBQUAD *pal = new RGBQUAD[nColors];
				 for (int i = 0; i < nColors; i++)
				 {
					 pal[i].rgbBlue = i;
					 pal[i].rgbGreen = i;
					 pal[i].rgbRed = i;
				 }
				 SetColorTable(0, nColors, pal);
				 delete[] pal;
			 }
		 }
		 m_nWidth = GetWidth();
		 m_nHeight = GetHeight();
		 m_nWidthBytes = abs(GetPitch());
		 m_nBitCount = GetBPP();
		 m_pDibBits = (unsigned char*)GetBits() + (m_nHeight - 1)*GetPitch();
		 m_pGrayValueCount = NULL;

		 for (int j = 0; j < m_nHeight; j++)
		 {
			 for (int i = 0; i < m_nWidth; i++)
			 {
				 ndata = (*(m_pRawdata + j * width + i) / maxdata)*255.0;
				 *(m_pDibBits + j * m_nWidthBytes + i) = (BYTE)ndata;
			 }
		 }
		 delete m_pRawdata;
	 }
	 else
	 {
		 cout << "Error: opening file." << endl;
	 }
 }


 void CDib::blank(long* pixel)
 {
	 // TODO: �ڴ˴����ʵ�ִ���.
	 if (!IsNull())
	 {
		 Destroy();
	 }
	 Create(256, 256, 8, 0);
	 m_nWidth = GetWidth();
	 m_nHeight = GetHeight();
	 m_nWidthBytes = abs(GetPitch());
	 m_nBitCount = GetBPP();
	 m_pDibBits = (unsigned char*)GetBits() + (m_nHeight - 1)*GetPitch();
	 m_pGrayValueCount = NULL;
	 if (IsIndexed())
	 {
		 int nColors = 256;
		 RGBQUAD *pal = new RGBQUAD[nColors];
		 for (int i = 0; i < nColors; ++i) {
			 pal[i].rgbBlue = i;
			 pal[i].rgbRed = i;
			 pal[i].rgbGreen = i;
		 }
		 SetColorTable(0, nColors, pal);
		 delete[] pal;
	 }
	 for (int i = 0; i < m_nHeight; ++i) {
		 for (int j = 0; j < m_nWidth; ++j) {
			 *(m_pDibBits + (i)* m_nWidthBytes + (j)) = pixel[i*m_nWidth+j];
		 }
	 }
 }


 void CDib::background()
 {
	 // TODO: �ڴ˴����ʵ�ִ���.
	 if (!IsNull())
	 {
		 Destroy();
	 }
	 Create(512, 512, 24, 0);
	 m_nWidth = GetWidth();
	 m_nHeight = GetHeight();
	 m_nWidthBytes = abs(GetPitch());
	 m_nBitCount = GetBPP();
	 m_pDibBits = (unsigned char*)GetBits() + (m_nHeight - 1)*GetPitch();
	 if (IsIndexed())
	 {
		 for (int i = 0; i < m_nWidth; ++i) {
			 for (int j = 0; j < m_nHeight; ++j) {
				 RGBTRIPLE *pal = (RGBTRIPLE*)(m_pDibBits + (i) * m_nWidthBytes + j*3);
				 pal->rgbtBlue = 0;
				 pal->rgbtGreen = 0;
				 pal->rgbtRed = 0;
			 }
		 }
	 }
 }


 void CDib::R()
 {
	 // TODO: �ڴ˴����ʵ�ִ���.
	 //int W = 50, H = 50;
	 for (int i = 0; i < m_nWidth; ++i) {
		 for (int j = 0; j < m_nHeight; ++j) {
			 double r = sqrt((j - m_nHeight / 2)*(j - m_nHeight / 2) + (i - m_nWidth / 2)*(i - m_nWidth / 2)); 
			 RGBTRIPLE* pRGB = (RGBTRIPLE*)(m_pDibBits + (j+60) * m_nWidthBytes + (i)*3);
			 if (r < 100) {
				 pRGB->rgbtRed = 255;
			 }
		 }
	 }
 }


 void CDib::G()
 {
	 // TODO: �ڴ˴����ʵ�ִ���.
	 for (int i = 0; i < m_nWidth; ++i) {
		 for (int j = 0; j < m_nHeight; ++j) {
			 double r = sqrt((j - m_nHeight / 2)*(j - m_nHeight / 2) + (i - m_nWidth / 2)*(i - m_nWidth / 2));
			 //int r = (i - 212)*(i - 212) + (j - 292)*(j - 292);
			 RGBTRIPLE* pRGB = (RGBTRIPLE*)(m_pDibBits + (j -30) * m_nWidthBytes + i * 3+6*int(1.732*15));
			 if (r < 100) {
				 pRGB->rgbtGreen = 255;
			 }
		 }
	 }
 }


 void CDib::B()
 {
	 // TODO: �ڴ˴����ʵ�ִ���.
	 for (int i = 0; i < m_nWidth; ++i) {
		 for (int j = 0; j < m_nHeight; ++j) {
			 double r = sqrt((j - m_nHeight / 2)*(j - m_nHeight / 2) + (i - m_nWidth / 2)*(i - m_nWidth / 2));
			 //int r = (i - 212)*(i - 212) + (j - 292)*(j - 292);
			 RGBTRIPLE* pRGB = (RGBTRIPLE*)(m_pDibBits + (j - 30) * m_nWidthBytes + i * 3 - 6 * int(1.732 * 15));
			 if (r < 100) {
				 pRGB->rgbtBlue = 255;
			 }
		 }
	 }
 }


 void CDib::H()
 {
	 // TODO: �ڴ˴����ʵ�ִ���.
	 double r, g, b,temp,temp2;
	 double H;
	 for (int i = 0; i < m_nWidth; ++i) {
		 for (int j = 0; j < m_nHeight; ++j) {
			 RGBTRIPLE* pRGB = (RGBTRIPLE*)(m_pDibBits + (j)* m_nWidthBytes + i * 3);
			 r = double(pRGB->rgbtRed) / 255;
			 g = double(pRGB->rgbtGreen) / 255;
			 b = double(pRGB->rgbtBlue) / 255;
			 temp = (2 * r - g - b) / 2;
			 temp2 = (r - g)*(r - g) + (r - b)*(g - b);
			 temp2 = pow(temp2, 0.5);
			 if (temp2 != 0)
				 temp = acos(temp / temp2);
			 else
				 temp = 0;
			 if (b > g)
				 H = (2*Pi - temp)  / (2*Pi);
			 else
				 H = (temp) / (2 * Pi);
			 pRGB->rgbtBlue = int(H * 255);
			 pRGB->rgbtRed = int(H * 255);
			 pRGB->rgbtGreen = int(H * 255);
		 }
	 }
 }


 void CDib::S()
 {
	 // TODO: �ڴ˴����ʵ�ִ���.
	 for (int i = 0; i < m_nWidth; ++i) {
		 for (int j = 0; j < m_nHeight; ++j) {
			 RGBTRIPLE* pRGB = (RGBTRIPLE*)(m_pDibBits + (j)* m_nWidthBytes + i * 3);
			 int min = 500;
			 int tp[3] = { int(pRGB->rgbtRed) ,int(pRGB->rgbtGreen) ,int(pRGB->rgbtBlue)};
			 for (int t = 0; t < 3; ++t) {
				 if (min > tp[t])
					 min = tp[t];
			 }

			 long temp = min*255;
			 long I = int(pRGB->rgbtBlue) + int(pRGB->rgbtRed) + int(pRGB->rgbtGreen);
			 I = I / 3;
			 //temp = temp / (int(pRGB->rgbtBlue) + int(pRGB->rgbtRed) + int(pRGB->rgbtGreen));
			 if (I == 0)
				 temp = 0;
			 else
				 temp = 255-temp / I;
			 pRGB->rgbtBlue = temp;
			 pRGB->rgbtRed = temp ;
			 pRGB->rgbtGreen = temp ;
		 }
		 //int t = 0;
	 }
 }


 void CDib::I()
 {
	 // TODO: �ڴ˴����ʵ�ִ���.
	 for (int i = 0; i < m_nWidth; ++i) {
		 for (int j = 0; j < m_nHeight; ++j) {
			 RGBTRIPLE* pRGB = (RGBTRIPLE*)(m_pDibBits + (j) * m_nWidthBytes + i * 3);
			 long I = int(pRGB->rgbtBlue) + int(pRGB->rgbtRed) + int(pRGB->rgbtGreen);
			 I = I / 3;
			 pRGB->rgbtBlue = I;
			 pRGB->rgbtRed = I;
			 pRGB->rgbtGreen = I;
		 }
	 }
 }


 long* CDib::Large()
 {
	 // TODO: �ڴ˴����ʵ�ִ���.
	 long W = m_nWidth * 1.2;
	 long H = m_nHeight * 1.2;
	 int x, y;
	 long *pixel = new long[long(m_nHeight*1.2)*long(m_nWidth*1.2)];
	 for (y = 0; y < H; ++y) {
		 for (x = 0; x < W; ++x) {
			 double y0 = double(y) / 1.2;
			 double x0 = double(x) / 1.2;
			 double temp1 = (1 - y0 + int(y0))*(*((unsigned char*)m_pDibBits + m_nWidth * int(y0) + int(x0))) + (y0 - int(y0))*(*((unsigned char*)m_pDibBits + m_nWidth * (int(y0) + 1) + int(x0)));
			 double temp2 = (1 - y0 + int(y0))*(*((unsigned char*)m_pDibBits + m_nWidth * int(y0) + int(x0) + 1)) + (y0 - int(y0))*(*((unsigned char*)m_pDibBits + m_nWidth * (int(y0) + 1) + int(x0) + 1));
			 pixel[y*W + x] = (1 - x0 + int(x0))*(temp1)+(x0 - int(x0))*temp2;
		 }
	 }
	 return pixel;
 }


 long* CDib::Small()
 {
	 // TODO: �ڴ˴����ʵ�ִ���.
	 long W = m_nWidth * 0.8;
	 long H = m_nHeight * 0.8;
	 int x, y;
	 long *pixel = new long[long(m_nHeight*0.8)*long(m_nWidth*0.8)];
	 for (y = 0; y < H; ++y) {
		 for (x = 0; x < W; ++x) {
			 double y0 = double(y) / 0.8;
			 double x0 = double(x) / 0.8;
			 double temp1 = (1 - y0 + int(y0))*(*((unsigned char*)m_pDibBits + m_nWidth * int(y0) + int(x0))) + (y0 - int(y0))*(*((unsigned char*)m_pDibBits + m_nWidth * (int(y0) + 1) + int(x0)));
			 double temp2 = (1 - y0 + int(y0))*(*((unsigned char*)m_pDibBits + m_nWidth * int(y0) + int(x0) + 1)) + (y0 - int(y0))*(*((unsigned char*)m_pDibBits + m_nWidth * (int(y0) + 1) + int(x0) + 1));
			 pixel[y*W + x] = (1 - x0 + int(x0))*(temp1)+(x0 - int(x0))*temp2;
		 }
	 }
	 
	 return pixel;
 }


 void CDib::init(int width, int height, long* pixel)
 {
	 // TODO: �ڴ˴����ʵ�ִ���.
	 if (!IsNull())
	 {
		 Destroy();
	 }
	 Create(width, height, 8, 0);
	 m_nWidth = GetWidth();
	 m_nHeight = GetHeight();
	 m_nWidthBytes = abs(GetPitch());
	 m_nBitCount = GetBPP();
	 m_pDibBits = (unsigned char*)GetBits() + (m_nHeight - 1)*GetPitch();
	 m_pGrayValueCount = NULL;
	 if (IsIndexed())
	 {
		 int nColors = 256;
		 RGBQUAD *pal = new RGBQUAD[nColors];
		 for (int i = 0; i < nColors; ++i) {
			 pal[i].rgbBlue = i;
			 pal[i].rgbRed = i;
			 pal[i].rgbGreen = i;
		 }
		 SetColorTable(0, nColors, pal);
		 delete[] pal;
	 }
	 for (int i = 0; i < m_nHeight; ++i) {
		 for (int j = 0; j < m_nWidth; ++j) {
			 *(m_pDibBits + (i)* m_nWidthBytes + (j)) = pixel[i*m_nWidth + j];
		 }
	 }
 }




 void CDib::binaryzation()
 {
	 // TODO: �ڴ˴����ʵ�ִ���.
	 if (m_nWidth != m_nWidthBytes) {
		 for (int i = 0; i < m_nWidth; ++i) {
			 for (int j = 0; j < m_nHeight; ++j) {
				 RGBTRIPLE* pRGB = (RGBTRIPLE*)(m_pDibBits + (j)* m_nWidthBytes + i * 3);
				 long I = int(pRGB->rgbtBlue) + int(pRGB->rgbtRed) + int(pRGB->rgbtGreen);
				 I = I / 3;
				 if (I < 218) {
					 pRGB->rgbtBlue = 0;
					 pRGB->rgbtRed = 0;
					 pRGB->rgbtGreen = 0;
				 }
				 else {
					 pRGB->rgbtBlue = 0xff;
					 pRGB->rgbtRed = 0xff;
					 pRGB->rgbtGreen = 0xff;
				 }
			 }
		 }
	 }
 }


 void CDib::complementary()
 {
	 // TODO: �ڴ˴����ʵ�ִ���.
	 if (m_nWidth != m_nWidthBytes) {
		 for (int i = 0; i < m_nWidth; ++i) {
			 for (int j = 0; j < m_nHeight; ++j) {
				 RGBTRIPLE* pRGB = (RGBTRIPLE*)(m_pDibBits + (j)* m_nWidthBytes + i * 3);
				 //long I = int(pRGB->rgbtBlue) + int(pRGB->rgbtRed) + int(pRGB->rgbtGreen);
				 //I = I / 3;
				 pRGB->rgbtBlue = 0xff- pRGB->rgbtBlue;
				 pRGB->rgbtRed = 0xff- pRGB->rgbtRed;
				 pRGB->rgbtGreen = 0xff-pRGB->rgbtGreen;
				 
			 }
		 }
	 }
 }


void CDib::Erosion()
 {
	 // TODO: �ڴ˴����ʵ�ִ���.
	 int *pixel = new int[m_nHeight*m_nWidth];
	 //int kernel[9] = { 1,1,1,1,1,1,1,1,1 };
	 for (int i = 0; i < m_nWidth; ++i) {
		 for (int j = 0; j < m_nHeight; ++j) {
			 RGBTRIPLE* pRGB = (RGBTRIPLE*)(m_pDibBits + (j)* m_nWidthBytes + i * 3);
			 pixel[j*m_nWidth + i] = pRGB->rgbtBlue;
		 }
	 }
	 for (int i = 1; i < m_nWidth-1; ++i) {
		 for (int j = 1; j < m_nHeight-1; ++j) {
			 RGBTRIPLE* pRGB = (RGBTRIPLE*)(m_pDibBits + (j)* m_nWidthBytes + i * 3);
			 if (pixel[(j-1)*m_nWidth + i-1] == 255 && pixel[(j-1)*m_nWidth + i] == 255 && pixel[(j-1)*m_nWidth + i+1] == 255 && pixel[j*m_nWidth + i-1] == 255 && pixel[j*m_nWidth + i] == 255 && pixel[j*m_nWidth + i+1] == 255 && pixel[(j+1)*m_nWidth + i-1] == 255 && pixel[(j+1)*m_nWidth + i] == 255 && pixel[(j+1)*m_nWidth + i+1] == 255 ) {
				 pRGB->rgbtBlue = 0xff;
				 pRGB->rgbtRed = 0xff;
				 pRGB->rgbtGreen = 0xff;
			 }
			 else {
				 pRGB->rgbtBlue = 0;
				 pRGB->rgbtRed = 0;
				 pRGB->rgbtGreen = 0;
			 }
		 }
	 }
	 delete pixel;
 }


void CDib::expand()
{
	// TODO: �ڴ˴����ʵ�ִ���.
	int *pixel = new int[m_nHeight*m_nWidth];
	for (int i = 0; i < m_nWidth; ++i) {
		for (int j = 0; j < m_nHeight; ++j) {
			RGBTRIPLE* pRGB = (RGBTRIPLE*)(m_pDibBits + (j)* m_nWidthBytes + i * 3);
			pixel[j*m_nWidth + i] = pRGB->rgbtBlue;
			if (j == 0 || j == m_nHeight - 1 || i == 0 || i == m_nWidth - 1)
				pixel[j*m_nWidth + i] = 0;
		}
	}
	for (int i = 1; i < m_nWidth - 1; ++i) {
		for (int j = 1; j < m_nHeight - 1; ++j) {
			RGBTRIPLE* pRGB = (RGBTRIPLE*)(m_pDibBits + (j)* m_nWidthBytes + i * 3);
			if (pixel[(j - 1)*m_nWidth + i - 1] == 255 || pixel[(j - 1)*m_nWidth + i] == 255 || pixel[(j - 1)*m_nWidth + i + 1] == 255 || pixel[j*m_nWidth + i - 1] == 255 || pixel[j*m_nWidth + i] == 255 || pixel[j*m_nWidth + i + 1] == 255 || pixel[(j + 1)*m_nWidth + i - 1] == 255 || pixel[(j + 1)*m_nWidth + i] == 255 && pixel[(j + 1)*m_nWidth + i + 1] == 255) {
				pRGB->rgbtBlue = 0xff;
				pRGB->rgbtRed = 0xff;
				pRGB->rgbtGreen = 0xff;
			}
			else {
				pRGB->rgbtBlue = 0;
				pRGB->rgbtRed = 0;
				pRGB->rgbtGreen = 0;
			}
		}
	}
	delete pixel;
}



void CDib::fill()
{
	// TODO: �ڴ˴����ʵ�ִ���.
	int *mask = new int[m_nHeight*m_nWidth];
	int *result = new int[m_nHeight*m_nWidth];
	int *pixel = new int[m_nHeight*m_nWidth];
	for (int i = 0; i < m_nWidth; ++i) {
		for (int j = 0; j < m_nHeight; ++j) {
			RGBTRIPLE* pRGB = (RGBTRIPLE*)(m_pDibBits + (j)* m_nWidthBytes + i * 3);
			mask[j*m_nWidth + i] = 0xff - pRGB->rgbtBlue;
			result[j*m_nWidth + i] = 0;
			pixel[j*m_nWidth + i] = 0;
			if (j == 0 || j == m_nHeight - 1 || i == 0 || i == m_nWidth - 1) {
				if (pRGB->rgbtBlue == 0) {
					result[j*m_nWidth + i] = 0xff;
					pixel[j*m_nWidth + i] = 0xff;
				}

			}
		}
	}
	bool finish = false;
	while (finish == false) {
		for (int i = 1; i < m_nWidth - 1; ++i) {
			for (int j = 1; j < m_nHeight - 1; ++j) {
				//RGBTRIPLE* pRGB = (RGBTRIPLE*)(m_pDibBits + (j)* m_nWidthBytes + i * 3);
				if (result[(j - 1)*m_nWidth + i - 1] == 255 || result[(j - 1)*m_nWidth + i] == 255 || result[(j - 1)*m_nWidth + i + 1] == 255 || result[j*m_nWidth + i - 1] == 255 || result[j*m_nWidth + i] == 255 || result[j*m_nWidth + i + 1] == 255 || result[(j + 1)*m_nWidth + i - 1] == 255 || result[(j + 1)*m_nWidth + i] == 255 && result[(j + 1)*m_nWidth + i + 1] == 255) {
					pixel[j*m_nWidth + i] = 0xff;
				}
				else {
					pixel[j*m_nWidth + i] = 0;
				}
			}
		}
		finish = true;
		for (int i = 1; i < m_nWidth - 1; ++i) {
			for (int j = 1; j < m_nHeight - 1; ++j) {
				if (pixel[j*m_nWidth + i] != mask[j*m_nWidth + i]) {
					pixel[j*m_nWidth + i] = 0;
					//finish = false;
				}
			}
		}
		for (int i = 1; i < m_nWidth - 1; ++i) {
			for (int j = 1; j < m_nHeight - 1; ++j) {
				if (pixel[j*m_nWidth + i] != result[j*m_nWidth + i]) {
					result[j*m_nWidth + i] = pixel[j*m_nWidth + i];
					finish = false;
				}
			}
		}
	}
	for (int i = 1; i < m_nWidth - 1; ++i) {
		for (int j = 1; j < m_nHeight - 1; ++j) {
			RGBTRIPLE* pRGB = (RGBTRIPLE*)(m_pDibBits + (j)* m_nWidthBytes + i * 3);
			pRGB->rgbtBlue =0xff-result[j*m_nWidth + i];
			pRGB->rgbtRed = 0xff - result[j*m_nWidth + i];
			pRGB->rgbtGreen = 0xff - result[j*m_nWidth + i];
		}
	}
	delete pixel;
	delete result;
	delete mask;
}



void CDib::outline()
{
	// TODO: �ڴ˴����ʵ�ִ���.
	int *pixel = new int[m_nHeight*m_nWidth];
	int *result = new int[m_nHeight*m_nWidth];
	//int kernel[9] = { 1,1,1,1,1,1,1,1,1 };
	for (int i = 0; i < m_nWidth; ++i) {
		for (int j = 0; j < m_nHeight; ++j) {
			RGBTRIPLE* pRGB = (RGBTRIPLE*)(m_pDibBits + (j)* m_nWidthBytes + i * 3);
			pixel[j*m_nWidth + i] = pRGB->rgbtBlue;
			result[j*m_nWidth + i] = pRGB->rgbtBlue;
		}
	}
	for (int i = 1; i < m_nWidth - 1; ++i) {
		for (int j = 1; j < m_nHeight - 1; ++j) {
			//RGBTRIPLE* pRGB = (RGBTRIPLE*)(m_pDibBits + (j)* m_nWidthBytes + i * 3);
			if (pixel[(j - 1)*m_nWidth + i - 1] == 255 && pixel[(j - 1)*m_nWidth + i] == 255 && pixel[(j - 1)*m_nWidth + i + 1] == 255 && pixel[j*m_nWidth + i - 1] == 255 && pixel[j*m_nWidth + i] == 255 && pixel[j*m_nWidth + i + 1] == 255 && pixel[(j + 1)*m_nWidth + i - 1] == 255 && pixel[(j + 1)*m_nWidth + i] == 255 && pixel[(j + 1)*m_nWidth + i + 1] == 255) {
				result[j*m_nWidth + i] = 0xff;
				
			}
			else {
				result[j*m_nWidth + i] = 0;
			
			}
		}
	}
	for (int i = 0; i < m_nWidth; ++i) {
		for (int j = 0; j < m_nHeight; ++j) {
			RGBTRIPLE* pRGB = (RGBTRIPLE*)(m_pDibBits + (j)* m_nWidthBytes + i * 3);
			pRGB->rgbtBlue = pixel[j*m_nWidth + i]- result[j*m_nWidth + i];
			pRGB->rgbtRed = pixel[j*m_nWidth + i] - result[j*m_nWidth + i];
			pRGB->rgbtGreen = pixel[j*m_nWidth + i] - result[j*m_nWidth + i];
		}
	}
	delete pixel;
	delete result;
}


int* CDib::DrawLine()
{
	// TODO: �ڴ˴����ʵ�ִ���.
	int *pixel = new int[m_nHeight*m_nWidth];
	for (int i = 0; i < m_nWidth; ++i) {
		for (int j = 0; j < m_nHeight; ++j) {
			RGBTRIPLE* pRGB = (RGBTRIPLE*)(m_pDibBits + (j)* m_nWidthBytes + i * 3);
			pixel[j*m_nWidth + i] = pRGB->rgbtBlue;
		}
	}
	return pixel;
}


void CDib::DrawLine2(int* line)
{
	// TODO: �ڴ˴����ʵ�ִ���.
	for (int i = 0; i < m_nWidth; ++i) {
		for (int j = 0; j < m_nHeight; ++j) {
			RGBTRIPLE* pRGB = (RGBTRIPLE*)(m_pDibBits + (j)* m_nWidthBytes + i * 3);
			if (line[j*m_nWidth + i] == 0xff) {
				pRGB->rgbtBlue = 0;
				pRGB->rgbtRed = 0;
				pRGB->rgbtGreen = 0;
			}
		}
	}
}

void CDib::Haar()
{
	// TODO: �ڴ˴����ʵ�ִ���. 
	double h[2] = { 0.5,0.5 };
	double g[2] = { 0.5,-0.5 };
	double* m_temp = new double[m_nWidth*m_nHeight];
	double *temp = new double[m_nWidth];
	int x, y;
	for (y = 0; y < m_nHeight; ++y) {
		for (x = 0; x < m_nWidth/2; ++x) {
			m_temp[(y)* m_nWidthBytes + x] = *(m_pDibBits + (y)* m_nWidthBytes + 2 * x)*h[1] + *(m_pDibBits + (y)* m_nWidthBytes + 2 * x + 1)*h[0];
			m_temp[(y)* m_nWidthBytes + x + m_nWidth / 2] = *(m_pDibBits + (y)* m_nWidthBytes + 2 * x)*g[1] + *(m_pDibBits + (y)* m_nWidthBytes + 2 * x + 1)*g[0];
		}
	}
	for (x = 0; x < m_nWidth; ++x) {
		for (y = 0; y < m_nHeight / 2; ++y) {
			*(m_pDibBits + (y)* m_nWidthBytes + x) = m_temp[(y * 2)* m_nWidth + x] * h[1] + m_temp[(y * 2 + 1)* m_nWidth + x] * h[0];
			*(m_pDibBits + (y + m_nHeight / 2)* m_nWidthBytes + x) = m_temp[(y * 2)* m_nWidth + x] * g[1] + m_temp[(y * 2 + 1)* m_nWidth + x] * g[0];
		}
	}
	for (y = 0; y < m_nHeight/2; ++y) {
		for (x = 0; x < m_nWidth / 4; ++x) {
			m_temp[(y)* m_nWidthBytes + x] = *(m_pDibBits + (y)* m_nWidthBytes + 2 * x)*h[1] + *(m_pDibBits + (y)* m_nWidthBytes + 2 * x + 1)*h[0];
			m_temp[(y)* m_nWidthBytes + x + m_nWidth / 4] = *(m_pDibBits + (y)* m_nWidthBytes + 2 * x)*g[1] + *(m_pDibBits + (y)* m_nWidthBytes + 2 * x + 1)*g[0];
		}
	}
	for (x = 0; x < m_nWidth/2; ++x) {
		for (y = 0; y < m_nHeight / 4; ++y) {
			*(m_pDibBits + (y)* m_nWidthBytes + x) = m_temp[(y * 2)* m_nWidth + x] * h[1] + m_temp[(y * 2 + 1)* m_nWidth + x] * h[0];
			*(m_pDibBits + (y + m_nHeight / 4)* m_nWidthBytes + x) = m_temp[(y * 2)* m_nWidth + x] * g[1] + m_temp[(y * 2 + 1)* m_nWidth + x] * g[0];
		}
	}

	delete temp;
	delete m_temp;
}




void CDib::Daubechies()
{
	// TODO: �ڴ˴����ʵ�ִ���.
	double h[4] = { (1+sqrt(3))/(4*sqrt(2)),(3 + sqrt(3)) / (4 * sqrt(2)),(3 - sqrt(3)) / (4 * sqrt(2)) ,(1 - sqrt(3)) / (4 * sqrt(2)) };
	double g[4] = { -h[3],-h[2],h[1],-h[0] };
	double* m_temp = new double[(m_nWidth+2)*(m_nHeight+2)];
	double *temp = new double[m_nWidth+2];
	int x, y;
	for (y = 0; y < m_nHeight; ++y) {
		for (x = 0; x < m_nWidth / 2; ++x) {
			m_temp[(y)* m_nWidthBytes + x] = *(m_pDibBits + (y)* m_nWidthBytes + 2 * x)*h[1] + *(m_pDibBits + (y)* m_nWidthBytes + 2 * x + 1)*h[0];
			m_temp[(y)* m_nWidthBytes + x + m_nWidth / 2] = *(m_pDibBits + (y)* m_nWidthBytes + 2 * x)*g[1] + *(m_pDibBits + (y)* m_nWidthBytes + 2 * x + 1)*g[0];
		}
	}
	for (x = 0; x < m_nWidth; ++x) {
		for (y = 0; y < m_nHeight / 2; ++y) {
			*(m_pDibBits + (y)* m_nWidthBytes + x) = m_temp[(y * 2)* m_nWidth + x] * h[1] + m_temp[(y * 2 + 1)* m_nWidth + x] * h[0];
			*(m_pDibBits + (y + m_nHeight / 2)* m_nWidthBytes + x) = m_temp[(y * 2)* m_nWidth + x] * g[1] + m_temp[(y * 2 + 1)* m_nWidth + x] * g[0];
		}
	}
	for (y = 0; y < m_nHeight / 2; ++y) {
		for (x = 0; x < m_nWidth / 4; ++x) {
			m_temp[(y)* m_nWidthBytes + x] = *(m_pDibBits + (y)* m_nWidthBytes + 2 * x)*h[1] + *(m_pDibBits + (y)* m_nWidthBytes + 2 * x + 1)*h[0];
			m_temp[(y)* m_nWidthBytes + x + m_nWidth / 4] = *(m_pDibBits + (y)* m_nWidthBytes + 2 * x)*g[1] + *(m_pDibBits + (y)* m_nWidthBytes + 2 * x + 1)*g[0];
		}
	}
	for (x = 0; x < m_nWidth / 2; ++x) {
		for (y = 0; y < m_nHeight / 4; ++y) {
			*(m_pDibBits + (y)* m_nWidthBytes + x) = m_temp[(y * 2)* m_nWidth + x] * h[1] + m_temp[(y * 2 + 1)* m_nWidth + x] * h[0];
			*(m_pDibBits + (y + m_nHeight / 4)* m_nWidthBytes + x) = m_temp[(y * 2)* m_nWidth + x] * g[1] + m_temp[(y * 2 + 1)* m_nWidth + x] * g[0];
		}
	}

	delete temp;
	delete m_temp;
}
