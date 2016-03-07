// $Id$
//
//    File: CalorimeterHit.h
// Created: Wed Jan 27 16:40:00 CET 2016
// Creator: celentan (on Linux apcx4 2.6.32-504.30.3.el6.x86_64 x86_64)
//

#ifndef _CalorimeterHit_
#define _CalorimeterHit_

#include <JANA/JObject.h>
#include <JANA/JFactory.h>
#include <TT/TranslationTable.h>


class CalorimeterHit:public jana::JObject{
	public:
		JOBJECT_PUBLIC(CalorimeterHit);
		
		// Add data members here. For example:
		// int id;
		// double E;
		
		// This method is used primarily for pretty printing
		// the second argument to AddString is printf style format
		void toStrings(vector<pair<string,string> > &items)const{
			// AddString(items, "id", "%4d", id);
			// AddString(items, "E", "%f", E);
		}
		double E1,E2,E;
		double T1,T2,T;
		double Q1,Q2,Q;   //I assume here at max 2 detectors per crystal..

		TranslationTable::CALO_Index_t m_channel; //Detector-specific ID. Since this is a detector-based object, the readout field will be ==0
};

#endif // _CalorimeterHit_

