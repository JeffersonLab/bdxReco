// $Id$
//
//    File: IntVetoDigiHit_factory.cc
// Created: Wed Jan 20 16:42:38 CET 2016
// Creator: celentan (on Linux apcx4 2.6.32-504.30.3.el6.x86_64 x86_64)
//

#include <iostream>
#include <iomanip>
#include <map>
using namespace std;

#include "IntVetoDigiHit_factory.h"

#include <IntVeto/IntVetoSiPMHit.h>
#include <TT/TranslationTable.h>

using namespace jana;

IntVetoDigiHit_factory::IntVetoDigiHit_factory() {
	USE_TOT_BDXMINI = 0;
	m_tt = 0;
	gPARMS->SetDefaultParameter("INTVETO:USE_TOT_BDXMINI", USE_TOT_BDXMINI, "USE TOT for INTVETO");
}

//------------------
// init
//------------------
jerror_t IntVetoDigiHit_factory::init(void) {
	return NOERROR;
}

//------------------
// brun
//------------------
jerror_t IntVetoDigiHit_factory::brun(jana::JEventLoop *eventLoop, int32_t runnumber) {

	jout << "IntVetoDigiHit_factory::brun new run number: " << runnumber << endl;
	m_tt = 0;
	eventLoop->GetSingle(m_tt);
	if (m_tt == 0) {
		jerr << " unable to get the TranslationTable from this run!" << endl;
		return OBJECT_NOT_AVAILABLE;
	}
	return NOERROR;
}

//------------------
// evnt
//------------------
jerror_t IntVetoDigiHit_factory::evnt(JEventLoop *loop, uint64_t eventnumber) {

	TranslationTable::INT_VETO_Index_t m_channel;
	IntVetoDigiHit *m_IntVetoDigiHit = 0;

	//1: Here, we get from the framework the objects we need to process
	//1a: create vectors
	vector<const IntVetoSiPMHit*> m_IntVetoSiPMHits;
	vector<const IntVetoSiPMHit*>::const_iterator it;
	const IntVetoSiPMHit* m_IntVetoSiPMHit;
	//1b: retrieve IntVetoSiPMHit objects
	if (USE_TOT_BDXMINI) {
		loop->Get(m_IntVetoSiPMHits,"BDXmini");
	} else {
		loop->Get(m_IntVetoSiPMHits);
	}
	for (it = m_IntVetoSiPMHits.begin(); it != m_IntVetoSiPMHits.end(); it++) {
		m_IntVetoSiPMHit = *it;
		m_channel = *(m_IntVetoSiPMHit->m_channel.int_veto);

		m_IntVetoDigiHit = new IntVetoDigiHit;
		m_IntVetoDigiHit->m_channel = m_channel;
		m_IntVetoDigiHit->Qphe = m_IntVetoSiPMHit->Qphe;
		m_IntVetoDigiHit->Qraw = m_IntVetoSiPMHit->Qraw;
		m_IntVetoDigiHit->Araw = m_IntVetoSiPMHit->Araw;
		m_IntVetoDigiHit->Aphe = m_IntVetoSiPMHit->Aphe;
		m_IntVetoDigiHit->T = m_IntVetoSiPMHit->T;

		m_IntVetoDigiHit->pedMean = m_IntVetoSiPMHit->pedMean;
		m_IntVetoDigiHit->pedRMS = m_IntVetoSiPMHit->pedRMS;
		m_IntVetoDigiHit->RMSflag = m_IntVetoSiPMHit->RMSflag;

		m_IntVetoDigiHit->AddAssociatedObject(m_IntVetoSiPMHit);

		_data.push_back(m_IntVetoDigiHit); //publish it
	}

	return NOERROR;
}

//------------------
// erun
//------------------
jerror_t IntVetoDigiHit_factory::erun(void) {
	return NOERROR;
}

//------------------
// fini
//------------------
jerror_t IntVetoDigiHit_factory::fini(void) {
	return NOERROR;
}

