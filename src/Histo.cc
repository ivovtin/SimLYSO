#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <string.h>

#include "Histo.hh"
#include <algorithm>    // std::fill
#include <numeric>      // std::accumulate


ST1dHistogramm::ST1dHistogramm(double l, double r, int n)
{
	dLeft=l;
	dRight=r;

	dStep = (dRight-dLeft)/n;
	vData.resize(n);

	//reset the data
	Reset();
}

ST1dHistogramm::ST1dHistogramm(char* name)
{
   double x, value;
   char   buffer[512];
   char   buf[256];
   char   seps[]=", ";	  //separatoren

   FILE* in = fopen(name,"r");
   if(in)
   {
	    //read caption and allocate buffers
	    fgets(buffer, sizeof(buffer), in);
        strcpy(buf,strtok(buffer,seps));
        dLeft = atof(buf);
        strcpy(buf,strtok(NULL,  seps));
        dRight = atof(buf);
        strcpy(buf,strtok(NULL,  seps));
        int nCells = atoi(buf);
	    dStep = (dRight-dLeft)/nCells;
		vData.resize(nCells);
 	    Reset();

		for(int i=0; i<nCells; ++i)
		{
		    fgets(buffer, sizeof(buffer),in);

            strcpy(buf,strtok(buffer,seps));
            x = atof(buf);
            
			strcpy(buf,strtok(NULL,  seps));
            value = atof(buf);

			AddEvent(x, value);
		}

        fclose(in);
   }  
}


ST1dHistogramm::~ST1dHistogramm()
{
}



void
ST1dHistogramm::AddEvent(double value, double increment)
{
    int index = static_cast<int>((value-dLeft)/dStep);
    AddEvent( index, increment);
}

void
ST1dHistogramm::AddEvent(int index, double increment)
{
    if(index<=0)index=0;
    if(index>=Length()) index=Length()-1;
    vData[index]+=increment;
}

double 
ST1dHistogramm::GetCell(int index)
{
    if(index<=0)index=0;
    if(index>=Length())index=Length()-1;
    return vData[index];
}

double 
ST1dHistogramm::GetCell(double vx)
{
    int index = static_cast<int>((vx-dLeft)/dStep);
    return GetCell(index);
}

double 
ST1dHistogramm::GetCellCenter(int index)
{
	double dRes = dLeft + (index + 0.5) * dStep;
	return dRes;
}


void   
ST1dHistogramm::AddHistogramm(ST1dHistogramm* hist)
{
  if(hist->Length()!=this->Length()) return;
  for(int i=0; i<Length(); ++i)vData[i]+=hist->GetCell(i);
}


void
ST1dHistogramm::Reset()
{
	std::fill(vData.begin(), vData.end(), 0.);
}

void
ST1dHistogramm::SaveData(const char* name, int numEvents)
{
  //sanity check
  if(numEvents==0)return;

  FILE* out = fopen(name,"w");
  if(out)
  {
     //fprintf(out,"%f %f %d\n", dLeft, dRight, Length());
     for(int i=0; i<Length(); ++i)
	 {
		 fprintf(out,"%f %E\n",dLeft+dStep*i, vData[i]/numEvents);
	 }
     fclose(out);
  }
}

double 
ST1dHistogramm::GetSumm()
{
  double sum_of_elems = std::accumulate(vData.begin(), vData.end(), 0.0f);
  return sum_of_elems;
 }

double 
ST1dHistogramm::GetMaxValue()
{

  double dMax = *std::max_element(vData.begin(), vData.end());
  return dMax;
}

void   
ST1dHistogramm::GetStat(double* center, double* rms, double* maxval, double* minval)
{
  double dCenter =0;

  //calculate cummulative summ
  double dSum = GetSumm();
  if(dSum==0)return;

  //Calculate center of gravity
  if(center)
  {
     for(int i=0; i<Length(); ++i)dCenter+=(i*vData[i]/dSum);
     *center = dCenter * dStep + dLeft;
  }

  //calculate RMS
  if(rms && center)
  {
     double dRMS = 0;
     for(int i=0; i<Length(); ++i)dRMS+=((i-dCenter)*(i-dCenter)*vData[i]/dSum);
     *rms=sqrt(dRMS)*dStep;
  }

  //Calculate Max value
  if(maxval)
  {
     *maxval = *std::max_element(vData.begin(), vData.end());
  }

  //Calculate Min value
  if(minval)
  {
     *minval = *std::min_element(vData.begin(), vData.end());;
  }

}


