
// $Id$
//
//    File: JEventProcessor_BDXMiniStability.cc
// Created: Fri May 24 19:02:10 CEST 2019
// Creator: celentan (on Linux apcx4 3.10.0-957.el7.x86_64 x86_64)
//

#include "JEventProcessor_BDXMiniStability.h"
using namespace jana;

// Routine used to create our JEventProcessor
#include <JANA/JApplication.h>
#include <JANA/JFactory.h>

// Routine used to create our JEventProcessor
#include <JANA/JApplication.h>
#include <JANA/JFactory.h>

#include "calibration/BDXMiniVetoCalibrationHit/BDXMiniVetoCalibrationHit.h"

#include <system/JROOTOutput.h>
#include <system/BDXEventProcessor.h>

#include "Calorimeter/CalorimeterDigiHit.h"
#include "Calorimeter/CalorimeterHit.h"
#include "Calorimeter/CalorimeterCluster.h"
#include "IntVeto/IntVetoDigiHit.h"
#include <DAQ/eventData.h>
#include <BDXmini/triggerDataBDXmini.h>

#include "TH1D.h"
#include "TH2D.h"
#include "TROOT.h"

//All events
static TH1D *hBDXMiniStability_allEvents = 0;
static TH1D *hBDXMiniStability_allEvents_distr = 0;
//Trgs
static TH1D *hBDXMiniStability_trg[triggerDataBDXmini::nTriggersMAX] = { 0 };
static TH1D *hBDXMiniStability_trg_distr[triggerDataBDXmini::nTriggersMAX] = { 0 };
//Veto Rates
static TH2D *hBDXMiniStability_VetoL0_Ch[10] = { 0 };
static TH2D *hBDXMiniStability_VetoL1_Ch[10] = { 0 };


//High-E and High-E, antiVeto
static TH1D *hBDXMiniStability_highE = 0;
static TH1D *hBDXMiniStability_highE_antiVeto = 0;

extern "C" {
void InitPlugin(JApplication *app) {
	InitJANAPlugin(app);
	app->AddProcessor(new JEventProcessor_BDXMiniStability());
}
} // "C"

//------------------
// JEventProcessor_BDXMiniStability (Constructor)
//------------------
JEventProcessor_BDXMiniStability::JEventProcessor_BDXMiniStability() {
	m_ROOTOutput=0;
}

//------------------
// ~JEventProcessor_BDXMiniStability (Destructor)
//------------------
JEventProcessor_BDXMiniStability::~JEventProcessor_BDXMiniStability() {

}

//------------------
// init
//------------------
jerror_t JEventProcessor_BDXMiniStability::init(void) {
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
	m_dT2 = 3000;

	gPARMS->SetDefaultParameter("BDXMiniStability:dT", m_dT, "Width of time binning in s");

	//The energy threshold in MeV
	m_thrEneTot = 100;
	gPARMS->SetDefaultParameter("BDXMiniStability:thrEneTot", m_thrEneTot, "Energy tot threshold in MeV");
	m_thrEneSeed = 20;
	gPARMS->SetDefaultParameter("BDXMiniStability:thrEneSeed", m_thrEneSeed, "Energy seed threshold in MeV");

	m_thrVetoCaps = 5.;
	// gPARMS->SetDefaultParameter("BDXMiniCalorimeterEnergyCalibration:THR_CAPS", m_thrIntVetoCaps, "Threshold in phe (charge) for each of the caps");

	m_thrVetoL = 5.;
	// gPARMS->SetDefaultParameter("BDXMiniCalorimeterEnergyCalibration:THR_1L", m_thrIntVetoL, "Threshold in phe (charge) for each of the lateral counters");

	return NOERROR;
}

