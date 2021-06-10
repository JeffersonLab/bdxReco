// $Id$
//
//    File: JEventProcessor_PiastraStability.cc
// Created: Fri May 24 19:02:10 CEST 2019
// Creator: celentan (on Linux apcx4 3.10.0-957.el7.x86_64 x86_64)
//

#include "JEventProcessor_PiastraStability.h"
using namespace jana;

// Routine used to create our JEventProcessor
#include <JANA/JApplication.h>
#include <JANA/JFactory.h>

// Routine used to create our JEventProcessor
#include <JANA/JApplication.h>
#include <JANA/JFactory.h>

#include "calibration/PiastraSIPMCalibrationHit/PiastraSIPMCalibrationHit.h"

#include <system/JROOTOutput.h>
#include <system/BDXEventProcessor.h>

#include "Calorimeter/CalorimeterDigiHit.h"
#include "Calorimeter/CalorimeterHit.h"
#include "Calorimeter/CalorimeterCluster.h"
#include "IntVeto/IntVetoDigiHit.h"
#include <DAQ/eventData.h>

#include "TH1D.h"
#include "TH2D.h"
#include "TROOT.h"

//All events
static TH1D *hPiastraStability_allEvents = 0;
static TH1D *hPiastraStability_allEvents_distr = 0;

//Veto Rates
static TH2D *hPiastraStability_Ch[10] = { 0 };

extern "C" {
void InitPlugin(JApplication *app) {
	InitJANAPlugin(app);
	app->AddProcessor(new JEventProcessor_PiastraStability());
}
} // "C"

//------------------
// JEventProcessor_PiastraStability (Constructor)
//------------------
JEventProcessor_PiastraStability::JEventProcessor_PiastraStability() {
	m_ROOTOutput = 0;
}

//------------------
// ~JEventProcessor_PiastraStability (Destructor)
//------------------
JEventProcessor_PiastraStability::~JEventProcessor_PiastraStability() {

}

//------------------
// init
//------------------
jerror_t JEventProcessor_PiastraStability::init(void) {
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

	//The width (in s) of binning in s;
	m_dT = 60;
	//The width (in s) of binning in s for Vetos diff. rate is;
	m_dT2 = 600;

	gPARMS->SetDefaultParameter("PiastraStability:dT", m_dT, "Width of time binning in s");
	gPARMS->SetDefaultParameter("PiastraStability:dT2", m_dT2, "Width of time binning 2D histos in s");


	return NOERROR;
}

