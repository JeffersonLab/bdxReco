// $Id$
//
//    File: IntVetoHit.h
// Created: Wed Jan 13 21:08:09 CET 2016
// Creator: celentan (on Linux localhost.localdomain 2.6.32-504.30.3.el6.x86_64 x86_64)
//

#ifndef _IntVetoHit_
#define _IntVetoHit_

#include <JANA/JObject.h>
#include <JANA/JFactory.h>
#include <TT/TranslationTable.h>
#include <system/BDXObject.h>

class TH1D;
class TCanvas;

class IntVetoHit:public BDXObject{
private:
	mutable vector<TH1D*> hWaves; //!  //Exclude from root dictionary

public:
	JOBJECT_PUBLIC(IntVetoHit);

	// Add data members here. For example:
	// int id;
	// double E;

	// This method is used primarily for pretty printing
	// the second argument to AddString is printf style format
	void toStrings(vector<pair<string,string> > &items)const{
		AddString(items, "sector", "%4d", m_channel.sector);
		AddString(items, "layer", "%4d", m_channel.layer);
		AddString(items, "component", "%4d", m_channel.component);
		AddString(items, "Q","%4f",Q);
		AddString(items, "A","%4f",A);
		AddString(items, "T","%4f",T);


	}
	TranslationTable::INT_VETO_Index_t m_channel; //Detector-specific ID. Since this is a detector-based object, the readout field will be ==0

	double A,Q,T; //A,Q is in phe

	virtual TCanvas* Draw(int id=0) const;//!  //Exclude from root dictionary


	ClassDef(IntVetoHit,1);

};

#endif // _IntVetoHit_

