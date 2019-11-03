// $Id$
//
//    File: JEventProcessor_BDXMiniIntVeto.cc
// Created: Tue Oct  8 15:16:44 CEST 2019
// Creator: celentan (on Linux apcx4 3.10.0-957.el7.x86_64 x86_64)
//

#include "JEventProcessor_BDXMiniIntVeto.h"
using namespace jana;

// Routine used to create our JEventProcessor
#include <JANA/JApplication.h>
#include <JANA/JFactory.h>

#include <system/JROOTOutput.h>

#include "TTree.h"

#include "system/BDXEventProcessor.h"
#include "IntVeto/IntVetoDigiHit.h"
#include "Calorimeter/CalorimeterHit.h"
#include "DAQ/eventData.h"

extern "C" {
void InitPlugin(JApplication *app) {
	InitJANAPlugin(app);
	app->AddProcessor(new JEventProcessor_BDXMiniIntVeto());
}
} // "C"

//------------------
// JEventProcessor_BDXMiniIntVeto (Constructor)
//------------------
JEventProcessor_BDXMiniIntVeto::JEventProcessor_BDXMiniIntVeto() :
		m_ROOTOutput(0) {
	m_isFirstCallToBrun = 1;
	t = 0;
	m_thrIntVetoCaps = 5; //for now hardcoded!
	m_thrCrystalsH = 15; //for now hardcoded!
	m_thrCrystalsL = 5; //for now hardcoded!

}

//------------------
// ~JEventProcessor_BDXMiniIntVeto (Destructor)
//------------------
JEventProcessor_BDXMiniIntVeto::~JEventProcessor_BDXMiniIntVeto() {

}

//------------------
// init
//------------------
jerror_t JEventProcessor_BDXMiniIntVeto::init(void) {
	// This is called once at program startup. If you are creating
	// and filling historgrams in this plugin, you should lock the
	// ROOT mutex like this:
	//
	// japp->RootWriteLock();
	//  ... fill historgrams or trees ...
	// japp->RootUnLock();
	//
	m_isMC = 0;
	gPARMS->GetParameter("MC", m_isMC);

	japp->RootWriteLock();
	t = new TTree("BDXMiniIntVeto", "BDXMiniIntVeto");
	t->Branch("eventN", &eventNumber);
	t->Branch("QL0", QL0, "QL0[10]/D");
	t->Branch("QL1", QL1, "QL1[10]/D");
	t->Branch("AL0", AL0, "AL0[10]/D");
	t->Branch("AL1", AL1, "AL1[10]/D");
	t->Branch("maxL0", &maxL0);
	t->Branch("maxL1", &maxL1);
	t->Branch("crsTOPl", crsTOPl, "crsTOPl[22]/O");
	t->Branch("crsBOTl", crsBOTl, "crsBOTl[22]/O");
	t->Branch("crsTOPh", crsTOPh, "crsTOPh[22]/O");
	t->Branch("crsBOTh", crsBOTh, "crsBOTh[22]/O");

	t->Branch("topbottom", &topbottom);
	japp->RootUnLock();

	/*Hard-coded XY -> id geometry*/
	geometry[make_pair(-2, -2)] = 1;
	geometry[make_pair(-1, -2)] = 2;
	geometry[make_pair(0, -2)] = 3;
	geometry[make_pair(1, -2)] = 4;
	geometry[make_pair(2, -2)] = 5;
	geometry[make_pair(0, -1)] = 6;
	geometry[make_pair(-2, 0)] = 7;
	geometry[make_pair(-1, 0)] = 8;
	geometry[make_pair(0, 0)] = 9;
	geometry[make_pair(1, 0)] = 10;
	geometry[make_pair(2, 0)] = 11;
	geometry[make_pair(-2, 1)] = 12;
	geometry[make_pair(-1, 1)] = 13;
	geometry[make_pair(0, 1)] = 14;
	geometry[make_pair(1, 1)] = 15;
	geometry[make_pair(2, 1)] = 16;
	geometry[make_pair(-2, 2)] = 17;
	geometry[make_pair(-1, 2)] = 18;
	geometry[make_pair(1, 2)] = 19;
	geometry[make_pair(2, 2)] = 20;
	geometry[make_pair(-1, 3)] = 21;
	geometry[make_pair(1, 3)] = 22;

	return NOERROR;
}

