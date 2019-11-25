// $Id$
//
//    File: IntVetoHit_factory.cc
// Created: Wed Jan 13 21:08:09 CET 2016
// Creator: celentan (on Linux localhost.localdomain 2.6.32-504.30.3.el6.x86_64 x86_64)
//

#include <iostream>
#include <iomanip>
#include <cmath>
using namespace std;
#include "IntVetoDigiHit.h"
#include "IntVetoHit.h"
#include "IntVetoHit_factory.h"
using namespace jana;

IntVetoHit_factory::IntVetoHit_factory() :
		isMC(0) {

	m_THR_singleReadout = 5;
	m_THR_multipleReadout = 3;
	m_DT_multipleReadout = 100; //ns
	m_N_multipleReadout = 2;
	m_hit_bottom_workAround = 0;

	gPARMS->SetDefaultParameter("INTVETO:HIT_THR_SINGLE", m_THR_singleReadout, "Threshold in phe (charge) for a detector with single readout");
	gPARMS->SetDefaultParameter("INTVETO:HIT_THR_MULTI", m_THR_multipleReadout, "Threshold in phe (charge) for a detector with multi readout");
	gPARMS->SetDefaultParameter("INTVETO:HIT_DT_MULTI", m_DT_multipleReadout, "Time coincidence window (in ns) for hits in a detector with multi readout");
	gPARMS->SetDefaultParameter("INTVETO:HIT_N_MULTI", m_N_multipleReadout, "Multiplicity for a detector with multi readout");
	gPARMS->SetDefaultParameter("INTVETO:HIT_BOTTOM_WORKAROUND", m_hit_bottom_workAround, "Workaround for bottom (component==3) that is not a multi counter but 4 singles");
}

//------------------
// init
//------------------
jerror_t IntVetoHit_factory::init(void) {
	gPARMS->GetParameter("MC", isMC);

	return NOERROR;
}

//------------------
// brun
//------------------
jerror_t IntVetoHit_factory::brun(jana::JEventLoop *eventLoop, int32_t runnumber) {

	return NOERROR;
}

