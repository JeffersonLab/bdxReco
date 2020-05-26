// $Id$
//
//    File: IntVetoDigiHit_factory.h
// Created: Wed Jan 20 16:42:38 CET 2016
// Creator: celentan (on Linux apcx4 2.6.32-504.30.3.el6.x86_64 x86_64)
//

#ifndef _IntVetoDigiHit_factory_
#define _IntVetoDigiHit_factory_

#include <JANA/JFactory.h>
#include "IntVetoDigiHit.h"
#include <TT/TranslationTable.h>

class IntVetoDigiHit_factory:public jana::JFactory<IntVetoDigiHit>{
	public:
		IntVetoDigiHit_factory();
		~IntVetoDigiHit_factory(){};


	private:
		jerror_t init(void);						///< Called once at program start.
		jerror_t brun(jana::JEventLoop *eventLoop, int32_t runnumber);	///< Called everytime a new run number is detected.
		jerror_t evnt(jana::JEventLoop *eventLoop, uint64_t eventnumber);	///< Called every event.
		jerror_t erun(void);						///< Called everytime run number changes, provided brun has been called.
		jerror_t fini(void);						///< Called after last event of last event source has been processed.

		const TranslationTable *m_tt;

		int USE_TOT_BDXMINI;
};

#endif // _IntVetoDigiHit_factory_