ST2dHistogramm::ST2dHistogramm(double l, double r, int nx, double b, double t, int ny)
{
   dX1=l;
   dX2=r;
   nXCells=nx;
   dXStep = (dX2-dX1)/nXCells;

   dY1=b;
   dY2   =t;
   nYCells=ny;
   dYStep = (dY2-dY1)/nYCells;

   dData = new double[nx*ny];
   Reset();
}

ST2dHistogramm::ST2dHistogramm(char* name)
{
   int    i;
   double x, y, value;
   char   buffer[512];
   char   buf[256];
   char   seps[]=", ";	  //separatoren

   FILE* in = fopen(name,"r");
   if(in)
   {
	    //read caption and allocate buffers
	    fgets(buffer, sizeof(buffer), in);
        strcpy(buf,strtok(buffer,seps));
        dX1 = atof(buf);
        strcpy(buf,strtok(NULL,  seps));
        dX2 = atof(buf);
        strcpy(buf,strtok(NULL,  seps));
        nXCells = atoi(buf);
        strcpy(buf,strtok(NULL,seps));
        dY1 = atof(buf);
        strcpy(buf,strtok(NULL,  seps));
        dY2 = atof(buf);
        strcpy(buf,strtok(NULL,  seps));
        nYCells = atoi(buf);

        dXStep = (dX2-dX1)/nXCells;
        dYStep = (dY2-dY1)/nYCells;

		dData = new double[nXCells*nYCells];
 	    Reset();

		for(i=0;i<nXCells*nYCells;i++)
		{
		    fgets(buffer, sizeof(buffer),in);

            strcpy(buf,strtok(buffer,seps));
            x = atof(buf);
            strcpy(buf,strtok(NULL,  seps));
            y = atof(buf);
			strcpy(buf,strtok(NULL,  seps));
            value = atof(buf);

			AddEvent(x, y, value);
		}

        fclose(in);
   }  
}


ST2dHistogramm::~ST2dHistogramm()
{
	delete[] dData;
}


void
ST2dHistogramm::Reset()
{
	memset(dData,0,sizeof(double)*nXCells*nYCells);
}


void
ST2dHistogramm::AddEvent(double vx, double vy, double increment)
{
	int    x_index = (int)((vx-dX1)/dXStep);
	int    y_index = (int)((vy-dY1)/dYStep);

	AddEvent( x_index, y_index, increment);
}

void
ST2dHistogramm::AddEvent(int x_index, int y_index, double increment)
{
	if(x_index<=0)x_index=0;
	if(x_index>=nXCells)x_index=nXCells-1;

	if(y_index<=0)y_index=0;
	if(y_index>=nYCells)y_index=nYCells-1;

	dData[y_index*nXCells+x_index]+=increment;
}

double ST2dHistogramm::GetCell(double vx, double vy)
{
	int    x_index = (int)((vx-dX1)/dXStep);
	int    y_index = (int)((vy-dY1)/dYStep);

	return GetCell(x_index, y_index);
}

double ST2dHistogramm::GetCell(int x_index, int y_index)
{
	if(x_index<=0)x_index=0;
	if(x_index>=nXCells)x_index=nXCells-1;

	if(y_index<=0)y_index=0;
	if(y_index>=nYCells)y_index=nYCells-1;

	return dData[y_index*nXCells+x_index];
}

double 
ST2dHistogramm::GetMaxValue(double dInitValue)
{
  int x, y, Offset;
  double dMax = dInitValue;
  double dValue; 

  for( y=0;y<nYCells;y++)
  {
	  Offset = y*nXCells;
	  for(x=0;x<nXCells;x++)
	  {
         dValue = dData[Offset+x];
	     if(dMax<dValue)dMax=dValue;
	  }
  }
  return dMax;
}

