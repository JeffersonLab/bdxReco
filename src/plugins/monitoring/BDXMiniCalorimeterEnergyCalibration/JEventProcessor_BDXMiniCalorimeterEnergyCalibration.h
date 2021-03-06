// $Id$
//
//    File: JEventProcessor_BDXMiniCalorimeterEnergyCalibration.h
// Created: Wed Apr  3 18:00:19 CEST 2019
// Creator: celentan (on Linux apcx4 2.6.32-696.13.2.el6.x86_64 x86_64)
//

#ifndef _JEventProcessor_BDXMiniCalorimeterEnergyCalibration_
#define _JEventProcessor_BDXMiniCalorimeterEnergyCalibration_

#include <JANA/JEventProcessor.h>

#include "TH1D.h"
#include "TFile.h"
#include <map>
class JROOTOutput;
class JEventProcessor_BDXMiniCalorimeterEnergyCalibration: public jana::JEventProcessor {
public:
	JEventProcessor_BDXMiniCalorimeterEnergyCalibration();
	~JEventProcessor_BDXMiniCalorimeterEnergyCalibration();
	const char* className(void) {
		return "JEventProcessor_BDXMiniCalorimeterEnergyCalibration";
	}

private:
	virtual jerror_t init(void);						///< Called once at program start.
	virtual jerror_t brun(jana::JEventLoop *eventLoop, int32_t runnumber);	///< Called everytime a new run number is detected.
	virtual jerror_t evnt(jana::JEventLoop *eventLoop, uint64_t eventnumber);	///< Called every event.
	virtual jerror_t erun(void);						///< Called everytime run number changes, provided brun has been called.
	virtual jerror_t fini(void);						///< Called after last event of last event source has been processed.


	std::map<std::pair<int,int>,int> geometry;

	int m_isFirstCallToBrun;
	JROOTOutput *m_ROOTOutput;
	int m_isMC;

};

#endif // _JEventProcessor_BDXMiniCalorimeterEnergyCalibration_

