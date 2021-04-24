#pragma once
#include "atlimage.h"

#include <complex>
using namespace std;

#define  MAX_SIZE 1000
const int Smoth_Kernel[9] = {1,1,1,1,1,1,1,1,1};
#define Pi 3.1415926535897932354626
#define Ei 2.71828

class CDib :
	public CImage
{
public:
	CDib(void);
	CDib(CDib &Dib);
	~CDib(void);

public:
	enum chan_color {chan_red=1,chan_green,chan_blue,chan_black};
	enum HSIChannel{HUE=0,SATURATION,INTENSITY};
	enum type{L_Ideal,L_BW,L_Gauss,H_Ideal,H_BW,H_Gauss, Tur_D,Anti_F,Wiener};
public:
	void LoadFile(LPCTSTR lpszPathName);

public:
	void Invert();

public:
	long* GrayValueCount();

public:
	BOOL FFT(unsigned char*  , long, long);
	void FFT_2D(complex<double> * , int, int , complex<double> * );
	void FFT_1D(complex<double> * , complex<double> * , int);
	void IFFT_2D(complex<double> * , complex<double> * , int, int);
	void IFFT_1D(complex<double> *, complex<double> *, int);

private:
	long m_nWidth;
	long m_nHeight;
	int m_nWidthBytes;
	int m_nBitCount;
	unsigned char *m_pDibBits;
	
private:
	long *m_pGrayValueCount;
public:
	void Red();
	void Green();
	void Blue();
	void Window(int min, int max);
	void HistogramEqualization();
	void smooth();
	void Sobel();
	void Laplace();
	void White();
	void FFT_VIEW();
	int** Conv(int** pixel,int kernel[9]);
	int** AddZeros();
	void Filtering(enum type t, bool pad);
	double Filter(int type1, int type2, int p, int q,int x,int y);
	void Center_filter();
	int Sort_center(int x[9]);
};

