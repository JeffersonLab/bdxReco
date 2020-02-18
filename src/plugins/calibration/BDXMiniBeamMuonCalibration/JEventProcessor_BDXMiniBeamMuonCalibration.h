// $Id$
//
//    File: JEventProcessor_BDXMiniBeamMuonCalibration.h
// Created: Tue Feb 18 16:14:29 CET 2020
// Creator: celentan (on Linux apcx4 3.10.0-957.el7.x86_64 x86_64)
//

#ifndef _JEventProcessor_BDXMiniBeamMuonCalibration_
#define _JEventProcessor_BDXMiniBeamMuonCalibration_

#include <JANA/JEventProcessor.h>

class JEventProcessor_BDXMiniBeamMuonCalibration:public jana::JEventProcessor{
	public:
		JEventProcessor_BDXMiniBeamMuonCalibration();
		~JEventProcessor_BDXMiniBeamMuonCalibration();
		const char* className(void){return "JEventProcessor_BDXMiniBeamMuonCalibration";}

	private:
		jerror_t init(void);						///< Called once at program start.
		jerror_t brun(jana::JEventLoop *eventLoop, int32_t runnumber);	///< Called everytime a new run number is detected.
		jerror_t evnt(jana::JEventLoop *eventLoop, uint64_t eventnumber);	///< Called every event.
		jerror_t erun(void);						///< Called everytime run number changes, provided brun has been called.
		jerror_t fini(void);						///< Called after last event of last event source has been processed.
};

#endif // _JEventProcessor_BDXMiniBeamMuonCalibration_

