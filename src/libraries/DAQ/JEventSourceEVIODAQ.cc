// JANA headers
#include <JANA/JEvent.h>
#include <JANA/JParameterManager.h>
#include <JANA/JApplication.h>
// bdx headers
#include <system/JEventSourceEVIOGenerator.h>
#include "JEventSourceEVIODAQ.h"

#include "CLHEP/Units/PhysicalConstants.h"
using namespace CLHEP;

// C++ headers
#include <iostream>
#include <time.h>
#include <unistd.h>
using namespace std;

//DAQ
#include <DAQ/fa250Mode1Hit.h>
#include <DAQ/fa250Mode7Hit.h>
#include <DAQ/eventData.h>
#include <DAQ/epicsRawData.h>

// Constructor
JEventSourceEvioDAQ::JEventSourceEvioDAQ(const char* source_name) :
		JEventSource(source_name), chan(0), EDT(0), vme_mother_tag(0), child_mode1_tag(0), child_mode7_tag(0), eventHeader_tag(0), curRunNumber(0), curEventNumber(0) {

	jout << "JEventSourceEvioDAQ creator: " << this << endl;

	/*Here follows some hard-coded definitions, all of them have a corresponding SetDefaultParameter call,
	 * to be used to change this value via configuration file
	 */
	vme_mother_tag = 0x50;	//80
	epics_mother_tag = 0x81; //129
	child_mode1_tag = 0xe101;
	child_mode7_tag = 0xe102;
	child_trigger_tag = 0xe118;
	eventHeader_tag = 0xe10F;      //57615 - HEAD bank
	eventHeader_CODA_tag = 0xc000;
	child_epics_tag = 0xe114;
	prestart_tag = 0x11;  //decimal 17
	end_tag = 0x14; //decimal 20
	overwriteRunNumber = -1;

	eventTypeDAQ = 129;
	eventTypeEPICS = 31;
	curEventType = 0;

	BUFFER_SIZE = 3000000;
	buff = 0; /*Used just for ET*/
	source_type = kNoSource;

	ET_STATION_NEVENTS = 100;
	ET_STATION_CREATE_BLOCKING = false;
	TIMEOUT = 2;
	quit_on_next_ET_timeout = false;
	et_connected = false;

	gPARMS->SetDefaultParameter("DAQ:VME_MOTHER_TAG", vme_mother_tag);
	gPARMS->SetDefaultParameter("DAQ:EPICS_MOTHER_TAG", epics_mother_tag);
	gPARMS->SetDefaultParameter("DAQ:CHILD_MODE1_TAG", child_mode1_tag);
	gPARMS->SetDefaultParameter("DAQ:CHILD_MODE7_TAG", child_mode7_tag);
	gPARMS->SetDefaultParameter("DAQ:CHILD_TRIGGER_TAG", child_trigger_tag);
	gPARMS->SetDefaultParameter("DAQ:CHILD_EPICS_TAG", child_epics_tag);
	gPARMS->SetDefaultParameter("DAQ:EVENTHEADER_TAG", eventHeader_tag);
	gPARMS->SetDefaultParameter("DAQ:PRESTART_TAG", prestart_tag);

	gPARMS->SetDefaultParameter("DAQ:RUN_NUMBER", overwriteRunNumber);

	gPARMS->SetDefaultParameter("DAQ:BUFFER_SIZE", BUFFER_SIZE, "Size in bytes to allocate for holding a single EVIO event.");

	gPARMS->SetDefaultParameter("ET:ET_STATION_NEVENTS", ET_STATION_NEVENTS, "Number of events to use if we have to create the ET station. Ignored if station already exists.");
	gPARMS->SetDefaultParameter("ET:ET_STATION_CREATE_BLOCKING", ET_STATION_CREATE_BLOCKING, "Set this to create station in blocking mode (default is to create it in non-blocking mode). Ignored if station already exists.");
	gPARMS->SetDefaultParameter("ET:TIMEOUT", TIMEOUT, "Set the timeout in seconds for each attempt at reading from ET system (repeated attempts will still be made indefinitely until program quits or the quit_on_et_timeout flag is set.");

	gPARMS->SetDefaultParameter("DAQ:EVENT_TYPE_DAQ", eventTypeDAQ);
	gPARMS->SetDefaultParameter("DAQ:EVENT_TYPE_EPICS", eventTypeEPICS);

	gPARMS->SetDefaultParameter("DAQ:VERBOSE", m_VERBOSE, "Set verbosity level for processing and debugging statements while parsing. 0=no debugging messages. 10=all messages");

	// open EVIO file - buffer is hardcoded at 3M... that right?
	jout << " Opening input source: " << source_name << endl;

	try {
		chan = new evioFileChannel(source_name, "r", BUFFER_SIZE);
		chan->open();
		source_type = kFileSource;
	} catch (evioException e) { /*Check if we can open ET-system*/
#ifdef  ET_SUPPORT_ENABLE
		if (this->source_name.substr(0, 3) == "ET:") {
			jout << "Attempting to open \"" << this->source_name << "\" as ET (network) source..." << endl;
			ConnectToET(source_name);
			source_type = kETSource;
		}
		if (!et_connected) throw JException("Failed to open ET system: " + this->source_name);
#else
		// No ET and the file didn't work so re-throw the exception
		if (this->source_name.substr(0, 3) == "ET:") {
			cerr << endl;
			cerr << "=== ERROR: ET source specified and this was compiled without    ===" << endl;
			cerr << "===        ET support. You need recompile with ET support, i.e. ===" << endl;
			cerr << "===        scons ET=1                                           ===" << endl;
			throw JException("Failed to open ET system - no ET support enabled: " + this->source_name);
		} else {
			jerr << " Here" << endl;
			jerr << e.toString() << endl;
			throw e;
		}
#endif
		chan = 0;
	}
}

