/*
 * TEventHeader.h
 *
 *  Created on: Dec 23, 2016
 *      Author: celentan
 */

#ifndef TEVENTHEADER_H_
#define TEVENTHEADER_H_

#include "TObject.h"

typedef enum {
	CataniaProto1Event = 1, CataniaProto2Event = 2, JLabFluxEvent = 4, BDXminiEvent = 5, CataniaProto1MC = 10, CataniaProto2MC = 20, FullMC = 30, JLabFluxEventMC = 40, BDXminiEventMC = 50
} eventType;

class TEventHeader: public TObject {

private:

	std::vector<uint32_t> m_triggerWords;
	uint32_t m_eventFineTime; //this is the timestamp as reported by FADC boards, in particular I save here time from fadc in slot4, reading the calorimeter (there should be always a calo hit for events of interest). Units are: 4ns*/

	int m_runNumber;
	int m_eventNumber;
	int m_eventTS;

	int m_eventTime; //Unix Timestamp

	double m_weight;

	uint8_t m_eventType;


public:
	TEventHeader();
	virtual ~TEventHeader();

	int getEventNumber() const {
		return m_eventNumber;
	}

	void setEventNumber(int eventNumber) {
		m_eventNumber = eventNumber;
	}

	int getEventTS() const {
		return m_eventTS;
	}

	void setEventTS(int eventTS) {
		m_eventTS = eventTS;
	}

	int getRunNumber() const {
		return m_runNumber;
	}

	void setRunNumber(int runNumber) {
		m_runNumber = runNumber;
	}

	std::vector<uint32_t> getTriggerWords() const {
		return m_triggerWords;
	}

	void setTriggerWords(std::vector<uint32_t> triggerWords) {
		m_triggerWords = triggerWords;
	}

	int getEventTime() const {
		return m_eventTime;
	}

	void setEventTime(int eventTime) {
		m_eventTime = eventTime;
	}

	int getEventType() const {
		return m_eventType;
	}

	void setEventType(int eventType) {
		m_eventType = eventType;
	}

	uint32_t getEventFineTime() const {
		return m_eventFineTime;
	}

	void setEventFineTime(uint32_t eventFineTime) {
		m_eventFineTime = eventFineTime;
	}



	double getWeight() {
		return m_weight;
	}
	void setWeight(double w) {
		m_weight = w;
	}

ClassDef(TEventHeader,4)
	;

};

#endif /* TEVENTHEADER_H_ */
