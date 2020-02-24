// $Id$
//
//    File: JEventProcessor_sipm_calib.cc
// Created: Tue Feb  2 18:52:32 CET 2016
// Creator: celentan (on Linux apcx4 2.6.32-504.30.3.el6.x86_64 x86_64)
//

#include "JEventProcessor_IntVeto_SipmCalib.h"
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

#include <ExtVeto/ExtVetoPMTHit.h>
#include <ExtVeto/ExtVetoDigiHit.h>

#include <system/JROOTOutput.h>

#include "TH1D.h"
#include "TH2D.h"
#include "TTree.h"

extern "C" {
void InitPlugin(JApplication *app) {
	InitJANAPlugin(app);
	app->AddProcessor(new JEventProcessor_IntVeto_SipmCalib());
}
} // "C"

//------------------
// JEventProcessor_sipm_calib (Constructor)
//------------------
JEventProcessor_IntVeto_SipmCalib::JEventProcessor_IntVeto_SipmCalib() :
		m_ROOTOutput(0) {
	m_isFirstCallToBrun = 1;
	h = new TH1D("h", "h", 500, -0.5, 499.5);
	t = 0;
}

//------------------
// ~JEventProcessor_sipm_calib (Destructor)
//------------------
JEventProcessor_IntVeto_SipmCalib::~JEventProcessor_IntVeto_SipmCalib() {

}

//------------------
// init
//------------------
jerror_t JEventProcessor_IntVeto_SipmCalib::init(void) {
	// This is called once at program startup. If you are creating
	// and filling historgrams in this plugin, you should lock the
	// ROOT mutex like this:
	//
	// japp->RootWriteLock();
	//  ... fill historgrams or trees ...
	// japp->RootUnLock();
	//

	triggerBit = -1;
	gPARMS->SetDefaultParameter("andrea:IntVeto_SipmCalib:triggerBit", triggerBit);

	japp->RootWriteLock();
	t = new TTree("IntVeto_SipmCalib", "IntVeto_SipmCalib");

	t->Branch("sector", &m_sector);
	t->Branch("layer", &m_layer);
	t->Branch("component", &m_component);
	t->Branch("readout", &m_readout);
	t->Branch("type", &m_type);
	t->Branch("Qphe", &Qphe);
	t->Branch("Qraw", &Qraw);
	t->Branch("T", &T);
	t->Branch("A", &A);
	t->Branch("eventN", &eventNumber);

	t->Branch("tWord", &tWord);

	japp->RootUnLock();

	return NOERROR;
}

//------------------
// brun
//------------------
jerror_t JEventProcessor_IntVeto_SipmCalib::brun(JEventLoop *eventLoop, int32_t runnumber) {
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
jerror_t JEventProcessor_IntVeto_SipmCalib::evnt(JEventLoop *loop, uint64_t eventnumber) {

	vector<const IntVetoSiPMHit*> data;
	vector<const IntVetoSiPMHit*>::const_iterator data_it;
	loop->Get(data);

	const eventData* tData;
	try {
		loop->GetSingle(tData);
	} catch (unsigned long e) {
		//	jout << "JEventProcessor_IntVetoSipm::evnt::evnt no eventData bank this event" << std::endl;
		return OBJECT_NOT_AVAILABLE;
	}
	int word1, word2;
	/*Decode trigger words*/
	for (int ii = 0; ii < tData->triggerWords.size() / 2; ii++) {
		word1 = tData->triggerWords[ii * 2];
		word2 = tData->triggerWords[ii * 2 + 1];

		tWord = word1;

	}

	if ((triggerBit == -1) || (((tWord >> triggerBit) & 0x1) == 0x1)) {
		japp->RootWriteLock();
		//  ... fill historgrams or trees ...
		for (data_it = data.begin(); data_it < data.end(); data_it++) {
			m_sector = (*data_it)->m_channel.int_veto->sector;
			m_layer = (*data_it)->m_channel.int_veto->layer;
			m_component = (*data_it)->m_channel.int_veto->component;
			m_readout = (*data_it)->m_channel.int_veto->readout;
			m_type = (*data_it)->m_type;
			T = (*data_it)->T;
			A = (*data_it)->Araw;
			Qphe = (*data_it)->Qphe;
			Qraw = (*data_it)->Qraw;
			eventNumber = eventnumber;
			t->Fill();
		}
		japp->RootUnLock();
	}
	return NOERROR;
}

//------------------
// erun
//------------------
jerror_t JEventProcessor_IntVeto_SipmCalib::erun(void) {
	// This is called whenever the run number changes, before it is
	// changed to give you a chance to clean up before processing
	// events from the next run number.
	return NOERROR;
}

//------------------
// fini
//------------------
jerror_t JEventProcessor_IntVeto_SipmCalib::fini(void) {
	// Called before program exit after event processing is finished.
	return NOERROR;
}

