// $Id$
//
//    File: JEventProcessor_NA64EcalHits.h
// Created: Thu Oct  3 17:27:30 CEST 2019
// Creator: celentan (on Linux apcx4 3.10.0-957.el7.x86_64 x86_64)
//

#ifndef _JEventProcessor_NA64EcalHits_
#define _JEventProcessor_NA64EcalHits_

#include <JANA/JEventProcessor.h>
#include "TH1D.h"
#include "TFile.h"
class JROOTOutput;

class JEventProcessor_NA64EcalHits: public jana::JEventProcessor {
public:
	JEventProcessor_NA64EcalHits();
	~JEventProcessor_NA64EcalHits();
	const char* className(void) {
		return "JEventProcessor_NA64EcalHits";
	}

private:
	jerror_t init(void);						///< Called once at program start.
	jerror_t brun(jana::JEventLoop *eventLoop, int32_t runnumber);	///< Called everytime a new run number is detected.
	jerror_t evnt(jana::JEventLoop *eventLoop, uint64_t eventnumber);	///< Called every event.
	jerror_t erun(void);						///< Called everytime run number changes, provided brun has been called.
	jerror_t fini(void);						///< Called after last event of last event source has been processed.

	int m_isFirstCallToBrun;
	JROOTOutput *m_ROOTOutput;
	int m_isMC;

	std::map<std::pair<int,int>,int> geometry;
};

#endif // _JEventProcessor_NA64EcalHits_

