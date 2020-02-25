// $Id$
//
//    File: JEventProcessor_BDXMiniVetoSIPMCalibration.cc
// Created: Tue Aug 20 15:47:25 CEST 2019
// Creator: celentan (on Linux apcx4 3.10.0-957.el7.x86_64 x86_64)
//

#include "JEventProcessor_BDXMiniVetoSIPMCalibration.h"
using namespace jana;

// Routine used to create our JEventProcessor
#include <JANA/JApplication.h>
#include <JANA/JFactory.h>

#include "calibration/BDXMiniVetoSIPMCalibrationHit/BDXMiniVetoSIPMCalibrationHit.h"
#include "DAQ/eventData.h"

#include <system/JROOTOutput.h>
#include <system/BDXEventProcessor.h>

#include "TROOT.h"
/*Here goes the histograms*/
static const int nTOT = 10;

//Charge
static TH1D *hBDXMiniVetoSIPMCalibrationQL0[nTOT] = { 0 };
static TH1D *hBDXMiniVetoSIPMCalibrationQL1[nTOT] = { 0 };

//Charge-PHE
static TH1D *hBDXMiniVetoSIPMCalibrationQPHEL0[nTOT] = { 0 };
static TH1D *hBDXMiniVetoSIPMCalibrationQPHEL1[nTOT] = { 0 };

//Amplitude
static TH1D *hBDXMiniVetoSIPMCalibrationAL0[nTOT] = { 0 };
static TH1D *hBDXMiniVetoSIPMCalibrationAL1[nTOT] = { 0 };

//Amplitude-PHE
static TH1D *hBDXMiniVetoSIPMCalibrationAPHEL0[nTOT] = { 0 };
static TH1D *hBDXMiniVetoSIPMCalibrationAPHEL1[nTOT] = { 0 };

extern "C" {
void InitPlugin(JApplication *app) {
	InitJANAPlugin(app);
	app->AddProcessor(new JEventProcessor_BDXMiniVetoSIPMCalibration());
}
} // "C"

//------------------
// JEventProcessor_BDXMiniVetoSIPMCalibration (Constructor)
//------------------
JEventProcessor_BDXMiniVetoSIPMCalibration::JEventProcessor_BDXMiniVetoSIPMCalibration() :
		m_ROOTOutput(0), m_isMC(0) {
	m_isFirstCallToBrun = 1;
}

//------------------
// ~JEventProcessor_BDXMiniVetoSIPMCalibration (Destructor)
//------------------
JEventProcessor_BDXMiniVetoSIPMCalibration::~JEventProcessor_BDXMiniVetoSIPMCalibration() {

}

//------------------
// init
//------------------
jerror_t JEventProcessor_BDXMiniVetoSIPMCalibration::init(void) {
	// This is called once at program startup. If you are creating
	// and filling historgrams in this plugin, you should lock the
	// ROOT mutex like this:
	//
	// japp->RootWriteLock();
	//  ... fill historgrams or trees ...
	// japp->RootUnLock();
	//
	japp->RootWriteLock();

	if (hBDXMiniVetoSIPMCalibrationQL0[0] != NULL) {
		japp->RootUnLock();
		return NOERROR;
	}

	gROOT->cd();
	TDirectory *main = gDirectory;
	gDirectory->mkdir("BDXMiniVetoSIPMCalib")->cd();

	/*Create all the histograms*/

	double Qmin, Qmax, Emin, Emax, Amin, Amax;
	int NA,NQ;

	if (m_isMC) {
		Qmin = -100;
		Qmax = 300;

		Amin=-20;
		Amax=70;

		NQ = 500;
		NA = 500;
	} else {
		Qmin = -100.;
		Qmax = 1000;

		Amin=-20;
		Amax=70;

		NQ = 500;
		NA = 500;
	}

	for (int ii = 0; ii < nTOT; ii++) {
		hBDXMiniVetoSIPMCalibrationQL0[ii] = new TH1D(Form("hBDXMiniVetoSIPMQ_L0_C%i", ii + 1), Form("hBDXMiniVetoSIPMQ_L0_C%i", ii + 1), NQ, Qmin, Qmax);
		hBDXMiniVetoSIPMCalibrationQL1[ii] = new TH1D(Form("hBDXMiniVetoSIPMQ_L1_C%i", ii + 1), Form("hBDXMiniVetoSIPMQ_L1_C%i", ii + 1), NQ, Qmin, Qmax);

		hBDXMiniVetoSIPMCalibrationQPHEL0[ii] = new TH1D(Form("hBDXMiniVetoSIPMQPHE_L0_C%i", ii + 1), Form("hBDXMiniVetoSIPMQPHE_L0_c%i", ii + 1), NQ, -3, 10);
		hBDXMiniVetoSIPMCalibrationQPHEL1[ii] = new TH1D(Form("hBDXMiniVetoSIPMQPHE_L1_C%i", ii + 1), Form("hBDXMiniVetoSIPMQPHE_L1_c%i", ii + 1), NQ, -3, 10);

		hBDXMiniVetoSIPMCalibrationAL0[ii] = new TH1D(Form("hBDXMiniVetoSIPMA_L0_C%i", ii + 1), Form("hBDXMiniVetoSIPMA_L0_C%i", ii + 1), NA, Amin, Amax);
		hBDXMiniVetoSIPMCalibrationAL1[ii] = new TH1D(Form("hBDXMiniVetoSIPMA_L1_C%i", ii + 1), Form("hBDXMiniVetoSIPMA_L1_C%i", ii + 1), NA, Amin, Amax);

		hBDXMiniVetoSIPMCalibrationAPHEL0[ii] = new TH1D(Form("hBDXMiniVetoSIPMAPHE_L0_C%i", ii + 1), Form("hBDXMiniVetoSIPMAPHE_L0_c%i", ii + 1), NA, -3, 10);
		hBDXMiniVetoSIPMCalibrationAPHEL1[ii] = new TH1D(Form("hBDXMiniVetoSIPMAPHE_L1_C%i", ii + 1), Form("hBDXMiniVetoSIPMAPHE_L1_c%i", ii + 1), NA, -3, 10);

	}

	// back to main dir
	main->cd();
	japp->RootUnLock();

	return NOERROR;
}

