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
	 int nColor = GetMaxColorTableEntries();
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
	 // TODO: �ڴ˴����ʵ�ִ���.3*3
	 int** pixel = AddZeros();
	 int** result;
	 int laplace[9] = { 1,1,1,1,1,1,1,1,1 };

	 result = Conv(pixel, laplace);

	 for (int i = 0; i < m_nHeight; ++i) {
		 for (int j = 0; j < m_nWidthBytes; ++j) {
			 *(m_pDibBits + i * m_nWidthBytes + j) = result[i][j]/9;

		 }
		 delete[] pixel[i];
		 delete[] result[i];
	 }
	 delete[] pixel;
	 delete[] result;
 }



 void CDib::Sobel()
 {
	 int** pixel = AddZeros();
	 int** result1;
	 int** result2;
	 int Sobel1[9] = { -1,-2,-1,0,0,0,1,2,1 };
	 int Sobel2[9] = { -1,0,1,-2,0,2,-1,0,1 };

	 result1 = Conv(pixel, Sobel1);
	 result2 = Conv(pixel, Sobel2);


	 for (int i = 0; i < m_nHeight; ++i) {
		 for (int j = 0; j < m_nWidthBytes; ++j) {
			 *(m_pDibBits + i * m_nWidthBytes + j) = abs(result1[i][j])+abs(result2[i][j]);
			 
		 }
		 delete[] pixel[i];
		 delete[] result1[i];
		 delete[] result2[i];
	 }
	 delete[] pixel;
	 delete[] result1;
	 delete[] result2;
 }


 void CDib::Laplace()
 {
	 int** pixel = AddZeros();
	 int** result;
	 int laplace[9] = { 0,1,0,1,-4,1,0,1,0 };

	 result = Conv(pixel, laplace);

	 for (int i = 0; i < m_nHeight; ++i) {
		 for (int j = 0; j < m_nWidthBytes; ++j) {
			 *(m_pDibBits + i * m_nWidthBytes + j) = result[i][j];

		 }
		 delete[] pixel[i];
		 delete[] result[i];
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
	 m_nWidth = 512;
	 m_nHeight = 512;
	 m_nWidthBytes = 512;
	 m_nBitCount = 8;
	 Create(512, 512, 8, 0);
	 m_pDibBits = (unsigned char*)GetBits() + (m_nHeight - 1)*GetPitch();
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
	 int *pixel=new int[m_nWidth*m_nHeight];
	 long max = 0;
	 for (y = 0; y < m_nHeight; y++)								// ���任�����ݴ���lpDIBBits
	 {
		 for (x = 0; x < m_nWidth; x++)
		 {
			 //��Ҫ�����źŵ����������Լ�ʵ�����õ�ͼ�������ǻҶ�ֵ����ԭʼ����
			 dReal = pCTData[y*nTransWidth + x].real();		// ʵ��
			 dImag = pCTData[y*nTransWidth + x].imag();		// �鲿
			 
			 pixel[y*m_nWidth+x] = sqrt(dReal*dReal + dImag* dImag);
			 if (max < sqrt(dReal*dReal + dImag * dImag))
				 max = sqrt(dReal*dReal + dImag * dImag);
			 // ָ��DIB��y�У���x�����ص�ָ��
			 //lpSrc = (unsigned char*)m_pDibBits + m_nWidth * (m_nHeight - 1 - y) + x;
			 //*lpSrc = (BYTE)unchValue
		 }
	 }
	 for (int i = 0; i < m_nHeight; ++i) {
		 for (int j = 0; j < m_nWidthBytes; ++j) {
			 *(m_pDibBits + (i)* m_nWidthBytes + (j)) = pixel[i*m_nWidthBytes + j]*255 / max;
		 }
	 }
	 delete pCTData;										// �ͷ��ڴ�
	 delete pCFData;										// �ͷ��ڴ�
	 pCTData = NULL;
	 pCFData = NULL;
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



 int** CDib::AddZeros()
 {
	 // TODO: �ڴ˴����ʵ�ִ���.
	 int **pixel = new int*[(m_nHeight + 2)];
	 for (int i = 0; i < (m_nHeight + 2); ++i) {
		 pixel[i] = new int[m_nWidthBytes + 2];
	 }
	 for (int i = 0; i < (m_nHeight + 2); ++i) {
		 for (int j = 0; j < (m_nWidthBytes+2); ++j) {
			 if (i<1 || i>(m_nWidthBytes - 2) || j<1 || j>(m_nHeight  - 2))
				 pixel[i][j] = 0;
			 else {
				 pixel[i][j] = *(m_pDibBits + (i - 1)*m_nWidthBytes + (j - 1));
			 }
		 }
	 }
	 return pixel;
 }