//------------------
// brun
//------------------
jerror_t JEventProcessor_BDXMiniIntVeto::brun(JEventLoop *eventLoop, int32_t runnumber) {
	// This is called whenever the run number changes

	if (m_isFirstCallToBrun) {
		string class_name, this_class_name;
		string joutput_name;
		BDXEventProcessor *m_BDXEventProcessor;
		vector<JEventProcessor*> m_processors = app->GetProcessors();
		vector<JEventProcessor*>::iterator m_processors_it;

		m_isFirstCallToBrun = 0;
		class_name = "BDXEventProcessor";
		joutput_name = "JROOTOutput";
		//Now I need to determine which processor is the one holding the output. Discussing with David, he suggested just to check the class name, since
		//a dynamic cast may not work with plugins
		for (m_processors_it = m_processors.begin(); m_processors_it != m_processors.end(); m_processors_it++) {
			if ((*m_processors_it) != 0) {
				this_class_name = string((*m_processors_it)->className());
				if (this_class_name == class_name) {
					m_BDXEventProcessor = (BDXEventProcessor*) (*m_processors_it);
					if (m_BDXEventProcessor->getOutput() == 0) {
						jerr << "BDXEventProcessor JOutput is null!" << endl;
						break;
					}
					if (string(m_BDXEventProcessor->getOutput()->className()) == joutput_name) {
						m_ROOTOutput = (JROOTOutput*) (m_BDXEventProcessor->getOutput());
						jout << "Got JROOTOutput!" << endl;
					} else {
						jerr << "BDXEventProcessor JOutput is not null BUT class is: " << m_BDXEventProcessor->getOutput()->className() << endl;
					}
				}
			}
		}
		/*For ALL objects you want to add to ROOT file, use the following:*/
		if (m_ROOTOutput) {
			m_ROOTOutput->AddObject(t);
		}
	}

	return NOERROR;
}

