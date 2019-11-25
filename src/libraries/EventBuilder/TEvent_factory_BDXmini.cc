#include <iostream>
#include <iomanip>
using namespace std;

#include <EventBuilder/TEvent.h>
#include <EventBuilder/TEventHeader.h>

#include <Calorimeter/CalorimeterCluster.h>
#include <Calorimeter/CalorimeterHit.h>
#include <Calorimeter/CalorimeterDigiHit.h>
#include <IntVeto/IntVetoHit.h>

#include <DAQ/eventData.h>
#include <DAQ/fa250Mode1CalibPedSubHit.h>

#include <MC/MCType.h>
#ifdef MC_SUPPORT_ENABLEfa250
#include <Calorimeter/CalorimeterMCRealHit.h>
#include <MC/GenParticle.h>
#include <MC/UserMCData.h>
#endif

#include <BDXmini/triggerDataBDXmini.h>

#include "TClonesArray.h"
#include "TEvent_factory_BDXmini.h"

#include <JANA/JApplication.h>
using namespace jana;

TEvent_factory_BDXmini::TEvent_factory_BDXmini() {
	m_isMC = 0;
	m_MCRunNumber = 0;
	m_IntVetoHits = 0;
	m_CaloHits = 0;
	m_fa250Mode1CalibPedSubHits = 0;
	m_ADD_TRIGGER_WORDS = 1;
	gPARMS->SetDefaultParameter("TEVENT_FACTORY_BDXMINI:ADD_TRIGGER_WORDS", m_ADD_TRIGGER_WORDS, "Add trigger words to event header");

	/*Following is to decide whenever to save waveforms or not in the collections of object for the event
	 * We save ALL waveforms in an event if
	 *
	 * -> There is at least one electromagnetic cluster with energy > thrEneTot (thrEneTot=100 MeV default, programmable)
	 * AND
	 * -> All the veto counter sipm hits see LESS THAN thrNpheVeto phe (thrNpheVeto=5, programmable)
	 *
	 * The idea is to not leave any important event - to be further scrutinized - behind
	 */
	m_thrNpheVeto = 5;
	m_thrEneTot = 100;

	gPARMS->SetDefaultParameter("TEVENT_FACTORY_BDXMINI:thrEneTot", m_thrEneTot, "Threshold energy for calorimeter clusters to decide whenever to save all waveforms of an egent");
	gPARMS->SetDefaultParameter("TEVENT_FACTORY_BDXMINI:thrNpheVeto", m_thrNpheVeto, "Threshold number of photo-electrons per each Veto SiPM to decide whenever to save all waveforms of an egent");
}
//------------------
// init
//------------------
jerror_t TEvent_factory_BDXmini::init(void) {

	jout << "TEvent_factory_BDXmini::init was called" << endl;
	m_tag = "";
	gPARMS->GetParameter("MC", m_isMC);
	if (m_isMC) {
		jout << "BDXmini event build - MC mode" << endl;
		gPARMS->GetParameter("MC:RUN_NUMBER", m_MCRunNumber);
		m_tag = "MC";
	}
	if ((m_isMC) && (m_isMC != MCType::BDXmini_V1)) {
		jout << "Error! Can use this only with MC==200, i.e. BDXmini" << endl;
		return VALUE_OUT_OF_RANGE;
	}

	japp->RootWriteLock();

	m_CaloHits = new TClonesArray("CalorimeterHit");
	m_IntVetoHits = new TClonesArray("IntVetoHit");
	m_fa250Mode1CalibPedSubHits = new TClonesArray("fa250Mode1CalibPedSubHit");

#ifdef MC_SUPPORT_ENABLE
	m_GenParticles = new TClonesArray("GenParticle");
	m_CaloMCRealHits = new TClonesArray("CalorimeterMCRealHit");
#endif
	japp->RootUnLock();

	return NOERROR;
}

//------------------
// brun
//------------------
jerror_t TEvent_factory_BDXmini::brun(jana::JEventLoop *eventLoop, int32_t runnumber) {
	return NOERROR;
}

