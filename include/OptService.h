#pragma once
#ifndef _OPTSERVICE_HH_
#define _OPTSERVICE_HH_

#include <vector>
#include <string>
#include <iomanip>
#include <fstream>
#include <sstream>
//#include </Users/macalex/geant4/geant4.10.02.p02-install/include/Geant4/CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/SystemOfUnits.h>
#include "spline.h"

//Service functions to operate with Optical parameters of materials
namespace OptService
{
	//! Convert [eV] to wavelength in [nm]
	inline double eV2nm(double E_eV)
	{
		double dWl = (2.99792458E+8 * 6.62606896e-34)/ (E_eV * 1.602176487E-19) / 1E-9; //Wl(nm)=c[м*с]*h[Дж*с]/E[Дж] * на перевод nm
		return dWl;
	}

	//! Convert wavelength in [nm] to [eV]
	inline double nm2eV(double Wl_nm)
	{
		double dE = (2.99792458E+8 * 6.62606896e-34 / (Wl_nm * 1E-9)) / 1.602176487E-19; //E(ev)=c[м*с]*h[Дж*с]/lambda[м] * на перевод Дж а эВ
		return dE;
	}


	//! Convert wavelength in [nm] to [eV] in place
	inline void WaveLength2Energy(std::vector<double>& dData, double dEUnits = 1.0)
	{
		for (auto it = dData.begin(); it != dData.end(); it++)
		{
			//get energy
			G4double energy = OptService::nm2eV(*it) * dEUnits;
			*it = energy;
		}
	}


	//! Convert wavelength in [nm] to [eV]
	inline void WaveLength2Energy(const std::vector<double>& dWl, std::vector<double>& dEng, double dEUnits = 1.0)
	{
		for (auto it = dWl.begin(); it != dWl.end(); it++)
		{
			//get energy
			G4double energy = OptService::nm2eV(*it) * dEUnits;
			dEng.push_back(energy);
		}
	}

	// Serice class to save rational data for refraction
	class RATIONAL
	{
	private:
		std::vector<double> A;
		std::vector<double> B;
	public:
		//! Constructor 
		RATIONAL(std::vector<double> a, std::vector<double> b)
		{
			A = a; B = b;
		}

		//!Evaluate a value as (A0*dL^0+A1*dL^1+A2*dL^2+...)/(B0*dL^0+B1*dL^1+B2*dL^2+...)
		double Evaluate(double dL)
		{
			double dNom = 0.;
			for (auto it = A.begin(); it != A.end(); it++)
			{
				//get index of the element 
				int nPow = it - A.begin();
				if (nPow == 0){	dNom += *it; }
				else { dNom += *it * pow(dL, nPow);	}
			}

			double dDeNom = 0.;
			for (auto it = B.begin(); it != B.end(); it++)
			{
				//get index of the element 
				int nPow = it - B.begin();
				if (nPow == 0){ dDeNom += *it; }
				else{ dDeNom += *it * pow(dL, nPow); }
			}

			return (dDeNom != 0.) ? dNom / dDeNom : 0.;
		}
	};

	// The refractive index could be defined in different way as:
	// N^2 - 1 = frac{A1*L^2}{A2+A3*L^2}+frac{B1*L^2}{B2+B3*L^2}+frac{C1*L^2}{C2+C3*L^2}
	// or N = A + B*L^2 + C/L^2...
	// that is why we should define EQ_pow to normalize the result as pow(dRes, 1/EQ_pow)
	// We use the data from http://refractiveindex.info/
	// and for details you can read https://en.wikipedia.org/wiki/Sellmeier_equation
    inline double GetRefractiveIndex(double Wl_nm, double EQ_pow, std::vector<RATIONAL> vparams)
	{
		double dLambda = Wl_nm / 1000.; //convert to um (micrometers)
        //calculate summ
		double dRes = 0;
		for (auto it = vparams.begin(); it != vparams.end(); ++it)
		{
			dRes += it->Evaluate(dLambda);
		}
		//normalize to take in account the law
		double dN = pow(dRes, 1/EQ_pow);
		return dN;
	}

	//Read parameter values from file.
	//Read the data: WAVELENGTH  PARAMETER, if "bConvert" is defined as TRUE, the WAVELENTH will be recalculated to ENERGY and the data will be resorted
	inline void ReadParameters(std::string fName, std::vector<double>& vWl, std::vector<double>& vParam, bool bConvert = false, double dUnit1=1.0, double dUnit2 = 1.0)
	{
		vWl.clear();
		vParam.clear();

		std::ifstream infile(fName);
		std::string line;
		while (std::getline(infile, line))
		{
			if(line.find('#')!=std::string::npos)continue;

			std::istringstream iss(line);
			double a, b;
			if (!(iss >> a >> b)) { break; } // error
			// process pair (a,b)
			vWl.push_back(a);
			vParam.push_back(b * dUnit2);
		}

		//revert wavelength from incresed WL to increased ENERGY
		if (bConvert)
		{
			std::reverse(vWl.begin(), vWl.end());
			std::reverse(vParam.begin(), vParam.end());
			OptService::WaveLength2Energy(vWl, dUnit1);
		}
	}


	//Correct photocathode efficiancy to take in account its reflectivity
	inline void CorrectEfficiancyWithReflectance(const std::vector<double>& dEffEnergy, std::vector<double>& dEff, const std::vector<double>& dRefEnergy, const std::vector<double>& dRef)
	{
		//calculate spline coefficents
		tk::spline s;
		s.set_points(dRefEnergy, dRef);    // currently it is required that X is already sorted

		auto it = dEffEnergy.begin();
		auto it2 = dEff.begin();
		for (; it != dEffEnergy.end(); it++, it2++)
		{
			double dT = s(*it);
			*it2 = *it2 / (1-dT);
		}
	}

}
#endif
