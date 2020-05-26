// $Id$
//
//    File: VetoIntDigiHit_factory_BDXmini.h
// Created: Tue Jan 12 11:52:41 CET 2016
// Creator: celentan (on Linux apcx4 2.6.32-504.30.3.el6.x86_64 x86_64)
//

#ifndef _VetoIntSIPMHit_factory_BDXmini_
#define _VetoIntSIPMHit_factory_BDXmini_

#include <JANA/JFactory.h>
#include <system/BDXFactory.h>
#include "IntVetoSiPMHit.h"
#include <system/CalibrationHandler.h>

class IntVetofa250Converter;





class IntVetoSiPMHit_factory_BDXmini:public BDXFactory<IntVetoSiPMHit>{
	public:
		IntVetoSiPMHit_factory_BDXmini();
		~IntVetoSiPMHit_factory_BDXmini(){};

		const char* Tag(void){return "BDXmini";}

	private:
		jerror_t init(void);						///< Called once at program start.
		jerror_t brun(jana::JEventLoop *eventLoop, int32_t runnumber);	///< Called everytime a new run number is detected.
		jerror_t evnt(jana::JEventLoop *eventLoop, uint64_t eventnumber);	///< Called every event.
		jerror_t erun(void);						///< Called everytime run number changes, provided brun has been called.
		jerror_t fini(void);						///< Called after last event of last event source has been processed.

		const TranslationTable *m_tt;
		const IntVetofa250Converter *m_intVetofa250Converter;

		CalibrationHandler<TranslationTable::INT_VETO_Index_t> *m_sipm_gain;
		CalibrationHandler<TranslationTable::INT_VETO_Index_t> *m_sipm_ampl;

		int VERBOSE;

};

#endif // _VetoIntDigiHit_factory_BDXmini_

