// $Id$
//
//    File: JEventProcessor_BDXMiniBeamMuonCalibration.cc
// Created: Tue Feb 18 16:14:29 CET 2020
// Creator: celentan (on Linux apcx4 3.10.0-957.el7.x86_64 x86_64)
//

#include "JEventProcessor_BDXMiniBeamMuonCalibration.h"
using namespace jana;

// Routine used to create our JEventProcessor
#include <JANA/JApplication.h>
#include <JANA/JFactory.h>

#include <system/JROOTOutput.h>
#include <system/BDXEventProcessor.h>

//Add the factories for bdxReco
#include "DAQ/eventData.h"
#include "Calorimeter/CalorimeterHit.h"
#include "IntVeto/IntVetoDigiHit.h"

#include "TH1D.h"
#include "TH2D.h"

extern "C" {
void InitPlugin(JApplication *app) {
	InitJANAPlugin(app);
	app->AddProcessor(new JEventProcessor_BDXMiniBeamMuonCalibration());
}
} // "C"

//------------------
// JEventProcessor_BDXMiniBeamMuonCalibration (Constructor)
//------------------
JEventProcessor_BDXMiniBeamMuonCalibration::JEventProcessor_BDXMiniBeamMuonCalibration() {

}

//------------------
// ~JEventProcessor_BDXMiniBeamMuonCalibration (Destructor)
//------------------
JEventProcessor_BDXMiniBeamMuonCalibration::~JEventProcessor_BDXMiniBeamMuonCalibration() {

}