// Destructor
JEventSourceEvioDAQ::~JEventSourceEvioDAQ() {
	cout << " Closing input file " << source_name << "." << endl;
	if (chan) {
		chan->close();
		delete chan;
	}

#ifdef HAVE_ET
	if(et_connected) {
		if(m_VERBOSE>0) evioout << "Closing ET connection \"" << this->source_name << "\"" <<endl;
		et_close(sys_id);
		et_connected = false;
	}
#endif
}

// GetEvent
jerror_t JEventSourceEvioDAQ::GetEvent(JEvent &event) {

	event.SetRef(NULL);

	if (this->source_type == kFileSource) {

		if (chan->read()) {
			EDT = new evioDOMTree(chan);
			event.SetRef(EDT);
			event.SetJEventSource(this);

			evio::evioDOMNodeListP fullList = EDT->getNodeList();
			evio::evioDOMNodeList::const_iterator iter;

			/*Check if this is an EPICS event, if so call the  SetSequential() method on it*/
			/*To do so, read the first tag in the nodeList, that is the one defining the event.*/
			iter = fullList->begin();
			curEventType = (*iter)->tag;
			if (curEventType == eventTypeEPICS) {
				event.SetSequential();
			}

			for (iter = fullList->begin(); iter != fullList->end(); iter++) {

				if (((*iter)->tag == prestart_tag) && (overwriteRunNumber == -1)) {
					const evio::evioCompositeDOMLeafNode *leaf = static_cast<const evio::evioCompositeDOMLeafNode*>(*iter);
					vector<uint32_t> *pData = const_cast<vector<uint32_t> *>(&(leaf->data));
					curRunNumber = (*pData)[1];
				}
				if ((*iter)->tag == eventHeader_tag) {
					const evio::evioCompositeDOMLeafNode *leaf = static_cast<const evio::evioCompositeDOMLeafNode*>(*iter);
					vector<uint32_t> *pData = const_cast<vector<uint32_t> *>(&(leaf->data));
					event.SetEventNumber((*pData)[2]);
					curRunNumber = (*pData)[1];
					curEventType = (*pData)[4];
				}
			}

			if (overwriteRunNumber != -1) {
				event.SetRunNumber(overwriteRunNumber);
			} else {
				event.SetRunNumber(curRunNumber);
			}
			return NOERROR;
		} else {
			jout << "Source done" << endl;
			chan->close();
			if (chan) {
				delete chan;
				chan = 0;
			}
			return NO_MORE_EVENTS_IN_SOURCE;
		}
	} else if (this->source_type == kETSource) {
#ifdef ET_SUPPORT_ENABLE
		if (buff != 0) {
			delete buff;
		}
		if (m_VERBOSE > 5) jout << "before new: " << buff << " I am: " << this << endl;
		buff = new uint32_t[BUFFER_SIZE];
		if (m_VERBOSE > 5) jout << "after new: " << buff << " I am: " << this << endl;
		// Loop until we get an event or are told to stop
		struct timespec timeout;
		timeout.tv_sec = (unsigned int) floor(TIMEOUT);// set ET timeout
		timeout.tv_nsec = (unsigned int) floor(1.0E9 * (TIMEOUT - (float) timeout.tv_sec));
		et_event *pe = NULL;
		while (!japp->GetQuittingStatus()) {

			int err = et_event_get(sys_id, att_id, &pe, ET_TIMED, &timeout);

			if (err == ET_OK && pe != NULL) break; // got an event. break out of while loop

			if (err == ET_OK && pe == NULL) {
				jout << "  !!! ET returned no error, but event pointer is NULL!!!" << endl;
				return NO_MORE_EVENTS_IN_SOURCE;
			}

			if (err == ET_ERROR_TIMEOUT) {
				if (quit_on_next_ET_timeout) return NO_MORE_EVENTS_IN_SOURCE;
			} else if (err != ET_OK) {
				jout << " Error reading from ET. This probably means the ET" << endl;
				jout << "system has gone away (possibly due to run ending or" << endl;
				jout << "DAQ crashing). At any rate, we are quitting now as this" << endl;
				jout << "error is currently unrecoverable." << endl;
				return NO_MORE_EVENTS_IN_SOURCE;
			}

			usleep(10);
		}
		if (japp->GetQuittingStatus() && pe == NULL) return NO_MORE_EVENTS_IN_SOURCE;
		// Get pointer to event buffer in the ET-owned memory
		uint32_t *et_buff = NULL;
		et_event_getdata(pe, (void**) &et_buff);
		if (et_buff == NULL) {
			jerr << " Got event from ET, but pointer to data is NULL!" << endl;
			return NO_MORE_EVENTS_IN_SOURCE;
		}
		// Check byte order of event by looking at magic #
		bool swap_needed = false;
		uint32_t magic = et_buff[7];
		if (m_VERBOSE > 5) jout << "Magic word is: " << hex << et_buff[7] << endl;
		switch (magic) {
			case 0xc0da0100:
			swap_needed = false;
			break;
			case 0x0001dac0:
			swap_needed = true;
			break;
			default:
			jerr << "EVIO magic word not present!" << endl;
			return NO_MORE_EVENTS_IN_SOURCE;
		}
		uint32_t len = et_buff[0];
		if (swap_needed) len = EVIO_SWAP32(len);
		if (m_VERBOSE > 3) {
			jout << "Swapping is " << (swap_needed ? "" : "not ") << "needed" << endl;
			jout << " Num. words in EVIO buffer: " << len << endl;
		}

		// Size of events in bytes
		uint32_t bufsize_bytes = (len + 1) * sizeof(uint32_t);// +1 is for buffer length word
		if (bufsize_bytes > BUFFER_SIZE) {
			jerr << " ET event larger than our BUFFER_SIZE!!!" << endl;
			jerr << " " << bufsize_bytes << " > " << BUFFER_SIZE << endl;
			jerr << " Will stop reading from this source now. Try restarting" << endl;
			jerr << " with -PEVIO:BUFFER_SIZE=X where X is greater than " << bufsize_bytes << endl;
			if (m_VERBOSE > 3) {
				jout << "First few words in case you are trying to debug:" << endl;
				for (unsigned int j = 0; j < 3; j++) {
					char str[512];
					for (unsigned int i = 0; i < 5; i++) {
						sprintf(str, " %08x", et_buff[i + j * 5]);
						jout << str;
					}
					jout << endl;
				}
			}
			return NO_MORE_EVENTS_IN_SOURCE;
		}

		if (m_VERBOSE > 8) {
			jout << "First few ET words in case you are trying to debug:" << endl;
			for (unsigned int j = 0; j < 3; j++) {
				char str[512];
				for (unsigned int i = 0; i < 5; i++) {
					sprintf(str, " %08x", et_buff[i + j * 5]);
					jout << str;
				}
				jout << endl;
			}
		}

		// Copy event into "buff", byte swapping if needed.
		// The evioswap routine will not handle the NTH correctly
		// so we need to swap that separately and then swap each
		// event in the stack using evioswap so that the different
		// bank types are handled properly. If no swapping is
		// needed, we just copy it all over in one go.
		if (!swap_needed) {
			// Copy NTH and all events without swapping
			if (m_VERBOSE > 5) jout << "Before memcpy: buff is: " << buff << " et_buff is: " << et_buff << " bufsize_bytes is: " << bufsize_bytes << " I am: " << this << endl;
			memcpy(buff, et_buff, bufsize_bytes);
			if (m_VERBOSE > 5) jout << "After memcpy: buff is: " << buff << " et_buff is: " << et_buff << " bufsize_bytes is: " << bufsize_bytes << " I am: " << this << endl;

		} else {

			// Swap+copy NTH
			swap_int32_t(et_buff, 8, buff);

			// Loop over events in stack
			int Nevents_in_stack = 0;
			uint32_t idx = 8;
			while (idx < len) {
				uint32_t mylen = EVIO_SWAP32(et_buff[idx]);
				if (m_VERBOSE > 7) jout << "        swapping event: idx=" << idx << " mylen=" << mylen << endl;
				if ((idx + mylen) > len) {
					jerr << "Bad word count while swapping events in ET event stack!" << endl;
					jerr << "idx=" << idx << " mylen=" << mylen << " len=" << len << endl;
					jerr << "This indicates a problem either with the DAQ system" << endl;
					jerr << "or this parser code! " << endl;
					break;
				}
				swap_int32_t(&et_buff[idx], mylen + 1, &buff[idx]);
				idx += mylen + 1;
				Nevents_in_stack++;
			}

			if (m_VERBOSE > 3) jout << "        Found " << Nevents_in_stack << " events in the ET event stack." << endl;
		}
		// Put ET event back since we're done with it
		et_event_put(sys_id, att_id, pe);
		if (m_VERBOSE > 5) jout << "before EDT creation" << endl;
		EDT = new evioDOMTree(&buff[8]); /*A.C. this assumes the following is true: 1 ET_event contains 1 EVIO block that just has 1 EVIO event*/
		event.SetRef(EDT);
		event.SetJEventSource(this);
		if (m_VERBOSE > 5) jout << "After EDT creation and reference set" << endl;

		evio::evioDOMNodeListP fullList = EDT->getNodeList();
		evio::evioDOMNodeList::const_iterator iter;

		/*Check if this is an EPICS event, if so call the  SetSequential() method on it*/
		/*To do so, read the first tag in the nodeList, that is the one defining the event.*/
		iter = fullList->begin();
		curEventType=(*iter)->tag;
		if (curEventType==eventTypeEPICS) {
			event.SetSequential();
		}

		for (iter = fullList->begin(); iter != fullList->end(); iter++) {
			if ((*iter)->tag == end_tag) { //it means the run ended.
				jout << "Got end event" << endl;
				return NO_MORE_EVENTS_IN_SOURCE;
			}
			if (((*iter)->tag == prestart_tag) && (overwriteRunNumber == -1)) {
				const evio::evioCompositeDOMLeafNode *leaf = static_cast<const evio::evioCompositeDOMLeafNode*>(*iter);
				vector<uint32_t> *pData = const_cast<vector<uint32_t> *>(&(leaf->data));
				curRunNumber = (*pData)[1];
			}
			if ((*iter)->tag == eventHeader_tag) {
				const evio::evioCompositeDOMLeafNode *leaf = static_cast<const evio::evioCompositeDOMLeafNode*>(*iter);
				vector<uint32_t> *pData = const_cast<vector<uint32_t> *>(&(leaf->data));
				event.SetEventNumber((*pData)[2]);
				curRunNumber = (*pData)[1];
				curEventType = (*pData)[4];
			}
		}
		if (overwriteRunNumber != -1) {
			event.SetRunNumber(overwriteRunNumber);
		} else {
			event.SetRunNumber(curRunNumber);
		}
		return NOERROR;

#else
		japp->Quit();
		jout << "Attempting to read from ET system using binary that" << endl;
		jout << "does not have ET support built in! Try recompiling" << endl;
		jout << "programs/Utilities/plugins/DAQ with ETROOT defined" << endl;
		jout << "and pointing to an ET installation." << endl;
		return OBJECT_NOT_AVAILABLE;
#endif
	}
}

