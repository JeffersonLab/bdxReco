// $Id$
//
//    File: JEventProcessor_PiastraSIPMCalibration.cc
// Created: Tue Aug 20 15:47:25 CEST 2019
// Creator: celentan (on Linux apcx4 3.10.0-957.el7.x86_64 x86_64)
//

#include "JEventProcessor_PiastraSIPMCalibration.h"
using namespace jana;

// Routine used to create our JEventProcessor
#include <JANA/JApplication.h>
#include <JANA/JFactory.h>

#include "calibration/PiastraSIPMCalibrationHit/PiastraSIPMCalibrationHit.h"
#include "DAQ/eventData.h"

#include <system/JROOTOutput.h>
#include <system/BDXEventProcessor.h>

#include "TROOT.h"
/*Here goes the histograms*/
static const int nTOT = 4;

//Charge
static TH1D *hPiastraSIPMCalibrationQ[nTOT] = { 0 };

//Charge-PHE
static TH1D *hPiastraSIPMCalibrationQPHE[nTOT] = { 0 };

//Amplitude
static TH1D *hPiastraSIPMCalibrationA[nTOT] = { 0 };

//Amplitude-PHE
static TH1D *hPiastraSIPMCalibrationAPHE[nTOT] = { 0 };

extern "C" {
void InitPlugin(JApplication *app) {
	InitJANAPlugin(app);
	app->AddProcessor(new JEventProcessor_PiastraSIPMCalibration());
}
} // "C"

//------------------
// JEventProcessor_PiastraSIPMCalibration (Constructor)
//------------------
JEventProcessor_PiastraSIPMCalibration::JEventProcessor_PiastraSIPMCalibration() :
		m_ROOTOutput(0), m_isMC(0) {
	m_isFirstCallToBrun = 1;
}

//------------------
// ~JEventProcessor_PiastraSIPMCalibration (Destructor)
//------------------
JEventProcessor_PiastraSIPMCalibration::~JEventProcessor_PiastraSIPMCalibration() {

}

//------------------
// init
//------------------
jerror_t JEventProcessor_PiastraSIPMCalibration::init(void) {
	// This is called once at program startup. If you are creating
	// and filling historgrams in this plugin, you should lock the
	// ROOT mutex like this:
	//
	// japp->RootWriteLock();
	//  ... fill historgrams or trees ...
	// japp->RootUnLock();
	//
	japp->RootWriteLock();

	if (hPiastraSIPMCalibrationQ[0] != NULL) {
		japp->RootUnLock();
		return NOERROR;
	}

	gROOT->cd();
	TDirectory *main = gDirectory;
	gDirectory->mkdir("PiastraSIPMCalib")->cd();

	/*Create all the histograms*/

	double Qmin, Qmax, Emin, Emax, Amin, Amax;
	int NA, NQ;

	Qmin = -100.;
	Qmax = 1000;

	Amin = -20;
	Amax = 500;

	NQ = 500;
	NA = 1100;

	for (int ii = 0; ii < nTOT; ii++) {
		hPiastraSIPMCalibrationQ[ii] = new TH1D(Form("hPiastraSIPMQ_C%i", ii + 1), Form("hPiastraSIPMQ_C%i", ii + 1), NQ, Qmin, Qmax);
		hPiastraSIPMCalibrationQPHE[ii] = new TH1D(Form("hPiastraSIPMQPHE_C%i", ii + 1), Form("hPiastraSIPMQPHE_c%i", ii + 1), NQ, -3, 25);
		hPiastraSIPMCalibrationA[ii] = new TH1D(Form("hPiastraSIPMA_C%i", ii + 1), Form("hPiastraSIPMA_C%i", ii + 1), NA, Amin, Amax);
		hPiastraSIPMCalibrationAPHE[ii] = new TH1D(Form("hPiastraSIPMAPHE_C%i", ii + 1), Form("hPiastraSIPMAPHE_c%i", ii + 1), NA, -3, 50);

	}

	// back to main dir
	main->cd();
	japp->RootUnLock();

	return NOERROR;
}

//------------------
// brun
//------------------
jerror_t JEventProcessor_PiastraSIPMCalibration::brun(JEventLoop *eventLoop, int32_t runnumber) {
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
jerror_t JEventProcessor_PiastraSIPMCalibration::evnt(JEventLoop *loop, uint64_t eventnumber) {
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

	vector<const PiastraSIPMCalibrationHit*> veto_hits;
	vector<const PiastraSIPMCalibrationHit*>::const_iterator veto_hits_it;
	const PiastraSIPMCalibrationHit* veto_hit;

	loop->Get(veto_hits);

	int layer, component;

	for (veto_hits_it = veto_hits.begin(); veto_hits_it != veto_hits.end(); veto_hits_it++) {
		veto_hit = *veto_hits_it;
		layer = veto_hit->m_channel.layer;
		component = veto_hit->m_channel.component;


		//A.C. skip SIPM in csc (0,1,2), i.e. Sector0 Layer0 Component3

		hPiastraSIPMCalibrationQ[component]->Fill(veto_hit->Qraw);
		hPiastraSIPMCalibrationQPHE[component]->Fill(veto_hit->Qphe);

		hPiastraSIPMCalibrationA[component]->Fill(veto_hit->Araw);
		hPiastraSIPMCalibrationAPHE[component]->Fill(veto_hit->Aphe);

	}

	return NOERROR;
}

//------------------
// erun
//------------------
jerror_t JEventProcessor_PiastraSIPMCalibration::erun(void) {
	// This is called whenever the run number changes, before it is
	// changed to give you a chance to clean up before processing
	// events from the next run number.
	if (m_ROOTOutput != 0) {
		jout << "JEventProcessor_PiastraSIPMCalibration adding histos to m_ROOTOutput" << endl;
		for (int ii = 0; ii < nTOT; ii++) {
			m_ROOTOutput->AddObject(hPiastraSIPMCalibrationQ[ii]);
			m_ROOTOutput->AddObject(hPiastraSIPMCalibrationQPHE[ii]);
			m_ROOTOutput->AddObject(hPiastraSIPMCalibrationA[ii]);
			m_ROOTOutput->AddObject(hPiastraSIPMCalibrationAPHE[ii]);
		}
	}

	return NOERROR;
}

//------------------
// fini
//------------------
jerror_t JEventProcessor_PiastraSIPMCalibration::fini(void) {
	// Called before program exit after event processing is finished.
	return NOERROR;
}

