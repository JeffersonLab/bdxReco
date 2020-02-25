// $Id$
//
//    File: BDXMiniVetoSIPMCalibrationHit_factory.cc
// Created: Mon Aug 19 18:11:03 CEST 2019
// Creator: celentan (on Linux apcx4 3.10.0-957.el7.x86_64 x86_64)
//

#include <iostream>
#include <iomanip>
using namespace std;

#include "BDXMiniVetoSIPMCalibrationHit_factory.h"
#include "JFactoryGenerator_BDXMiniVetoSIPMCalibrationHit.h"

#include "IntVeto/IntVetoDigiHit.h"
#include <DAQ/eventData.h>
#include <BDXmini/triggerDataBDXmini.h>

using namespace jana;

// Routine used to create our JEventProcessor
#include <JANA/JApplication.h>
extern "C" {
void InitPlugin(JApplication *app) {
	InitJANAPlugin(app);
	app->AddFactoryGenerator(new JFactoryGenerator_BDXMiniVetoSIPMCalibrationHit());
}
} // "C"

//------------------
// init
//------------------
jerror_t BDXMiniVetoSIPMCalibrationHit_factory::init(void) {
	m_isMC = 0;
	gPARMS->GetParameter("MC", m_isMC);

	m_Tmin = 100;
	gPARMS->SetDefaultParameter("BDXMiniVetoSIPMCalibrationHit_factory:TMIN", m_Tmin, "Minimum signal time (ns) for SiPM in the readout window");

	m_Tmax = 400;
	gPARMS->SetDefaultParameter("BDXMiniVetoSIPMCalibrationHit_factory:TMAX", m_Tmax, "Maximum signal time (ns) for SiPM in the readout window");

	return NOERROR;
}

//------------------
// brun
//------------------
jerror_t BDXMiniVetoSIPMCalibrationHit_factory::brun(jana::JEventLoop *eventLoop, int32_t runnumber) {
	return NOERROR;
}

//------------------
// evnt
//------------------
jerror_t BDXMiniVetoSIPMCalibrationHit_factory::evnt(JEventLoop *loop, uint64_t eventnumber) {

	const IntVetoDigiHit* m_IntVetoDigiHit;
	vector<const IntVetoDigiHit*> intveto_hits;
	vector<const IntVetoDigiHit*>::const_iterator intveto_hits_it;

	const eventData* eData;
	const triggerDataBDXmini* tData;

	BDXMiniVetoSIPMCalibrationHit *m_BDXMiniVetoSIPMCalibrationHit;

	/*Get objects from JANA factories*/
	if (m_isMC) {
		loop->Get(intveto_hits, "MC");
	} else {
		loop->Get(intveto_hits);
	}



	/*The following happens for EPICS events*/
	if (!m_isMC) {
		try {
			loop->GetSingle(tData);
		} catch (unsigned long e) {
			return OBJECT_NOT_AVAILABLE;
		}

		try {
			loop->GetSingle(eData);
		} catch (unsigned long e) {
			return OBJECT_NOT_AVAILABLE;
		}
	}

	/*Do a trigger selection to select only randoms?*/
	//(tData->getNtriggers_single(31)>0)

	for (intveto_hits_it = intveto_hits.begin(); intveto_hits_it != intveto_hits.end(); intveto_hits_it++) {

		m_IntVetoDigiHit = *intveto_hits_it;

		//select only the IntVetoDigiHits well centered in the readout window
		if ((m_IntVetoDigiHit->T > m_Tmax) || (m_IntVetoDigiHit->T < m_Tmin)) continue;

		m_BDXMiniVetoSIPMCalibrationHit = new BDXMiniVetoSIPMCalibrationHit;
		m_BDXMiniVetoSIPMCalibrationHit->m_channel = m_IntVetoDigiHit->m_channel;
		m_BDXMiniVetoSIPMCalibrationHit->Qraw = m_IntVetoDigiHit->Qraw;
		m_BDXMiniVetoSIPMCalibrationHit->Qphe = m_IntVetoDigiHit->Qphe;
		m_BDXMiniVetoSIPMCalibrationHit->Araw = m_IntVetoDigiHit->Araw;
		m_BDXMiniVetoSIPMCalibrationHit->Aphe = m_IntVetoDigiHit->Aphe;
		m_BDXMiniVetoSIPMCalibrationHit->T = m_IntVetoDigiHit->T;

		_data.push_back(m_BDXMiniVetoSIPMCalibrationHit);
	}

	return NOERROR;
}

//------------------
// erun
//------------------
jerror_t BDXMiniVetoSIPMCalibrationHit_factory::erun(void) {
	return NOERROR;
}

//------------------
// fini
//------------------
jerror_t BDXMiniVetoSIPMCalibrationHit_factory::fini(void) {
	return NOERROR;
}

