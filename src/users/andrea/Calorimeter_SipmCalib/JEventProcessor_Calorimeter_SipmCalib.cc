// $Id$
//
//    File: JEventProcessor_Calorimeter_SipmCalib.cc
// Created: Thu Feb 18 11:55:18 CET 2016
// Creator: celentan (on Linux apcx4 2.6.32-504.30.3.el6.x86_64 x86_64)
//

#include "JEventProcessor_Calorimeter_SipmCalib.h"


#include <system/BDXEventProcessor.h>

#include <DAQ/fa250Mode1Hit.h>
#include <DAQ/fa250Mode1CalibPedSubHit.h>


#include <DAQ/eventData.h>

#include <TT/TranslationTable.h>




#include <Calorimeter/CalorimeterSiPMHit.h>



#include <system/JROOTOutput.h>

#include "TH1D.h"
#include "TH2D.h"
#include "TTree.h"
#include "TF1.h"

// Routine used to create our JEventProcessor
#include <JANA/JApplication.h>
#include <JANA/JFactory.h>
using namespace jana;

extern "C"{
void InitPlugin(JApplication *app){
	jout<<"Before InitJANAPlugin"<<endl;
	InitJANAPlugin(app);
	jout<<"After InitJANAPlugin"<<endl;
	app->AddProcessor(new JEventProcessor_Calorimeter_SipmCalib());
}
} // "C"


//------------------
// JEventProcessor_Calorimeter_SipmCalib (Constructor)
//------------------
JEventProcessor_Calorimeter_SipmCalib::JEventProcessor_Calorimeter_SipmCalib():
		m_ROOTOutput(0)
{
	m_isFirstCallToBrun=1;
}

//------------------
// ~JEventProcessor_Calorimeter_SipmCalib (Destructor)
//------------------
JEventProcessor_Calorimeter_SipmCalib::~JEventProcessor_Calorimeter_SipmCalib()
{

}

//------------------
// init
//------------------
jerror_t JEventProcessor_Calorimeter_SipmCalib::init(void)
{
	// This is called once at program startup. If you are creating
	// and filling historgrams in this plugin, you should lock the
	// ROOT mutex like this:
	//
	// japp->RootWriteLock();
	//  ... fill historgrams or trees ...
	// japp->RootUnLock();
	//
	japp->RootWriteLock();
	h=new TH1D("h","h",500,-0.5,499.5);
	t=new TTree("Calorimeter_SipmCalib","Calorimeter_SipmCalib");

	t->Branch("evnt",&eventNumber);
	t->Branch("sector",&m_sector);
	t->Branch("x",&m_x);
	t->Branch("y",&m_y);
	t->Branch("readout",&m_readout);
	t->Branch("type",&m_type);
	t->Branch("nsingles",&m_singles);
	t->Branch("nsignals",&m_signals);

	t->Branch("Qraw",&Qraw);
	t->Branch("Qphe",&Qphe);
	t->Branch("T",&T);
	t->Branch("A",&A);
	t->Branch("ped",&ped);
	t->Branch("pedSigma",&pedSigma);
	t->Branch("average",&average);
	japp->RootUnLock();

	return NOERROR;
}

//------------------
// brun
//------------------
jerror_t JEventProcessor_Calorimeter_SipmCalib::brun(JEventLoop *eventLoop, int32_t runnumber)
{
	if (m_isFirstCallToBrun){
		string class_name,this_class_name;
		string joutput_name;
		BDXEventProcessor *m_BDXEventProcessor;
		vector<JEventProcessor*> m_processors=app->GetProcessors();
		vector<JEventProcessor*>::iterator m_processors_it;

		m_isFirstCallToBrun=0;
		class_name="BDXEventProcessor";
		joutput_name="JROOTOutput";
		//Now I need to determine which processor is the one holding the output. Discussing with David, he suggested just to check the class name, since
		//a dynamic cast may not work with plugins
		for (m_processors_it=m_processors.begin();m_processors_it!=m_processors.end();m_processors_it++){
			if ((*m_processors_it)!=0){
				this_class_name=string((*m_processors_it)->className());
				if (this_class_name==class_name){
					m_BDXEventProcessor=(BDXEventProcessor*)(*m_processors_it);
					if (m_BDXEventProcessor->getOutput()==0){
						jerr<<"BDXEventProcessor JOutput is null!"<<endl;
						break;
					}
					if (string(m_BDXEventProcessor->getOutput()->className())==joutput_name){
						m_ROOTOutput=(JROOTOutput*)(m_BDXEventProcessor->getOutput());
						jout<<"Got JROOTOutput!"<<endl;
					}
					else{
						jerr<<"BDXEventProcessor JOutput is not null BUT class is: "<<m_BDXEventProcessor->getOutput()->className()<<endl;
					}
				}
			}
		}
		/*For ALL objects you want to add to ROOT file, use the following:*/
		if (m_ROOTOutput){
			m_ROOTOutput->AddObject(h);
			m_ROOTOutput->AddObject(t);
		}
	}
	return NOERROR;
}

//------------------
// evnt
//------------------
jerror_t JEventProcessor_Calorimeter_SipmCalib::evnt(JEventLoop *loop, uint64_t eventnumber)
{
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
	vector<const CalorimeterSiPMHit*> data;
	vector<const CalorimeterSiPMHit*>::const_iterator data_it;
	const fa250Mode1CalibPedSubHit *m_waveform;
	loop->Get(data);

	japp->RootWriteLock();

	eventNumber=eventnumber;

	for (data_it=data.begin();data_it<data.end();data_it++){

		h->Reset();
		h->SetName(Form("h_%i_%i",eventnumber,(*data_it)->m_channel.calorimeter->readout));
		m_waveform=0;
		(*data_it)->GetSingle(m_waveform);

		m_sector=(*data_it)->m_channel.calorimeter->sector;
		m_x=(*data_it)->m_channel.calorimeter->x;
		m_y=(*data_it)->m_channel.calorimeter->y;
		m_readout=(*data_it)->m_channel.calorimeter->readout;
		m_type=(*data_it)->type;
		m_singles=(*data_it)->m_nSingles;
		m_signals=(*data_it)->m_nSignals;
		Qphe=(*data_it)->Qphe;
		Qraw=(*data_it)->Qraw;
		A=(*data_it)->A;
		T=(*data_it)->T;
		average=(*data_it)->average;
		ped=(*data_it)->pedMean;
		pedSigma=(*data_it)->pedRMS;
		//for (int ii=0;ii<m_waveform->samples.size();ii++) h->Fill(ii,m_waveform->samples.at(ii));
	//	h->Write();



		t->Fill();

	}

	japp->RootUnLock();
	return NOERROR;
}

//------------------
// erun
//------------------
jerror_t JEventProcessor_Calorimeter_SipmCalib::erun(void)
{
	// This is called whenever the run number changes, before it is
	// changed to give you a chance to clean up before processing
	// events from the next run number.
	return NOERROR;
}

//------------------
// fini
//------------------
jerror_t JEventProcessor_Calorimeter_SipmCalib::fini(void)
{
	// Called before program exit after event processing is finished.
	return NOERROR;
}

