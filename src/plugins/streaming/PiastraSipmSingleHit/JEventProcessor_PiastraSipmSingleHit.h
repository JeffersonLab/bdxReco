// $Id$
//
//    File: JEventProcessor_PiastraSipmSingleHit.h
// Created: Fri Mar  6 18:54:14 CET 2020
// Creator: celentan (on Darwin celentano-macbook 19.3.0 i386)
//

#ifndef _JEventProcessor_PiastraSipmSingleHit_
#define _JEventProcessor_PiastraSipmSingleHit_

#include <JANA/JEventProcessor.h>

class TTree;
class TH1D;
class JROOTOutput;
class JEventProcessor_PiastraSipmSingleHit: public jana::JEventProcessor {
public:
	JEventProcessor_PiastraSipmSingleHit();
	~JEventProcessor_PiastraSipmSingleHit();
	const char* className(void) {
		return "JEventProcessor_PiastraSipmSingleHit";
	}

private:
	jerror_t init(void);						///< Called once at program start.
	jerror_t brun(jana::JEventLoop *eventLoop, int32_t runnumber);	///< Called everytime a new run number is detected.
	jerror_t evnt(jana::JEventLoop *eventLoop, uint64_t eventnumber);	///< Called every event.
	jerror_t erun(void);						///< Called everytime run number changes, provided brun has been called.
	jerror_t fini(void);						///< Called after last event of last event source has been processed.

	TTree *m_t;
	int eventNumber;
	uint tWord;
	int m_isFirstCallToBrun;

	int counter;
	ULong64_t m_tSec,m_tNanoSec;

	JROOTOutput *m_ROOTOutput;
};

#endif // _JEventProcessor_PiastraSipmSingleHit_