void   
ST2dHistogramm::GetStat(double* mean, double* rms, double* maxval, double* minval)
{
  int i, j;
  double dCounter=0;
  double Mean    =0;
  double Rms     =0;
  double dMax    =1;
  double dMin;

  //get mean value
  for(i=1;i<nXCells-1;i++)
  {
	  for(j=1;j<nYCells-1;j++)
	  {
		  Mean+=GetCell(i,j);
		  dCounter+=1;
	  }
  }

  //Sanity check
  if(dCounter<2)return;
  Mean/=dCounter; 
  *mean=Mean;

  //get RMS
  for(i=1;i<nXCells-1;i++)
  {
	  for(j=1;j<nYCells-1;j++)
	  {
		  Rms+=(GetCell(i,j)-Mean)*(GetCell(i,j)-Mean);
	  }
  }
  Rms/=(dCounter-1);
  *rms=sqrt(fabs(Rms));

  //Get Max Value
  for(i=1;i<nXCells-1;i++)
  {
	  for(j=1;j<nYCells-1;j++)
	  {
		  if(GetCell(i,j)>dMax)dMax=GetCell(i,j);
	  }
  }
 
  dMin=dMax;
  *maxval=dMax;

  //Get Min Value
  for(i=1;i<nXCells-1;i++)
  {
	  for(j=1;j<nYCells-1;j++)
	  {
		  if(GetCell(i,j)<dMin)dMin=GetCell(i,j);
	  }
  }

  *minval=dMin;
}

void
ST2dHistogramm::SaveData(const char* name, int numEvents)
{
  int    nx,ny;

  //sanity check
  if(numEvents==0)return;

  FILE* out = fopen(name,"w");
  if(out)
  {
     //fprintf(out,"%f %f %d %f %f %d\n",dX1, dX2, nXCells, dY1, dY2, nYCells);
     for(nx=0;nx<nXCells;nx++)
     {
        for(ny=0;ny<nYCells;ny++) 
		{
			fprintf(out,"\n %f %f %f",dX1+dXStep*nx,dY1+dYStep*ny, (GetCell(nx, ny)/numEvents));
		}
     }
     fclose(out);
  }
}

ST1dHistogramm* 
ST2dHistogramm::Get1D_RMS(int dir)
{
	double Center, RMS;
	ST1dHistogramm* hist;
	ST1dHistogramm* tmphist;

	switch(dir)
	{
	case 1: //X
	        hist    = new ST1dHistogramm(dY1, dY2,  nYCells);
			tmphist = new ST1dHistogramm(dX1, dX2,  nXCells);

			for(int j=0;j<nYCells;++j)
	        {
				tmphist->Reset();
				for(int i=0;i<nXCells;++i) tmphist->AddEvent(i,this->GetCell(i,j));
				tmphist->GetStat(&Center,&RMS,0,0);
				hist->AddEvent(j,RMS);
	        }
		break;
	case 2: //Y
	        tmphist = new ST1dHistogramm(dY1, dY2,  nYCells);
			hist    = new ST1dHistogramm(dX1, dX2,  nXCells);

	        for(int i=0;i<nXCells;i++)
	        {
				tmphist->Reset();
				for(int j=0;j<nYCells;j++) tmphist->AddEvent(j,this->GetCell(i,j));
				tmphist->GetStat(&Center,&RMS,0,0);
				hist->AddEvent(i,RMS);
	        }

		break;
	default:
		return NULL;
	}

    delete tmphist;
	return hist;
}

ST1dHistogramm*
ST2dHistogramm::Get1D_Mean(int dir)
{
	double Center, RMS;
	ST1dHistogramm* hist;
	ST1dHistogramm* tmphist;

	switch (dir)
	{
	case 1: //X
		hist = new ST1dHistogramm(dY1, dY2, nYCells);
		tmphist = new ST1dHistogramm(dX1, dX2, nXCells);

		for (int j = 0; j<nYCells; ++j)
		{
			tmphist->Reset();
			for (int i = 0; i<nXCells; ++i) tmphist->AddEvent(i, this->GetCell(i, j));
			tmphist->GetStat(&Center, &RMS, 0, 0);
			hist->AddEvent(j, Center);
		}
		break;
	case 2: //Y
		tmphist = new ST1dHistogramm(dY1, dY2, nYCells);
		hist = new ST1dHistogramm(dX1, dX2, nXCells);

		for (int i = 0; i<nXCells; i++)
		{
			tmphist->Reset();
			for (int j = 0; j<nYCells; j++) tmphist->AddEvent(j, this->GetCell(i, j));
			tmphist->GetStat(&Center, &RMS, 0, 0);
			hist->AddEvent(i, Center);
		}

		break;
	default:
		return NULL;
	}

	delete tmphist;
	return hist;
}



