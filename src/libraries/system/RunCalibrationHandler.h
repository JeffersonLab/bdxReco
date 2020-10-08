/*
 * RunCalibrationHandler.h
 *
 *  Created on: Oct 8, 2020
 *      Author: celentan
 */

#ifndef SRC_LIBRARIES_SYSTEM_RUNCALIBRATIONHANDLER_H_
#define SRC_LIBRARIES_SYSTEM_RUNCALIBRATIONHANDLER_H_

#include <JANA/JObject.h>
#include <JANA/jerror.h>
#include <TT/TranslationTable.h>
#include <system/CalibrationHandler.h>
#include <vector>
#include <map>
#include <algorithm>
#include <string>
using namespace std;

class RunCalibrationHandler: public CalibrationHandlerBase {
public:
	RunCalibrationHandler(string name):CalibrationHandlerBase(name) {};
	virtual jerror_t fillCalib(const std::vector<std::vector<double> > &calib_data);
	vector<double> getCalib();


	double getCalibSingle();



	//	virtual ~CalibrationHandler();
private:

	std::vector<double> m_calib;

};

#endif /* SRC_LIBRARIES_SYSTEM_RUNCALIBRATIONHANDLER_H_ */