//------------------
// init
//------------------
jerror_t JEventProcessor_BDXMiniBeamMuonCalibration::init(void) {
	// This is called once at program startup. If you are creating
	// and filling historgrams in this plugin, you should lock the
	// ROOT mutex like this:
	//
	//japp->RootWriteLock();
	//  ... fill historgrams or trees ...
	// japp->RootUnLock();

	m_isFirstCallToBrun = 1;
	m_isMC = 0;
	gPARMS->GetParameter("MC", m_isMC);

//	japp->RootWriteLock();
	hTrj_all_top = new TH1D("hTrj_all_top","hTrj_all_top",8,0.5,8.5);
	hTrj_all_bot = new TH1D("hTrj_all_bot","hTrj_all_bot",8,0.5,8.5);

	hTrj_corr_top = new TH2D("hTrj_corr_top","hTrj_corr_top",8,0.5,8.5,8,0.5,8.5);
	hTrj_corr_bot = new TH2D("hTrj_corr_bot","hTrj_corr_bot",8,0.5,8.5,8,0.5,8.5);

	hTrj_corr_crs_top = new TH2D("hTrj_corr_crs_top","hTrj_corr_crs_top",8,0.5,8.5,22,0.5,22.5);
	hTrj_corr_crs_bot = new TH2D("hTrj_corr_crs_bot","hTrj_corr_crs_bot",8,0.5,8.5,22,0.5,22.5);

	hCrs_m2_p0_T1_top = new TH1D("hCrs_m2_p0_T1_top","hCrs_m2_p0_T1_top",200,0,200);
	hCrs_m2_p1_T1_top = new TH1D("hCrs_m2_p1_T1_top","hCrs_m2_p1_T1_top",200,0,200);

	hCrs_m2_m2_T2_top= new TH1D("hCrs_m2_m2_T2_top","hCrs_m2_m2_T2_top",200,0,200);
	hCrs_m1_p1_T2_top= new TH1D("hCrs_m1_p1_T2_top","hCrs_m1_p1_T2_top",200,0,200);
	hCrs_m1_p0_T2_top= new TH1D("hCrs_m1_p0_T2_top","hCrs_m1_p0_T2_top",200,0,200);
	hCrs_m2_p2_T2_top= new TH1D("hCrs_m2_p2_T2_top","hCrs_m2_p2_T2_top",200,0,200);


	hCrs_m1_m2_T3_top = new TH1D("hCrs_m1_m2_T3_top","hCrs_m1_m2_T3_top",200,0,200);
	hCrs_m1_p0_T3_top = new TH1D("hCrs_m1_p0_T3_top","hCrs_m1_p0_T3_top",200,0,200);;
	hCrs_m1_p1_T3_top= new TH1D("hCrs_m1_p1_T3_top","hCrs_m1_p1_T3_top",200,0,200);;
	hCrs_m2_p2_T3_top= new TH1D("hCrs_m2_p2_T3_top","hCrs_m2_p2_T3_top",200,0,200);

	hCrs_p0_m2_T4_top= new TH1D("hCrs_p0_m2_T4_top","hCrs_p0_m2_T4_top",200,0,200);
	hCrs_p0_m1_T4_top= new TH1D("hCrs_p0_m1_T4_top","hCrs_p0_m1_T4_top",200,0,200);
	hCrs_p0_p0_T4_top= new TH1D("hCrs_p0_p0_T4_top","hCrs_p0_p0_T4_top",200,0,200);
	hCrs_p0_p1_T4_top= new TH1D("hCrs_p0_p1_T4_top","hCrs_p0_p1_T4_top",200,0,200);
	hCrs_p1_p2_T4_top= new TH1D("hCrs_p1_p2_T4_top","hCrs_p1_p2_T4_top",200,0,200);
	hCrs_p1_p3_T4_top= new TH1D("hCrs_p1_p3_T4_top","hCrs_p1_p3_T4_top",200,0,200);

	hCrs_p0_m2_T5_top= new TH1D("hCrs_p0_m2_T5_top","hCrs_p0_m2_T5_top",200,0,200);
	hCrs_p0_m1_T5_top= new TH1D("hCrs_p0_m1_T5_top","hCrs_p0_m1_T5_top",200,0,200);
	hCrs_p0_p0_T5_top= new TH1D("hCrs_p0_p0_T5_top","hCrs_p0_p0_T5_top",200,0,200);
	hCrs_p0_p1_T5_top= new TH1D("hCrs_p0_p1_T5_top","hCrs_p0_p1_T5_top",200,0,200);
	hCrs_m1_p2_T5_top= new TH1D("hCrs_m1_p2_T5_top","hCrs_m1_p2_T5_top",200,0,200);
	hCrs_m1_p3_T5_top= new TH1D("hCrs_m1_p3_T5_top","hCrs_m1_p3_T5_top",200,0,200);

	hCrs_p1_m2_T6_top= new TH1D("hCrs_p1_m2_T6_top","hCrs_p1_m2_T6_top",200,0,200);
	hCrs_p1_p0_T6_top= new TH1D("hCrs_p1_p0_T6_top","hCrs_p1_p0_T6_top",200,0,200);
	hCrs_p1_p1_T6_top= new TH1D("hCrs_p1_p1_T6_top","hCrs_p1_p1_T6_top",200,0,200);
	hCrs_p2_p2_T6_top= new TH1D("hCrs_p2_p2_T6_top","hCrs_p2_p2_T6_top",200,0,200);

	hCrs_p2_m2_T7_top= new TH1D("hCrs_p2_m2_T7_top","hCrs_p2_m2_T7_top",200,0,200);
	hCrs_p1_p0_T7_top= new TH1D("hCrs_p1_p0_T7_top","hCrs_p1_p0_T7_top",200,0,200);
	hCrs_p1_p1_T7_top= new TH1D("hCrs_p1_p1_T7_top","hCrs_p1_p1_T7_top",200,0,200);
	hCrs_p2_p2_T7_top= new TH1D("hCrs_p2_p2_T7_top","hCrs_p2_p2_T7_top",200,0,200);

	hCrs_p2_p0_T8_top= new TH1D("hCrs_p2_p0_T8_top","hCrs_p2_p0_T8_top",200,0,200);
	hCrs_p2_p1_T8_top= new TH1D("hCrs_p2_p1_T8_top","hCrs_p2_p1_T8_top",200,0,200);




	hCrs_m2_p0_T1_bot = new TH1D("hCrs_m2_p0_T1_bot","hCrs_m2_p0_T1_bot",200,0,200);
	hCrs_m2_p1_T1_bot = new TH1D("hCrs_m2_p1_T1_bot","hCrs_m2_p1_T1_bot",200,0,200);

	hCrs_m2_m2_T2_bot= new TH1D("hCrs_m2_m2_T2_bot","hCrs_m2_m2_T2_bot",200,0,200);
	hCrs_m1_p0_T2_bot= new TH1D("hCrs_m1_p0_T2_bot","hCrs_m1_p0_T2_bot",200,0,200);
	hCrs_m1_p1_T2_bot= new TH1D("hCrs_m1_p1_T2_bot","hCrs_m1_p1_T2_bot",200,0,200);
	hCrs_m2_p2_T2_bot= new TH1D("hCrs_m2_p2_T2_bot","hCrs_m2_p2_T2_bot",200,0,200);

	hCrs_m1_m2_T3_bot = new TH1D("hCrs_m1_m2_T3_bot","hCrs_m1_m2_T3_bot",200,0,200);
	hCrs_m1_p0_T3_bot = new TH1D("hCrs_m1_p0_T3_bot","hCrs_m1_p0_T3_bot",200,0,200);
	hCrs_m1_p1_T3_bot= new TH1D("hCrs_m1_p1_T3_bot","hCrs_m1_p1_T3_bot",200,0,200);
	hCrs_m2_p2_T3_bot= new TH1D("hCrs_m2_p2_T3_bot","hCrs_m2_p2_T3_bot",200,0,200);

	hCrs_p0_m2_T4_bot= new TH1D("hCrs_p0_m2_T4_bot","hCrs_p0_m2_T4_bot",200,0,200);
	hCrs_p0_m1_T4_bot= new TH1D("hCrs_p0_m1_T4_bot","hCrs_p0_m1_T4_bot",200,0,200);
	hCrs_p0_p0_T4_bot= new TH1D("hCrs_p0_p0_T4_bot","hCrs_p0_p0_T4_bot",200,0,200);
	hCrs_p0_p1_T4_bot= new TH1D("hCrs_p0_p1_T4_bot","hCrs_p0_p1_T4_bot",200,0,200);
	hCrs_p1_p2_T4_bot= new TH1D("hCrs_p1_p2_T4_bot","hCrs_p1_p2_T4_bot",200,0,200);
	hCrs_p1_p3_T4_bot= new TH1D("hCrs_p1_p3_T4_bot","hCrs_p1_p3_T4_bot",200,0,200);

	hCrs_p0_m2_T5_bot= new TH1D("hCrs_p0_m2_T5_bot","hCrs_p0_m2_T5_bot",200,0,200);
	hCrs_p0_m1_T5_bot= new TH1D("hCrs_p0_m1_T5_bot","hCrs_p0_m1_T5_bot",200,0,200);
	hCrs_p0_p0_T5_bot= new TH1D("hCrs_p0_p0_T5_bot","hCrs_p0_p0_T5_bot",200,0,200);
	hCrs_p0_p1_T5_bot= new TH1D("hCrs_p0_p1_T5_bot","hCrs_p0_p1_T5_bot",200,0,200);
	hCrs_m1_p2_T5_bot= new TH1D("hCrs_m1_p2_T5_bot","hCrs_m1_p2_T5_bot",200,0,200);
	hCrs_m1_p3_T5_bot= new TH1D("hCrs_m1_p3_T5_bot","hCrs_m1_p3_T5_bot",200,0,200);

	hCrs_p1_m2_T6_bot= new TH1D("hCrs_p1_m2_T6_bot","hCrs_p1_m2_T6_bot",200,0,200);
	hCrs_p1_p0_T6_bot= new TH1D("hCrs_p1_p0_T6_bot","hCrs_p1_p0_T6_bot",200,0,200);
	hCrs_p1_p1_T6_bot= new TH1D("hCrs_p1_p1_T6_bot","hCrs_p1_p1_T6_bot",200,0,200);
	hCrs_p2_p2_T6_bot= new TH1D("hCrs_p2_p2_T6_bot","hCrs_p2_p2_T6_bot",200,0,200);

	hCrs_p2_m2_T7_bot= new TH1D("hCrs_p2_m2_T7_bot","hCrs_p2_m2_T7_bot",200,0,200);
	hCrs_p1_p0_T7_bot= new TH1D("hCrs_p1_p0_T7_bot","hCrs_p1_p0_T7_bot",200,0,200);
	hCrs_p1_p1_T7_bot= new TH1D("hCrs_p1_p1_T7_bot","hCrs_p1_p1_T7_bot",200,0,200);
	hCrs_p2_p2_T7_bot= new TH1D("hCrs_p2_p2_T7_bot","hCrs_p2_p2_T7_bot",200,0,200);

	hCrs_p2_p0_T8_bot= new TH1D("hCrs_p2_p0_T8_bot","hCrs_p2_p0_T8_bot",200,0,200);
	hCrs_p2_p1_T8_bot= new TH1D("hCrs_p2_p1_T8_bot","hCrs_p2_p1_T8_bot",200,0,200);


//		japp->RootUnLock();

	return NOERROR;
}

