// $Id$
//
//    File: fa250Mode1CalibHit_factory.h
// Created: Wed Feb  3 07:38:15 EST 2016
// Creator: davidl (on Darwin harriet.jlab.org 13.4.0 i386)
//

#ifndef _fa250Mode1CalibHit_factory_
#define _fa250Mode1CalibHit_factory_

#include <JANA/JFactory.h>
#include "fa250Mode1CalibHit.h"

class fa250Mode1CalibHit_factory:public jana::JFactory<fa250Mode1CalibHit>{
	public:
		fa250Mode1CalibHit_factory():LSB(0){};
		~fa250Mode1CalibHit_factory(){};

		double LSB;

	private:
		jerror_t init(void);						///< Called once at program start.
		jerror_t brun(jana::JEventLoop *eventLoop, int32_t runnumber);	///< Called everytime a new run number is detected.
		jerror_t evnt(jana::JEventLoop *eventLoop, uint64_t eventnumber);	///< Called every event.
		jerror_t erun(void);						///< Called everytime run number changes, provided brun has been called.
		jerror_t fini(void);						///< Called after last event of last event source has been processed.
};

#endif // _fa250Mode1CalibHit_factory_

