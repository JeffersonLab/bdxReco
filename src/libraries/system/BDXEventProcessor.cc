// C++ headers
#include <cstdio>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <string>
using namespace std;

// bdx headers
#include "BDXEventProcessor.h"
#include "string_utilities.h"
//DAQ
#include <DAQ/eventData.h>
//EPICS
#include <EPICS/epicsData.h>
// TT
#include <TT/TranslationTable.h>

//DST
#include <EventBuilder/TEvent.h>
#include <EventBuilder/TEventHeader.h>

#include "TTree.h"

#include "JOutput.h"
#include "JROOTOutput.h"

// Constructor
BDXEventProcessor::BDXEventProcessor() :
		m_output(0), m_tt(0), m_isMC(0), m_eventDST(0), m_runInfo(0), m_event(0), eventN(0), m_eventHeader(0), eventT(0), deltaTime(0), runN(0), isET(0) {
	bout << "BDXEventProcessor creator start" << endl;
	optf = "";
	m_DObuildDST = 0;
	m_buildDST = "";
	isFirstCallToBrun = 1;
	startTime = 9999999999;
	stopTime = -9999999999;

	bout.SetTag("BDXEventProcessor >>");
	berr.SetTag("BDXEventProcessorError >>");

	bout << "BDXEventProcessor creator end" << endl;
}

// Destructor
BDXEventProcessor::~BDXEventProcessor() {

}

// init
jerror_t BDXEventProcessor::init(void) {

	bout << "BDXEventProcessor::init" << endl;
	gPARMS->GetParameter("MC", m_isMC);
	bout << "BDXEventProcess: "<<m_isMC<<endl;
	gPARMS->SetDefaultParameter("SYSTEM:BUILD_DST", m_buildDST, "Enable DST, using the form \"TYPE\". TYPE is the name of an existing event builder. Example: -PSYSTEM:BUILD_DST=\"CataniaProto2\" (also: FullMC, JLabFlux)");
	bout << "Building DST is: " << m_buildDST << endl;

	if (m_buildDST.size() == 0) {
		bout << "No DST will be built" << endl;
		m_DObuildDST = 0;
	} else {
		m_DObuildDST = 1;
	}

	gPARMS->SetDefaultParameter("SYSTEM:OUTPUT", optf, "Set OUTPUT file type and name, using the form \"TYPE,FILENAME\". Type can be ROOT, EVIO, TXT. Example: -PSYSTEM:OUTPUT=\"ROOT,out.root\" ");
	bout << "Out string is: " << optf << endl;
	outType.assign(optf, 0, optf.find(","));
	outFile.assign(optf, optf.find(",") + 1, optf.size());

	std::transform(outType.begin(), outType.end(), outType.begin(), ::tolower);

	if (optf != "none") {
		bout << "Again, out string is: " << optf << endl;
		bout << "Out file type is: " << outType << endl;
		if (outType == "root") {
			m_output = new JROOTOutput();
		} else if (outType == "root_et") { /*Special case, when we connect to ET-ring*/
#ifdef ET_SUPPORT_ENABLE
			isET = 1;
			m_output = new JROOTOutput();
#else
			bout << "root_et file type was requested, but no ET-support was built. Exiting!" << endl;
			exit(1);
#endif
		} else if (outType == "evio") {
			berr << "evio not yet implemented" << endl;
		} else if (outType == "txt") {
			berr << "txt not yet implemented" << endl;
		} else {
			berr << "file type not recognized: " << outType << endl;
		}

	}

	// lock all root operations
	japp->RootWriteLock();
	/*Event header and runInfo are always created - as memory resident TTrees (these are quite small)*/
	gDirectory->cd();

	m_eventHeader = new TTree("EventHeader", "EventHeader");
	m_eventHeader->Branch("eventN", &eventN);
	m_eventHeader->Branch("runN", &runN);
	m_eventHeader->Branch("T", &eventT);

	m_runInfo = new TTree("RunInfo", "RunInfo");
	m_runInfo->Branch("runN", &runN);
	m_runInfo->Branch("dT", &deltaTime);
	japp->RootUnLock();

	return NOERROR;
}