//------------------
// evnt
//------------------
jerror_t TEvent_factory_BDXmini::evnt(JEventLoop *loop, uint64_t eventnumber) {

	/*Create the TEvent*/
	TEvent* m_event = new TEvent();
	TEventHeader *m_eventHeader = m_event->getEventHeader();

	const eventData* tData;
	const triggerDataBDXmini* bdxtData;

	triggerDataBDXmini* bdxtData_write = new triggerDataBDXmini();

	vector<const CalorimeterCluster*> cclusters;
	vector<const CalorimeterHit*> chits;
	vector<const CalorimeterDigiHit*> cdhits;

	vector<const IntVetoHit*> ivhits;

	vector<const fa250Mode1CalibPedSubHit*> fa250Hits;

	bool saveWaveforms_flagCalo = false;
	bool saveWaveforms_flagVeto = true;

#ifdef MC_SUPPORT_ENABLE
	vector<const CalorimeterMCRealHit*> chits_MCReal;
	vector<const GenParticle*> genParticles;
	vector<const UserMCData*> userMCdata;
#endif

	if (!m_isMC) {
		try {
			loop->GetSingle(tData);
		} catch (unsigned long e) {
			jout << "TEvent_factory_BDXmini::evnt no triggerData bank this event" << endl;
			return OBJECT_NOT_AVAILABLE;
		}

		try {
			loop->GetSingle(bdxtData);
		} catch (unsigned long e) {
			jout << "TEvent_factory_BDXmini::evnt no triggerDataBdxmini bank this event" << endl;
			return OBJECT_NOT_AVAILABLE;
		}

		m_eventHeader->setEventType(BDXminiEvent);
		m_eventHeader->setRunNumber(tData->runN);
		m_eventHeader->setEventNumber(tData->eventN);
		m_eventHeader->setEventTS(tData->eventTS);
		m_eventHeader->setEventTime(tData->time);
		if (m_ADD_TRIGGER_WORDS) {
			m_eventHeader->setTriggerWords(tData->triggerWords);
		}
		m_eventHeader->setEventFineTime(0); //TODO
		m_eventHeader->setWeight(1);
		*bdxtData_write = *bdxtData;
		m_event->addObject(bdxtData_write);

	} else {
		m_eventHeader->setEventType(BDXminiEvent);
		m_eventHeader->setEventNumber(eventnumber);
		m_eventHeader->setEventTime(0);
		m_eventHeader->setEventFineTime(0);
		//	m_eventHeader->setTriggerWords(); /*A.C. we don't have any trigger simulation*/
		m_eventHeader->setRunNumber(m_MCRunNumber);
	}

	/*Loop over JANA objects, clear collections and fill them*/
	loop->Get(chits);
	m_CaloHits->Clear("C");
	for (int ii = 0; ii < chits.size(); ii++) {
		((CalorimeterHit*) m_CaloHits->ConstructedAt(ii))->operator=(*(chits[ii]));
		m_event->AddAssociatedObject(chits[ii]);
	}
	m_event->addCollection(m_CaloHits);

	//Check for high-energy clusters
	//A.C. At the moment, we don't save these in the event collections
	loop->Get(cclusters);
	for (int ii = 0; ii < cclusters.size(); ii++) {
		if ((cclusters[ii])->E > m_thrEneTot) saveWaveforms_flagCalo = true; //flag is there is at least one cluster at high Energy
	}

	loop->Get(ivhits);
	m_IntVetoHits->Clear("C");
	for (int ii = 0; ii < ivhits.size(); ii++) {
		((IntVetoHit*) m_IntVetoHits->ConstructedAt(ii))->operator=(*(ivhits[ii]));
		m_event->AddAssociatedObject(ivhits[ii]);
		//Check for the presence of activity in the veto
		if ((ivhits[ii])->Q > m_thrNpheVeto) saveWaveforms_flagVeto = false; //flag if all counters are below threshold. If there is even one with Q > threshold, dont'save
	}
	m_event->addCollection(m_IntVetoHits);

	/*fa250Hits -only non MC*/
	if (!m_isMC) {
		m_fa250Mode1CalibPedSubHits->Clear("C");
		if ((saveWaveforms_flagVeto && saveWaveforms_flagCalo)) {
			loop->Get(fa250Hits);
			for (int ii = 0; ii < fa250Hits.size(); ii++) {
				((fa250Mode1CalibPedSubHit*) m_fa250Mode1CalibPedSubHits->ConstructedAt(ii))->operator=(*(fa250Hits[ii]));
				m_event->AddAssociatedObject(fa250Hits[ii]);
			}
			m_event->addCollection(m_fa250Mode1CalibPedSubHits); //So that the collection is here only if it is not empty
		}
	}

#ifdef MC_SUPPORT_ENABLE
	if (m_isMC) {

		loop->Get(genParticles);
		m_GenParticles->Clear("C");
		for (int ii = 0; ii < genParticles.size(); ii++) {
			((GenParticle*) m_GenParticles->ConstructedAt(ii))->operator=(*(genParticles[ii]));
			m_event->AddAssociatedObject(genParticles[ii]);
		}
		m_event->addCollection(m_GenParticles);

		loop->Get(chits_MCReal);
		m_CaloMCRealHits->Clear("C");
		for (int ii = 0; ii < chits_MCReal.size(); ii++) {
			((CalorimeterMCRealHit*) m_CaloMCRealHits->ConstructedAt(ii))->operator=(*(chits_MCReal[ii]));
			m_event->AddAssociatedObject(chits_MCReal[ii]);
		}
		m_event->addCollection(m_CaloMCRealHits);

		loop->Get(userMCdata);
		for (int ii=0;ii<userMCdata.size();ii++) {
			if (userMCdata[ii]->N==1) {
				m_eventHeader->setWeight(userMCdata[ii]->data);
				if (m_eventHeader->getWeight()==0) {
					m_eventHeader->setWeight(1.);
				}
			}
			/*Adding following lines of code, so that in case of LUND, the weight will be taken from the tenth number,
			 *In other cases, when there is a single number, the event weight is taken from it (N==1)
			 */
			if (userMCdata[ii]->N==10) {
				m_eventHeader->setWeight(userMCdata[ii]->data);
				if (m_eventHeader->getWeight()==0) {
					m_eventHeader->setWeight(1.);
				}
			}
		}

	}
#endif

	/*publish the event*/
	_data.push_back(m_event);

	return NOERROR;
}

//------------------
// erun
//------------------
jerror_t TEvent_factory_BDXmini::erun(void) {
	return NOERROR;
}

//------------------
// fini
//------------------
jerror_t TEvent_factory_BDXmini::fini(void) {
	japp->RootWriteLock();
//	if (m_CaloHits!=0) delete (m_CaloHits);
//	if (m_IntVetoHits!=0) delete (m_IntVetoHits);
//	if (m_ExtVetoHits!=0) delete (m_ExtVetoHits);
	japp->RootUnLock();

	return NOERROR;
}

