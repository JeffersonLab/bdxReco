// $Id$
//
//    File: JEventProcessor_BDXMiniTimeDiff.h
// Created: Wed Oct  7 18:28:33 CEST 2020
// Creator: celentan (on Linux apcx4 3.10.0-957.el7.x86_64 x86_64)
//

#ifndef _JEventProcessor_BDXMiniTimeDiff_
#define _JEventProcessor_BDXMiniTimeDiff_

#include <JANA/JEventProcessor.h>

class JROOTOutput;
class TH1D;
class TH2D;
class JEventProcessor_BDXMiniTimeDiff:public jana::JEventProcessor{
	public:
		JEventProcessor_BDXMiniTimeDiff();
		~JEventProcessor_BDXMiniTimeDiff();
		const char* className(void){return "JEventProcessor_BDXMiniTimeDiff";}

	private:
		jerror_t init(void);						///< Called once at program start.
		jerror_t brun(jana::JEventLoop *eventLoop, int32_t runnumber);	///< Called everytime a new run number is detected.
		jerror_t evnt(jana::JEventLoop *eventLoop, uint64_t eventnumber);	///< Called every event.
		jerror_t erun(void);						///< Called everytime run number changes, provided brun has been called.
		jerror_t fini(void);						///< Called after last event of last event source has been processed.

		JROOTOutput *m_ROOTOutput;
		int m_isFirstCallToBrun;
		int m_isMC;

		TH1D *hTimeDiff;
		TH2D *hTimeDiffvsEventN;
		long long int timeThis,timePrev;

		int eventTypeThis,eventTypePrev;
};

#endif // _JEventProcessor_BDXMiniTimeDiff_

