// $Id$
//
//    File: CalorimeterDigiHit.h
// Created: Wed Jan 27 16:39:57 CET 2016
// Creator: celentan (on Linux apcx4 2.6.32-504.30.3.el6.x86_64 x86_64)
//

#ifndef _CalorimeterDigiHit_
#define _CalorimeterDigiHit_

#include <JANA/JObject.h>
#include <JANA/JFactory.h>
#include <TT/TranslationTable.h>
#include "CalorimeterRawDataHit.h"
#include "TObject.h"

class CalorimeterDigiHit:public jana::JObject{
public:

	JOBJECT_PUBLIC(CalorimeterDigiHit);


	// This method is used primarily for pretty printing
	// the second argument to AddString is printf style format
	void toStrings(vector<pair<string,string> > &items)const{
		AddString(items, "sector", "%i", m_channel.sector);
		AddString(items, "x", "%i", m_channel.x);
		AddString(items, "y", "%i", m_channel.y);
		AddString(items, "readout","%i",m_channel.readout);
		AddString(items, "Q", "%f", Q);
		AddString(items, "T", "%f", T);
		AddString(items, "A", "%f", A);
	}

	double Q,T,A; //Charge (u.a.), time (ns), Amplitude (mV)
	double pedMean,pedRMS;
	bool RMSflag;
	TranslationTable::CALO_Index_t m_channel; //Detector-specific ID. Since this is a detector-based object, the readout field will be 0



	ClassDef(CalorimeterDigiHit,1);
};

#endif // _CalorimeterDigiHit_

