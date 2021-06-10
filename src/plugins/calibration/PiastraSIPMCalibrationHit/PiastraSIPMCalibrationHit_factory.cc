// $Id$
//
//    File: PiastraSIPMCalibrationHit_factory.cc
// Created: Mon Aug 19 18:11:03 CEST 2019
// Creator: celentan (on Linux apcx4 3.10.0-957.el7.x86_64 x86_64)
//

#include <iostream>
#include <iomanip>
using namespace std;

#include "PiastraSIPMCalibrationHit_factory.h"
#include "../PiastraSIPMCalibrationHit/JFactoryGenerator_PiastraSIPMCalibrationHit.h"

#include "IntVeto/IntVetoDigiHit.h"
#include <DAQ/eventData.h>
#include <BDXmini/triggerDataBDXmini.h>

using namespace jana;

// Routine used to create our JEventProcessor
#include <JANA/JApplication.h>
extern "C" {
void InitPlugin(JApplication *app) {
	InitJANAPlugin(app);
	app->AddFactoryGenerator(new JFactoryGenerator_PiastraSIPMCalibrationHit());
}
} // "C"

//------------------
// init
//------------------
jerror_t PiastraSIPMCalibrationHit_factory::init(void) {
	m_isMC = 0;
	gPARMS->GetParameter("MC", m_isMC);

	m_Tmin = 50;
	gPARMS->SetDefaultParameter("PiastraSIPMCalibrationHit_factory:TMIN", m_Tmin, "Minimum signal time (ns) for SiPM in the readout window");

	m_Tmax = 200;
	gPARMS->SetDefaultParameter("PiastraSIPMCalibrationHit_factory:TMAX", m_Tmax, "Maximum signal time (ns) for SiPM in the readout window");

	m_doCoinc = 0;
	gPARMS->SetDefaultParameter("PiastraSIPMCalibrationHit_factory:DO_COINC", m_doCoinc, "Do 4-fold coincidence (100 ns) to select events");

	m_deltaCoinc = 100;
	return NOERROR;
}

//------------------
// brun
//------------------
jerror_t PiastraSIPMCalibrationHit_factory::brun(jana::JEventLoop *eventLoop, int32_t runnumber) {
	return NOERROR;
}

//------------------
// evnt
//------------------
jerror_t PiastraSIPMCalibrationHit_factory::evnt(JEventLoop *loop, uint64_t eventnumber) {

	const IntVetoDigiHit *m_IntVetoDigiHit;
	vector<const IntVetoDigiHit*> intveto_hits;
	vector<const IntVetoDigiHit*>::const_iterator intveto_hits_it;

	const eventData *eData;
	const triggerDataBDXmini *tData;

	PiastraSIPMCalibrationHit *m_PiastraSIPMCalibrationHit;

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
	bool flagCoinc = false;
	if (m_doCoinc) {
		double T[4] = { -999, -999, 999, 999 };
		for (intveto_hits_it = intveto_hits.begin(); intveto_hits_it != intveto_hits.end(); intveto_hits_it++) {
			m_IntVetoDigiHit = *intveto_hits_it;
			if ((m_IntVetoDigiHit->T > m_Tmax) || (m_IntVetoDigiHit->T < m_Tmin)) continue;
			T[m_IntVetoDigiHit->m_channel.component]=m_IntVetoDigiHit->T;
		}
		if ((fabs(T[0] - T[1]) < m_deltaCoinc) && (fabs(T[0] - T[2]) < m_deltaCoinc) && (fabs(T[0] - T[3]) < m_deltaCoinc) && (fabs(T[1] - T[2]) < m_deltaCoinc) && (fabs(T[1] - T[3]) < m_deltaCoinc) && (fabs(T[2] - T[3]) < m_deltaCoinc)) {
			flagCoinc = true;
		}
	} else {
		flagCoinc = true;
	}

	if (!flagCoinc) {
		return NOERROR;
	}
	for (intveto_hits_it = intveto_hits.begin(); intveto_hits_it != intveto_hits.end(); intveto_hits_it++) {

		m_IntVetoDigiHit = *intveto_hits_it;

		//select only the IntVetoDigiHits well centered in the readout window
		if ((m_IntVetoDigiHit->T > m_Tmax) || (m_IntVetoDigiHit->T < m_Tmin)) continue;

		m_PiastraSIPMCalibrationHit = new PiastraSIPMCalibrationHit;
		m_PiastraSIPMCalibrationHit->m_channel = m_IntVetoDigiHit->m_channel;
		m_PiastraSIPMCalibrationHit->Qraw = m_IntVetoDigiHit->Qraw;
		m_PiastraSIPMCalibrationHit->Qphe = m_IntVetoDigiHit->Qphe;
		m_PiastraSIPMCalibrationHit->Araw = m_IntVetoDigiHit->Araw;
		m_PiastraSIPMCalibrationHit->Aphe = m_IntVetoDigiHit->Aphe;
		m_PiastraSIPMCalibrationHit->T = m_IntVetoDigiHit->T;

		_data.push_back(m_PiastraSIPMCalibrationHit);
	}

	return NOERROR;
}

//------------------
// erun
//------------------
jerror_t PiastraSIPMCalibrationHit_factory::erun(void) {
	return NOERROR;
}

//------------------
// fini
//------------------
jerror_t PiastraSIPMCalibrationHit_factory::fini(void) {
	return NOERROR;
}