//------------------
// evnt
//------------------
jerror_t IntVetoHit_factory::evnt(JEventLoop *loop, uint64_t eventnumber) {

	//1: Here, we get from the framework the objects we need to process
	//1a: create vectors
	vector<const IntVetoDigiHit*> m_IntVetoDigiHits;
	vector<const IntVetoDigiHit*>::const_iterator it;
	const IntVetoDigiHit* m_IntVetoDigiHit;
	IntVetoHit *m_IntVetoHit = 0;

	double A,Q, T, Qtot,Atot, Amax,Qmax, Tmax;
	int nReadout;
	int nCountersTHR;

	//1b: retrieve IntVetoDigiHit objects
	/*This is very important!! Select - or not - the MC case*/
	if (isMC) {
		loop->Get(m_IntVetoDigiHits, "MC");
	} else {
		loop->Get(m_IntVetoDigiHits);
	}

	m_map.clear();
	for (it = m_IntVetoDigiHits.begin(); it != m_IntVetoDigiHits.end(); it++) {
		m_channel = ((*it)->m_channel);
		m_channel.readout = 0;
		m_map[m_channel].push_back(*it);
	}


	/*Now the map is full: each key is a PHYSICAL element of the detector (readout==0), and the value is a vector with IntVetoDigiHits belonging to that element*/
	for (m_map_it = m_map.begin(); m_map_it != m_map.end(); m_map_it++) {
		m_IntVetoDigiHits = m_map_it->second;
		Qmax = -9999;
		Amax = -9999;

		if (m_IntVetoDigiHits.size() == 1) { //Single readout
			m_IntVetoDigiHit = m_IntVetoDigiHits[0];
			Q = m_IntVetoDigiHit->Qphe;
			A = m_IntVetoDigiHit->Aphe;
			T = m_IntVetoDigiHit->T;


			if (Q > m_THR_singleReadout) {
				m_IntVetoHit = new IntVetoHit();
				m_IntVetoHit->m_channel = m_IntVetoDigiHit->m_channel;
				m_IntVetoHit->m_channel.readout = 0;
				m_IntVetoHit->Q = Q;
				m_IntVetoHit->T = T;
				m_IntVetoHit->A = A;
				m_IntVetoHit->AddAssociatedObject(m_IntVetoDigiHit);
				_data.push_back(m_IntVetoHit);
			}
		}
		//work-around
		else if ((m_hit_bottom_workAround) && (m_map_it->first.component == 3)) {
			for (int idigi = 0; idigi < m_IntVetoDigiHits.size(); idigi++) {
				Q = m_IntVetoDigiHits[idigi]->Qphe;
				A = m_IntVetoDigiHits[idigi]->Aphe;
				T = m_IntVetoDigiHits[idigi]->T;
				if (Q > Qmax) {
					m_IntVetoDigiHit = m_IntVetoDigiHits[idigi];
					Qmax = Q;
					Tmax = T;
					Amax = A;
				}
			}
			if (Qmax > m_THR_singleReadout) {
				m_IntVetoHit = new IntVetoHit();
				m_IntVetoHit->m_channel = m_IntVetoDigiHit->m_channel;
				m_IntVetoHit->m_channel.readout = 0;
				m_IntVetoHit->Q = Qmax;
				m_IntVetoHit->T = Tmax;
				m_IntVetoHit->A = Amax;
				m_IntVetoHit->AddAssociatedObject(m_IntVetoDigiHit);
				_data.push_back(m_IntVetoHit);
			}
		} else { /*multiCounter case*/
			nCountersTHR = 0;
			Qmax = -9999;
			Qtot = 0;
			for (int idigi = 0; idigi < m_IntVetoDigiHits.size(); idigi++) {
				Q = m_IntVetoDigiHits[idigi]->Qphe;
				A = m_IntVetoDigiHits[idigi]->Aphe;
				T = m_IntVetoDigiHits[idigi]->T;
				if (Q > Qmax) {
					Qmax = Q;
					Tmax = T;
				}
			}

			/*Now use timing too: Tmax is the time of the hit with the highest charge. See if the other counters are in coincidence with this*/
			if (Qmax < m_THR_multipleReadout) continue; //if the max charge is less than the treshold, by definition this hit is irrelevant.
			for (int idigi = 0; idigi < m_IntVetoDigiHits.size(); idigi++) {
				Q = m_IntVetoDigiHits[idigi]->Qphe;
				A = m_IntVetoDigiHits[idigi]->Aphe;
				T = m_IntVetoDigiHits[idigi]->T;
				if ((Q > m_THR_multipleReadout) && (fabs(T - Tmax) < m_DT_multipleReadout)) {
					nCountersTHR++;
					Qtot += Q;
					Atot += A;
				}
			}

			/*Ok. From this point on, nCountersTHR is the number of hits above thr, that are grouped together wrt the hit with the maximum charge*/
			if (nCountersTHR >= m_N_multipleReadout) {
				m_IntVetoHit = new IntVetoHit();
				m_IntVetoHit->m_channel = m_map_it->first;
				m_IntVetoHit->Q = Qtot;
				m_IntVetoHit->T = Tmax;
				m_IntVetoHit->A = Amax;
				for (int idigi = 0; idigi < m_IntVetoDigiHits.size(); idigi++) {
					Q = m_IntVetoDigiHits[idigi]->Qphe;
					T = m_IntVetoDigiHits[idigi]->T;
					A = m_IntVetoDigiHits[idigi]->Aphe;
					if ((Q > m_THR_multipleReadout) && (fabs(T - Tmax) < m_DT_multipleReadout)) {
						m_IntVetoHit->AddAssociatedObject(m_IntVetoDigiHits[idigi]);
					}
				}
				_data.push_back(m_IntVetoHit);
			}
		}
	} //End loop on the map

	return NOERROR;
}

//------------------
// erun
//------------------
jerror_t IntVetoHit_factory::erun(void) {
	return NOERROR;
}

//------------------
// fini
//------------------
jerror_t IntVetoHit_factory::fini(void) {
	return NOERROR;
}

