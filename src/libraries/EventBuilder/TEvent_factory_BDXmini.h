// $Id$
//
//    File: TEvent_factory_BDXmini.h
// Created: Tue Nov  7 18:43:32 CET 2017
// Creator: celentan (on Linux apcx4 2.6.32-696.13.2.el6.x86_64 x86_64)
//

#ifndef _TEvent_factory_BDXmini_
#define _TEvent_factory_BDXmini_

#include <JANA/JFactory.h>
#include "TEvent.h"

class TEvent_factory_BDXmini: public jana::JFactory<TEvent> {
public:
	TEvent_factory_BDXmini();
	~TEvent_factory_BDXmini() {
	}
	;
	const char* Tag(void) {
		return "BDXmini";
	}

private:
	jerror_t init(void);						///< Called once at program start.
	jerror_t brun(jana::JEventLoop *eventLoop, int32_t runnumber);	///< Called everytime a new run number is detected.
	jerror_t evnt(jana::JEventLoop *eventLoop, uint64_t eventnumber);	///< Called every event.
	jerror_t erun(void);						///< Called everytime run number changes, provided brun has been called.
	jerror_t fini(void);						///< Called after last event of last event source has been processed.

	TClonesArray *m_CaloHits;
	TClonesArray *m_IntVetoHits;
	TClonesArray *m_fa250Mode1CalibPedSubHits;

#ifdef MC_SUPPORT_ENABLE
	TClonesArray *m_GenParticles;
	TClonesArray *m_CaloMCRealHits;
#endif

	int m_isMC, m_MCRunNumber;
	std::string m_tag;

	int m_ADD_TRIGGER_WORDS;
	int m_thrNpheVeto;
	double m_thrEneTot;

	double m_thrCrosscorrSinTotMin,	m_thrCrosscorrSinTotMax;
	double m_thrCrosscorrSignTotMin, m_thrCrosscorrSignTotMax;
	double m_thrCrosscorrSinCaloMin, m_thrCrosscorrSinCaloMax;
	double m_thrCrosscorrSignCaloMin ,m_thrCrosscorrSignCaloMax;
	double m_thrCrosscorrSin5mVMin,m_thrCrosscorrSin5mVMax;
	double m_thrCrosscorrSign5mVMin,m_thrCrosscorrSign5mVMax;
	double m_thrCrosscorrSinCalo5mVMin, m_thrCrosscorrSinCalo5mVMax;
	double m_thrCrosscorrSignCalo5mVMin, m_thrCrosscorrSignCalo5mVMax;
};

#endif // _TEvent_factory_BDXmini_

