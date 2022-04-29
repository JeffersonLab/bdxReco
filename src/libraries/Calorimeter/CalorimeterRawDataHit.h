// $Id$
//
//    File: CalorimeterSiPMHit.h
// Created: Wed Jan 27 16:39:49 CET 2016
// Creator: celentan (on Linux apcx4 2.6.32-504.30.3.el6.x86_64 x86_64)
//

#ifndef _CalorimeterRawDataHit_
#define _CalorimeterRawDataHit_

#include <JANA/JObject.h>
#include <JANA/JFactory.h>
#include <DAQ/fa250ConvertedHit.h>
#include <TT/TranslationTable.h>


#include <TObject.h>


class CalorimeterRawDataHit:public fa250ConvertedHit{


public:
	JOBJECT_PUBLIC(CalorimeterRawDataHit);




	// Add data members here. For example:
	// int id;
	// double E;

	// This method is used primarily for pretty printing
	// the second argument to AddString is printf style format
	void toStrings(vector<pair<string,string> > &items)const{
		AddString(items, "sector","%i",m_channel.calorimeter->sector);
		AddString(items, "x","%i",m_channel.calorimeter->x);
		AddString(items, "y","%i",m_channel.calorimeter->y);
		AddString(items, "readout","%i",m_channel.calorimeter->readout);
		AddString(items, "Qraw", "%f",Qraw);
		AddString(items, "Q","%f",Q);
		AddString(items, "A","%f",A);
		AddString(items, "T","%f",T);
		AddString(items, "pedMean","%f",pedMean);
		AddString(items, "pedRMS","%f",pedRMS);
		AddString(items, "rms_flag","%i",1*RMSflag);
	}

	//A.C. do not touch these
	//both crate-slot channel and detector-specific ID. Since this is a sensor-based object, the readout field will be !=0
	TranslationTable::ChannelInfo m_channel;//!



	double Qraw,Q;
	double T,A;
	double pedMean,pedRMS;
	bool RMSflag;


	uint64_t timestamp;



	ClassDef(CalorimeterRawDataHit,1);


};
#endif // _CalorimeterRawDataHit_

