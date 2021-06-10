// $Id$
//
//    File: triggerData.h
// Created: Thu Jan 28 18:19:51 CET 2016
// Creator: celentan (on Linux apcx4 2.6.32-504.30.3.el6.x86_64 x86_64)
//

#ifndef _eventData_
#define _eventData_

#include <JANA/JObject.h>
#include <JANA/JFactory.h>

#include "TString.h"
//#include <vector>


#include <TRIDAS/ptfile_reader.hpp>
using namespace tridas::post_trigger;

typedef enum {
	DAQ=1,EPICS=2,CONF=3
} eventSource;

class eventData:public jana::JObject{
	public:
		JOBJECT_PUBLIC(eventData);
		
		unsigned long int eventN;
		unsigned long int eventTS; //This has been added for the TRIDAS system, where an event is identified uniquely by an TS id and an event ID
		unsigned int runN;
		unsigned long int time; //event time in UNIX time stamp (seconds)
		unsigned long int fineTime; //The nano-seconds inside the event time

		std::vector<uint32_t> triggerWords;
		

		int eventType;


		// This method is used primarily for pretty printing
		// the second argument to AddString is printf style format
		void toStrings(vector<pair<string,string> > &items)const{
			AddString(items,"runN","%i",runN);
			AddString(items,"eventN","%lu",eventN);
			AddString(items,"eventTS","%lu",eventTS);
			AddString(items,"time","%lu",time);
			AddString(items,"fineTime","%lu",fineTime);
			for (int iword=0;iword<triggerWords.size();iword++){
			 AddString(items, Form("word_%i",iword), "%x", triggerWords[iword]);
			}
		}
		
		eventData(){
			eventType=0;
			eventN=0;
			eventTS=0;
			runN=0;
			time=0;
		}

};

#endif // _eventData_

