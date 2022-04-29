// $Id$
//
//    File: JEventProcessor_simpleEB.h
// Created: Thu Apr 28 15:13:02 UTC 2022
// Creator: clasrun (on Linux jdaq6 3.10.0-1160.31.1.el7.x86_64 x86_64)
//

#ifndef _JEventProcessor_simpleEB_
#define _JEventProcessor_simpleEB_

#include <JANA/JEventProcessor.h>
class JROOTOutput;
class TTree;
class JEventProcessor_simpleEB: public jana::JEventProcessor {
public:
    JEventProcessor_simpleEB();
    ~JEventProcessor_simpleEB();
    const char* className(void) {
        return "JEventProcessor_simpleEB";
    }

private:
    jerror_t init(void);						///< Called once at program start.
    jerror_t brun(jana::JEventLoop *eventLoop, int32_t runnumber);	///< Called everytime a new run number is detected.
    jerror_t evnt(jana::JEventLoop *eventLoop, uint64_t eventnumber);	///< Called every event.
    jerror_t erun(void);						///< Called everytime run number changes, provided brun has been called.
    jerror_t fini(void);						///< Called after last event of last event source has been processed.

    int m_isFirstCallToBrun;
    JROOTOutput *m_ROOTOutput;
    int m_isMC;

    std::map<int,std::pair<int, int>> geometry;

    TTree *m_tout;

    vector<vector<double>> m_samples;
    double m_A[3][3];
    double m_Q[3][3];
    double m_T[3][3];



    double m_samplingT;
    int m_evnN, m_runN;

};

#endif // _JEventProcessor_simpleEB_

