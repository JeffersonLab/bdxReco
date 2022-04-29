// $Id$
//
//    File: CalorimeterSiPMHit_factory.h
// Created: Wed Jan 27 16:39:49 CET 2016
// Creator: celentan (on Linux apcx4 2.6.32-504.30.3.el6.x86_64 x86_64)
//

#ifndef _CalorimeterSiPMHit_factory_
#define _CalorimeterSiPMHit_factory_

#include <JANA/JFactory.h>
#include <system/CalibrationHandler.h>
#include <TT/TranslationTable.h>
#include <system/BDXFactory.h>
#include "CalorimeterRawDataHit.h"


class CalorimeterRawDataHit_factory:public BDXFactory<CalorimeterRawDataHit>{
	public:
		CalorimeterRawDataHit_factory():m_wb_gain(0),m_tt(0),VERBOSE(0){};
		~CalorimeterRawDataHit_factory(){};


	private:
		jerror_t init(void);						///< Called once at program start.
		jerror_t brun(jana::JEventLoop *eventLoop, int32_t runnumber);	///< Called everytime a new run number is detected.
		jerror_t evnt(jana::JEventLoop *eventLoop, uint64_t eventnumber);	///< Called every event.
		jerror_t erun(void);						///< Called everytime run number changes, provided brun has been called.
		jerror_t fini(void);						///< Called after last event of last event source has been processed.

		const TranslationTable *m_tt;


		CalibrationHandler<TranslationTable::CALO_Index_t> *m_wb_gain;

		int m_ped,m_first,m_last;


		int VERBOSE;
};

#endif // _CalorimeterSiPMHit_factory_