//------------------
// brun
//------------------
jerror_t JEventProcessor_BDXMiniBeamMuonCalibration::brun(JEventLoop *eventLoop, int32_t runnumber) {
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
jerror_t JEventProcessor_BDXMiniBeamMuonCalibration::evnt(JEventLoop *loop, uint64_t eventnumber) {
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

	//japp->RootWriteLock();
	//  ... fill historgrams or trees ...

	const eventData* tData;
	if (!m_isMC) {
		try {
			loop->GetSingle(tData);
		} catch (unsigned long e) {
			jout << "JEventProcessor_BDXMiniCalorimeterEnergyCalibration::evnt no eventData bank this event" << std::endl;
			return OBJECT_NOT_AVAILABLE;
		}
	}



	///.....,

	////  Vetos  ////
	int trig_veto = 1;

	//const IntVetoDigiHit* m_IntVetoDigiHit;
	//vector<const IntVetoDigiHit*> intveto_hits;
	//vector<const IntVetoDigiHit*>::const_iterator intveto_hits_it;






	//// Crystals ////

	int iX, iY, sector;
	double crs[22][2] = {0};
    //Trajectories (0 -> top, 1-> bottom):
	int Tr[8][2] = {0};
    //treshold
	double thr = 5;




	const CalorimeterHit* m_CaloHit;
	vector<const CalorimeterHit*> calo_hits;
	vector<const CalorimeterHit*>::const_iterator calo_hits_it;

	loop->Get(calo_hits);

	for (calo_hits_it = calo_hits.begin(); calo_hits_it != calo_hits.end(); calo_hits_it++) {
			m_CaloHit = *calo_hits_it;
			iX = m_CaloHit->m_channel.x;
			iY = m_CaloHit->m_channel.y;
			sector = m_CaloHit->m_channel.sector;
			//cout << iX << "  " << iY << "  " << sector << "  " << m_CaloHit->E << endl;
			int ID = getCaloIDXFromXY(iX,iY);
			crs[ID-1][sector] = m_CaloHit->E;
	}
	//cout << "Event END" << endl;

//trajectory check:

	for(int i =0; i<2; i++){
		if(crs[6][i]>thr && crs[11][i]>thr) Tr[0][i] = 1;  //Tr1
		if(crs[0][i]>thr && crs[7][i]>thr && crs[12][i]>thr && crs[16][i]>thr) Tr[1][i] = 1;  //Tr2
		if(crs[1][i]>thr && crs[7][i]>thr && crs[12][i]>thr && crs[16][i]>thr) Tr[2][i] = 1;  //Tr3
		if(crs[2][i]>thr && crs[5][i]>thr && crs[8][i]>thr && crs[13][i]>thr && crs[18][i]>thr && crs[21][i]>thr) Tr[3][i]=1; //Tr4
		if(crs[2][i]>thr && crs[5][i]>thr && crs[8][i]>thr && crs[13][i]>thr && crs[17][i]>thr && crs[20][i]>thr) Tr[4][i]=1; //Tr5
		if(crs[3][i]>thr && crs[9][i]>thr && crs[14][i]>thr && crs[19][i]>thr) Tr[5][i] = 1;  //Tr6
		if(crs[4][i]>thr && crs[9][i]>thr && crs[14][i]>thr && crs[19][i]>thr) Tr[6][i] = 1;  //Tr7
		if(crs[10][i]>thr && crs[15][i]>thr) Tr[7][i] = 1;  //Tr8
	}

	//fill Histograms
	double Tr_test =0;

	for(int i =0; i<8; i++){

		for(int j=0; j< 2; j++){
			Tr_test += Tr[i][j];
		//	cout << Tr[i][j] << " ";

		}
	}


	for(int i =0; i<8; i++){

			for(int j=0; j< 2; j++){
				if(Tr[i][j]==1 && j==0){
					hTrj_corr_top->Fill(Tr_test,i+1);
					hTrj_all_top->Fill(i+1);
				}
				if(Tr[i][j]==1 && j==1) {
					hTrj_all_bot->Fill(i+1);
					hTrj_corr_bot->Fill(Tr_test,i+1);
			}
		}
	}


	for(int i =0; i<8; i++){
		for(int j=0; j< 2; j++){
			for(int k=0; k< 22; k++){
				if(Tr[i][j]==1 && j==0 && crs[k][j]>thr){
					hTrj_corr_crs_top->Fill(i+1,k+1);
				}
				if(Tr[i][j]==1 && j==1 && crs[k][j]>thr){
					hTrj_corr_crs_bot->Fill(i+1,k+1);
				}
			}
		}
	}


	//cout<< endl;

	if(Tr_test >= 1){
		if(trig_veto == 1){

			//TOP
			if(Tr[0][0] == 1){
				//cout << "tr1" << endl;
				hCrs_m2_p0_T1_top->Fill(crs[6][0]);
				hCrs_m2_p1_T1_top->Fill(crs[11][0]);
			}
			if(Tr[1][0] == 1){
				//cout << "tr2" << endl;
				hCrs_m2_m2_T2_top->Fill(crs[0][0]);
				hCrs_m1_p0_T2_top->Fill(crs[7][0]);
				hCrs_m1_p1_T2_top->Fill(crs[12][0]);
				hCrs_m2_p2_T2_top->Fill(crs[16][0]);
			}
			if(Tr[2][0] == 1){
				//cout << "tr3" << endl;
				hCrs_m1_m2_T3_top->Fill(crs[1][0]);
				hCrs_m1_p0_T3_top->Fill(crs[7][0]);
				hCrs_m1_p1_T3_top->Fill(crs[12][0]);
				hCrs_m2_p2_T3_top->Fill(crs[16][0]);
			}
			if(Tr[3][0] == 1){
				//cout << "tr4" << endl;
				hCrs_p0_m2_T4_top->Fill(crs[2][0]);
				hCrs_p0_m1_T4_top->Fill(crs[5][0]);
				hCrs_p0_p0_T4_top->Fill(crs[8][0]);
				hCrs_p0_p1_T4_top->Fill(crs[13][0]);
				hCrs_p1_p2_T4_top->Fill(crs[18][0]);
				hCrs_p1_p3_T4_top->Fill(crs[21][0]);
			}
			if(Tr[4][0] == 1){
				//cout << "tr5" << endl;
				hCrs_p0_m2_T5_top->Fill(crs[2][0]);
				hCrs_p0_m1_T5_top->Fill(crs[5][0]);
				hCrs_p0_p0_T5_top->Fill(crs[8][0]);
				hCrs_p0_p1_T5_top->Fill(crs[13][0]);
				hCrs_m1_p2_T5_top->Fill(crs[17][0]);
				hCrs_m1_p3_T5_top->Fill(crs[20][0]);
			}
			if(Tr[5][0] == 1){
				//cout << "tr6" << endl;
				hCrs_p1_m2_T6_top->Fill(crs[3][0]);
				hCrs_p1_p0_T6_top->Fill(crs[9][0]);
				hCrs_p1_p1_T6_top->Fill(crs[14][0]);
				hCrs_p2_p2_T6_top->Fill(crs[19][0]);
			}
			if(Tr[6][0] == 1){
				//cout << "tr7" << endl;
				hCrs_p2_m2_T7_top->Fill(crs[4][0]);
				hCrs_p1_p0_T7_top->Fill(crs[9][0]);
				hCrs_p1_p1_T7_top->Fill(crs[14][0]);
				hCrs_p2_p2_T7_top->Fill(crs[19][0]);
			}
			if(Tr[7][0] == 1){
				//cout << "tr8" << endl;
				hCrs_p2_p0_T8_top->Fill(crs[10][0]);
				hCrs_p2_p1_T8_top->Fill(crs[15][0]);
			}


			//BOTTOM
			if(Tr[0][1] == 1){
				hCrs_m2_p0_T1_bot->Fill(crs[6][1]);
				hCrs_m2_p1_T1_bot->Fill(crs[11][1]);
			}
			if(Tr[1][1] == 1){
				hCrs_m2_m2_T2_bot->Fill(crs[0][1]);
				hCrs_m1_p0_T2_bot->Fill(crs[7][1]);
				hCrs_m1_p1_T2_bot->Fill(crs[12][1]);
				hCrs_m2_p2_T2_bot->Fill(crs[16][1]);
			}
			if(Tr[2][1] == 1){
				hCrs_m1_m2_T3_bot->Fill(crs[1][1]);
				hCrs_m1_p0_T3_bot->Fill(crs[7][1]);
				hCrs_m1_p1_T3_bot->Fill(crs[12][1]);
				hCrs_m2_p2_T3_bot->Fill(crs[16][1]);
			}
			if(Tr[3][1] == 1){
				hCrs_p0_m2_T4_bot->Fill(crs[2][1]);
				hCrs_p0_m1_T4_bot->Fill(crs[5][1]);
				hCrs_p0_p0_T4_bot->Fill(crs[8][1]);
				hCrs_p0_p1_T4_bot->Fill(crs[13][1]);
				hCrs_p1_p2_T4_bot->Fill(crs[18][1]);
				hCrs_p1_p3_T4_bot->Fill(crs[21][1]);
			}
			if(Tr[4][1 == 1]){
				hCrs_p0_m2_T5_bot->Fill(crs[2][1]);
				hCrs_p0_m1_T5_bot->Fill(crs[5][1]);
				hCrs_p0_p0_T5_bot->Fill(crs[8][1]);
				hCrs_p0_p1_T5_bot->Fill(crs[13][1]);
				hCrs_m1_p2_T5_bot->Fill(crs[17][1]);
				hCrs_m1_p3_T5_bot->Fill(crs[20][1]);
			}
			if(Tr[5][1] == 1){
				hCrs_p1_m2_T6_bot->Fill(crs[3][1]);
				hCrs_p1_p0_T6_bot->Fill(crs[9][1]);
				hCrs_p1_p1_T6_bot->Fill(crs[14][1]);
				hCrs_p2_p2_T6_bot->Fill(crs[19][1]);
			}
			if(Tr[6][1] == 1){
				hCrs_p2_m2_T7_bot->Fill(crs[4][1]);
				hCrs_p1_p0_T7_bot->Fill(crs[9][1]);
				hCrs_p1_p1_T7_bot->Fill(crs[14][1]);
				hCrs_p2_p2_T7_bot->Fill(crs[19][1]);
			}
			if(Tr[7][1] == 1){
				hCrs_p2_p0_T8_bot->Fill(crs[10][1]);
				hCrs_p2_p1_T8_bot->Fill(crs[15][1]);
			}

		}
	}
//	japp->RootUnLock();
	return NOERROR;
}

//------------------
// erun
//------------------
jerror_t JEventProcessor_BDXMiniBeamMuonCalibration::erun(void) {
	// This is called whenever the run number changes, before it is
	// changed to give you a chance to clean up before processing
	// events from the next run number.


	//TOP histos

	m_ROOTOutput->AddObject(hTrj_all_top);
	m_ROOTOutput->AddObject(hTrj_all_bot);

	m_ROOTOutput->AddObject(hTrj_corr_top);
	m_ROOTOutput->AddObject(hTrj_corr_bot);

	m_ROOTOutput->AddObject(hTrj_corr_crs_top);
	m_ROOTOutput->AddObject(hTrj_corr_crs_bot);

	m_ROOTOutput->AddObject(hCrs_m2_p0_T1_top);
	m_ROOTOutput->AddObject(hCrs_m2_p1_T1_top);

	m_ROOTOutput->AddObject(hCrs_m2_m2_T2_top);
	m_ROOTOutput->AddObject(hCrs_m1_p0_T2_top);
	m_ROOTOutput->AddObject(hCrs_m1_p1_T2_top);
	m_ROOTOutput->AddObject(hCrs_m2_p2_T2_top);

	m_ROOTOutput->AddObject(hCrs_m1_m2_T3_top);
	m_ROOTOutput->AddObject(hCrs_m1_p0_T3_top);
	m_ROOTOutput->AddObject(hCrs_m1_p1_T3_top);
	m_ROOTOutput->AddObject(hCrs_m2_p2_T3_top);

	m_ROOTOutput->AddObject(hCrs_p0_m2_T4_top);
	m_ROOTOutput->AddObject(hCrs_p0_m1_T4_top);
	m_ROOTOutput->AddObject(hCrs_p0_p0_T4_top);
	m_ROOTOutput->AddObject(hCrs_p0_p1_T4_top);
	m_ROOTOutput->AddObject(hCrs_p1_p2_T4_top);
	m_ROOTOutput->AddObject(hCrs_p1_p3_T4_top);

	m_ROOTOutput->AddObject(hCrs_p0_m2_T5_top);
	m_ROOTOutput->AddObject(hCrs_p0_m1_T5_top);
	m_ROOTOutput->AddObject(hCrs_p0_p0_T5_top);
	m_ROOTOutput->AddObject(hCrs_p0_p1_T5_top);
	m_ROOTOutput->AddObject(hCrs_m1_p2_T5_top);
	m_ROOTOutput->AddObject(hCrs_m1_p3_T5_top);

	m_ROOTOutput->AddObject(hCrs_p1_m2_T6_top);
	m_ROOTOutput->AddObject(hCrs_p1_p0_T6_top);
	m_ROOTOutput->AddObject(hCrs_p1_p1_T6_top);
	m_ROOTOutput->AddObject(hCrs_p2_p2_T6_top);

	m_ROOTOutput->AddObject(hCrs_p2_m2_T7_top);
	m_ROOTOutput->AddObject(hCrs_p1_p0_T7_top);
	m_ROOTOutput->AddObject(hCrs_p1_p1_T7_top);
	m_ROOTOutput->AddObject(hCrs_p2_p2_T7_top);

	m_ROOTOutput->AddObject(hCrs_p2_p0_T8_top);
	m_ROOTOutput->AddObject(hCrs_p2_p1_T8_top);



	//BOTTOM HISTOS
	m_ROOTOutput->AddObject(hCrs_m2_p0_T1_bot);
	m_ROOTOutput->AddObject(hCrs_m2_p1_T1_bot);

	m_ROOTOutput->AddObject(hCrs_m2_m2_T2_bot);
	m_ROOTOutput->AddObject(hCrs_m1_p0_T2_bot);
	m_ROOTOutput->AddObject(hCrs_m1_p1_T2_bot);
	m_ROOTOutput->AddObject(hCrs_m2_p2_T2_bot);

	m_ROOTOutput->AddObject(hCrs_m1_m2_T3_bot);
	m_ROOTOutput->AddObject(hCrs_m1_p0_T3_bot);
	m_ROOTOutput->AddObject(hCrs_m1_p1_T3_bot);
	m_ROOTOutput->AddObject(hCrs_m2_p2_T3_bot);

	m_ROOTOutput->AddObject(hCrs_p0_m2_T4_bot);
	m_ROOTOutput->AddObject(hCrs_p0_m1_T4_bot);
	m_ROOTOutput->AddObject(hCrs_p0_p0_T4_bot);
	m_ROOTOutput->AddObject(hCrs_p0_p1_T4_bot);
	m_ROOTOutput->AddObject(hCrs_p1_p2_T4_bot);
	m_ROOTOutput->AddObject(hCrs_p1_p3_T4_bot);

	m_ROOTOutput->AddObject(hCrs_p0_m2_T5_bot);
	m_ROOTOutput->AddObject(hCrs_p0_m1_T5_bot);
	m_ROOTOutput->AddObject(hCrs_p0_p0_T5_bot);
	m_ROOTOutput->AddObject(hCrs_p0_p1_T5_bot);
	m_ROOTOutput->AddObject(hCrs_m1_p2_T5_bot);
	m_ROOTOutput->AddObject(hCrs_m1_p3_T5_bot);

	m_ROOTOutput->AddObject(hCrs_p1_m2_T6_bot);
	m_ROOTOutput->AddObject(hCrs_p1_p0_T6_bot);
	m_ROOTOutput->AddObject(hCrs_p1_p1_T6_bot);
	m_ROOTOutput->AddObject(hCrs_p2_p2_T6_bot);

	m_ROOTOutput->AddObject(hCrs_p2_m2_T7_bot);
	m_ROOTOutput->AddObject(hCrs_p1_p0_T7_bot);
	m_ROOTOutput->AddObject(hCrs_p1_p1_T7_bot);
	m_ROOTOutput->AddObject(hCrs_p2_p2_T7_bot);

	m_ROOTOutput->AddObject(hCrs_p2_p0_T8_bot);
	m_ROOTOutput->AddObject(hCrs_p2_p1_T8_bot);













	return NOERROR;
}

//------------------
// fini
//------------------
jerror_t JEventProcessor_BDXMiniBeamMuonCalibration::fini(void) {
	// Called before program exit after event processing is finished.
	return NOERROR;
}

