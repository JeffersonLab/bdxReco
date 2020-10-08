// $Id$
//
//    File: epicsData.h
// Created: Fri Mar  9 11:22:49 EST 2018
// Creator: celentan (on Darwin celentano-macbook 17.4.0 i386)
//

#ifndef _epicsData_
#define _epicsData_

#include <JANA/JObject.h>
#include <JANA/JFactory.h>

class epicsData: public jana::JObject {
private:


	map<string, double> values;
	map<string, int> times;
public:
	JOBJECT_PUBLIC(epicsData);



	// This method is used primarily for pretty printing
	// the second argument to AddString is printf style format
	void toStrings(vector<pair<string, string> > &items) const {
		// AddString(items, "id", "%4d", id);
		// AddString(items, "E", "%f", E);
	}

	int runNumber, eventNumber, time;
	bool hasData(string key) const;
	double getDataValue(string key) const;
	int getDataTime(string key) const;


	void decode(string rawData,long int deltaTime);


	epicsData() {};
	virtual ~epicsData() {};

	/*TObject virtual methods*/
	virtual void Print(Option_t *option="") const;

	ClassDef(epicsData,1);
};

#endif // _epicsData_

