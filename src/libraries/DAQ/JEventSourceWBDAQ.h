#ifndef _JEVENT_SOURCEWBDAQ_H_
#define _JEVENT_SOURCEWBDAQ_H_

// #include <vector>
// #include <string>
// #include <fstream>
// using namespace std;

// JANA headers
#include <JANA/JEventSource.h>
#include <JANA/jerror.h>

using namespace jana;

#define PHYSICS_EVENT_TYPE 1
#define EPICS_EVENT_TYPE 31

// bdx headers



// banks header
#ifdef MC_SUPPORT_ENABLE
#include "banks.h"
#endif

//ET
#ifdef ET_SUPPORT_ENABLE
#include "et.h"
#endif

class JEventSourceWBDAQ: public JEventSource {
public:

	enum DAQSourceType {
		kNoSource, kFileSource, kETSource
	};

	JEventSourceWBDAQ(const char* source_name);
	virtual ~JEventSourceWBDAQ();

	virtual const char* className(void) {
		return static_className();
	}
	static const char* static_className(void) {
		return "JEventSourceWBDAQ";
	}

	jerror_t GetEvent(JEvent &event);
	void FreeEvent(JEvent &event);
	jerror_t GetObjects(JEvent &event, JFactory_base *factory);
	DAQSourceType GetDAQSourceType(void) {
		return source_type;
	}


private:

	std::ifstream inputFile;


	int epics_mother_tag; 	//tag of the bank containg data with tag vrm vme
	int child_mode1_tag;
	int child_trigger_tag;
	int child_epics_tag;
	int eventHeader_tag;
	int prestart_tag;
	int end_tag;
	int curRunNumber, curEventNumber,curEventType;
	int overwriteRunNumber;

	int eventTypeDAQ;
	int eventTypeEPICS;


	int m_VERBOSE;
	float TIMEOUT;

	uint32_t BUFFER_SIZE;

	DAQSourceType source_type;

	//ET stuff
	void ConnectToET(const char* source_name);
	bool et_connected;
	int ET_STATION_NEVENTS;
	bool ET_STATION_CREATE_BLOCKING;
	bool quit_on_next_ET_timeout;

#ifdef ET_SUPPORT_ENABLE
	et_sys_id sys_id;
	et_att_id att_id;
	et_stat_id sta_id;
#endif

};

#endif
