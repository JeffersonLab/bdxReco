// $Id$
//
//    File: JEventProcessor_BDXMiniSlowControls.cc
// Created: Mon Oct  7 09:20:42 CEST 2019
// Creator: celentan (on Linux apcx4 3.10.0-957.el7.x86_64 x86_64)
//

#include "JEventProcessor_BDXMiniSlowControls.h"
using namespace jana;

// Routine used to create our JEventProcessor
#include <JANA/JApplication.h>
#include <JANA/JFactory.h>

#include <system/JROOTOutput.h>
#include <system/BDXEventProcessor.h>
#include <DAQ/eventData.h>
#include <EPICS/epicsData.h>

#include <map>

#include "TH1D.h"
#include "TROOT.h"

static TH1D *hBDXMiniSlowControls_arduinoT1 = 0;
static TH1D *hBDXMiniSlowControls_arduinoT2 = 0;
static TH1D *hBDXMiniSlowControls_arduinoH1 = 0;
static TH1D *hBDXMiniSlowControls_arduinoH2 = 0;

static TH1D *hBDXMiniSlowControls_daqLT = 0;

static TH1D *hBDXMiniSlowControls_daqT[16] = { 0 };

static TH1D *hBDXMiniSlowControls_beamI = 0;
static TH1D *hBDXMiniSlowControls_beamE = 0;

static TH1D *hBDXMiniSlowControls_envtentT1 = 0;
static TH1D *hBDXMiniSlowControls_envtentT2 = 0;
static TH1D *hBDXMiniSlowControls_envtentH1 = 0;
static TH1D *hBDXMiniSlowControls_envtentH2 = 0;

extern "C" {
void InitPlugin(JApplication *app) {
	InitJANAPlugin(app);
	app->AddProcessor(new JEventProcessor_BDXMiniSlowControls());
}
} // "C"

//------------------
// JEventProcessor_BDXMiniSlowControls (Constructor)
//------------------
JEventProcessor_BDXMiniSlowControls::JEventProcessor_BDXMiniSlowControls() {
	m_ROOTOutput = 0;
}

//------------------
// ~JEventProcessor_BDXMiniSlowControls (Destructor)
//------------------
JEventProcessor_BDXMiniSlowControls::~JEventProcessor_BDXMiniSlowControls() {

}

//------------------
// init
//------------------
jerror_t JEventProcessor_BDXMiniSlowControls::init(void) {
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
	m_dT = 300;

	gPARMS->SetDefaultParameter("BDXMiniSlowControls:dT", m_dT, "Width of time binning in s");

	return NOERROR;
}