//------------------
// evnt
//------------------
jerror_t JEventProcessor_BDXMiniIntVeto::evnt(JEventLoop *loop, uint64_t eventnumber) {
	// This is called for every event. Use of common resources like writing
	// to a file or filling a histogram should be mutex protected. Using
	// loop->Get(...) to get reconstructed objects (and thereby activating the
	// reconstruction algorithm) should be done outside of any mutex lock
	// since multiple threads may call this method at the same time.

	const eventData* tData;

	vector<const IntVetoDigiHit*> digi_hits;
	vector<const IntVetoDigiHit*>::iterator digi_hits_it;
	const IntVetoDigiHit* digi_hit;

	const CalorimeterHit* m_CaloHit;
	vector<const CalorimeterHit*> calo_hits;
	vector<const CalorimeterHit*>::const_iterator calo_hits_it;

	if (!m_isMC) {
		try {
			loop->GetSingle(tData);
		} catch (unsigned long e) {
			//	jout << "JEventProcessor_IntVetoSipm::evnt::evnt no eventData bank this event" << std::endl;
			return OBJECT_NOT_AVAILABLE;
		}
	}
	int nTopCaps = 0;
	int nBottomCaps = 0;

	int maxComponent0L = -1;
	int maxComponent1L = -1;

	double maxComponentQ0L = -9999;
	double maxComponentQ1L = -9999;

	int X, Y, id;

	/*Get objects from JANA factories*/
	if (m_isMC) {
		loop->Get(digi_hits, "MC");
	} else {
		loop->Get(digi_hits);
	}

	for (int ii = 0; ii < 22; ii++) {
		crsBOTl[ii] = false;
		crsTOPl[ii] = false;
		crsBOTh[ii] = false;
		crsTOPh[ii] = false;
	}

	/*Crystals thresholds*/
	loop->Get(calo_hits);
	for (calo_hits_it = calo_hits.begin(); calo_hits_it != calo_hits.end(); calo_hits_it++) {
		m_CaloHit = *calo_hits_it;
		X = m_CaloHit->m_channel.x;
		Y = m_CaloHit->m_channel.y;
		id = geometry[make_pair(X, Y)];
		id = id - 1;

		if (m_CaloHit->E > m_thrCrystalsH) {
			if (m_CaloHit->m_channel.sector == 0) {
				crsTOPh[id] = true;
			} else {
				crsBOTh[id] = true;
			}
		}

		if (m_CaloHit->E > m_thrCrystalsL) {
			if (m_CaloHit->m_channel.sector == 0) {
				crsTOPl[id] = true;
			} else {
				crsBOTl[id] = true;
			}
		}
	}

	/*Here goes the code to create the objects*/
	for (digi_hits_it = digi_hits.begin(); digi_hits_it != digi_hits.end(); digi_hits_it++) {
		digi_hit = (*digi_hits_it);

		//A.C. skip SIPM in csc (0,1,2), i.e. Sector0 Layer0 Component3
		if ((digi_hit->m_channel.layer == 0) && (digi_hit->m_channel.component == 3)) continue;

		if (digi_hit->m_channel.component == 10) { //TOP
			if (digi_hit->Qphe > m_thrIntVetoCaps) {
				nTopCaps++;
				if (digi_hit->m_channel.layer == 0) {
					QL0[digi_hit->m_channel.component - 1] = digi_hit->Qphe;
					AL0[digi_hit->m_channel.component - 1] = digi_hit->Aphe;
				} else {
					QL1[digi_hit->m_channel.component - 1] = digi_hit->Qphe;
					AL1[digi_hit->m_channel.component - 1] = digi_hit->Aphe;
				}

			}
		} else if (digi_hit->m_channel.component == 9) { //BOTTOM
			if (digi_hit->Qphe > m_thrIntVetoCaps) {
				nBottomCaps++;
				if (digi_hit->m_channel.layer == 0) {
					QL0[digi_hit->m_channel.component - 1] = digi_hit->Qphe;
					AL0[digi_hit->m_channel.component - 1] = digi_hit->Aphe;
				} else {
					QL1[digi_hit->m_channel.component - 1] = digi_hit->Qphe;
					AL1[digi_hit->m_channel.component - 1] = digi_hit->Aphe;
				}
			}
		} else if (digi_hit->m_channel.component <= 8) {
			if (digi_hit->m_channel.layer == 0) {
				QL0[digi_hit->m_channel.component - 1] = digi_hit->Qphe;
				AL0[digi_hit->m_channel.component - 1] = digi_hit->Aphe;
				if (digi_hit->Aphe > maxComponentQ0L) {
					maxComponent0L = digi_hit->m_channel.component;
					maxComponentQ0L = digi_hit->Aphe;
				}
			} else if (digi_hit->m_channel.layer == 1) {
				QL1[digi_hit->m_channel.component - 1] = digi_hit->Qphe;
				AL1[digi_hit->m_channel.component - 1] = digi_hit->Aphe;
				if ((digi_hit->Aphe > maxComponentQ1L)) {
					maxComponent1L = digi_hit->m_channel.component;
					maxComponentQ1L = digi_hit->Aphe;
				}
			}
		}
	}
	maxL0 = maxComponent0L;
	maxL1 = maxComponent1L;

	if ((nTopCaps == 2) || (nBottomCaps == 2)) {
		if ((nTopCaps == 2) && (nBottomCaps == 0)) topbottom = 0;
		else if ((nTopCaps == 0) && (nBottomCaps == 2)) topbottom = 1;
		else
			topbottom = 2;

		japp->RootWriteLock();
		t->Fill();
		japp->RootUnLock();
	}

	return NOERROR;
}

//------------------
// erun
//------------------
jerror_t JEventProcessor_BDXMiniIntVeto::erun(void) {
// This is called whenever the run number changes, before it is
// changed to give you a chance to clean up before processing
// events from the next run number.
	return NOERROR;
}

//------------------
// fini
//------------------
jerror_t JEventProcessor_BDXMiniIntVeto::fini(void) {
// Called before program exit after event processing is finished.
	return NOERROR;
}