ST3dHistogramm::ST3dHistogramm(double x1,double x2, int nx, double y1, double y2, int ny, double z1, double z2, int nz)
{
   dX1  = x1;   dX2  = x2;  nXCells=nx;
   dY1  = y1;   dY2  = y2;  nYCells=ny;
   dZ1  = z1;   dZ2  = z2;  nZCells=nz;

   dXStep = (dX2-dX1)/nXCells;
   dYStep = (dY2-dY1)/nYCells;
   dZStep = (dZ2-dZ1)/nZCells;

   H2Stack = new ST2dHistogramm*[nz];
   for(int i=0;i<nZCells;i++)H2Stack[i] = new ST2dHistogramm(x1, x2, nx, y1, y2, ny);
}

ST3dHistogramm::~ST3dHistogramm()
{
   for(int i=0;i<nZCells;i++)delete H2Stack[i];
   delete[] H2Stack;
}

void   
ST3dHistogramm::Reset()
{
   for(int i=0;i<nZCells;i++)H2Stack[i]->Reset();
}

void   
ST3dHistogramm::AddEvent(double x, double y, double z, double increment)
{
	int    x_index = (int)((x-dX1)/dXStep);
	int    y_index = (int)((y-dY1)/dYStep);
	int    z_index = (int)((z-dZ1)/dZStep);

	AddEvent( x_index, y_index, z_index, increment);

}

void   
ST3dHistogramm::AddEvent(int x_index, int y_index, int z_index, double increment)
{
	if(z_index<=0)z_index=0;
	if(z_index>=nZCells)z_index=nZCells-1;

	H2Stack[z_index]->AddEvent(x_index, y_index, increment);
}

double 
ST3dHistogramm::GetCell (double x, double y, double z)
{
	int    x_index = (int)((x-dX1)/dXStep);
	int    y_index = (int)((y-dY1)/dYStep);
	int    z_index = (int)((z-dZ1)/dZStep);

	return GetCell ( x_index, y_index, z_index);
}

double 
ST3dHistogramm::GetCell (int x_index, int y_index, int z_index)
{
	if(z_index<=0)z_index=0;
	if(z_index>=nZCells)z_index=nZCells-1;

	return H2Stack[z_index]->GetCell(x_index, y_index);
}


double 
ST3dHistogramm::GetMaxValue(double dInitValue)
{
  double dMax = dInitValue;
  double dValue;

  for(int i=0;i<nZCells;i++)
  {
      dValue = H2Stack[i]->GetMaxValue(dMax);
	  if(dMax<dValue)dMax=dValue;
  }
  return dMax;
}

void   
ST3dHistogramm::SaveValuesHistogram(char* fname, int nBins)
{
  double dMValue = GetMaxValue();
  ST1dHistogramm  Hist(0 , dMValue, nBins);

  //Fill histogramm
  int    x, y, z;
  double dValue;

  for(z=0;z<nZCells;z++)
  {
	  for(y=0;y<nYCells;y++)
	  {
		   for(x=0;x<nXCells;x++)
		   {
			   dValue = GetCell(x,y,z);
			   Hist.AddEvent(dValue, 1.);
		   }
	  }
  }

  //Save it
  Hist.SaveData(fname);
}

