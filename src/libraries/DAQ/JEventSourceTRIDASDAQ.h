/*
 * JEventSourceTRIDASDAQ.h
 *
 *  Created on: Jan 24, 2019
 *      Author: celentan
 */

#ifndef SRC_LIBRARIES_DAQ_JEVENTSOURCETRIDASDAQ_H_
#define SRC_LIBRARIES_DAQ_JEVENTSOURCETRIDASDAQ_H_

// JANA headers
#include <JANA/JEventSource.h>
#include <JANA/jerror.h>
using namespace jana;
//TRIDAS headers


#include <TRIDAS/ptfile_reader.hpp>
using namespace tridas::post_trigger;


#define PHYSICS_EVENT_TYPE 1
#define EPICS_EVENT_TYPE 31

// bdx headers

// banks header
#ifdef MC_SUPPORT_ENABLE

#endif

class JEventSourceTRIDASDAQ: public JEventSource {
public:

	enum TRIDASDAQSourceType {
		kNoSource, kFileSource
	};

	JEventSourceTRIDASDAQ(const char* source_name);
	virtual ~JEventSourceTRIDASDAQ();

	virtual const char* className(void) {
		return static_className();
	}
	static const char* static_className(void) {
		return "JEventSourceEvioDAQ";
	}

	jerror_t GetEvent(JEvent &event);
	void FreeEvent(JEvent &event);
	jerror_t GetObjects(JEvent &event, JFactory_base *factory);
	TRIDASDAQSourceType GetDAQSourceType(void) {
		return source_type;
	}



private:

	TRIDASDAQSourceType source_type;
	int curRunNumber, curEventNumber, curEventType;
	int overwriteRunNumber;
	int year;
	int year_ut;
	PtFileReader<sample::uncompressed> *ptReader;

	//ITerator on file, each instance is a timeslice
	PtFileReader<sample::uncompressed>::iterator it_ptReader;
	TimeSlice<sample::uncompressed> *ptTimeSlice;

	//Iterator on timeslice, each instance is an event
	TimeSlice<sample::uncompressed>::iterator it_ptTimeSlice;
	Event<sample::uncompressed> *ptEvent;

	//int currEventTimeSlice;
	int nEventsTimeSlice;

//	struct


};

#endif /* SRC_LIBRARIES_DAQ_JEVENTSOURCETRIDASDAQ_H_ */
