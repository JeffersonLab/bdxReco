// $Id$
//
//    File: JEventProcessor_WBpeds.h
// Created: Wed Apr 27 16:03:14 UTC 2022
// Creator: clasrun (on Linux jdaq6 3.10.0-1160.31.1.el7.x86_64 x86_64)
//

#ifndef _JEventProcessor_WBpeds_
#define _JEventProcessor_WBpeds_

#include <JANA/JEventProcessor.h>
class TH1D;
class TH2D;
class JROOTOutput;

class JEventProcessor_WBpeds:public jana::JEventProcessor{
	public:
		JEventProcessor_WBpeds();
		~JEventProcessor_WBpeds();
		const char* className(void){return "JEventProcessor_WBpeds";}

	private:
		jerror_t init(void);						///< Called once at program start.
		jerror_t brun(jana::JEventLoop *eventLoop, int32_t runnumber);	///< Called everytime a new run number is detected.
		jerror_t evnt(jana::JEventLoop *eventLoop, uint64_t eventnumber);	///< Called every event.
		jerror_t erun(void);						///< Called everytime run number changes, provided brun has been called.
		jerror_t fini(void);						///< Called after last event of last event source has been processed.


		int m_isFirstCallToBrun;
		int m_isMC;
		int m_nSamples;
		JROOTOutput *m_ROOTOutput;



};

#endif // _JEventProcessor_WBpeds_