//Get new 2d histogram from 3d
ST2dHistogramm* 
ST3dHistogramm::Get2D(int dir)
{
	int    x, y, z;
	double l;
	double r;
	int nx;
	double b;
	double t;
	int ny;
	double dValue;

	switch(dir)
	{
	case 1: //X
		l = dY1;
		r = dY2;
		nx= nYCells;
		b = dZ1;
		t = dZ2;
		ny= nZCells;
		break;
	case 2: //Y
		l = dX1;
		r = dX2;
		nx= nXCells;
		b = dZ1;
		t = dZ2;
		ny= nZCells;
		break;
	case 3: //Z
		l = dX1;
		r = dX2;
		nx= nXCells;
		b = dY1;
		t = dY2;
		ny= nYCells;
		break;
	default:
		return NULL;
	}

	ST2dHistogramm* hist = new ST2dHistogramm(l, r, nx, b, t, ny);

	switch(dir)
	{
	case 1: //X
		for(y=0;y<nYCells;y++)
        {
            for(z=0;z<nZCells;z++)
	        {
				 dValue = 0;
		         for(x=0;x<nXCells;x++)dValue+=GetCell(x,y,z);
				 hist->AddEvent(y,z,dValue);
	        }
        }
		break;
	case 2: //Y
		for(x=0;x<nXCells;x++)
        {
            for(z=0;z<nZCells;z++)
	        {
				 dValue = 0;
		         for(y=0;y<nYCells;y++)dValue+=GetCell(x,y,z);
				 hist->AddEvent(x,z,dValue);
	        }
        }
		break;
	case 3: //Z
		for(x=0;x<nXCells;x++)
        {
            for(y=0;y<nYCells;y++)
	        {
				 dValue = 0;
		         for(z=0;z<nZCells;z++)dValue+=GetCell(x,y,z);
				 hist->AddEvent(x,y,dValue);
	        }
        }
		break;
	}

	return hist;
}

//Get new 1d histogram from 3d
ST1dHistogramm* 
ST3dHistogramm::Get1D(int dir)
{
	int    x, y, z;
	double l;
	double r;
	int    n;
	double dValue;

	switch(dir)
	{
	case 1: //X
		l = dX1;
		r = dX2;
		n = nXCells;
		break;
	case 2: //Y
		l = dY1;
		r = dY2;
		n = nYCells;
		break;
	case 3: //Z
		l = dZ1;
		r = dZ2;
		n = nZCells;
		break;
	default:
		return NULL;
	}

	ST1dHistogramm* hist = new ST1dHistogramm(l, r, n);

	switch(dir)
	{
	case 1: //X
		for(x=0;x<nXCells;x++)
        {
			dValue = 0;
            for(y=0;y<nYCells;y++)
	        {
		         for(z=0;z<nZCells;z++)dValue+=GetCell(x,y,z);
	        }
			hist->AddEvent(x,dValue);
        }
		break;
	case 2: //Y
		for(y=0;y<nYCells;y++)
        {
			dValue = 0;
            for(z=0;z<nZCells;z++)
	        {
		         for(x=0;x<nXCells;x++)dValue+=GetCell(x,y,z);
	        }
			hist->AddEvent(y,dValue);
        }
		break;
	case 3: //Z
		for(z=0;z<nZCells;z++)
        {
			dValue = 0;
            for(y=0;y<nYCells;y++)
	        {
		         for(x=0;x<nXCells;x++)dValue+=GetCell(x,y,z);
	        }
			hist->AddEvent(z,dValue);
        }
		break;
	}

	return hist;
}

void   
ST3dHistogramm::SaveData(const char* fname, int numEvents)
{
  int    x, y, z;
  double dValue;

  //sanity check
  if(numEvents==0)return;

  FILE* out = fopen(fname,"w");
  if(out)
  {
	  fprintf(out,"Nx=%5d X1=%E X2=%E dX=%E\n",nXCells, dX1, dX2, dXStep);
	  fprintf(out,"Ny=%5d Y1=%E Y2=%E dY=%E\n",nXCells, dY1, dY2, dYStep);
	  fprintf(out,"Nz=%5d Z1=%E Z2=%E dZ=%E\n",nXCells, dZ1, dZ2, dZStep);
	  fseek(out, 512, SEEK_SET);
	  
	  for(x=0;x<nXCells;x++)
	  {
		  for(y=0;y<nYCells;y++)
		  {	
			   for(z=0;z<nZCells;z++)
		       {
					dValue = GetCell(x,y,z)/numEvents;
					fwrite(&dValue, sizeof(double), 1, out);
		       }
	      }
	  }

	  fclose(out);
  }
}
