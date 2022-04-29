// $Id$
//
//    File: JEventProcessor_WBpeds.cc
// Created: Wed Apr 27 16:03:14 UTC 2022
// Creator: clasrun (on Linux jdaq6 3.10.0-1160.31.1.el7.x86_64 x86_64)
//

#include "JEventProcessor_WBpeds.h"
#include <system/BDXEventProcessor.h>
#include <system/JROOTOutput.h>
#include <DAQ/fa250WaveboardV1Hit.h>


#include "TROOT.h"
#include "TH1D.h"

using namespace jana;

//create here the histogram pointers
static const int nTOT = 12;

//Amplitude
static TH1D *hWBPedMean[nTOT] = { 0 };
static TH1D *hWBPedRMS[nTOT] = { 0 };

// Routine used to create our JEventProcessor
#include <JANA/JApplication.h>
extern "C" {
void InitPlugin(JApplication *app) {
	InitJANAPlugin(app);
	app->AddProcessor(new JEventProcessor_WBpeds());
}
} // "C"

//------------------
// JEventProcessor_WBpeds (Constructor)
//------------------
JEventProcessor_WBpeds::JEventProcessor_WBpeds() {

}

//------------------
// ~JEventProcessor_WBpeds (Destructor)
//------------------
JEventProcessor_WBpeds::~JEventProcessor_WBpeds() {

}

//------------------
// init
//------------------
jerror_t JEventProcessor_WBpeds::init(void) {
	// This is called once at program startup
	m_isFirstCallToBrun = 1;
	m_isMC = 0;
	gPARMS->GetParameter("MC", m_isMC);

	m_nSamples = 0;
	gPARMS->GetParameter("WBpeds:nSamples", m_nSamples);

	japp->RootWriteLock();

	if (hWBPedMean[0] != NULL) {
		japp->RootUnLock();
		return NOERROR;
	}

	gROOT->cd();
	TDirectory *main = gDirectory;
	gDirectory->mkdir("WBpeds")->cd();

	double pedMeanMin, pedMeanMax;
	double pedRMSMin, pedRMSMax;
	int pedMeanN, pedRMSN;

	gPARMS->GetParameter("WBpeds:pedMeanMin", pedMeanMin);
	gPARMS->GetParameter("WBpeds:pedMeanMax", pedMeanMax);
	gPARMS->GetParameter("WBpeds:pedMeanN", pedMeanN);

	gPARMS->GetParameter("WBpeds:pedRMSMin", pedRMSMin);
	gPARMS->GetParameter("WBpeds:pedRMSMax", pedRMSMax);
	gPARMS->GetParameter("WBpeds:pedRMSN", pedRMSN);

	for (int ii = 0; ii < nTOT; ii++) {

		hWBPedMean[ii] = new TH1D(Form("hWBPedMean_ch%i", ii),
				Form("hWBPedMean_ch%i", ii), pedMeanN, pedMeanMin, pedMeanMax);
		hWBPedMean[ii]->GetXaxis()->SetTitle("ped mean");

		hWBPedRMS[ii] = new TH1D(Form("hWBPedRMS_ch%i", ii),
				Form("hWBPedRMS_ch%i", ii), pedRMSN, pedRMSMin, pedRMSMax);
		hWBPedRMS[ii]->GetXaxis()->SetTitle("ped rms");

	}

	// back to main dir
	main->cd();
	japp->RootUnLock();

	return NOERROR;
}

//------------------
// brun
//------------------
jerror_t JEventProcessor_WBpeds::brun(JEventLoop *eventLoop,
		int32_t runnumber) {
	jout << "JEventProcessor_WBHits::brun--> " << m_isFirstCallToBrun << endl;

	if (m_isFirstCallToBrun) {

		jout << "JEventProcessor_WBHits::brun searching m_RootOutput" << endl;
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
jerror_t JEventProcessor_WBpeds::evnt(JEventLoop *loop, uint64_t eventnumber) {
	// This is called for every event. Use of common resources like writing
	// to a file or filling a histogram should be mutex protected. Using
	// loop->Get(...) to get reconstructed objects (and thereby activating the
	// reconstruction algorithm) should be done outside of any mutex lock
	// since multiple threads may call this method at the same time.

	vector<const fa250WaveboardV1Hit*> hits;
	loop->Get(hits);

	double pedMean[nTOT] = { 0 };
	double pedRMS[nTOT] = { 0 };

	for (int ii = 0; ii < hits.size(); ii++) {
		fa250WaveboardV1Hit hit = *(hits[ii]);
		//get the info about the channel
		int ch = hit.m_channel.channel;

		for (int jj = 0; jj < hit.samples.size(); jj++) {
			if (jj < m_nSamples) {
				pedMean[ch] += hit.samples[jj];
				pedRMS[ch] += (hit.samples[jj]) * (hit.samples[jj]);
			} else {
				break;
			}
		}
		pedMean[ch] = pedMean[ch] / m_nSamples;
		pedRMS[ch] = pedRMS[ch] / m_nSamples;
		pedRMS[ch] = sqrt(pedRMS[ch] - pedMean[ch] * pedMean[ch]);

	}

	japp->RootWriteLock();
	for (int ii = 0; ii < nTOT; ii++) {
		if (pedRMS[ii] > 0) {
			hWBPedMean[ii]->Fill(pedMean[ii]);
			hWBPedRMS[ii]->Fill(pedRMS[ii]);
		}
	}
	japp->RootUnLock();

	return NOERROR;
}

//------------------
// erun
//------------------
jerror_t JEventProcessor_WBpeds::erun(void) {
	// This is called whenever the run number changes, before it is
	// changed to give you a chance to clean up before processing
	// events from the next run number.

	if (m_ROOTOutput != 0) {
		jout << "JEventProcessor_BDXMiniEBHits adding histos to m_ROOTOutput"
				<< endl;
		for (int ii = 0; ii < nTOT; ii++) {
			m_ROOTOutput->AddObject(hWBPedMean[ii]);
		}
		for (int ii = 0; ii < nTOT; ii++) {
			m_ROOTOutput->AddObject(hWBPedRMS[ii]);
		}

	}

	return NOERROR;
}

//------------------
// fini
//------------------
jerror_t JEventProcessor_WBpeds::fini(void) {
	// Called before program exit after event processing is finished.
	return NOERROR;
}

