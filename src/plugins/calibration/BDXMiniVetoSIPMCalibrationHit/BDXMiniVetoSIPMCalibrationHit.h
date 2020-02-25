// $Id$
//
//    File: BDXMiniVetoSIPMCalibrationHit.h
// Created: Mon Aug 19 18:11:03 CEST 2019
// Creator: celentan (on Linux apcx4 3.10.0-957.el7.x86_64 x86_64)
//

#ifndef _BDXMiniVetoSIPMCalibrationHit_
#define _BDXMiniVetoSIPMCalibrationHit_

#include <JANA/JObject.h>
#include <JANA/JFactory.h>
#include <TT/TranslationTable.h>

class BDXMiniVetoSIPMCalibrationHit:public jana::JObject{
	public:
		JOBJECT_PUBLIC(BDXMiniVetoSIPMCalibrationHit);
		
		// Add data members here. For example:
		// int id;
		// double E;
		
		// This method is used primarily for pretty printing
		// the second argument to AddString is printf style format
		void toStrings(vector<pair<string,string> > &items)const{
			// AddString(items, "id", "%4d", id);
			// AddString(items, "E", "%f", E);
		}
		
		TranslationTable::INT_VETO_Index_t m_channel;      //Detector-specific ID. Since this is a detector-based object, the readout field will be ==0

		double Qphe, Qraw, T, Araw,Aphe;


};

#endif // _BDXMiniVetoSIPMCalibrationHit_

