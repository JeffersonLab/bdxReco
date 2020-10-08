/*
 * RunCalibrationHandler.cc
 *
 *  Created on: Oct 8, 2020
 *      Author: celentan
 */

#include "RunCalibrationHandler.h"

jerror_t RunCalibrationHandler::fillCalib(const std::vector<std::vector<double> > &calib_data) {

	if (calib_data.size() != 1) {
		jout << "RunCalibrationHandler:: more than 1 line in the table, that is however run-dependent only" << endl;
		return VALUE_OUT_OF_RANGE;
	}

	m_calib.clear();
	m_rawCalibData.clear();
	m_rawCalibData = calib_data;


	m_calib = m_rawCalibData[0];

	return NOERROR;
}

vector<double> RunCalibrationHandler::getCalib() {
	return m_calib;
}

double RunCalibrationHandler::getCalibSingle() {

	if (m_calib.size() == 1) {
		return m_calib[0];
	} else {
		jerr << "RunCalibrationHandler::getCalibSingle error: more than 1 entry, there are:" << m_calib.size() << std::endl;
		return VALUE_OUT_OF_RANGE;
	}
}
