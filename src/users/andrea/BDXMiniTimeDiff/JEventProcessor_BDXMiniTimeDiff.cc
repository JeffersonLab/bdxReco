// $Id$
//
//    File: JEventProcessor_BDXMiniTimeDiff.cc
// Created: Wed Oct  7 18:28:33 CEST 2020
// Creator: celentan (on Linux apcx4 3.10.0-957.el7.x86_64 x86_64)
//

#include "JEventProcessor_BDXMiniTimeDiff.h"

#include "system/BDXEventProcessor.h"
#include "IntVeto/IntVetoDigiHit.h"
#include "Calorimeter/CalorimeterHit.h"
#include "DAQ/eventData.h"

#include <system/JROOTOutput.h>
using namespace jana;

#include "TH1D.h"
#include "TH2D.h"

// Routine used to create our JEventProcessor
#include <JANA/JApplication.h>
#include <JANA/JFactory.h>
extern "C" {
void InitPlugin(JApplication *app) {
	InitJANAPlugin(app);
	app->AddProcessor(new JEventProcessor_BDXMiniTimeDiff());
}
} // "C"

//------------------
// JEventProcessor_BDXMiniTimeDiff (Constructor)
//------------------
JEventProcessor_BDXMiniTimeDiff::JEventProcessor_BDXMiniTimeDiff() :
		m_ROOTOutput(0), hTimeDiff(0),hTimeDiffvsEventN(0) {
	m_isFirstCallToBrun = 1;
	m_isMC = 0;
}

//------------------
// ~JEventProcessor_BDXMiniTimeDiff (Destructor)
//------------------
JEventProcessor_BDXMiniTimeDiff::~JEventProcessor_BDXMiniTimeDiff() {

}

//------------------
// init
//------------------
jerror_t JEventProcessor_BDXMiniTimeDiff::init(void) {
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
	hTimeDiff = new TH1D("h", "h", 8000, -3999.5, 4000.5);
	hTimeDiffvsEventN= new TH2D("h2", "h2",4000,0,2E6,8000, -3999.5, 4000.5);
	japp->RootUnLock();
	timeThis=0;
	timePrev=0;

	return NOERROR;
}

//------------------
// brun
//------------------
jerror_t JEventProcessor_BDXMiniTimeDiff::brun(JEventLoop *eventLoop, int32_t runnumber) {
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
			m_ROOTOutput->AddObject(hTimeDiff);
			m_ROOTOutput->AddObject(hTimeDiffvsEventN);
		}
	}

	return NOERROR;
}

//------------------
// evnt
//------------------
jerror_t JEventProcessor_BDXMiniTimeDiff::evnt(JEventLoop *loop, uint64_t eventnumber) {
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
	const eventData* tData;


	if (!m_isMC) {
		try {
			loop->GetSingle(tData);
		} catch (unsigned long e) {
			//	jout << "JEventProcessor_IntVetoSipm::evnt::evnt no eventData bank this event" << std::endl;
			return OBJECT_NOT_AVAILABLE;
		}
	}
	japp->RootWriteLock();
	timePrev=timeThis;
	timeThis=tData->time;
	hTimeDiff->Fill(timeThis-timePrev);
	hTimeDiffvsEventN->Fill(tData->eventN,timeThis-timePrev);
	japp->RootUnLock();
	return NOERROR;
}

//------------------
// erun
//------------------
jerror_t JEventProcessor_BDXMiniTimeDiff::erun(void) {
	// This is called whenever the run number changes, before it is
	// changed to give you a chance to clean up before processing
	// events from the next run number.

	return NOERROR;
}

//------------------
// fini
//------------------
jerror_t JEventProcessor_BDXMiniTimeDiff::fini(void) {
	// Called before program exit after event processing is finished.
	return NOERROR;
}

