// $Id$
//
//    File: epicsData_factory.h
// Created: Fri Mar  9 11:22:49 EST 2018
// Creator: celentan (on Darwin celentano-macbook 17.4.0 i386)
//

#ifndef _epicsData_factory_
#define _epicsData_factory_

#include <JANA/JFactory.h>
#include <system/BDXFactory.h>
#include "epicsData.h"

class RunCalibrationHandler;
class epicsData_factory:public BDXFactory<epicsData>{
	public:
		epicsData_factory();
		~epicsData_factory(){};


	private:
		jerror_t init(void);						///< Called once at program start.
		jerror_t brun(jana::JEventLoop *eventLoop, int32_t runnumber);	///< Called everytime a new run number is detected.
		jerror_t evnt(jana::JEventLoop *eventLoop, uint64_t eventnumber);	///< Called every event.
		jerror_t erun(void);						///< Called everytime run number changes, provided brun has been called.
		jerror_t fini(void);						///< Called after last event of last event source has been processed.

		long int m_deltaTime; 						//this is the time (in s) added to the each EPICS entry - to account for possible mis-matches.

		RunCalibrationHandler *m_dTs;
};

#endif // _epicsData_factory_