// brun
jerror_t BDXEventProcessor::brun(JEventLoop *eventLoop, int32_t runnumber) {

	bout << "BDXEventProcessor::brun " << runnumber << "(isFirstCallToBrun: " << isFirstCallToBrun << " m_output: " << m_output << ")" << endl;

	// lock all root operations
	japp->RootWriteLock();
	if (isFirstCallToBrun) {
		if (m_output != 0) {
			bout << "got m_output, className is: " << m_output->className() << endl;
			if (strcmp(m_output->className(), "JROOTOutput") == 0) {
				JROOTOutput* m_ROOTOutput = (JROOTOutput*) m_output;
				if (isET == 1) {
					outFile = string(Form("out.%i.online.root", runnumber));
					bout << "Running on ET with root thus changing file name to: " << outFile << endl;
				}
				m_ROOTOutput->OpenOutput(outFile);

				/*DST TTree created as file-resident TTree, only if output exists*/
				if (m_DObuildDST) {
					m_eventDST = new TTree("EventDST", "EventDST");
					m_eventDST->Branch("Event", &m_event);
					if (m_isMC == 0) m_eventDST->AddFriend(m_eventHeader);
					m_ROOTOutput->AddObject(m_eventDST);
				}

				/*If an output exists, add the eventHeader and runInfo*/
				(m_ROOTOutput)->AddObject(m_eventHeader);
				(m_ROOTOutput)->AddObject(m_runInfo);
			}
		}
	}

	if (m_isMC == 0) {
		eventLoop->GetSingle(m_tt);
	}
	// lock all root operations
	japp->RootUnLock();
	return NOERROR;
}

// evnt
jerror_t BDXEventProcessor::evnt(JEventLoop *loop, uint64_t eventnumber) {

	const eventData* tData;
	vector<const TEvent*> events;
	const epicsData* eData;

	/*For non-MC case, check that:
	 * There's a valid eventData (true for both DAQ and EPICS)
	 * There's a valid epicsData (true for both DAQ and EPICS - DAQ uses persistency)
	 * The event is from DAQ - otherwise skip. Don't want to save non-DAQ events.
	 */
	if (m_isMC == 0) {
		try {
			loop->GetSingle(tData);
		} catch (unsigned long e) {
			bout << "No eventData bank this event " << endl;
			return OBJECT_NOT_AVAILABLE;
		}
		/*This is the EPICS part. The call here will force getting data from the epicsDataProcessed_factory, that takes care of persistency*/
		try {
			loop->GetSingle(eData);
		} catch (unsigned long e) {
			return OBJECT_NOT_AVAILABLE;
		}

		if (tData->eventType != eventSource::DAQ) {
			//	jout<<"BDXEventProcessor exit: tData->eventType: "<<tData->eventType<<endl;
			return OBJECT_NOT_AVAILABLE;
		}
	}
	if (m_DObuildDST) {
		loop->Get(events, m_buildDST.c_str());
		if (events.size() != 1) {
			return RESOURCE_UNAVAILABLE;
		}
		/*Add EPICS data in case of non-MC*/
		if (m_output != 0) {

			if (m_isMC == 0) events[0]->getEventHeader()->copyEpicsData(eData);
			japp->RootWriteLock();
			m_event = events[0];
			m_eventDST->Fill();
			japp->RootUnLock();
		}
	}

	if (m_isMC == 0) {
		if (m_output != 0) {
			japp->RootWriteLock();
			eventT = tData->time;
			eventN = eventnumber;

			runN = tData->runN;
			m_eventHeader->Fill();
			//Time
			if (eventT < startTime) startTime = eventT;
			if (eventT > stopTime) stopTime = eventT;
			japp->RootUnLock();
		}
	}
	return NOERROR;
}

// erun
jerror_t BDXEventProcessor::erun(void) {
	japp->RootWriteLock();
	deltaTime = stopTime - startTime;
	bout << "BDXEventProcessor::erun " << endl;
	bout << "Run start: " << startTime << " stop: " << stopTime << " diff: " << deltaTime << endl;
	m_runInfo->Fill();

	if (m_output && (isET == 1)) {
		m_output->CloseOutput();
	}
	japp->RootUnLock();
	return NOERROR;
}

// fini
jerror_t BDXEventProcessor::fini(void) {
	// If another EventProcessor is in the list ahead of this one, then
	// it will have finished before this is called. e.g. closed the
	// ROOT file!
	bout << "BDXEventProcessor fini called" << endl;
	fflush(stdout);
	japp->RootWriteLock();
	if (m_output) {
		m_output->CloseOutput(); /*This is ok, CloseOutput takes care of m_output already closed*/
	}
	japp->RootUnLock();
	bout << "BDXEventProcessor fini ends" << endl;
	fflush(stdout);
	return NOERROR;
}

