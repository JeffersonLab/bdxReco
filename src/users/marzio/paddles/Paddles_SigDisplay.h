// $Id$
//
//    File: JEventProcessor_test.h
// Created: Mon Dec 28 15:48:21 CET 2015
// Creator: celentan (on Linux apcx4 2.6.32-504.30.3.el6.x86_64 x86_64)
//

#ifndef _Paddles_SigDisplay_
#define _Paddles_SigDisplay_

#include <JANA/JEventProcessor.h>

class TH1D;
class TH2D;
class TTree;
class JROOTOutput;

class Paddles_SigDisplay:public jana::JEventProcessor{
	public:
		Paddles_SigDisplay();
		~Paddles_SigDisplay();
		const char* className(void){return "Paddles_SigDisplay";}

	private:
		jerror_t init(void);						///< Called once at program start.
		jerror_t brun(jana::JEventLoop *eventLoop, int32_t runnumber);	///< Called everytime a new run number is detected.
		jerror_t evnt(jana::JEventLoop *eventLoop, uint64_t eventnumber);	///< Called every event.
		jerror_t erun(void);						///< Called everytime run number changes, provided brun has been called.
		jerror_t fini(void);						///< Called after last event of last event source has been processed.

		int m_isFirstCallToBrun;
//		TH1D *h;
		TTree *t;
		JROOTOutput *m_ROOTOutput;

		static const int time_window=100;  // Samples
		double Q_first;
		double Q_after;
		double Q_evtbefore;
		double Q;
		double T;

		double Q0;
		double Q1;

		double T0;
		double T1;
		double T0_twc;
		double T1_twc;

		double ped_first;
		double ped_after;

		double amp[time_window];
		int time[time_window];
		int component,eventN,tword;

		double Npe;



};

#endif // _Paddles_SigDisplay_

