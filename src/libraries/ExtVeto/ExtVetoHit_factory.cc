// $Id$
//
//    File: ExtVetoHit_factory.cc
// Created: Wed Jan 13 21:06:44 CET 2016
// Creator: celentan (on Linux localhost.localdomain 2.6.32-504.30.3.el6.x86_64 x86_64)
//


#include <iostream>
#include <iomanip>
using namespace std;

#include "ExtVetoDigiHit.h"
//#include "ExtVetoHit.h"

#include "ExtVetoHit_factory.h"
#include <TT/TranslationTable.h>

using namespace jana;

//------------------
// init
//------------------
jerror_t ExtVetoHit_factory::init(void)
{
	gPARMS->GetParameter("MC",isMC);

	m_ENE_gain=new CalibrationHandler<TranslationTable::EXT_VETO_Index_t>("/ExtVeto/Ene");
	this->mapCalibrationHandler(m_ENE_gain);

	return NOERROR;
}

//------------------
// brun
//------------------
jerror_t ExtVetoHit_factory::brun(jana::JEventLoop *eventLoop, int32_t runnumber)
{


	this->updateCalibrationHandler(m_ENE_gain,eventLoop);

	return NOERROR;
}

//------------------
// evnt
//------------------
jerror_t ExtVetoHit_factory::evnt(JEventLoop *loop, uint64_t eventnumber)
{


	//1: Here, we get from the framework the objects we need to process
	//1a: create vectors
	vector <const ExtVetoDigiHit*> m_ExtVetoDigiHits;
	vector <const ExtVetoDigiHit*>::const_iterator it;
	const ExtVetoDigiHit* m_ExtVetoDigiHit;

	double Q,Qmax,m_EneCalib;

	ExtVetoHit *m_ExtVetoHit=0;

	//1b: retrieve ExtVetoDigiHit objects

	/*This is very important!! Select - or not - the MC case*/
	if (isMC){
		loop->Get(m_ExtVetoDigiHits,"MC");
	}
	else{
		loop->Get(m_ExtVetoDigiHits);
	}


	m_map.clear();
	for (it=m_ExtVetoDigiHits.begin(); it != m_ExtVetoDigiHits.end() ; it++){
		m_channel = ((*it)->m_channel);
		m_channel.readout = 0;
		m_map[m_channel].push_back(*it);
	}


	for (m_map_it=m_map.begin();m_map_it!=m_map.end();m_map_it++){
			//do here further elaborations!
			//Compute the charge as the sum of the charges
			//Compute the hit-time as time of the PMT-hit with largest charge
			m_ExtVetoDigiHits=m_map_it->second;
			m_ExtVetoHit=new ExtVetoHit();
			m_ExtVetoHit->m_channel=m_map_it->first;
			m_ExtVetoHit->E=0;
			Qmax=-9999;
			Q=0;
			for (int ihit=0;ihit<m_ExtVetoDigiHits.size();ihit++){
				Q+=m_ExtVetoDigiHits[ihit]->Q;
				if (m_ExtVetoDigiHits[ihit]->Q>Qmax){
					Qmax=m_ExtVetoDigiHits[ihit]->Q;
					m_ExtVetoHit->T=m_ExtVetoDigiHits[ihit]->T;
				}
			}
			m_EneCalib=m_ENE_gain->getCalibSingle(m_ExtVetoHit->m_channel);
			m_ExtVetoHit->E=Q*m_EneCalib;
			_data.push_back(m_ExtVetoHit); //publish it
		}




	return NOERROR;
}

//------------------
// erun
//------------------
jerror_t ExtVetoHit_factory::erun(void)
{

	this->clearCalibrationHandler(m_ENE_gain);

	return NOERROR;
}

//------------------
// fini
//------------------
jerror_t ExtVetoHit_factory::fini(void)
{
	return NOERROR;
}

