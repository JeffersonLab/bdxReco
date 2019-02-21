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

typedef enum {
	DAQ=1,EPICS=2,CONF=3
} eventSource;

class eventData:public jana::JObject{
	public:
		JOBJECT_PUBLIC(eventData);
		
		int eventN;
		int runN;
		int time; //event time in UNIX time stamp (seconds)
		std::vector<uint32_t> triggerWords;
		
		int eventType;


		// This method is used primarily for pretty printing
		// the second argument to AddString is printf style format
		void toStrings(vector<pair<string,string> > &items)const{
			AddString(items,"runN","%i",runN);
			AddString(items,"eventN","%i",eventN);
			AddString(items,"time","%i",time);
			for (int iword=0;iword<triggerWords.size();iword++){
			 AddString(items, Form("word_%i",iword), "%x", triggerWords[iword]);
			}
		}
		
		eventData(){
			eventType=0;
			eventN=0;
			runN=0;
			time=0;
		}

};

#endif // _eventData_