void BDXEventProcessor::updateCalibration(CalibrationHandlerBase* cal, JEventLoop* eventLoop) {
	/*First, check the table name for cal*/
	string name = cal->getTable();
	/*Verify the key is in the map*/
	m_calibrations_it = m_calibrations.find(name);
	if (m_calibrations_it == m_calibrations.end()) {
		berr << "BDXEventProcessor::updateCalibration error, calibration handler associated with table " << name << " was not registered!" << endl;
		return;
	}

	vector<CalibrationHandlerBase*> calibrations = m_calibrations[name];
	vector<CalibrationHandlerBase*>::iterator calibrations_it;

	/*Another check*/
	calibrations_it = find(calibrations.begin(), calibrations.end(), cal);
	if (calibrations_it == calibrations.end()) {
		berr << "UpdateCalibration, key: " << name << " was found but not this specific calibrationHandler!" << endl;
		return;
	}
	/*Verify if all the calibrators already have been set for this run*/
	/*Since we are multi-thread mode, it is possible this method is called by different factories at different times.
	 * Need to perform a clever check:
	 * 1) Loop over all the calibrations handlers for the given table.
	 * 1A) If ALL of them have been loaded, do nothing
	 * 1B) If NONE of them have been loaded, do that
	 * 1C) If ONE of them at least have been loaded, use it for all the non-calibrated ones
	 */
	bool flagAll = true;
	int calibratedOne = -1;
	for (calibrations_it = calibrations.begin(); calibrations_it != calibrations.end(); calibrations_it++) {
		if ((*calibrations_it)->hasLoadedCurrentRun() == false) flagAll = false;
		else
			calibratedOne = std::distance(calibrations.begin(), calibrations_it); //save the index of this calibrated object
	}
	if (flagAll) { /*flagAll is true if ALL off them have been loaded*/
		bout << "Going to fill CalibrationHandlers for table: " << name << " there are: " << calibrations.size() << " I AM: " << this << " ALREADY DONE " << endl;
		return;
	} else if (calibratedOne != -1) { /*It means there is at least an already-calibrated object!*/
		bout << "Going to fill CalibrationHandlers for table: " << name << " there are: " << calibrations.size() << " I AM: " << this << " Load from data: " << calibratedOne << endl;
		fflush(stdout);
		for (int ical = 0; ical < calibrations.size(); ical++) {
			if (ical == calibratedOne) continue;
			else if (calibrations[ical]->hasLoadedCurrentRun() == true) continue;
			else {
				calibrations[ical]->fillCalib(calibrations[calibratedOne]->getRawCalibData());
				calibrations[ical]->setLoadedCurrentRun(true);
			}
		}
	} else {
		/*Get the data*/
		vector<vector<double> > m_data;
		eventLoop->GetCalib(name, m_data);
		bout << "Going to fill CalibrationHandlers for table: " << name << " there are: " << calibrations.size() << " I AM: " << this << " Load from DB " << endl;

		for (calibrations_it = calibrations.begin(); calibrations_it != calibrations.end(); calibrations_it++) {
			(*calibrations_it)->fillCalib(m_data);
			(*calibrations_it)->setLoadedCurrentRun(true);
		}
	}
	bout << "Done table: " << name << endl;
	fflush(stdout);
	return;
}

void BDXEventProcessor::clearCalibration(CalibrationHandlerBase* cal) {
	/*First, check the table name for cal*/
	string name = cal->getTable();
	/*Verify the key is in the map*/
	m_calibrations_it = m_calibrations.find(name);
	if (m_calibrations_it == m_calibrations.end()) {
		berr << "BDXEventProcessor::clearCalibration error, calibration handler associated with table " << name << " was not registered!" << endl;
		return;
	}

	vector<CalibrationHandlerBase*> calibrations = m_calibrations[name];
	vector<CalibrationHandlerBase*>::iterator calibrations_it;

	/*Another check*/
	calibrations_it = find(calibrations.begin(), calibrations.end(), cal);
	if (calibrations_it == calibrations.end()) {
		berr << "BDXEventProcessor::clearCalibration, key: " << name << " was found but not this specific calibrationHandler!" << endl;
		return;
	}
	for (calibrations_it = calibrations.begin(); calibrations_it != calibrations.end(); calibrations_it++) {
		(*calibrations_it)->setLoadedCurrentRun(false);
	}
}

void BDXEventProcessor::addCalibration(CalibrationHandlerBase* cal) {
	this->m_calibrations[cal->getTable()].push_back(cal);
}