//------------------
// brun
//------------------
jerror_t JEventProcessor_BDXMiniVetoSIPMCalibration::brun(JEventLoop *eventLoop, int32_t runnumber) {
	jout << m_isFirstCallToBrun << endl;

	if (m_isFirstCallToBrun) {

		jout << "JEventProcessor_BDXMiniCalorimeterEnergyCalibration::brun searching m_RootOutput" << endl;
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

	// This is called whenever the run number changes
	return NOERROR;

}

//------------------
// evnt
//------------------
jerror_t JEventProcessor_BDXMiniVetoSIPMCalibration::evnt(JEventLoop *loop, uint64_t eventnumber) {
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

	vector<const BDXMiniVetoSIPMCalibrationHit*> veto_hits;
	vector<const BDXMiniVetoSIPMCalibrationHit*>::const_iterator veto_hits_it;
	const BDXMiniVetoSIPMCalibrationHit* veto_hit;

	loop->Get(veto_hits);

	int layer, component;

	for (veto_hits_it = veto_hits.begin(); veto_hits_it != veto_hits.end(); veto_hits_it++) {
		veto_hit = *veto_hits_it;
		layer = veto_hit->m_channel.layer;
		component = veto_hit->m_channel.component;

		//A.C. skip SIPM in csc (0,1,2), i.e. Sector0 Layer0 Component3
		if ((layer == 0) && (component == 3)) continue;

		if (layer == 0) {
			hBDXMiniVetoSIPMCalibrationQL0[component - 1]->Fill(veto_hit->Qraw);
			hBDXMiniVetoSIPMCalibrationQPHEL0[component - 1]->Fill(veto_hit->Qphe);

			hBDXMiniVetoSIPMCalibrationAL0[component - 1]->Fill(veto_hit->Araw);
			hBDXMiniVetoSIPMCalibrationAPHEL0[component - 1]->Fill(veto_hit->Aphe);
		} else if (layer == 1) {
			hBDXMiniVetoSIPMCalibrationQL1[component - 1]->Fill(veto_hit->Qraw);
			hBDXMiniVetoSIPMCalibrationQPHEL1[component - 1]->Fill(veto_hit->Qphe);

			hBDXMiniVetoSIPMCalibrationAL1[component - 1]->Fill(veto_hit->Araw);
			hBDXMiniVetoSIPMCalibrationAPHEL1[component - 1]->Fill(veto_hit->Aphe);
		}

	}

	return NOERROR;
}

//------------------
// erun
//------------------
jerror_t JEventProcessor_BDXMiniVetoSIPMCalibration::erun(void) {
	// This is called whenever the run number changes, before it is
	// changed to give you a chance to clean up before processing
	// events from the next run number.
	if (m_ROOTOutput != 0) {
		jout << "JEventProcessor_BDXMiniVetoSIPMCalibration adding histos to m_ROOTOutput" << endl;
		for (int ii = 0; ii < nTOT; ii++) {
			m_ROOTOutput->AddObject(hBDXMiniVetoSIPMCalibrationQL0[ii]);
			m_ROOTOutput->AddObject(hBDXMiniVetoSIPMCalibrationQL1[ii]);
			m_ROOTOutput->AddObject(hBDXMiniVetoSIPMCalibrationQPHEL0[ii]);
			m_ROOTOutput->AddObject(hBDXMiniVetoSIPMCalibrationQPHEL1[ii]);

			m_ROOTOutput->AddObject(hBDXMiniVetoSIPMCalibrationAL0[ii]);
			m_ROOTOutput->AddObject(hBDXMiniVetoSIPMCalibrationAL1[ii]);
			m_ROOTOutput->AddObject(hBDXMiniVetoSIPMCalibrationAPHEL0[ii]);
			m_ROOTOutput->AddObject(hBDXMiniVetoSIPMCalibrationAPHEL1[ii]);
		}
	}

	return NOERROR;
}

//------------------
// fini
//------------------
jerror_t JEventProcessor_BDXMiniVetoSIPMCalibration::fini(void) {
	// Called before program exit after event processing is finished.
	return NOERROR;
}