//------------------
// brun
//------------------
jerror_t JEventProcessor_PiastraStability::brun(JEventLoop *eventLoop, int32_t runnumber) {
	// This is called whenever the run number changes
	if (m_isFirstCallToBrun) {

		jout << "JEventProcessor_PiastraStability searching m_RootOutput" << endl;
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

	m_isT0Set = 0;
	return NOERROR;
}

//------------------
// evnt
//------------------
jerror_t JEventProcessor_PiastraStability::evnt(JEventLoop *loop, uint64_t eventnumber) {
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


	vector<const PiastraSIPMCalibrationHit*> piastraCalibrationHits;

	const eventData* eData;

	int Qbin;

	//plugin doesn't work for MC.
	if (m_isMC) {
		return NOERROR;
	}

	/*The following happens for EPICS events*/
	if (!m_isMC) {
		try {
			loop->GetSingle(eData);
		} catch (unsigned long e) {
			return OBJECT_NOT_AVAILABLE;
		}
	}

	//Retrieve objects
	loop->Get(piastraCalibrationHits);

	/*In a multi-thread system, it may be that the current event time is later than another event time
	 (in other words, the first event passing here is not the first event in the file / stream)
	 However, since the time is in seconds, the effect should be small and tolerable for a monitoring plugin.
	 */
	japp->RootWriteLock();
	if (!m_isT0Set) {
		m_T0 = eData->time;
		m_isT0Set = 1;
		cout << "PiastraStability set T0 to time: " << m_T0 <<"in s"<<std::endl;
	}
	m_T = (eData->time - m_T0);
	index = m_T / m_dT;
	index2 = m_T / m_dT2;

	if (index < 0) return OBJECT_NOT_AVAILABLE;
	if (index2 < 0) return OBJECT_NOT_AVAILABLE;

	//the position in the map is given by m_T/m_dT
	allEvents[index]=allEvents[index]+1;

	for (int ii = 0; ii < piastraCalibrationHits.size(); ii++) {
		if ((piastraCalibrationHits[ii]->Aphe < m_chargemin) || (piastraCalibrationHits[ii]->Aphe > m_chargemax)) continue;

		Qbin = int(m_nchargestep * (piastraCalibrationHits[ii]->Aphe - m_chargemin) / (m_chargemax - m_chargemin));

		if (piastraCalibrationHits[ii]->m_channel.layer == 0) {
			rate[piastraCalibrationHits[ii]->m_channel.component][index2][Qbin] = rate[piastraCalibrationHits[ii]->m_channel.component][index2][Qbin] + 1;
		}
	}

	japp->RootUnLock();

	return NOERROR;
}

//------------------
// erun
//------------------
jerror_t JEventProcessor_PiastraStability::erun(void) {
	// This is called whenever the run number changes, before it is
	// changed to give you a chance to clean up before processing
	// events from the next run number.
	japp->RootWriteLock();

	if (hPiastraStability_allEvents != NULL) {
		japp->RootUnLock();
		return NOERROR;
	}
	gROOT->cd();
	TDirectory *mainD = gDirectory;
	gDirectory->mkdir("PiastraStability")->cd();

	//Note that we are here assuming that each "bin" in the map is filled - i.e. all the indexes are reported.
	m_nbins = allEvents.size();

	//the "allEvents" map size should determine all binning for all histograms.
	hPiastraStability_allEvents = new TH1D("hPiastraStability_allEvents", "hPiastraStability_allEvents", m_nbins, 0, m_nbins * m_dT);
	hPiastraStability_allEvents_distr = new TH1D("hPiastraStability_allEvents_distr", "hPiastraStability_allEvents_distr", 800, 0, 120);



	for (map_it = allEvents.begin(); map_it != allEvents.end(); map_it++) {
		index = map_it->first;

		hPiastraStability_allEvents->SetBinContent(index + 1, 1. * allEvents[index] / m_dT);
		hPiastraStability_allEvents->SetBinError(index + 1, sqrt(1. * allEvents[index]) / m_dT);
		hPiastraStability_allEvents_distr->Fill(1. * allEvents[index] / m_dT);

	}

	for (int i = 0; i < 4; i++) {
		m_nbinsArray[i] = rate[i].size();
		hPiastraStability_Ch[i] = new TH2D(Form("hPiastraStability_Ch%i", i + 1), Form("hPiastraStability_Ch%i", i + 1), m_nbinsArray[i], 0, m_dT2 * m_nbinsArray[i], m_nchargestep, m_chargemin, m_chargemax);
	}

	for (int i = 0; i < 4; i++) {
		for (rate_it[i] = rate[i].begin(); rate_it[i] != rate[i].end(); rate_it[i]++) {
			for (int k = 0; k < m_nchargestep; k++) {
				hPiastraStability_Ch[i]->Fill((rate_it[i]->first) * m_dT2, m_chargemin + (k + 0.5) * (m_chargemax - m_chargemin) / m_nchargestep, rate_it[i]->second[k]);
			}
		}
	}

	mainD->cd();
	japp->RootUnLock();

	if (m_ROOTOutput != 0) {
		m_ROOTOutput->AddObject(hPiastraStability_allEvents);
		m_ROOTOutput->AddObject(hPiastraStability_allEvents_distr);

		for (int i = 0; i < 4; i++) {

			m_ROOTOutput->AddObject(hPiastraStability_Ch[i]);
		}

	}

	return NOERROR;
}

//------------------
// fini
//------------------
jerror_t JEventProcessor_PiastraStability::fini(void) {
// Called before program exit after event processing is finished.

	return NOERROR;
}

