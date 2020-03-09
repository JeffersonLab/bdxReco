// $Id$
//
//    File: JEventProcessor_PiastraSipm.cc
// Created: Fri Mar  6 18:54:14 CET 2020
// Creator: celentan (on Darwin celentano-macbook 19.3.0 i386)
//

#include "JEventProcessor_PiastraSipm.h"
using namespace jana;

// Routine used to create our JEventProcessor
#include <JANA/JApplication.h>
#include <JANA/JFactory.h>


#include "system/BDXEventProcessor.h"

#include <DAQ/fa250Mode1Hit.h>
#include <DAQ/fa250Mode1CalibPedSubHit.h>
#include <TT/TranslationTable.h>

#include <DAQ/eventData.h>

#include <IntVeto/IntVetoDigiHit.h>
#include <IntVeto/IntVetoSiPMHit.h>


#include <system/JROOTOutput.h>

#include "TTree.h"


extern "C" {
void InitPlugin(JApplication *app) {
	InitJANAPlugin(app);
	app->AddProcessor(new JEventProcessor_PiastraSipm());
}
} // "C"

//------------------
// JEventProcessor_PiastraSipm (Constructor)
//------------------
JEventProcessor_PiastraSipm::JEventProcessor_PiastraSipm() {
	m_ROOTOutput = 0;
	m_isFirstCallToBrun = 1;
	m_t = 0;
}

//------------------
// ~JEventProcessor_PiastraSipm (Destructor)
//------------------
JEventProcessor_PiastraSipm::~JEventProcessor_PiastraSipm() {

}

//------------------
// init
//------------------
jerror_t JEventProcessor_PiastraSipm::init(void) {

	//

	japp->RootWriteLock();
	m_t = new TTree("IntVeto_SipmCalib", "IntVeto_SipmCalib");

	//add branches
	m_t->Branch("Q",&m_A[0],"A[4]/D");
	m_t->Branch("A",&m_Q[0],"A[4]/D");
	m_t->Branch("T",&m_T[0],"A[4]/D");

	m_t->Branch("tSec",&m_tSec);
	m_t->Branch("tNanoSec",&m_tNanoSec);


	japp->RootUnLock();

	return NOERROR;
}

//------------------
// brun
//------------------
jerror_t JEventProcessor_PiastraSipm::brun(JEventLoop *eventLoop, int32_t runnumber) {
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
			m_ROOTOutput->AddObject(m_t);
		}
	}

	return NOERROR;
}

//------------------
// evnt
//------------------
jerror_t JEventProcessor_PiastraSipm::evnt(JEventLoop *loop, uint64_t eventnumber) {
	// This is called for every event. Use of common resources like writing
	// to a file or filling a histogram should be mutex protected. Using
	// loop->Get(...) to get reconstructed objects (and thereby activating the
	// reconstruction algorithm) should be done outside of any mutex lock
	// since multiple threads may call this method at the same time.
	// Here's an example:
	//
	// vector<const MyDataClass*> mydataclasses;
	// loop->Get(mydataclasses);
	//
	// japp->RootWriteLock();
	//  ... fill historgrams or trees ...
	// japp->RootUnLock();

	vector<const IntVetoDigiHit*> hits;

	vector<int> idx;



	for (int ii=0;ii<hits.size();ii++){
		const IntVetoDigiHit* hit=hits[ii];

	}


	return NOERROR;
}

//------------------
// erun
//------------------
jerror_t JEventProcessor_PiastraSipm::erun(void) {
	// This is called whenever the run number changes, before it is
	// changed to give you a chance to clean up before processing
	// events from the next run number.
	return NOERROR;
}

//------------------
// fini
//------------------
jerror_t JEventProcessor_PiastraSipm::fini(void) {
	// Called before program exit after event processing is finished.
	return NOERROR;
}

