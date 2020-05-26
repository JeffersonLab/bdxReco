// $Id$
//
//    File: VetoIntDigiHit.h
// Created: Tue Jan 12 11:52:41 CET 2016
// Creator: celentan (on Linux apcx4 2.6.32-504.30.3.el6.x86_64 x86_64)
//

#ifndef _VetoIntSiPMHit_
#define _VetoIntSiPMHit_

#include <JANA/JObject.h>
#include <JANA/JFactory.h>
#include <DAQ/fa250ConvertedHit.h>
#include <TT/TranslationTable.h>

#ifdef __ROOTCINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ class IntVetoSiPMHit+;
#endif

class IntVetoSiPMHit:public fa250ConvertedHit{
public:
	JOBJECT_PUBLIC(IntVetoSiPMHit);

	// Add data members here. For example:
	// int id;
	// double E;

	// This method is used primarily for pretty printing
	// the second argument to AddString is printf style format
	void toStrings(vector<pair<string,string> > &items)const{
		AddString(items, "sector", "%4d", m_channel.int_veto->sector);
		AddString(items, "layer", "%4d", m_channel.int_veto->layer);
		AddString(items, "component", "%4d", m_channel.int_veto->component);
		AddString(items, "readout", "%4d", m_channel.int_veto->readout);
		AddString(items,"Qraw","%4f",Qraw);
		AddString(items,"Qphe","%4f",Qphe);
		AddString(items,"T","%4f",T);
		AddString(items,"TOT","%4f",Ttot);
		AddString(items,"Araw","%4f",Araw);
		AddString(items,"Aphe","%4f",Aphe);
		AddString(items,"pedMean","%4f",pedMean);
		AddString(items,"pedRMS","%4f",pedRMS);
		AddString(items,"rmsFlag","%i",1*RMSflag);
		AddString(items,"type","%i",m_type);
	}
	typedef enum{
		noise=0,
		real_signal=1, //not much more to distinguish!
		num_hit_type
	}hit_type;



	//A.C. do not touch these
	//both crate-slot channel and detector-specific ID. Since this is a sensor-based object, the readout field will be !=0
	TranslationTable::ChannelInfo m_channel;

	double Qraw,Qphe;
	double T;
	double Araw;
	double Aphe;
	double Ttot;
	double average;
	double pedMean,pedRMS;
	int nSingles;
	bool RMSflag;
	hit_type m_type;
};

#endif // _VetoIntDigiHit_