//------------------
// brun
//------------------
jerror_t JEventProcessor_BDXMiniStability::brun(JEventLoop *eventLoop, int32_t runnumber) {
	// This is called whenever the run number changes
	if (m_isFirstCallToBrun) {

		jout << "JEventProcessor_BDXMiniStability searching m_RootOutput" << endl;
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
	trgs.clear();
	trgs.resize(triggerDataBDXmini::nTriggersMAX);

	m_isT0Set = 0;
	return NOERROR;
}

//------------------
// evnt
//------------------
jerror_t JEventProcessor_BDXMiniStability::evnt(JEventLoop *loop, uint64_t eventnumber) {
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

	vector<const CalorimeterCluster*> caloClusters;
	vector<const IntVetoDigiHit*> intVetoDigiHits;

	const eventData* eData;
	const triggerDataBDXmini* tData;

	int Qbin;

	//plugin doesn't work for MC.
	if (m_isMC) {
		return NOERROR;
	}

	/*The following happens for EPICS events*/
	if (!m_isMC) {
		try {
			loop->GetSingle(tData); /*This excludes reading all slow-control events before the first physics event*/
		} catch (unsigned long e) {
			return OBJECT_NOT_AVAILABLE;
		}

		try {
			loop->GetSingle(eData);
		} catch (unsigned long e) {
			return OBJECT_NOT_AVAILABLE;
		}
	}

	//Retrieve objects
	loop->Get(caloClusters);
	loop->Get(intVetoDigiHits);

	/*In a multi-thread system, it may be that the current event time is later than another event time
	 (in other words, the first event passing here is not the first event in the file / stream)
	 However, since the time is in seconds, the effect should be small and tolerable for a monitoring plugin.
	 */
	japp->RootWriteLock();
	if (!m_isT0Set) {
		m_T0 = eData->time;
		m_isT0Set = 1;
		cout<<"BDXMiniStability set T0 to unix time: "<<m_T0<<std::endl;
	}
	m_T = (eData->time - m_T0);
	index = m_T / m_dT;
	index2 = m_T / m_dT2;

	if (index<0) return OBJECT_NOT_AVAILABLE;

	//the position in the map is given by m_T/m_dT

	//all events
	allEvents[index] = allEvents[index] + 1; //the lhs [] operator creates the element at index position if doesn't exists and init to 0 (as from C++ standard)

	//Per each trigger
	for (int itrg = 0; itrg < triggerDataBDXmini::nTriggersMAX; itrg++) {
		if (tData->getNtriggers_single(itrg)) trgs[itrg][index] = trgs[itrg][index] + 1;
	}

	//high energy events
	int nHighEnergy = 0;
	for (int ii = 0; ii < caloClusters.size(); ii++) {
		if ((caloClusters[ii]->Eseed > m_thrEneSeed) && (caloClusters[ii]->E > m_thrEneTot)) {
			nHighEnergy++;
		}
	}

	//veto
	int nVetoL0 = 0;
	int nVetoL1 = 0;

	for (int ii = 0; ii < intVetoDigiHits.size(); ii++) {
		if ((intVetoDigiHits[ii]->m_channel.component <= 8) && (intVetoDigiHits[ii]->Qphe > m_thrVetoL)) {
			if (intVetoDigiHits[ii]->m_channel.layer == 0) nVetoL0++;
			else if (intVetoDigiHits[ii]->m_channel.layer == 1) nVetoL1++;
		} else if ((intVetoDigiHits[ii]->m_channel.component > 8) && (intVetoDigiHits[ii]->Qphe > m_thrVetoCaps)) {
			if (intVetoDigiHits[ii]->m_channel.layer == 0) nVetoL0++;
			else if (intVetoDigiHits[ii]->m_channel.layer == 1) nVetoL1++;
		}
	}

	for (int ii = 0; ii < intVetoDigiHits.size(); ii++) {
		if ((intVetoDigiHits[ii]->Aphe<m_chargemin)||(intVetoDigiHits[ii]->Aphe>m_chargemax)) continue;
		if ((intVetoDigiHits[ii]->T<m_timemin)||(intVetoDigiHits[ii]->T>m_timemax)) continue;
		Qbin = int(m_nchargestep*(intVetoDigiHits[ii]->Aphe-m_chargemin)/(m_chargemax-m_chargemin));

		if (intVetoDigiHits[ii]->m_channel.layer == 0) {
			rateL0[intVetoDigiHits[ii]->m_channel.component-1][index2][Qbin] = rateL0[intVetoDigiHits[ii]->m_channel.component-1][index2][Qbin] +1;
		}
		else if (intVetoDigiHits[ii]->m_channel.layer == 1) {
			rateL1[intVetoDigiHits[ii]->m_channel.component-1][index2][Qbin] = rateL1[intVetoDigiHits[ii]->m_channel.component-1][index2][Qbin] +1;
		}
	}

	if (nHighEnergy) {
		highE[index] = highE[index] + 1;
		if ((nVetoL0 == 0) && (nVetoL1 == 0)) {
			//jout << "BDXMiniStability:highE_antiVeto " << eData->eventN << " " << nVetoL0 << " " << nVetoL1 << endl << endl;
			highE_antiVeto[index] = highE_antiVeto[index] + 1;
		}
	}




	japp->RootUnLock();

	return NOERROR;
}

//------------------
// erun
//------------------
jerror_t JEventProcessor_BDXMiniStability::erun(void) {
	// This is called whenever the run number changes, before it is
	// changed to give you a chance to clean up before processing
	// events from the next run number.
	japp->RootWriteLock();

	if (hBDXMiniStability_allEvents != NULL) {
		japp->RootUnLock();
		return NOERROR;
	}
	gROOT->cd();
	TDirectory *mainD = gDirectory;
	gDirectory->mkdir("BDXMiniStability")->cd();

	//Note that we are here assuming that each "bin" in the map is filled - i.e. all the indexes are reported.
	m_nbins = allEvents.size();


	//the "allEvents" map size should determine all binning for all histograms.
	hBDXMiniStability_allEvents = new TH1D("hBDXMiniStability_allEvents", "hBDXMiniStability_allEvents", m_nbins, 0, m_nbins * m_dT);
	hBDXMiniStability_allEvents_distr = new TH1D("hBDXMiniStability_allEvents_distr", "hBDXMiniStability_allEvents_distr", 400, 0, 20);
	//Trgs
	for (int itrg = 0; itrg < triggerDataBDXmini::nTriggersMAX; itrg++) {
		hBDXMiniStability_trg[itrg] = new TH1D(Form("hBDXMiniStability_trg_%i", itrg), Form("hBDXMiniStability_trg_%i", itrg), m_nbins, 0, m_nbins * m_dT);
		hBDXMiniStability_trg_distr[itrg] = new TH1D(Form("hBDXMiniStability_trg_distr_%i", itrg), Form("hBDXMiniStability_trg_distr_%i", itrg), 400, 0, 20);
	}

	hBDXMiniStability_highE = new TH1D("hBDXMiniStability_highE", "hBDXMiniStability_highE", m_nbins, 0, m_nbins * m_dT);
	hBDXMiniStability_highE_antiVeto = new TH1D("hBDXMiniStability_highE_antiVeto", "hBDXMiniStability_highE_antiVeto", m_nbins, 0, m_nbins * m_dT);

	for (map_it = allEvents.begin(); map_it != allEvents.end(); map_it++) {
		index = map_it->first;

		hBDXMiniStability_allEvents->SetBinContent(index+1, 1. * allEvents[index] / m_dT);
		hBDXMiniStability_allEvents->SetBinError(index+1, sqrt(1. * allEvents[index]) / m_dT);
		hBDXMiniStability_allEvents_distr->Fill(1. * allEvents[index] / m_dT);

		for (int itrg = 0; itrg < triggerDataBDXmini::nTriggersMAX; itrg++) {
			hBDXMiniStability_trg[itrg]->SetBinContent(index+1, 1. * trgs[itrg][index] / m_dT);
			hBDXMiniStability_trg[itrg]->SetBinError(index+1, sqrt(1. * trgs[itrg][index]) / m_dT);
			hBDXMiniStability_trg_distr[itrg]->Fill(1. * trgs[itrg][index] / m_dT);
		}

		hBDXMiniStability_highE->SetBinContent(index+1, 1. * highE[index] / m_dT);
		hBDXMiniStability_highE->SetBinError(index+1, sqrt(1. * highE[index]) / m_dT);

		hBDXMiniStability_highE_antiVeto->SetBinContent(index+1, 1. * highE_antiVeto[index] / m_dT);
		hBDXMiniStability_highE_antiVeto->SetBinError(index+1, sqrt(1. * highE_antiVeto[index]) / m_dT);

	}



	for(int i =0; i< 10; i++){

		m_nbinsL0[i] = rateL0[i].size();
		m_nbinsL1[i] = rateL1[i].size();

		hBDXMiniStability_VetoL0_Ch[i] = new TH2D(Form("hBDXMiniStability_VetoL0_Ch%i", i+1), Form("hBDXMiniStability_VetoL0_Ch%i", i+1), m_nbinsL0[i],0, m_dT2*m_nbinsL0[i],m_nchargestep,m_chargemin,m_chargemax);
		hBDXMiniStability_VetoL1_Ch[i] = new TH2D(Form("hBDXMiniStability_VetoL1_Ch%i", i+1), Form("hBDXMiniStability_VetoL1_Ch%i", i+1), m_nbinsL1[i],0, m_dT2*m_nbinsL1[i],m_nchargestep,m_chargemin,m_chargemax);
	}


	for(int i =0; i < 10; i++){
		for (rateL0_it[i] = rateL0[i].begin(); rateL0_it[i] != rateL0[i].end(); rateL0_it[i]++) {
			for (int k =0; k< m_nchargestep; k++){
				    hBDXMiniStability_VetoL0_Ch[i]->Fill( (rateL0_it[i]->first)*m_dT2 ,m_chargemin+(k+0.5)*(m_chargemax-m_chargemin)/m_nchargestep ,rateL0_it[i]->second[k]);
			}
		}
	}

	for(int i =0; i < 10; i++){
			for (rateL1_it[i] = rateL1[i].begin(); rateL1_it[i] != rateL1[i].end(); rateL1_it[i]++) {

				for (int k =0; k< m_nchargestep; k++){
					hBDXMiniStability_VetoL1_Ch[i]->Fill((rateL1_it[i]->first)*m_dT2 ,m_chargemin+(k+0.5)*(m_chargemax-m_chargemin)/m_nchargestep,rateL1_it[i]->second[k]);
				}
			}
		}


	mainD->cd();
	japp->RootUnLock();

	if (m_ROOTOutput != 0) {
		m_ROOTOutput->AddObject(hBDXMiniStability_allEvents);
		m_ROOTOutput->AddObject(hBDXMiniStability_allEvents_distr);
		for (int itrg = 0; itrg < triggerDataBDXmini::nTriggersMAX; itrg++) {
			m_ROOTOutput->AddObject(hBDXMiniStability_trg[itrg]);
			m_ROOTOutput->AddObject(hBDXMiniStability_trg_distr[itrg]);
		}
		for(int i = 0; i<10; i++) {

			m_ROOTOutput->AddObject(hBDXMiniStability_VetoL0_Ch[i]);
		}
		for(int i = 0; i<10; i++) {

			m_ROOTOutput->AddObject(hBDXMiniStability_VetoL1_Ch[i]);
				}
		m_ROOTOutput->AddObject(hBDXMiniStability_highE);
		m_ROOTOutput->AddObject(hBDXMiniStability_highE_antiVeto);
	}

	return NOERROR;
}

//------------------
// fini
//------------------
jerror_t JEventProcessor_BDXMiniStability::fini(void) {
// Called before program exit after event processing is finished.

	return NOERROR;
}

