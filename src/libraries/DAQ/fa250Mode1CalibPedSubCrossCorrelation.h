// $Id$
//
//    File: fa250Mode1CalibPedSubCrossCorrelation.h
// Created: Wed Feb 10 15:01:00 CET 2021
// Creator: mspreafico (on Linux apcx18 3.10.0-514.el7.x86_64 x86_64)
//

#ifndef _fa250Mode1CalibPedSubCrossCorrelation_
#define _fa250Mode1CalibPedSubCrossCorrelation_

#include <JANA/JObject.h>
#include <JANA/JFactory.h>

class fa250Mode1CalibPedSubCrossCorrelation:public jana::JObject{
	public:
		JOBJECT_PUBLIC(fa250Mode1CalibPedSubCrossCorrelation);
		
		// Add data members here. For example:
		// int id;
		// double E;
		

		double crossCorrSine_tot;
		double crossCorrSignal_tot;
		double crossCorrSine_5mVth;
		double crossCorrSignal_5mVth;
		double crossCorrSine_calo;
		double crossCorrSignal_calo;
		double crossCorrSine_5mVth_calo;
		double crossCorrSignal_5mVth_calo;

		// This method is used primarily for pretty printing
		// the second argument to AddString is printf style format
		void toStrings(vector<pair<string,string> > &items)const{
			// AddString(items, "id", "%4d", id);
			// AddString(items, "E", "%f", E);
		}
		
};

#endif // _fa250Mode1CalibPedSubCrossCorrelation_

