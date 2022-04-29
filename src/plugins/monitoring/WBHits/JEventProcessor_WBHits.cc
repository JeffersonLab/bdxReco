// $Id$
//
//    File: JEventProcessor_WBHits.cc
// Created: Sat Feb 24 13:11:49 CET 2018
// Creator: celentan (on Darwin celentano-macbook 17.4.0 i386)
//

#include "JEventProcessor_WBHits.h"

#include <DAQ/fa250WaveboardV1Hit.h>
#include <DAQ/eventData.h>
#include <TT/TranslationTable.h>

#include <system/JROOTOutput.h>
#include <system/BDXEventProcessor.h>
using namespace jana;

#include <TDirectory.h>
#include <TH2.h>
#include <TH1.h>
#include <TProfile2D.h>
#include <TStyle.h>
#include "TROOT.h"

// Routine used to create our JEventProcessor
#include <JANA/JApplication.h>
#include <JANA/JFactory.h>
extern "C" {
void InitPlugin(JApplication *app) {
	InitJANAPlugin(app);
	app->AddProcessor(new JEventProcessor_WBHits());
}
} // "C"

//------------------
// JEventProcessor_WBHits (Constructor)
//------------------
JEventProcessor_WBHits::JEventProcessor_WBHits(): m_ROOTOutput(0){
	m_isFirstCallToBrun = 1;

	hNumHits=0;
	hNumSamplesPerCh=0;
}

//------------------
// ~JEventProcessor_WBHits (Destructor)
//------------------
JEventProcessor_WBHits::~JEventProcessor_WBHits() {

}

//------------------
// init
//------------------
jerror_t JEventProcessor_WBHits::init(void) {
	// This is called once at program startup. If you are creating
	// and filling historgrams in this plugin, you should lock the
	// ROOT mutex like this:
	//
	// japp->RootWriteLock();
	//  ... fill historgrams or trees ...
	// japp->RootUnLock();
	//

	japp->RootWriteLock();
	if (hNumHits != NULL) {
		japp->RootUnLock();
		return NOERROR;
	}
	gROOT->cd();
	TDirectory *main = gDirectory;
	gDirectory->mkdir("WBHits")->cd();

	/*Create all the histograms*/
	hNumHits = new TH1D("hNumHits", "hNumHits", 13, -0.5, 12.5);
	hNumSamplesPerCh = new TH2D("hNumSamplesPerC", "hhNumSamplesPerC", 13, -0.5,
			12.5, 1000, -0.5, 999.5);
	// back to main dir
	main->cd();
	japp->RootUnLock();

	return NOERROR;
}

//------------------
// brun
//------------------
jerror_t JEventProcessor_WBHits::brun(JEventLoop *eventLoop,
		int32_t runnumber) {
	// This is called whenever the run number changes

	jout <<"JEventProcessor_WBHits::brun--> "<< m_isFirstCallToBrun << endl;

	if (m_isFirstCallToBrun) {

		jout << "JEventProcessor_WBHits::brun searching m_RootOutput"
				<< endl;
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
		for (m_processors_it = m_processors.begin();
				m_processors_it != m_processors.end(); m_processors_it++) {
			if ((*m_processors_it) != 0) {
				this_class_name = string((*m_processors_it)->className());
				if (this_class_name == class_name) {
					m_BDXEventProcessor =
							(BDXEventProcessor*) (*m_processors_it);
					if (m_BDXEventProcessor->getOutput() == 0) {
						jerr << "BDXEventProcessor JOutput is null!" << endl;
						break;
					}
					if (string(m_BDXEventProcessor->getOutput()->className())
							== joutput_name) {
						m_ROOTOutput =
								(JROOTOutput*) (m_BDXEventProcessor->getOutput());
						jout << "Got JROOTOutput!" << endl;
					} else {
						jerr
								<< "BDXEventProcessor JOutput is not null BUT class is: "
								<< m_BDXEventProcessor->getOutput()->className()
								<< endl;
					}
				}
			}
		}
	}
	return NOERROR;
}

//------------------
// evnt
//------------------
jerror_t JEventProcessor_WBHits::evnt(JEventLoop *loop, uint64_t eventnumber) {
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

	int word1, word2;
	bool triggers[32] = { false };
	vector<const fa250WaveboardV1Hit*> hits;
	vector<const fa250WaveboardV1Hit*>::const_iterator data_it;
	loop->Get(hits);

	const eventData *tData;

	try {
		loop->GetSingle(tData);
	} catch (unsigned long e) {
		return OBJECT_NOT_AVAILABLE;
	}

	japp->RootWriteLock();

	int Nch = 0;
	for (auto hit : hits) {
		Nch++;

		hNumSamplesPerCh->Fill(hit->m_channel.channel, hit->samples.size());
	}
	hNumHits->Fill(Nch);

	japp->RootUnLock();
	return NOERROR;
}

//------------------
// erun
//------------------
jerror_t JEventProcessor_WBHits::erun(void) {
// This is called whenever the run number changes, before it is
// changed to give you a chance to clean up before processing
// events from the next run number.

	if (m_ROOTOutput != 0) {
		jout << "JEventProcessor_BDXMiniEBHits adding histos to m_ROOTOutput"
				<< endl;

		m_ROOTOutput->AddObject(hNumHits);
		m_ROOTOutput->AddObject(hNumSamplesPerCh);

	}

	return NOERROR;
}

//------------------
// fini
//------------------
jerror_t JEventProcessor_WBHits::fini(void) {
// Called before program exit after event processing is finished.
	return NOERROR;
}

