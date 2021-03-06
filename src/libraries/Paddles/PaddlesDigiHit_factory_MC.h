// $Id$
//
//    File: PaddlesDigiHit_factory_MC.h
// Created: Tue Mar  8 13:31:20 CET 2016
// Creator: celentan (on Linux apcx4 2.6.32-504.30.3.el6.x86_64 x86_64)
//

#ifndef _PaddlesDigiHit_factory_MC_
#define _PaddlesDigiHit_factory_MC_

#include <JANA/JFactory.h>
#include "PaddlesDigiHit.h"

#include <map>
#include <utility>

class PaddlesDigiHit_factory_MC:public jana::JFactory<PaddlesDigiHit>{
	public:
		PaddlesDigiHit_factory_MC(){};
		~PaddlesDigiHit_factory_MC(){};
		const char* Tag(void){return "MC";}

	private:
		jerror_t init(void);						///< Called once at program start.
		jerror_t brun(jana::JEventLoop *eventLoop, int32_t runnumber);	///< Called everytime a new run number is detected.
		jerror_t evnt(jana::JEventLoop *eventLoop, uint64_t eventnumber);	///< Called every event.
		jerror_t erun(void);						///< Called everytime run number changes, provided brun has been called.
		jerror_t fini(void);						///< Called after last event of last event source has been processed.

		/*This code is here because MC could generate more than 1 hit per sector!*/
		/*The key is the way MC is organized: sector - channel (where sector and channel have a different meaning than in the real data!!!)*/
		std::map<std::pair<int,int>,PaddlesDigiHit*> m_map;
		std::map<std::pair<int,int>,PaddlesDigiHit*>::iterator m_map_it;
};

#endif // _PaddlesDigiHit_factory_MC_

