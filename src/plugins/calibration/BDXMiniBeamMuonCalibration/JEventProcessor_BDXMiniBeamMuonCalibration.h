//  $Id$
//
//    File: JEventProcessor_BDXMiniBeamMuonCalibration.h
// Created: Tue Feb 18 16:14:29 CET 2020
// Creator: celentan (on Linux apcx4 3.10.0-957.el7.x86_64 x86_64)
//

#ifndef _JEventProcessor_BDXMiniBeamMuonCalibration_
#define _JEventProcessor_BDXMiniBeamMuonCalibration_

#include <JANA/JEventProcessor.h>

class TH1D;
class TH2D;
class JROOTOutput;

class JEventProcessor_BDXMiniBeamMuonCalibration: public jana::JEventProcessor {
public:
	JEventProcessor_BDXMiniBeamMuonCalibration();
	~JEventProcessor_BDXMiniBeamMuonCalibration();
	const char* className(void) {
		return "JEventProcessor_BDXMiniBeamMuonCalibration";
	}

private:
	jerror_t init(void);						///< Called once at program start.
	jerror_t brun(jana::JEventLoop *eventLoop, int32_t runnumber);	///< Called everytime a new run number is detected.
	jerror_t evnt(jana::JEventLoop *eventLoop, uint64_t eventnumber);	///< Called every event.
	jerror_t erun(void);						///< Called everytime run number changes, provided brun has been called.
	jerror_t fini(void);						///< Called after last event of last event source has been processed.

	int m_isFirstCallToBrun;


	TH1D *hTrj_all_top;
	TH1D *hTrj_all_bot;
	TH2D *hTrj_corr_top;
	TH2D *hTrj_corr_bot;

	TH2D *hTrj_corr_crs_top;
	TH2D *hTrj_corr_crs_bot;

	//Crs Histos:

	//T1
	TH1D *hCrs_m2_p0_T1_top, *hCrs_m2_p1_T1_top;
	TH1D *hCrs_m2_p0_T1_bot, *hCrs_m2_p1_T1_bot;

	//T2
	TH1D *hCrs_m2_m2_T2_top, *hCrs_m1_p0_T2_top, *hCrs_m1_p1_T2_top, *hCrs_m2_p2_T2_top;
	TH1D *hCrs_m2_m2_T2_bot, *hCrs_m1_p0_T2_bot, *hCrs_m1_p1_T2_bot, *hCrs_m2_p2_T2_bot;

	//T3
	TH1D *hCrs_m1_m2_T3_top, *hCrs_m1_p0_T3_top, *hCrs_m1_p1_T3_top, *hCrs_m2_p2_T3_top;
	TH1D *hCrs_m1_m2_T3_bot, *hCrs_m1_p0_T3_bot, *hCrs_m1_p1_T3_bot, *hCrs_m2_p2_T3_bot;

	//T4
	TH1D *hCrs_p0_m2_T4_top, *hCrs_p0_m1_T4_top, *hCrs_p0_p0_T4_top, *hCrs_p0_p1_T4_top, *hCrs_p1_p2_T4_top, *hCrs_p1_p3_T4_top;
	TH1D *hCrs_p0_m2_T4_bot, *hCrs_p0_m1_T4_bot, *hCrs_p0_p0_T4_bot, *hCrs_p0_p1_T4_bot, *hCrs_p1_p2_T4_bot, *hCrs_p1_p3_T4_bot;

	//T5
	TH1D *hCrs_p0_m2_T5_top, *hCrs_p0_m1_T5_top, *hCrs_p0_p0_T5_top, *hCrs_p0_p1_T5_top, *hCrs_m1_p2_T5_top, *hCrs_m1_p3_T5_top;
	TH1D *hCrs_p0_m2_T5_bot, *hCrs_p0_m1_T5_bot, *hCrs_p0_p0_T5_bot, *hCrs_p0_p1_T5_bot, *hCrs_m1_p2_T5_bot, *hCrs_m1_p3_T5_bot;

	//T6
	TH1D *hCrs_p1_m2_T6_top, *hCrs_p1_p0_T6_top, *hCrs_p1_p1_T6_top, *hCrs_p2_p2_T6_top;
	TH1D *hCrs_p1_m2_T6_bot, *hCrs_p1_p0_T6_bot, *hCrs_p1_p1_T6_bot, *hCrs_p2_p2_T6_bot;

	//T7
	TH1D *hCrs_p2_m2_T7_top, *hCrs_p1_p0_T7_top, *hCrs_p1_p1_T7_top, *hCrs_p2_p2_T7_top;
	TH1D *hCrs_p2_m2_T7_bot, *hCrs_p1_p0_T7_bot, *hCrs_p1_p1_T7_bot, *hCrs_p2_p2_T7_bot;

	//T8
	TH1D *hCrs_p2_p0_T8_top, *hCrs_p2_p1_T8_top;
	TH1D *hCrs_p2_p0_T8_bot, *hCrs_p2_p1_T8_bot;

	JROOTOutput *m_ROOTOutput;
	int m_isMC;


	int getCaloIDXFromXY(const int ix, const int iy) {

		if(ix==-2 && iy==-2) return 1;
		if(ix==-1 && iy==-2) return 2;
		if(ix==0 && iy==-2) return 3;
		if(ix==1 && iy==-2) return 4;
		if(ix==2 && iy==-2) return 5;
		if(ix==0 && iy==-1) return 6;

		if(ix==-2 && iy==0) return 7;
		if(ix==-1 && iy==0) return 8;
		if(ix==0 && iy==0) return 9;
		if(ix==1 && iy==0) return 10;
		if(ix==2 && iy==0) return 11;

		if(ix==-2 && iy==1) return 12;
		if(ix==-1 && iy==1) return 13;
		if(ix==0 && iy==1) return 14;
		if(ix==1 && iy==1) return 15;
		if(ix==2 && iy==1) return 16;

		if(ix==-2 && iy==2) return 17;
		if(ix==-1 && iy==2) return 18;
		if(ix==1 && iy==2) return 19;
		if(ix==2 && iy==2) return 20;

		//if(x==1 && y==3) return 21;
		//if(x==2 && y==3) return 22;

		if(ix==-1 && iy==3) return 21;
		if(ix==1 && iy==3) return 22;
		}




};

#endif // _JEventProcessor_BDXMiniBeamMuonCalibration_

