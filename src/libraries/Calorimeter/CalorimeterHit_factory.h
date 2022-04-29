// $Id$
//
//    File: CalorimeterHit_factory.h
// Created: Wed Jan 27 16:40:00 CET 2016
// Creator: celentan (on Linux apcx4 2.6.32-504.30.3.el6.x86_64 x86_64)
//

#ifndef _CalorimeterHit_factory_
#define _CalorimeterHit_factory_

#include <JANA/JFactory.h>
#include "CalorimeterHit.h"
#include <system/CalibrationHandler.h>
#include <system/BDXFactory.h>
class TranslationTable;

class CalorimeterDigiHit;

class CalorimeterHit_factory:public BDXFactory<CalorimeterHit>{
	public:
		CalorimeterHit_factory();
		~CalorimeterHit_factory(){};


	private:
		jerror_t init(void);						///< Called once at program start.
		jerror_t brun(jana::JEventLoop *eventLoop, int32_t runnumber);	///< Called everytime a new run number is detected.
		jerror_t evnt(jana::JEventLoop *eventLoop, uint64_t eventnumber);	///< Called every event.
		jerror_t erun(void);						///< Called everytime run number changes, provided brun has been called.
		jerror_t fini(void);						///< Called after last event of last event source has been processed.

		int isMC;
		const TranslationTable *m_tt;

		int m_THR;
		int VERBOSE;

		CalibrationHandler<TranslationTable::CALO_Index_t> *m_ene;

		TranslationTable::CALO_Index_t m_channel;
		std::map<TranslationTable::CALO_Index_t,vector <const CalorimeterDigiHit*>> m_map;
		std::map<TranslationTable::CALO_Index_t,vector <const CalorimeterDigiHit*>>::iterator m_map_it;
};

#endif // _CalorimeterHit_factory_

