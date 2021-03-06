// $Id$
//
//    File: JEventProcessor_test.h
// Created: Mon Dec 28 15:48:21 CET 2015
// Creator: celentan (on Linux apcx4 2.6.32-504.30.3.el6.x86_64 x86_64)
//

#ifndef _ExtVeto_SigDisplay_
#define _ExtVeto_SigDisplay_

#include <JANA/JEventProcessor.h>

class TH1D;
class TH2D;
class TTree;
class JROOTOutput;

class ExtVeto_SigDisplay:public jana::JEventProcessor{
	public:
		ExtVeto_SigDisplay();
		~ExtVeto_SigDisplay();
		const char* className(void){return "ExtVeto_SigDisplay";}

	private:
		jerror_t init(void);						///< Called once at program start.
		jerror_t brun(jana::JEventLoop *eventLoop, int32_t runnumber);	///< Called everytime a new run number is detected.
		jerror_t evnt(jana::JEventLoop *eventLoop, uint64_t eventnumber);	///< Called every event.
		jerror_t erun(void);						///< Called everytime run number changes, provided brun has been called.
		jerror_t fini(void);						///< Called after last event of last event source has been processed.

		int m_isFirstCallToBrun;
		TH1D *h;
		TTree *t;
		JROOTOutput *m_ROOTOutput;

		double Q;
		int component,eventN;


};

#endif // _ExtVeto_SigDisplay_

