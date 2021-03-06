// $Id$
//
//    File: IntVetoSummary_factory.h
// Created: Tue Mar 29 15:07:39 CEST 2016
// Creator: celentan (on Linux apcx4 2.6.32-504.30.3.el6.x86_64 x86_64)
//

#ifndef _IntVetoSummary_factory_
#define _IntVetoSummary_factory_

#include <JANA/JFactory.h>
#include "IntVetoSummary.h"

class IntVetoSummary_factory:public jana::JFactory<IntVetoSummary>{
	public:
		IntVetoSummary_factory(){};
		~IntVetoSummary_factory(){};


	private:
		jerror_t init(void);						///< Called once at program start.
		jerror_t brun(jana::JEventLoop *eventLoop, int32_t runnumber);	///< Called everytime a new run number is detected.
		jerror_t evnt(jana::JEventLoop *eventLoop, uint64_t eventnumber);	///< Called every event.
		jerror_t erun(void);						///< Called everytime run number changes, provided brun has been called.
		jerror_t fini(void);						///< Called after last event of last event source has been processed.

		std::map<int,IntVetoSummary*> m_map;
		std::map<int,IntVetoSummary*>::iterator m_it;



};

#endif // _IntVetoSummary_factory_

