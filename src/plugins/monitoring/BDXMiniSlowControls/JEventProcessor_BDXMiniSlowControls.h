// $Id$
//
//    File: JEventProcessor_BDXMiniSlowControls.h
// Created: Mon Oct  7 09:20:42 CEST 2019
// Creator: celentan (on Linux apcx4 3.10.0-957.el7.x86_64 x86_64)
//

#ifndef _JEventProcessor_BDXMiniSlowControls_
#define _JEventProcessor_BDXMiniSlowControls_

#include <JANA/JEventProcessor.h>
class JROOTOutput;

class JEventProcessor_BDXMiniSlowControls: public jana::JEventProcessor {
public:
	JEventProcessor_BDXMiniSlowControls();
	~JEventProcessor_BDXMiniSlowControls();
	const char* className(void) {
		return "JEventProcessor_BDXMiniSlowControls";
	}

private:
	jerror_t init(void);						///< Called once at program start.
	jerror_t brun(jana::JEventLoop *eventLoop, int32_t runnumber);	///< Called everytime a new run number is detected.
	jerror_t evnt(jana::JEventLoop *eventLoop, uint64_t eventnumber);	///< Called every event.
	jerror_t erun(void);						///< Called everytime run number changes, provided brun has been called.
	jerror_t fini(void);						///< Called after last event of last event source has been processed.

	int m_isMC;
	int m_isFirstCallToBrun;
	JROOTOutput *m_ROOTOutput;

	int m_nbins;
	unsigned long int m_dT;
	unsigned long int m_T0, m_T;
	int m_isT0Set;
	unsigned long int index;

	map<unsigned long int, unsigned int> arduinoEvents;
	map<unsigned long int, unsigned int>::iterator arduinoEvents_it;
	map<unsigned long int, double> arduinoT1;
	map<unsigned long int, double> arduinoT2;
	map<unsigned long int, double> arduinoH1;
	map<unsigned long int, double> arduinoH2;

	map<unsigned long int, unsigned int> daqEvents;
	map<unsigned long int, unsigned int>::iterator daqEvents_it;
	map<unsigned long int, double> daqLT;
	map<unsigned long int, double> daqT[16];

	map<unsigned long int, unsigned int> beamEvents;
	map<unsigned long int, unsigned int>::iterator beamEvents_it;
	map<unsigned long int, double> beamI, beamE;

	map<unsigned long int, unsigned int> envtentEvents;
	map<unsigned long int, unsigned int>::iterator envtentEvents_it;
	map<unsigned long int, double> envtentT1, envtentT2, envtentH1, envtentH2;

};

#endif // _JEventProcessor_BDXMiniSlowControls_

