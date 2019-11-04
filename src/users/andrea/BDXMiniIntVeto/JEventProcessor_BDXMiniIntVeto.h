// $Id$
//
//    File: JEventProcessor_BDXMiniIntVeto.h
// Created: Tue Oct  8 15:16:44 CEST 2019
// Creator: celentan (on Linux apcx4 3.10.0-957.el7.x86_64 x86_64)
//

#ifndef _JEventProcessor_BDXMiniIntVeto_
#define _JEventProcessor_BDXMiniIntVeto_

#include <JANA/JEventProcessor.h>

class TTree;
class TH1D;
class JROOTOutput;


class JEventProcessor_BDXMiniIntVeto:public jana::JEventProcessor{
	public:
		JEventProcessor_BDXMiniIntVeto();
		~JEventProcessor_BDXMiniIntVeto();
		const char* className(void){return "JEventProcessor_BDXMiniIntVeto";}

	private:
		jerror_t init(void);						///< Called once at program start.
		jerror_t brun(jana::JEventLoop *eventLoop, int32_t runnumber);	///< Called everytime a new run number is detected.
		jerror_t evnt(jana::JEventLoop *eventLoop, uint64_t eventnumber);	///< Called every event.
		jerror_t erun(void);						///< Called everytime run number changes, provided brun has been called.
		jerror_t fini(void);						///< Called after last event of last event source has been processed.



		TTree *t;
		int eventNumber;
		int m_isFirstCallToBrun;
		int m_isMC;
		double QL0[10],QL1[10];
		double AL0[10],AL1[10];
		int maxL0,maxL1;
		int topbottom;
		bool crsTOPl[22],crsTOPh[22];
		bool crsBOTl[22],crsBOTh[22];


		JROOTOutput *m_ROOTOutput;

		double m_thrIntVetoCaps,m_thrCrystalsH,m_thrCrystalsL;
		std::map<std::pair<int,int>,int> geometry;


};

#endif // _JEventProcessor_BDXMiniIntVeto_

