// $Id$
//
//    File: CataniaEvent_factory.h
// Created: Mon Apr 25 23:25:44 CEST 2016
// Creator: celentan (on Linux dhcp-visitor-enr-116-196.slac.stanford.edu 2.6.32-573.12.1.el6.x86_64 x86_64)
//

#ifndef _MCEvent_factory_
#define _MCEvent_factory_

#include <JANA/JFactory.h>
#include "MCEvent.h"

class MCEvent_factory:public jana::JFactory<MCEvent>{
	public:
		MCEvent_factory(){};
		~MCEvent_factory(){};


	private:
		jerror_t init(void);						///< Called once at program start.
		jerror_t brun(jana::JEventLoop *eventLoop, int32_t runnumber);	///< Called everytime a new run number is detected.
		jerror_t evnt(jana::JEventLoop *eventLoop, uint64_t eventnumber);	///< Called every event.
		jerror_t erun(void);						///< Called everytime run number changes, provided brun has been called.
		jerror_t fini(void);						///< Called after last event of last event source has been processed.
		double random;

	public:

		double m_EC2_cut;					   //cut on the EC2 energy.
		double m_ExtVeto_timeWindows;          //cut on the ExtVeto time - coincidence with the crystal
		double m_IntVeto_timeWindows;          //cut on the IntVeto time - coincidence with the crystal
};

#endif // _MCEvent_factory_