//------------------
// brun
//------------------
jerror_t JEventProcessor_BDXMiniSlowControls::brun(JEventLoop *eventLoop, int32_t runnumber) {
	// This is called whenever the run number changes
	if (m_isFirstCallToBrun) {

		jout << "JEventProcessor_BDXSlowControls searching m_RootOutput" << endl;
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
jerror_t JEventProcessor_BDXMiniSlowControls::evnt(JEventLoop *loop, uint64_t eventnumber) {
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

	//plugin doesn't work for MC.
	if (m_isMC) {
		return NOERROR;
	}

	const eventData* eData;
	const epicsData* epicsData;
	/*The following happens for EPICS events*/

	if (!m_isMC) {
		try {
			loop->GetSingle(eData);
		} catch (unsigned long e) {
			return OBJECT_NOT_AVAILABLE;
		}
		try {
			loop->GetSingle(epicsData);
		} catch (unsigned long e) {
			return OBJECT_NOT_AVAILABLE;
		}
	}

	if (eData->eventType != eventSource::EPICS) return OBJECT_NOT_AVAILABLE;

	japp->RootWriteLock();
	if (!m_isT0Set) {
		m_T0 = eData->time; //Time of the first EPICS event here
		m_isT0Set = 1;
		cout << "BDXMiniSlowControls set T0 to unix time: " << m_T0 << std::endl;
	}
	m_T = (eData->time - m_T0);
	index = m_T / m_dT;
	if (index < 0) return OBJECT_NOT_AVAILABLE;

	//Temperatures and humidities from arduino
	if (epicsData->hasData("BDXarduinoT1")) { //All infos from Arduino
		arduinoEvents[index] = arduinoEvents[index] + 1;
		arduinoT1[index] = arduinoT1[index] + epicsData->getDataValue("BDXarduinoT1");
		arduinoT2[index] = arduinoT2[index] + epicsData->getDataValue("BDXarduinoT2");
		arduinoH1[index] = arduinoH1[index] + epicsData->getDataValue("BDXarduinoH1");
		arduinoH2[index] = arduinoH2[index] + epicsData->getDataValue("BDXarduinoH2");

		daqT[0][index] = daqT[0][index] + epicsData->getDataValue("B_DET_BDX_V1725_1_00:Temperature");
		daqT[1][index] = daqT[1][index] + epicsData->getDataValue("B_DET_BDX_V1725_1_04:Temperature");
		daqT[2][index] = daqT[2][index] + epicsData->getDataValue("B_DET_BDX_V1725_1_08:Temperature");
		daqT[3][index] = daqT[3][index] + epicsData->getDataValue("B_DET_BDX_V1725_1_12:Temperature");

		daqT[4][index] = daqT[4][index] + epicsData->getDataValue("B_DET_BDX_V1725_2_00:Temperature");
		daqT[5][index] = daqT[5][index] + epicsData->getDataValue("B_DET_BDX_V1725_2_04:Temperature");
		daqT[6][index] = daqT[6][index] + epicsData->getDataValue("B_DET_BDX_V1725_2_08:Temperature");
		daqT[7][index] = daqT[7][index] + epicsData->getDataValue("B_DET_BDX_V1725_2_12:Temperature");

		daqT[8][index] = daqT[8][index] + epicsData->getDataValue("B_DET_BDX_V1725_3_00:Temperature");
		daqT[9][index] = daqT[9][index] + epicsData->getDataValue("B_DET_BDX_V1725_3_04:Temperature");
		daqT[10][index] = daqT[10][index] + epicsData->getDataValue("B_DET_BDX_V1725_3_08:Temperature");
		daqT[11][index] = daqT[11][index] + epicsData->getDataValue("B_DET_BDX_V1725_3_12:Temperature");

		daqT[12][index] = daqT[12][index] + epicsData->getDataValue("B_DET_BDX_V1725_4_00:Temperature");
		daqT[13][index] = daqT[13][index] + epicsData->getDataValue("B_DET_BDX_V1725_4_04:Temperature");
		daqT[14][index] = daqT[14][index] + epicsData->getDataValue("B_DET_BDX_V1725_4_08:Temperature");
		daqT[15][index] = daqT[15][index] + epicsData->getDataValue("B_DET_BDX_V1725_4_12:Temperature");
	}

	//DAQ
	if (epicsData->hasData("B_DET_BDX_FPGA:livetime")) {
		daqEvents[index] = daqEvents[index] + 1;
		daqLT[index] = daqLT[index] + epicsData->getDataValue("B_DET_BDX_FPGA:livetime");

	}

	//Current and energy
	/*	JANA >>HALLA:dpp 0 1570040004
	 JANA >>HALLA:p 0 1570040004
	 JANA >>MMSHLAE 950.78 1570040004
	 JANA >>hac_bcm_dvm1_current 0.122284 1570040004
	 JANA >>hac_bcm_dvm2_current 0.102519 1570040004
	 JANA >>pcrexHallA_beam_current 0 1570040004
	 */
	if (epicsData->hasData("HALLA:dpp")) {
		beamEvents[index] = beamEvents[index] + 1;
		beamI[index] = beamI[index] + epicsData->getDataValue("pcrexHallA_beam_current"); //CHECK THIS IS STILL OK
		beamE[index] = beamE[index] + epicsData->getDataValue("HALLA:p"); //CHECK THIS IS STILL OK
	}

	//Tent environment
	if (epicsData->hasData("BDX_HW_AKCP1_Ch0_Temp")) {
		envtentEvents[index] = envtentEvents[index] + 1;
		envtentT1[index] = envtentT1[index] + epicsData->getDataValue("BDX_HW_AKCP1_Ch0_Temp");
		envtentT2[index] = envtentT2[index] + epicsData->getDataValue("BDX_HW_AKCP1_Ch1_Temp");
		envtentH1[index] = envtentH1[index] + epicsData->getDataValue("BDX_HW_AKCP1_Ch0_Humid");
		envtentH2[index] = envtentH2[index] + epicsData->getDataValue("BDX_HW_AKCP1_Ch1_Humid");
	}

	japp->RootUnLock();

	return NOERROR;
}

//------------------
// erun
//------------------
jerror_t JEventProcessor_BDXMiniSlowControls::erun(void) {
	// This is called whenever the run number changes, before it is
	// changed to give you a chance to clean up before processing
	// events from the next run number.

	japp->RootWriteLock();

	if (hBDXMiniSlowControls_arduinoT1 != NULL) {
		japp->RootUnLock();
		return NOERROR;
	}
	gROOT->cd();
	TDirectory *mainD = gDirectory;
	gDirectory->mkdir("BDXMiniSlowControls")->cd();

	/*Temperatures*/
	m_nbins = (arduinoEvents.rbegin())->first;
	hBDXMiniSlowControls_arduinoT1 = new TH1D("hBDXMiniSlowControls_arduinoT1", "hBDXMiniSlowControls_arduinoT1", m_nbins, 0, m_nbins * m_dT);
	hBDXMiniSlowControls_arduinoT2 = new TH1D("hBDXMiniSlowControls_arduinoT2", "hBDXMiniSlowControls_arduinoT2", m_nbins, 0, m_nbins * m_dT);
	hBDXMiniSlowControls_arduinoH1 = new TH1D("hBDXMiniSlowControls_arduinoH1", "hBDXMiniSlowControls_arduinoH1", m_nbins, 0, m_nbins * m_dT);
	hBDXMiniSlowControls_arduinoH2 = new TH1D("hBDXMiniSlowControls_arduinoH2", "hBDXMiniSlowControls_arduinoH2", m_nbins, 0, m_nbins * m_dT);

	for (arduinoEvents_it = arduinoEvents.begin(); arduinoEvents_it != arduinoEvents.end(); arduinoEvents_it++) {
		index = arduinoEvents_it->first;
		if (next(arduinoEvents_it) != arduinoEvents.end()) {
			auto arduinoEvents_it2 = arduinoEvents_it;
			arduinoEvents_it2++;
			for (int index2 = index; index2 < arduinoEvents_it2->first; index2++) {
				hBDXMiniSlowControls_arduinoT1->SetBinContent(index2 + 1, 1. * arduinoT1[index] / arduinoEvents[index]);
				hBDXMiniSlowControls_arduinoT2->SetBinContent(index2 + 1, 1. * arduinoT2[index] / arduinoEvents[index]);
				hBDXMiniSlowControls_arduinoH1->SetBinContent(index2 + 1, 1. * arduinoH1[index] / arduinoEvents[index]);
				hBDXMiniSlowControls_arduinoH2->SetBinContent(index2 + 1, 1. * arduinoH2[index] / arduinoEvents[index]);
				for (int ii = 0; ii < 16; ii++) {
					hBDXMiniSlowControls_daqT[ii]->SetBinContent(index2 + 1, 1. * daqT[ii][index] / arduinoEvents[index]);
				}
			}
		} else {
			hBDXMiniSlowControls_arduinoT1->SetBinContent(index + 1, 1. * arduinoT1[index] / arduinoEvents[index]);
			hBDXMiniSlowControls_arduinoT2->SetBinContent(index + 1, 1. * arduinoT2[index] / arduinoEvents[index]);
			hBDXMiniSlowControls_arduinoH1->SetBinContent(index + 1, 1. * arduinoH1[index] / arduinoEvents[index]);
			hBDXMiniSlowControls_arduinoH2->SetBinContent(index + 1, 1. * arduinoH2[index] / arduinoEvents[index]);
			for (int ii = 0; ii < 16; ii++) {
				hBDXMiniSlowControls_daqT[ii]->SetBinContent(index + 1, 1. * daqT[ii][index] / arduinoEvents[index]);
			}
		}
	}
	hBDXMiniSlowControls_arduinoT1->SetBinContent(m_nbins, hBDXMiniSlowControls_arduinoT1->GetBinContent(m_nbins - 1)); //just for graphics, last bin fix
	hBDXMiniSlowControls_arduinoT2->SetBinContent(m_nbins, hBDXMiniSlowControls_arduinoT2->GetBinContent(m_nbins - 1)); //just for graphics, last bin fix
	hBDXMiniSlowControls_arduinoH1->SetBinContent(m_nbins, hBDXMiniSlowControls_arduinoH1->GetBinContent(m_nbins - 1)); //just for graphics, last bin fix
	hBDXMiniSlowControls_arduinoH2->SetBinContent(m_nbins, hBDXMiniSlowControls_arduinoH2->GetBinContent(m_nbins - 1)); //just for graphics, last bin fix

	/*DAQ*/
	m_nbins = (daqEvents.rbegin())->first;
	hBDXMiniSlowControls_daqLT = new TH1D("hBDXMiniSlowControls_daqLT", "hBDXMiniSlowControls_daqLT", m_nbins, 0, m_nbins * m_dT);
	for (int ii = 0; ii < 16; ii++) {
		int slot = ii / 4;
		int ch = ii % 4;
		hBDXMiniSlowControls_daqT[ii] = new TH1D(Form("hBDXMiniSlowControls_daqT_slot%i_ch%i", slot, ch * 4), Form("hBDXMiniSlowControls_daqT_slot%i_ch%i", slot, ch * 4), m_nbins, 0, m_nbins * m_dT);
	}
//	B_DET_BDX_V1725_1_00:Temperature

	for (daqEvents_it = daqEvents.begin(); daqEvents_it != daqEvents.end(); daqEvents_it++) {
		index = daqEvents_it->first;
		if (next(daqEvents_it) != daqEvents.end()) {
			auto daqEvents_it2 = daqEvents_it;
			daqEvents_it2++;
			for (int index2 = index; index2 < daqEvents_it2->first; index2++) {
				hBDXMiniSlowControls_daqLT->SetBinContent(index2 + 1, 1. * daqLT[index] / daqEvents[index]);

			}
		} else {
			hBDXMiniSlowControls_daqLT->SetBinContent(index + 1, 1. * daqLT[index] / daqEvents[index]);

		}
	}
	hBDXMiniSlowControls_daqLT->SetBinContent(m_nbins, hBDXMiniSlowControls_daqLT->GetBinContent(m_nbins - 1)); //just for graphics, last bin fix

	/*BEAM*/
	m_nbins = (beamEvents).rbegin()->first;
	hBDXMiniSlowControls_beamI = new TH1D("hBDXMiniSlowControls_beamI", "hBDXMiniSlowControls_beamI", m_nbins, 0, m_nbins * m_dT);
	hBDXMiniSlowControls_beamE = new TH1D("hBDXMiniSlowControls_beamE", "hBDXMiniSlowControls_beamE", m_nbins, 0, m_nbins * m_dT);
	for (beamEvents_it = beamEvents.begin(); beamEvents_it != beamEvents.end(); beamEvents_it++) {
		index = beamEvents_it->first;
		if (next(beamEvents_it) != beamEvents.end()) {
			auto beamEvents_it2 = beamEvents_it;
			beamEvents_it2++;
			for (int index2 = index; index2 < beamEvents_it2->first; index2++) {
				hBDXMiniSlowControls_beamI->SetBinContent(index2 + 1, 1. * beamI[index] / beamEvents[index]);
				hBDXMiniSlowControls_beamE->SetBinContent(index2 + 1, 1. * beamE[index] / beamEvents[index]);
			}
		} else {
			hBDXMiniSlowControls_beamI->SetBinContent(index + 1, 1. * beamI[index] / beamEvents[index]);
			hBDXMiniSlowControls_beamE->SetBinContent(index + 1, 1. * beamE[index] / beamEvents[index]);
		}
	}
	hBDXMiniSlowControls_beamI->SetBinContent(m_nbins, hBDXMiniSlowControls_beamI->GetBinContent(m_nbins - 1)); //just for graphics, last bin fix
	hBDXMiniSlowControls_beamE->SetBinContent(m_nbins, hBDXMiniSlowControls_beamE->GetBinContent(m_nbins - 1)); //just for graphics, last bin fix

	/*TENT*/
	m_nbins = (envtentEvents).rbegin()->first;
	hBDXMiniSlowControls_envtentT1 = new TH1D("hBDXMiniSlowControls_envtentT1", "hBDXMiniSlowControls_envtentT1", m_nbins, 0, m_nbins * m_dT);
	hBDXMiniSlowControls_envtentT2 = new TH1D("hBDXMiniSlowControls_envtentT2", "hBDXMiniSlowControls_envtentT2", m_nbins, 0, m_nbins * m_dT);
	hBDXMiniSlowControls_envtentH1 = new TH1D("hBDXMiniSlowControls_envtentH1", "hBDXMiniSlowControls_envtentH1", m_nbins, 0, m_nbins * m_dT);
	hBDXMiniSlowControls_envtentH2 = new TH1D("hBDXMiniSlowControls_envtentH2", "hBDXMiniSlowControls_envtentH2", m_nbins, 0, m_nbins * m_dT);
	for (envtentEvents_it = envtentEvents.begin(); envtentEvents_it != envtentEvents.end(); envtentEvents_it++) {
		index = envtentEvents_it->first;
		if (next(envtentEvents_it) != envtentEvents.end()) {
			auto envtentEvents_it2 = envtentEvents_it;
			envtentEvents_it2++;
			for (int index2 = index; index2 < envtentEvents_it2->first; index2++) {
				hBDXMiniSlowControls_envtentT1->SetBinContent(index2 + 1, 1. * envtentT1[index] / envtentEvents[index]);
				hBDXMiniSlowControls_envtentT2->SetBinContent(index2 + 1, 1. * envtentT2[index] / envtentEvents[index]);
				hBDXMiniSlowControls_envtentH1->SetBinContent(index2 + 1, 1. * envtentH1[index] / envtentEvents[index]);
				hBDXMiniSlowControls_envtentH2->SetBinContent(index2 + 1, 1. * envtentH2[index] / envtentEvents[index]);
			}
		} else {
			hBDXMiniSlowControls_envtentT1->SetBinContent(index + 1, 1. * envtentT1[index] / envtentEvents[index]);
			hBDXMiniSlowControls_envtentT2->SetBinContent(index + 1, 1. * envtentT2[index] / envtentEvents[index]);
			hBDXMiniSlowControls_envtentH1->SetBinContent(index + 1, 1. * envtentH1[index] / envtentEvents[index]);
			hBDXMiniSlowControls_envtentH2->SetBinContent(index + 1, 1. * envtentH2[index] / envtentEvents[index]);
		}

	}
	hBDXMiniSlowControls_envtentT1->SetBinContent(m_nbins, hBDXMiniSlowControls_envtentT1->GetBinContent(m_nbins - 1)); //just for graphics, last bin fix
	hBDXMiniSlowControls_envtentT2->SetBinContent(m_nbins, hBDXMiniSlowControls_envtentT2->GetBinContent(m_nbins - 1)); //just for graphics, last bin fix
	hBDXMiniSlowControls_envtentH1->SetBinContent(m_nbins, hBDXMiniSlowControls_envtentH1->GetBinContent(m_nbins - 1)); //just for graphics, last bin fix
	hBDXMiniSlowControls_envtentH2->SetBinContent(m_nbins, hBDXMiniSlowControls_envtentH2->GetBinContent(m_nbins - 1)); //just for graphics, last bin fix

	/*SAVE*/
	mainD->cd();
	japp->RootUnLock();
	if (m_ROOTOutput != 0) {
		m_ROOTOutput->AddObject(hBDXMiniSlowControls_arduinoT1);
		m_ROOTOutput->AddObject(hBDXMiniSlowControls_arduinoT2);
		m_ROOTOutput->AddObject(hBDXMiniSlowControls_arduinoH1);
		m_ROOTOutput->AddObject(hBDXMiniSlowControls_arduinoH2);

		m_ROOTOutput->AddObject(hBDXMiniSlowControls_daqLT);

		m_ROOTOutput->AddObject(hBDXMiniSlowControls_beamI);
		m_ROOTOutput->AddObject(hBDXMiniSlowControls_beamE);

		m_ROOTOutput->AddObject(hBDXMiniSlowControls_envtentT1);
		m_ROOTOutput->AddObject(hBDXMiniSlowControls_envtentT2);
		m_ROOTOutput->AddObject(hBDXMiniSlowControls_envtentH1);
		m_ROOTOutput->AddObject(hBDXMiniSlowControls_envtentH2);

		for (int ii = 0; ii < 16; ii++) {
			m_ROOTOutput->AddObject(hBDXMiniSlowControls_daqT[ii]);
		}

	}

	return NOERROR;
}

//------------------
// fini
//------------------
jerror_t JEventProcessor_BDXMiniSlowControls::fini(void) {
	// Called before program exit after event processing is finished.
	return NOERROR;
}

