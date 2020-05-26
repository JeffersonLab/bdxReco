// $Id$
//
//    File: IntVetofa250Converter_factory.h
// Created: Mon Jan 25 17:02:27 CET 2016
// Creator: celentan (on Linux apcx4 2.6.32-504.30.3.el6.x86_64 x86_64)
//

#ifndef _IntVetofa250Converter_factory_
#define _IntVetofa250Converter_factory_

#include <JANA/JFactory.h>
#include "IntVetofa250Converter.h"
#include <system/BDXFactory.h>

class IntVetofa250Converter_factory:public BDXFactory<IntVetofa250Converter>{
	public:
		IntVetofa250Converter_factory();
		~IntVetofa250Converter_factory(){};


	private:
		jerror_t init(void);						///< Called once at program start.
		jerror_t brun(jana::JEventLoop *eventLoop, int32_t runnumber);	///< Called everytime a new run number is detected.
		jerror_t evnt(jana::JEventLoop *eventLoop, uint64_t eventnumber);	///< Called every event.
		jerror_t erun(void);						///< Called everytime run number changes, provided brun has been called.
		jerror_t fini(void);						///< Called after last event of last event source has been processed.

		int m_isFirstCallToBrun;
		IntVetofa250Converter *m_intVetofa250Converter;

		double m_minTot,m_thr;
		int m_NSB,m_NSA,m_NPED;
		double m_RMSTHRscale;
		double m_TOTTHR;
};

#endif // _IntVetofa250Converter_factory_

