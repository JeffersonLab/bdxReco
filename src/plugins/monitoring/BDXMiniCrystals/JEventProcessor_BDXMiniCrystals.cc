// $Id$
//
//    File: JEventProcessor_BDXMiniCrystals.cc
// Created: Thu Oct  3 17:27:30 CEST 2019
// Creator: celentan (on Linux apcx4 3.10.0-957.el7.x86_64 x86_64)
//

#include "JEventProcessor_BDXMiniCrystals.h"
using namespace jana;

// Routine used to create our JEventProcessor
#include <JANA/JApplication.h>
#include <JANA/JFactory.h>

#include "Calorimeter/CalorimeterHit.h"
#include "DAQ/eventData.h"

#include <system/JROOTOutput.h>
#include <system/BDXEventProcessor.h>

#include "TROOT.h"
// Routine used to create our JEventProcessor
#include <JANA/JApplication.h>
#include <JANA/JFactory.h>

/*Here goes the histograms*/
static const int nTOT = 22;

//Amplitude
static TH1D *hBDXMiniCrystalsEnergyTOP[nTOT] = { 0 };
static TH1D *hBDXMiniCrystalsEnergyBOTTOM[nTOT] = { 0 };

extern "C" {
void InitPlugin(JApplication *app) {
	InitJANAPlugin(app);
	app->AddProcessor(new JEventProcessor_BDXMiniCrystals());
}
} // "C"

//------------------
// JEventProcessor_BDXMiniCrystals (Constructor)
//------------------
JEventProcessor_BDXMiniCrystals::JEventProcessor_BDXMiniCrystals() :
		m_ROOTOutput(0), m_isMC(0) {
	m_isFirstCallToBrun = 1;

}

//------------------
// ~JEventProcessor_BDXMiniCrystals (Destructor)
//------------------
JEventProcessor_BDXMiniCrystals::~JEventProcessor_BDXMiniCrystals() {

}

//------------------
// init
//------------------
jerror_t JEventProcessor_BDXMiniCrystals::init(void) {
	// This is called once at program startup. If you are creating
	// and filling historgrams in this plugin, you should lock the
	// ROOT mutex like this:
	//
	// japp->RootWriteLock();
	//  ... fill historgrams or trees ...
	// japp->RootUnLock();
	//
	m_isFirstCallToBrun = 1;
	m_isMC = 0;
	gPARMS->GetParameter("MC", m_isMC);

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

	japp->RootWriteLock();

	if (hBDXMiniCrystalsEnergyTOP[0] != NULL) {
		japp->RootUnLock();
		return NOERROR;
	}

	gROOT->cd();
	TDirectory *main = gDirectory;
	gDirectory->mkdir("BDXMiniCrystals")->cd();

	/*Create all the histograms*/
	map<pair<int, int>, int>::iterator geometry_it;
	int iX, iY, id;

	double Emin, Emax;
	int NE;



	NE = 500;
	Emin = 100;
	Emax = 600; //600

	for (geometry_it = geometry.begin(); geometry_it != geometry.end(); geometry_it++) {
		//Do whatever you want. To access the first part of pair(pair is the key of the map here) you should write

		iX = (geometry_it->first).first;
		iY = (geometry_it->first).second;
		id = (geometry_it->second);

		id = id - 1;

		hBDXMiniCrystalsEnergyTOP[id] = new TH1D(Form("hBDXMiniCrsE_s0_x%i_y%i", iX, iY), Form("hBDXMiniCrsE_s0_x%i_y%i", iX, iY), NE, Emin, Emax);
		hBDXMiniCrystalsEnergyTOP[id]->GetXaxis()->SetTitle("E");

		hBDXMiniCrystalsEnergyBOTTOM[id] = new TH1D(Form("hBDXMiniCrsE_s1_x%i_y%i", iX, iY), Form("hBDXMiniCrsE_s1_x%i_y%i", iX, iY), NE, Emin, Emax);
		hBDXMiniCrystalsEnergyBOTTOM[id]->GetXaxis()->SetTitle("E");

	}

	// back to main dir
	main->cd();
	japp->RootUnLock();

	return NOERROR;

}

//------------------
// brun
//------------------
jerror_t JEventProcessor_BDXMiniCrystals::brun(JEventLoop *eventLoop, int32_t runnumber) {
	// This is called whenever the run number changes
	jout << m_isFirstCallToBrun << endl;

	if (m_isFirstCallToBrun) {

		jout << "JEventProcessor_BDXMiniClusters::brun searching m_RootOutput" << endl;
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
	}
	return NOERROR;
}

//------------------
// evnt
//------------------
jerror_t JEventProcessor_BDXMiniCrystals::evnt(JEventLoop *loop, uint64_t eventnumber) {
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

	vector<const CalorimeterHit*> hits;
	vector<const CalorimeterHit*>::iterator hits_it;
	const CalorimeterHit *m_hit;
	const eventData* tData;
	if (!m_isMC) {
		try {
			loop->GetSingle(tData);
		} catch (unsigned long e) {
			jout << "JEventProcessor_BDXMiniCrystals::evnt no eventData bank this event" << std::endl;
			return OBJECT_NOT_AVAILABLE;
		}
	}

	loop->Get(hits);
	int sector, X, Y, id;
	for (hits_it = hits.begin(); hits_it != hits.end(); hits_it++) {
		m_hit = *hits_it;
		sector = m_hit->m_channel.sector;
		X = m_hit->m_channel.x;
		Y = m_hit->m_channel.y;

		id = geometry[make_pair(X, Y)];
		id = id - 1;

		japp->RootWriteLock();
		if (sector == 0) { //TOP
			hBDXMiniCrystalsEnergyTOP[id]->Fill(m_hit->E); //E
		} else { //BOTTOM
			hBDXMiniCrystalsEnergyBOTTOM[id]->Fill(m_hit->E);
		}
		japp->RootUnLock();
	}

	return NOERROR;
}

//------------------
// erun
//------------------
jerror_t JEventProcessor_BDXMiniCrystals::erun(void) {
	// This is called whenever the run number changes, before it is
	// changed to give you a chance to clean up before processing
	// events from the next run number.

	if (m_ROOTOutput != 0) {
		jout << "JEventProcessor_BDXMiniCalorimeterEnergyCalibartion adding histos to m_ROOTOutput" << endl;
		for (int ii = 0; ii < nTOT; ii++) {
			m_ROOTOutput->AddObject(hBDXMiniCrystalsEnergyTOP[ii]);
			m_ROOTOutput->AddObject(hBDXMiniCrystalsEnergyBOTTOM[ii]);
		}

	}
	return NOERROR;
}

//------------------
// fini
//------------------
jerror_t JEventProcessor_BDXMiniCrystals::fini(void) {
	// Called before program exit after event processing is finished.
	return NOERROR;
}