// FreeEvent
void JEventSourceEvioDAQ::FreeEvent(JEvent &event) {
	if (event.GetRef() != NULL) {
		delete (evioDOMTree*) event.GetRef();
	}
}

// GetObjects
jerror_t JEventSourceEvioDAQ::GetObjects(JEvent &event, JFactory_base *factory) {
/// This gets called through the virtual method of the
/// JEventSource base class. It creates the objects of the type
/// which factory is based.

/// Example: DCsegment needs DCHit. If DCHit doesn't exist already, then
/// it will be read here.

	// We must have a factory to hold the data
	if (!factory) throw RESOURCE_UNAVAILABLE;

	// Get name of data class we're trying to extract
	string dataClassName = factory->GetDataClassName();

	//As suggested by David, do a check on the factory type to decide what to do
	JFactory<fa250Mode1Hit> *fac_fa250Mode1hit = dynamic_cast<JFactory<fa250Mode1Hit>*>(factory);
	JFactory<fa250Mode7Hit> *fac_fa250Mode7hit = dynamic_cast<JFactory<fa250Mode7Hit>*>(factory);
	JFactory<eventData> *fac_eventData = dynamic_cast<JFactory<eventData>*>(factory);
	JFactory<epicsRawData> *fac_epicsData = dynamic_cast<JFactory<epicsRawData>*>(factory);

	if (fac_fa250Mode1hit != NULL) {

		vector<fa250Mode1Hit*> data;
		evioDOMTree* local_EDT = (evioDOMTree*) event.GetRef();
		//	jout<<local_EDT->toString()<<endl;

		evio::evioDOMNodeListP fullList = local_EDT->getNodeList();
		evio::evioDOMNodeList::const_iterator iter;
		evio::evioDOMNodeList::const_iterator branch;
		EvioCompositeDecoder decoder;

		for (iter = fullList->begin(); iter != fullList->end(); iter++) {
			if ((*iter)->tag == vme_mother_tag) {
				evio::evioDOMNodeList *leafList = (*iter)->getChildList();
				for (branch = leafList->begin(); branch != leafList->end(); branch++) {
					if ((*branch)->tag == child_mode1_tag) {
						const evio::evioCompositeDOMLeafNode *leaf = static_cast<const evio::evioCompositeDOMLeafNode*>(*branch);
						int leafSize = leaf->getSize();
						vector<uint32_t> *pData = const_cast<vector<uint32_t> *>(&(leaf->data));
						decoder.decode(pData, leafSize);
						if (decoder.getData().size() > 0) {
							try {
								vector<CompositeADCRaw_t> decdata = decoder.getData();
								for (int loop = 0; loop < decdata.size(); loop++) {
									fa250Mode1Hit *hit = new fa250Mode1Hit();
									/*Why I do not save directly the CompositeADCRaw_t and use it as the "Jobject in jana:
									 1) CompositeADCRaw_t is just a simple structure to old data
									 2) CompositeADCRaw_t is just for evio-readout
									 3) fa250Mode1Hit is a more complex class, used within JANA
									 */
									hit->m_channel.rocid = 0;	///TODO better
									hit->m_channel.slot = decdata[loop].slot;
									hit->m_channel.channel = decdata[loop].channel;
									for (int isample = 0; isample < decdata[loop].samples.size(); isample++) {
										hit->samples.push_back(decdata[loop].samples[isample]);
									}

									hit->trigger = decdata[loop].trigger;
									hit->timestamp = decdata[loop].time;

									data.push_back(hit);
								}
							} catch (exception e) {
								cout << "ERROR" << endl;
							}
						}
					}
				}
			}
		}
		fac_fa250Mode1hit->CopyTo(data);
		return NOERROR;
	}

	else if (fac_fa250Mode7hit != NULL) {
		vector<fa250Mode7Hit*> data;
		evioDOMTree* local_EDT = (evioDOMTree*) event.GetRef();

		evio::evioDOMNodeListP fullList = local_EDT->getNodeList();
		evio::evioDOMNodeList::const_iterator iter;
		evio::evioDOMNodeList::const_iterator branch;
		EvioCompositeDecoder decoder;

		for (iter = fullList->begin(); iter != fullList->end(); iter++) {
			if ((*iter)->tag == vme_mother_tag) {

				evio::evioDOMNodeList *leafList = (*iter)->getChildList();

				for (branch = leafList->begin(); branch != leafList->end(); branch++) {
					if ((*branch)->tag == child_mode7_tag) {
						const evio::evioCompositeDOMLeafNode *leaf = static_cast<const evio::evioCompositeDOMLeafNode*>(*branch);
						int leafSize = leaf->getSize();
						vector<uint32_t> *pData = const_cast<vector<uint32_t> *>(&(leaf->data));
						decoder.decode(pData, leafSize);
						if (decoder.getData().size() > 0) {
							try {
								vector<CompositeADCPulse_t> decdata = decoder.getDataPulse();
								for (int loop = 0; loop < decdata.size(); loop++) {
									fa250Mode7Hit *hit = new fa250Mode7Hit();
									/*Why I do not save directly the CompositeADCPulse_t and use it as the "Jobject in jana:\
									 1) CompositeADCPulse_t is just a simple structure to old data
									 2) CompositeADCPulse_t is just for evio-readout
									 3) fa250Mode7Hit is a more complex class, used within JANA
									 */
									hit->m_channel.rocid = 0;	///TODO better
									hit->m_channel.slot = decdata[loop].slot;
									hit->m_channel.channel = decdata[loop].channel;
									hit->trigger = decdata[loop].trigger;
									hit->timestamp = decdata[loop].time;
									for (int ipulse = 0; ipulse < decdata[loop].pulses.size(); ipulse++) {
										fa250Mode7Hit::pulse_t pulse;
										pulse.tdc = decdata[loop].pulses[ipulse].tdc;
										pulse.adc = decdata[loop].pulses[ipulse].adc;
										pulse.ped = decdata[loop].pulses[ipulse].ped;
										pulse.max = decdata[loop].pulses[ipulse].max;
										hit->pulses.push_back(pulse);
									}
									data.push_back(hit);
								}
							} catch (exception e) {
								cout << "ERROR" << endl;
							}
						}
					}
				}
			}
		}
		fac_fa250Mode7hit->CopyTo(data);
		return NOERROR;
	} else if (fac_eventData != NULL) {
		vector<eventData*> data;
		evioDOMTree* local_EDT = (evioDOMTree*) event.GetRef();

		evio::evioDOMNodeListP fullList = local_EDT->getNodeList();
		evio::evioDOMNodeList::const_iterator iter;
		evio::evioDOMNodeList::const_iterator branch;

		eventData *this_eventData = new eventData();

		for (iter = fullList->begin(); iter != fullList->end(); iter++) {

			if (((*iter)->tag == vme_mother_tag) || ((*iter)->tag == epics_mother_tag)) {
				evio::evioDOMNodeList *leafList = (*iter)->getChildList();
				if ((*iter)->tag == vme_mother_tag) this_eventData->eventType = eventSource::DAQ;
				if ((*iter)->tag == epics_mother_tag) this_eventData->eventType = eventSource::EPICS;
				for (branch = leafList->begin(); branch != leafList->end(); branch++) {

					if ((*branch)->tag == child_trigger_tag) {
						const evio::evioCompositeDOMLeafNode *leaf = static_cast<const evio::evioCompositeDOMLeafNode*>(*branch);
						int leafSize = leaf->getSize();
						vector<uint32_t> *pData = const_cast<vector<uint32_t> *>(&(leaf->data));
						if (leafSize > 0) {
							for (int itrigWord = 0; itrigWord < pData->size(); itrigWord++) {
								this_eventData->triggerWords.push_back((*pData)[itrigWord]);
							}
						}

					}

					else if ((*branch)->tag == eventHeader_tag) {

						const evio::evioCompositeDOMLeafNode *leaf = static_cast<const evio::evioCompositeDOMLeafNode*>(*branch);
						int leafSize = leaf->getSize();
						vector<uint32_t> *pData = const_cast<vector<uint32_t> *>(&(leaf->data));
						if (leafSize != 5) {  //should have 5 words in head bank
							jerr << "Incompatible number of words in head bank: got " << leafSize << endl;
						} else {
							this_eventData->eventN = (*pData)[2];
							this_eventData->runN = (*pData)[1];
							this_eventData->time = (*pData)[3];




						}
					}
				}
			}
		}

		/*Only copy this further for VME or EPICS*/
		if ((this_eventData->eventType == eventSource::DAQ) || (this_eventData->eventType == eventSource::EPICS)) {
			data.push_back(this_eventData);
			fac_eventData->CopyTo(data);
		}
		return NOERROR;
	}

	/*Simply get all strings from the raw epics bank and save them as a vector of epicsRawData, for further processing*/
	else if (fac_epicsData != NULL) {
		vector<epicsRawData*> epicsData;
		evioDOMTree* local_EDT = (evioDOMTree*) event.GetRef();
		evio::evioDOMNodeListP fullList = local_EDT->getNodeList();
		evio::evioDOMNodeList::const_iterator iter;
		evio::evioDOMNodeList::const_iterator branch;

		for (iter = fullList->begin(); iter != fullList->end(); iter++) {
			if ((*iter)->tag == epics_mother_tag) {
				evio::evioDOMNodeList *leafList = (*iter)->getChildList();
				for (branch = leafList->begin(); branch != leafList->end(); branch++) {

					if ((*branch)->tag == child_epics_tag) {
						const evio::evioDOMLeafNode<string> *leaf = static_cast<const evio::evioDOMLeafNode<string>*>(*branch);
						/*EPICs data is saved as a gigantic string, in the form VALUE DESCRIPTION \n */
						for (int istr = 0; istr < leaf->data.size(); istr++) {
							epicsRawData *thisEpicsRawData = new epicsRawData();
							thisEpicsRawData->rawData = (leaf->data[istr]);
							epicsData.push_back(thisEpicsRawData);
						}
					}
				}

			}

		}
		fac_epicsData->CopyTo(epicsData);
		return NOERROR;
	}

// Just return. The _data vector should already be reset to have zero objects
	return OBJECT_NOT_AVAILABLE;
}

