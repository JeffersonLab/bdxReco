// $Id$
//
//    File: JEventProcessor_PiastraStability.h
// Created: Fri May 24 19:02:10 CEST 2019
// Creator: celentan (on Linux apcx4 3.10.0-957.el7.x86_64 x86_64)
//

#ifndef _JEventProcessor_PiastraStability_
#define _JEventProcessor_PiastraStability_

#include <JANA/JEventProcessor.h>
class JROOTOutput;




class JEventProcessor_PiastraStability: public jana::JEventProcessor {
public:
	JEventProcessor_PiastraStability();
	~JEventProcessor_PiastraStability();
	const char* className(void) {
		return "JEventProcessor_PiastraStability";
	}

private:
	jerror_t init(void);						///< Called once at program start.
	jerror_t brun(jana::JEventLoop *eventLoop, int32_t runnumber);	///< Called everytime a new run number is detected.
	jerror_t evnt(jana::JEventLoop *eventLoop, uint64_t eventnumber);	///< Called every event.
	jerror_t erun(void);						///< Called everytime run number changes, provided brun has been called.
	jerror_t fini(void);						///< Called after last event of last event source has been processed.

	int m_isMC;
	int m_isFirstCallToBrun;
	JROOTOutput *m_ROOTOutput;

	unsigned long int m_dT, m_dT2;
	unsigned long int    m_T0,m_T;
	int m_isT0Set;
	unsigned long int index, index2;

	int m_nbins;

	int m_nbinsArray[4];


	static const int m_nchargestep = 550;
	static constexpr double m_chargemin = -3;
	static constexpr double m_chargemax = 25;




	/*This algorithm is also supposed to run online.
	 * Hence, we don't know a priori the run length. We use maps to accumulate statistics.
	 */
	map<unsigned long int,unsigned int>::iterator map_it;

	map<unsigned long int,unsigned int[m_nchargestep]>::iterator rate_it[4];

	//Rate
	map<unsigned long int,unsigned int[m_nchargestep]> rate[4];
	map<unsigned long int,unsigned int> allEvents;



};

#endif // _JEventProcessor_PiastraStability_

