// $Id$
//
//    File: PiastraSIPMCalibrationHit_factory.h
// Created: Mon Aug 19 18:11:03 CEST 2019
// Creator: celentan (on Linux apcx4 3.10.0-957.el7.x86_64 x86_64)
//

#ifndef _PiastraSIPMCalibrationHit_factory_
#define _PiastraSIPMCalibrationHit_factory_

#include <JANA/JFactory.h>
#include <TT/TranslationTable.h>
#include "PiastraSIPMCalibrationHit.h"

class PiastraSIPMCalibrationHit_factory: public jana::JFactory<PiastraSIPMCalibrationHit> {
public:
	PiastraSIPMCalibrationHit_factory() {
	}
	;
	~PiastraSIPMCalibrationHit_factory() {
	}
	;

private:
	jerror_t init(void);						///< Called once at program start.
	jerror_t brun(jana::JEventLoop *eventLoop, int32_t runnumber);	///< Called everytime a new run number is detected.
	jerror_t evnt(jana::JEventLoop *eventLoop, uint64_t eventnumber);	///< Called every event.
	jerror_t erun(void);						///< Called everytime run number changes, provided brun has been called.
	jerror_t fini(void);						///< Called after last event of last event source has been processed.

	int m_isMC;
	int m_doCoinc;
	double m_Tmin,m_Tmax;
	double m_deltaCoinc;

};

#endif // _PiastraSIPMCalibrationHit_factory_

