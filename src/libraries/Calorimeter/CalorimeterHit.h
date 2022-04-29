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
#include <system/BDXObject.h>
#include <TObject.h>
#include "CalorimeterRawDataHit.h"

class TH1D;
class TCanvas;


class CalorimeterHit:public BDXObject{

public:

	CalorimeterHit();

	JOBJECT_PUBLIC(CalorimeterHit);

	// Add data members here. For example:
	// int id;
	// double E;

	// This method is used primarily for pretty printing
	// the second argument to AddString is printf style format
	void toStrings(vector<pair<string,string> > &items)const{
		// AddString(items, "id", "%4d", id);
		AddString(items, "sector", "%i", m_channel.sector);
		AddString(items, "x", "%i", m_channel.x);
		AddString(items, "y", "%i", m_channel.y);
		AddString(items, "E", "%f", E);
		AddString(items, "T", "%f", T);
		AddString(items, "A", "%f", A);

	}
	TranslationTable::CALO_Index_t m_channel; //Detector-specific ID. Since this is a detector-based object, the readout field will be ==0
	double E,T;
	double Eraw;
	double A; //amplitude of analogue signal in mV. -1 if not applicable.
	bool RMSflag;


	virtual TCanvas* Draw(int id)const; //!
	mutable TH1D* hWave; //!







	ClassDef(CalorimeterHit,2);
};

#endif // _CalorimeterHit_

