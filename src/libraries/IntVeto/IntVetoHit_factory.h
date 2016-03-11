// $Id$
//
//    File: IntVetoHit_factory.h
// Created: Wed Jan 13 21:08:09 CET 2016
// Creator: celentan (on Linux localhost.localdomain 2.6.32-504.30.3.el6.x86_64 x86_64)
//

#ifndef _IntVetoHit_factory_
#define _IntVetoHit_factory_

#include <JANA/JFactory.h>
#include "IntVetoHit.h"
#include <system/CalibrationHandler.h>

class IntVetoHit_factory:public jana::JFactory<IntVetoHit>{
	public:
		IntVetoHit_factory(){isMC=0;};
		~IntVetoHit_factory(){};


	private:
		int isMC;
		int VERBOSE;
		jerror_t init(void);						///< Called once at program start.
		jerror_t brun(jana::JEventLoop *eventLoop, int32_t runnumber);	///< Called everytime a new run number is detected.
		jerror_t evnt(jana::JEventLoop *eventLoop, uint64_t eventnumber);	///< Called every event.
		jerror_t erun(void);						///< Called everytime run number changes, provided brun has been called.
		jerror_t fini(void);						///< Called after last event of last event source has been processed.


		CalibrationHandler<TranslationTable::INT_VETO_Index_t> m_ene;
};

#endif // _IntVetoHit_factory_

