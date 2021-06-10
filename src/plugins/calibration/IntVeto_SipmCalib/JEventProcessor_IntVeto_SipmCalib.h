// $Id$
//
//    File: JEventProcessor_sipm_calib.h
// Created: Tue Feb  2 18:52:32 CET 2016
// Creator: celentan (on Linux apcx4 2.6.32-504.30.3.el6.x86_64 x86_64)
//

#ifndef _JEventProcessor_sipm_calib_
#define _JEventProcessor_sipm_calib_

#include <JANA/JEventProcessor.h>
#include <TT/TranslationTable.h>
class TTree;
class TH1D;
class JROOTOutput;
class JEventProcessor_IntVeto_SipmCalib:public jana::JEventProcessor{
	public:
		JEventProcessor_IntVeto_SipmCalib();
		~JEventProcessor_IntVeto_SipmCalib();
		const char* className(void){return "JEventProcessor_IntVeto_SipmCalib";}

	private:
		jerror_t init(void);						///< Called once at program start.
		jerror_t brun(jana::JEventLoop *eventLoop,int32_t runnumber);	///< Called everytime a new run number is detected.
		jerror_t evnt(jana::JEventLoop *eventLoop,uint64_t eventnumber);	///< Called every event.
		jerror_t erun(void);						///< Called everytime run number changes, provided brun has been called.
		jerror_t fini(void);						///< Called after last event of last event source has been processed.

		TTree *t;
		TH1D *h;
		uint eventNumber;
		uint eventTS;
		uint tWord;
		int m_isFirstCallToBrun;

		int m_sector,m_layer,m_component,m_readout,m_type;
		double Qphe,Qraw,T,Ttot,A,Aphe;


		JROOTOutput *m_ROOTOutput;

		int triggerBit;

};

#endif // _JEventProcessor_sipm_calib_

