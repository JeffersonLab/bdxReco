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

//------------------
// init
//------------------
jerror_t IntVetoDigiHit_factory::init(void)
{
	return NOERROR;
}

//------------------
// brun
//------------------
jerror_t IntVetoDigiHit_factory::brun(jana::JEventLoop *eventLoop, int32_t runnumber)
{

	jout<<"VetoIntDigiHit_factory::brun new run number: "<<runnumber<<endl;
	m_tt=0;
	eventLoop->GetSingle(m_tt);
	if (m_tt==0){
		jerr<<" unable to get the TranslationTable from this run!"<<endl;
		return OBJECT_NOT_AVAILABLE;
	}
	return NOERROR;
}

//------------------
// evnt
//------------------
jerror_t IntVetoDigiHit_factory::evnt(JEventLoop *loop, uint64_t eventnumber)
{

	TranslationTable::ChannelInfo m_channel;
//	TranslationTable::csc_t		  m_csc;
	IntVetoDigiHit *m_IntVetoDigiHit=0;

	//1: Here, we get from the framework the objects we need to process
	//1a: create vectors
	vector <const IntVetoSiPMHit*> m_IntVetoSiPMHit;
	vector <const IntVetoSiPMHit*>::const_iterator it;

	//1b: retrieve IntVetoSiPMHit objects
	loop->Get(m_IntVetoSiPMHit);
	jout<<"GOT: "<<m_IntVetoSiPMHit.size()<<endl;

	/*Do the matching
	 *Proceed in this way:
	 * loop over the hits
	 * get the hit index, but put the readout to 0 (active detector element!)
	 * check if, in the map, a key with this detector index already exists.
	 * if not exist, add it, and create a new IntVetoDigiHit
	 * if exist, get it, and add the SiPM hit to the list of sipm hits of the IntVetoDigiHit
	 */
	m_map.clear();
	for (it=m_IntVetoSiPMHit.begin(); it != m_IntVetoSiPMHit.end() ; it++){
		m_channel = (*it)->m_channel;
		m_channel.int_veto.readout = 0;
 		m_map_it=m_map.find(m_channel.int_veto);
 		if (m_map_it == m_map.end()){ //not here. Create a new VetoIntDigiHit object, and associate the id of this SiPM hit with it
 			m_IntVetoDigiHit=new IntVetoDigiHit;
 			m_IntVetoDigiHit->m_channel=m_channel;
 			m_IntVetoDigiHit->IntVetoSIPMHit_id.push_back((*it)->id);
 			m_map.insert(std::make_pair(m_channel.int_veto,m_IntVetoDigiHit));
		}
 		else{ //element already exists. Get the VetoIntDigiHit and add this hit as id.
 			m_IntVetoDigiHit=m_map[m_channel.int_veto];
 			m_IntVetoDigiHit->IntVetoSIPMHit_id.push_back((*it)->id);
 		}
	}

	/*Now the map is full of all the hits in different active elements of active veto, i.e. with different identifiers, BUT readout, that maps the sipm hits.
	 * Each hit has a reference to the SiPM hits that made it
	 */

	for (m_map_it=m_map.begin();m_map_it!=m_map.end();m_map_it++){
		//do here further elaborations!



		_data.push_back((m_map_it)->second); //publish it
	}

	return NOERROR;
}

//------------------
// erun
//------------------
jerror_t IntVetoDigiHit_factory::erun(void)
{
	return NOERROR;
}

//------------------
// fini
//------------------
jerror_t IntVetoDigiHit_factory::fini(void)
{
	return NOERROR;
}

