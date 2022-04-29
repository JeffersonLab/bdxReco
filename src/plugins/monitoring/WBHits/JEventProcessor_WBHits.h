// $Id$
//
//    File: JEventProcessor_WBHits.h
// Created: Sat Feb 24 13:11:49 CET 2018
// Creator: celentan (on Darwin celentano-macbook 17.4.0 i386)
//

#ifndef _JEventProcessor_WBHits_
#define _JEventProcessor_WBHits_

#include <JANA/JEventProcessor.h>
class TH1D;
class TH2D;
class JROOTOutput;

class JEventProcessor_WBHits: public jana::JEventProcessor {
public:
	JEventProcessor_WBHits();
	~JEventProcessor_WBHits();
	const char* className(void) {
		return "JEventProcessor_WBHits";
	}

private:
	jerror_t init(void);						///< Called once at program start.
	jerror_t brun(jana::JEventLoop *eventLoop, int32_t runnumber);	///< Called everytime a new run number is detected.
	jerror_t evnt(jana::JEventLoop *eventLoop, uint64_t eventnumber);	///< Called every event.
	jerror_t erun(void);						///< Called everytime run number changes, provided brun has been called.
	jerror_t fini(void);						///< Called after last event of last event source has been processed.


	int m_isFirstCallToBrun;
	JROOTOutput *m_ROOTOutput;

	TH1D *hNumHits;
	TH2D *hNumSamplesPerCh;



};

#endif // _JEventProcessor_WBHits_

