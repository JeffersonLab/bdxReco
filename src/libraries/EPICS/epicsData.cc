#include "epicsData.h"
#include <string>       // std::string
#include <iostream>     // std::cout
#include <sstream>      // std::istringstream

/*raw Data is in the format: value description \n */
void epicsData::decode(string rawData,int deltaTime) {
	std::istringstream stream;
	stream.str(rawData);
	double val;
	string desc;


	/*This is the decoding loop, taking care of getting data from rawData string
	 * and filling the map of values in epicsData.
	 * I don't reset the map, since not every epics event has all the epics entries
	 * (for example, for JLabFlux data entries from Hall-A current/beamPos/.. are not saved at same time as entries from DAQ)
	 */
	while (1) {
		stream >> val >> desc;
		if (stream.eof()) break;
		this->values[desc] = val;        //the [] operator takes care of adding a new record if key "desc" doesn't exists
		this->times[desc]  = this->time+deltaTime; //time should be set before calling this method
	}
}
bool epicsData::hasData(string key) const{
	bool ret = false;
	std::map<string, double>::const_iterator it;
	it = this->values.find(key);
	if (it != this->values.end()) ret = true;

	return ret;

}

double epicsData::getDataValue(string key) const {
	std::map<string,double>::const_iterator it;
	it = this->values.find(key);
	if (it != this->values.end()) {
		return it->second;
	}
	else{
		return -999;
	}
}

int epicsData::getDataTime(string key) const{
	std::map<string,int>::const_iterator it;
	it = this->times.find(key);
	if (it != this->times.end()) {
		return it->second;
	}
	else{
		return -999;
	}
}

void  epicsData::Print(Option_t *option) const{

	std::map<string,double>::const_iterator it1;
	std::map<string,int>::const_iterator it2;
	jout<<"epicsData print: "<<std::endl;
	for (it1 = this->values.begin();it1!=this->values.end();it1++){
		it2=times.find(it1->first);
		jout<<(it1)->first<<" "<<(it1)->second<<" "<<it2->second<<" "<<std::endl;
	}
}
