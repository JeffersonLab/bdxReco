// $Id$
//
//    File: CalorimeterHit_factory.cc
// Created: Wed Jan 27 16:40:00 CET 2016
// Creator: celentan (on Linux apcx4 2.6.32-504.30.3.el6.x86_64 x86_64)
//

#include <iostream>
#include <iomanip>
using namespace std;

#include "CalorimeterHit_factory.h"

//objects we need from the framework
#include <TT/TranslationTable.h>
#include <Calorimeter/CalorimeterDigiHit.h>
//objects we put in the framework
#include <Calorimeter/CalorimeterHit.h>

using namespace jana;

CalorimeterHit_factory::CalorimeterHit_factory() :
		m_ene(0), m_tt(0) {
	VERBOSE = 0;
	isMC = 0;
	m_THR=0;
	gPARMS->SetDefaultParameter("CALORIMETER:HIT_THR", m_THR, "Threshold in charge");
	gPARMS->GetParameter("CALORIMETER:VERBOSE", VERBOSE);

}

//------------------
// init
//------------------
jerror_t CalorimeterHit_factory::init(void) {

	m_ene = new CalibrationHandler<TranslationTable::CALO_Index_t>("/Calorimeter/Ene");
	this->mapCalibrationHandler(m_ene);

	gPARMS->GetParameter("MC", isMC);
	return NOERROR;
}

//------------------
// brun
//------------------
jerror_t CalorimeterHit_factory::brun(jana::JEventLoop *eventLoop, int32_t runnumber) {

	this->updateCalibrationHandler(m_ene, eventLoop);

	if (VERBOSE > 3) {
		std::map<TranslationTable::CALO_Index_t, std::vector<double> > gainCalibMap;
		std::map<TranslationTable::CALO_Index_t, std::vector<double> >::iterator gainCalibMap_it;
		gainCalibMap = m_ene->getCalibMap();
		jout << "Got following ene for run number: " << runnumber << endl;
		jout << "Rows: " << gainCalibMap.size() << endl;
		for (gainCalibMap_it = gainCalibMap.begin(); gainCalibMap_it != gainCalibMap.end(); gainCalibMap_it++) {
			jout << gainCalibMap_it->first.sector << " " << gainCalibMap_it->first.x << " " << gainCalibMap_it->first.y << " " << gainCalibMap_it->first.readout << " " << gainCalibMap_it->second[0] << " " << gainCalibMap_it->second[1] << endl;
		}
	}

	return NOERROR;
}

//------------------
// evnt
//------------------
jerror_t CalorimeterHit_factory::evnt(JEventLoop *loop, uint64_t eventnumber) {

	//1: Here, we get from the framework the objects we need to process
	//1a: create vectors
	vector<const CalorimeterDigiHit*> m_CalorimeterDigiHits;
	vector<const CalorimeterDigiHit*>::const_iterator it;

	const CalorimeterDigiHit *m_CalorimeterDigiHit;
	CalorimeterHit *m_CalorimeterHit = 0;

	int countOk;
	double Q, T, Qtot, Qmax, Tmax;
	double gain, ped;

	if (isMC) {
		loop->Get(m_CalorimeterDigiHits, "MC");
	} else {
		loop->Get(m_CalorimeterDigiHits);
	}
	/*Do the matching
	 */
	m_map.clear();
	for (it = m_CalorimeterDigiHits.begin(); it != m_CalorimeterDigiHits.end(); it++) {
		m_channel = ((*it)->m_channel);
		m_channel.readout = 0;
		m_map[m_channel].push_back(*it);
	}

	/*Now the map is full of all the hits in different active elements of calorimeter, i.e. with different identifiers, BUT readout, that maps the sipm hits.
	 * Each hit has a reference to the digi hits that made it
	 */
	vector<const CalorimeterDigiHit*> m_CalorimeterDigiHit_tmp;
	for (m_map_it = m_map.begin(); m_map_it != m_map.end(); m_map_it++) {

		m_CalorimeterDigiHit_tmp = m_map_it->second;

		//Do some processing
		if (m_CalorimeterDigiHit_tmp.size() == 1) { //single-ch readout
			m_CalorimeterDigiHit = m_CalorimeterDigiHit_tmp[0];
			Q = m_CalorimeterDigiHit->Q;
			T = m_CalorimeterDigiHit->T;
			if (Q > m_THR) {
				m_CalorimeterHit = new CalorimeterHit();
				m_CalorimeterHit->m_channel = m_CalorimeterDigiHit->m_channel;
				m_CalorimeterHit->m_channel.readout = 0;
				m_CalorimeterHit->T = T;
				m_CalorimeterHit->Eraw = Q;
				m_CalorimeterHit->A = m_CalorimeterDigiHit->A;
				m_CalorimeterHit->RMSflag = m_CalorimeterDigiHit->RMSflag;

				/*Try to calibrate in energy and ped-sub*/
				gain = m_ene->getCalib(m_CalorimeterHit->m_channel)[0];
				ped = m_ene->getCalib(m_CalorimeterHit->m_channel)[1];
				m_CalorimeterHit->E = (Q - ped);
				if (gain != 0) {
					m_CalorimeterHit->E /= gain;
				}
				m_CalorimeterHit->AddAssociatedObject(m_CalorimeterDigiHit);
				_data.push_back(m_CalorimeterHit); //publish it
			}
		}
	}

	return NOERROR;
}

//------------------
// erun
//------------------
jerror_t CalorimeterHit_factory::erun(void) {

	this->clearCalibrationHandler(m_ene);

	return NOERROR;
}

//------------------
// fini
//------------------
jerror_t CalorimeterHit_factory::fini(void) {
	return NOERROR;
}