//----------------
// ConnectToET
//----------------
void JEventSourceEvioDAQ::ConnectToET(const char* source_name) {
#ifdef ET_SUPPORT_ENABLE

/// Format for ET source strings is:
///
///  ET:session:station:host:port
///
/// The session is used to form the filename of the ET
/// system. For example, if an session of "eb" is specified,
/// then a file named "/tmp/et_sys_eb" is assumed to be
/// what should be opened. If no session is specified (or
/// an empty session name) then "none" is used as the session.
///
/// If the station name specified does not exist, it will
/// be created. If it does exist, the existing station will
/// be used. If no station is specified, then the station
/// name "DANA" will be used. Any station created will be
/// set to "blocking" *unless* the configuration paramter
/// EVIO:ET_STATION_CREATE_BLOCKING is set to "0"
/// in which case it will be set to non-blocking.
///
/// If the host is specified, then an attempt will be made
/// to open that system. If it is not specified, then
/// it will attempt to open an ET system on the local machine.
///
/// If port is specified, it is used as the TCP port number
/// on the remote host to attach to. If the host is not
/// specified (i.e. by having two colons and therefore
/// an empty string) then the port is ignored. If the
/// port is omitted or specified as "0", then the default
/// port is used.
///

// Split source name into session, station, etc...
	vector<string> fields;
	string str = source_name;
	size_t startpos = 0, endpos = 0;
	while ((endpos = str.find(":", startpos)) != str.npos) {
		size_t len = endpos - startpos;
		fields.push_back(len == 0 ? "" : str.substr(startpos, len));
		startpos = endpos + 1;
	}
	if (startpos < str.length()) fields.push_back(str.substr(startpos, str.npos));

	string session = fields.size() > 1 ? fields[1] : "";
	string station = fields.size() > 2 ? fields[2] : "";
	string host = fields.size() > 3 ? fields[3] : "localhost";
	int port = fields.size() > 4 ? atoi(fields[4].c_str()) : ET_SERVER_PORT;

	if (session == "") session = "none";
	if (station == "") station = "DANA";
	if (host == "") host = "localhost";
	string fname = session.at(0) == '/' ? session : (string("/tmp/et_sys_") + session);

// Report to user what we're doing
	jout << " Opening ET system:" << endl;
	if (session != fname) jout << "     session: " << session << endl;
	jout << "     station: " << station << endl;
	jout << " system file: " << fname << endl;
	jout << "        host: " << host << endl;
	if (port != 0) jout << "        port: " << port << endl;

// connect to the ET system
	et_openconfig openconfig;
	et_open_config_init(&openconfig);
	if (host != "") {
		et_open_config_setcast(openconfig, ET_DIRECT);
		et_open_config_setmode(openconfig, ET_HOST_AS_LOCAL); // ET_HOST_AS_LOCAL or ET_HOST_AS_REMOTE
		et_open_config_sethost(openconfig, host.c_str());
		et_open_config_setport(openconfig, ET_BROADCAST_PORT);
		et_open_config_setserverport(openconfig, port);
	}
	int err = et_open(&sys_id, fname.c_str(), openconfig);
	if (err != ET_OK) {
		cerr << __FILE__ << ":" << __LINE__ << " Problem opening ET system" << endl;
		cerr << et_perror(err);
		return;
	}

// create station config in case no station exists
	et_statconfig et_station_config;
	et_station_config_init(&et_station_config);
	et_station_config_setblock(et_station_config, ET_STATION_CREATE_BLOCKING ? ET_STATION_BLOCKING : ET_STATION_NONBLOCKING);
	et_station_config_setselect(et_station_config, ET_STATION_SELECT_ALL);
	et_station_config_setuser(et_station_config, ET_STATION_USER_MULTI);
	et_station_config_setrestore(et_station_config, ET_STATION_RESTORE_OUT);
	et_station_config_setcue(et_station_config, ET_STATION_NEVENTS);
	et_station_config_setprescale(et_station_config, 1);
	cout << "ET station configured\n";

// create station if not already created
	int status = et_station_create(sys_id, &sta_id, station.c_str(), et_station_config);
	if ((status != ET_OK) && (status != ET_ERROR_EXISTS)) {
		et_close(sys_id);
		cerr << "Unable to create station " << station << endl;
		cerr << et_perror(status);

		// Check that the number of events in the ET system is not
		// less than the number of events we specified for the station CUE.
		int Nevents = 0;
		et_system_getnumevents(sys_id, &Nevents);
		if (Nevents <= ET_STATION_NEVENTS) {
			jerr << "NOTE: The number of events specified for the station cue is equal to" << endl;
			jerr << "or greater than the number of events in the entire ET system:" << endl;
			jerr << endl;
			jerr << "     " << ET_STATION_NEVENTS << " >= " << Nevents << endl;
			jerr << endl;
			jerr << "Try re-running with: " << endl;
			jerr << endl;
			jerr << "      -PDAQ:ET_STATION_NEVENTS=" << (Nevents + 1) / 2 << endl;
			jerr << endl;
		}
		return;
	}
	if (status == ET_ERROR_EXISTS) {
		jout << " Using existing ET station " << station << endl;
	} else {
		jout << " ET station " << station << " created\n";
	}

// Attach to the ET station
	status = et_station_attach(sys_id, sta_id, &att_id);
	if (status != ET_OK) {
		et_close(sys_id);
		jerr << "Unable to attach to station " << station << endl;
		return;
	}

	jout << "...now connected to ET system: " << fname << ",   station: " << station << " (station id=" << sta_id << ", attach id=" << att_id << ")" << endl;

	et_connected = true;
// chan = new evioETChannel(sys_id, att_id);

// Make sure the size of event buffers we will allocate are at least as big
// as the event size used in the ET system
	size_t eventsize;
	et_system_geteventsize(sys_id, &eventsize);
	if ((uint32_t) eventsize > BUFFER_SIZE) {
		jout << " Events in ET system are larger than currently set buffer size:" << endl;
		jout << " " << eventsize << " > " << BUFFER_SIZE << endl;
		jout << " Setting BUFFER_SIZE to " << eventsize << endl;
		BUFFER_SIZE = (uint32_t) eventsize;
	} else {
		jout << " ET system event size:" << eventsize << "  JEventSource_DAQ.BUFFER_SIZE:" << BUFFER_SIZE << endl;
	}

#else
	jerr << endl;
	jerr << "You are attempting to connect to an ET system using a binary that" << endl;
	jerr << "was compiled without ET support. Please reconfigure and recompile" << endl;
	jerr << "To get ET support." << endl;
	jerr << endl;
	throw exception();
#endif  // HAVE_ET
}

