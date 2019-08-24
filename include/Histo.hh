#pragma once
#if !defined __HISTO_HH
#define __HISTO_HH

#define  MAXDATACELLS     100
#include <vector>

class ST1dHistogramm
{
   private:
	   double              dLeft;
	   double              dRight;
	   double              dStep;
	   std::vector<double> vData;

   public:
	    ST1dHistogramm(char*);
	    ST1dHistogramm(double l,double r,int n=MAXDATACELLS);
	   ~ST1dHistogramm();

	   //Add event to histogramm  
	   void   AddEvent(double value, double increment = 1.);
	   //service function to increment element defined by index
	   void   AddEvent(int    n,     double increment);
	   void   AddHistogramm(ST1dHistogramm* hist);


       void   Reset();
       double GetCell(int n);
       double GetCell(double vx);
       double GetCellCenter(int n);
	   double GetMaxValue();
	   void   SaveData(const char*, int n = 1);      //Save with normalization

       inline int Length(){return vData.size();}

       double GetSumm();                       //Get total signal in histogramm
	   // Get Center of gravity, Get RMS relative to CG, MAX and MIN valie in histogram.
	   void   GetStat(double* center, double* rms, double* maxval, double* minval); 
};

class ST2dHistogramm
{
   private:
	   int            nXCells;
	   double         dX1;
	   double         dX2;
	   double         dXStep;

	   int            nYCells;
	   double         dY1;
	   double         dY2;
	   double         dYStep;

       double*        dData;


   public:
	    ST2dHistogramm(char*);
	    ST2dHistogramm(double l,double r,int nX, double b,double t,int ny);
	   ~ST2dHistogramm();

 	   void   Reset();
	   void   AddEvent(double x, double y, double increment = 1);
	   void   AddEvent(int    i, int    j, double increment = 1);
	   double GetCell(int n, int m);
	   double GetCell(double vx, double vy);
	   double GetMaxValue(double dInitValue = 1.0);
	   void   SaveData(const char*, int n = 1); //save with normalization
       int    XLength(){return nXCells;}
       int    YLength(){return nYCells;}
	   void   GetStat(double* mean, double* rms, double* maxval, double* minval); //Get statistics in central area

	   ST1dHistogramm* Get1D_RMS(int dir); 
	   ST1dHistogramm* Get1D_Mean(int dir);

};

class ST3dHistogramm
{
   private:
	   int              nXCells;
	   int              nYCells;
	   int              nZCells;
	   double           dX1, dX2, dXStep;
	   double           dY1, dY2, dYStep;
	   double           dZ1, dZ2, dZStep;

	   ST2dHistogramm** H2Stack;


   public:

	    ST3dHistogramm(double x1,double x2, int nX, double y1, double y2, int ny, double z1, double z2, int nz);
	   ~ST3dHistogramm();

 	   void   Reset();
	   void   AddEvent(double x, double y, double z, double increment = 1);
	   void   AddEvent(int    i, int    j, int    k, double increment = 1);
	   double GetCell (int i, int j, int k);
	   double GetCell (double x, double y, double z);
       double GetMaxValue(double dInitValue = 1.0);
	   void   SaveData(const char*, int n =1); //save with normalization
	   void   SaveValuesHistogram(char*, int nBins);

	   ST1dHistogramm* Get1D(int dir); //desired direction
	   ST2dHistogramm* Get2D(int dir); //removed direction

};


#endif