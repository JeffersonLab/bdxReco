// $Id$
//
//    File: epicsData_factory.cc
// Created: Fri Mar  9 11:22:49 EST 2018
// Creator: celentan (on Darwin celentano-macbook 17.4.0 i386)
//

#include <iostream>
#include <iomanip>
using namespace std;

#include "epicsData_factory.h"
#include <DAQ/epicsRawData.h>
#include <DAQ/eventData.h>
#include <system/RunCalibrationHandler.h>
using namespace jana;

/*This is the global memory to keep track of epics data*/
epicsData m_data;

static inline bool is_not_alnum_space(char c) {
	return !(isalpha(c) || isdigit(c) || isspace(c) || (c == ':') || (c == '_') || (c == '.') || (c == '-'));
}

bool string_is_valid(const std::string &str) {

	bool ret = find_if(str.begin(), str.end(), is_not_alnum_space) == str.end();

	return ret;
}

epicsData_factory::epicsData_factory() :
		m_dTs(0) {

	m_deltaTime = 0;
	gPARMS->SetDefaultParameter("EPICS:DELTA_TIME", m_deltaTime, "Time (in s) to add to all EPICS times - can be >0, =0(default), <0");
}
//------------------
// init
//------------------
jerror_t epicsData_factory::init(void) {
	m_dTs = new RunCalibrationHandler("/DAQ/epics_dT");
	this->mapCalibrationHandler(m_dTs);

	return NOERROR;
}

//------------------
// brun
//------------------
jerror_t epicsData_factory::brun(jana::JEventLoop *eventLoop, int32_t runnumber) {
	this->updateCalibrationHandler(m_dTs, eventLoop);
	return NOERROR;
}

//------------------
// evnt
//------------------
jerror_t epicsData_factory::evnt(JEventLoop *loop, uint64_t eventnumber) {

	vector<const epicsRawData*> m_rawdata;
	const eventData *tData;
	loop->Get(m_rawdata);

	try {
		loop->GetSingle(tData);
	} catch (unsigned long e) {
		jout << "epicsData_factory::evnt no eventData bank this event" << endl;
		return OBJECT_NOT_AVAILABLE;
	}

	/*If m_rawdata.size is greater than 0, it means that the event is an epics event,
	 * and that the JEventSourceEVIODAQ got the epics data from EVIO/ET.
	 * By definition, this event is a sequential event, one thread only will process this.
	 * We can save everything to global variables. This is thread-safe, only one thread is doing so
	 */

	/*If no epics raw data was found, then this is a regular event. Copy the epics data from the
	 * last epics event, by reading from the global memory. This is thread-save, we just read from it
	 */

	bool flagOK = true;
	for (int ii = 0; ii < m_rawdata.size(); ii++) {
		if (string_is_valid(m_rawdata[ii]->rawData) == false) {
			flagOK = false;
			break;
		}
	}

	if (m_rawdata.size() > 0 && flagOK) {

		/*A.C. the time correction can be from the m_deltaTime, and/or from the dB, where there is an ADDICTIVE factor
		 * in the form corr = A + eventNumber * B
		 */

		double T0 = m_dTs->getCalib()[0];
		double mT = m_dTs->getCalib()[1];
		double tCorr = m_deltaTime + T0 + m_data.eventNumber * mT;

		m_data.runNumber = tData->runN;
		m_data.time = tData->time + (int) round(tCorr);

		for (int ii = 0; ii < m_rawdata.size(); ii++) {

			m_data.decode(m_rawdata[ii]->rawData, tCorr);

		}

	}

	epicsData *data = new epicsData();
	*data = m_data;

	_data.push_back(data);
	return NOERROR;
}

//------------------
// erun
//------------------
jerror_t epicsData_factory::erun(void) {
	return NOERROR;
}

//------------------
// fini
//------------------
jerror_t epicsData_factory::fini(void) {
	return NOERROR;
}

