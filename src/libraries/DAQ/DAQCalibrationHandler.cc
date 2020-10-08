#include "DAQCalibrationHandler.h"

jerror_t DAQCalibrationHandler::fillCalib(const std::vector<std::vector<double> > &calib_data) {
	TranslationTable::csc_t index;
	vector<double> data;
	int nData = 0;
	int prevNdata = 0;
	m_calib.clear();

	m_rawCalibData.clear();
	m_rawCalibData = calib_data;

	for (int irow = 0; irow < calib_data.size(); irow++) {
		data.clear();
		if (irow > 0) prevNdata = nData;
		nData = calib_data[irow].size() - 3;   //crate, slot channel
		if (nData <= 0) {
			jerr << "Error in DAQCalibrationHandler::fillCalib. No data?" << endl;
			m_calib.clear();
			return VALUE_OUT_OF_RANGE;
		}
		if ((nData != prevNdata) && (irow != 0)) {
			jerr << "Error in CalorimeterCalibration::fillCalib. Error on number of datas?" << endl;
			m_calib.clear();
			return VALUE_OUT_OF_RANGE;
		}
		/*Fill the indexes*/
		index.rocid = calib_data[irow][0];
		index.slot = calib_data[irow][1];
		index.channel = calib_data[irow][2];

		/*Fill the data*/
		for (int idata = 0; idata < nData; idata++) {
			data.push_back(calib_data[irow][idata + 3]);
		}
		m_insert_check = m_calib.insert(std::make_pair(index, data));
		if (m_insert_check.second == false) {
			jout << "DAQCalibrationHandler:: element already exists in the map Doing nothing" << endl;
			jout << "Index print: " << index.print() << endl;

		}
	}
	return NOERROR;
}

std::vector<double> DAQCalibrationHandler::operator[](const TranslationTable::csc_t &index) {
	return this->getCalib(index);
}

vector<double> DAQCalibrationHandler::getCalib(const TranslationTable::csc_t &index) {
	vector<double> ret(0);
	int pos;
	std::vector<TranslationTable::csc_t>::iterator vit;
	std::map<TranslationTable::csc_t, vector<double> >::iterator it;

	vit = find(m_actualCalibIndex.begin(), m_actualCalibIndex.end(), index);

	if (vit != m_actualCalibIndex.end()) {
		pos = vit - m_actualCalibIndex.begin();
		return m_actualCalib[pos];
	} else {
		it = m_calib.find(index);
		if (it == m_calib.end()) {
			jerr << "DAQCalibrationHandler:getCalib element not found. ROC:  " << 1.*index.rocid << " SLOT: " << 1.*index.slot << " CH: " << 1.*index.channel << endl;
			return ret;
		} else {

			m_actualCalibIndex.push_back(index);
			m_actualCalib.push_back(it->second);
			return it->second;
		}
	}

}

double DAQCalibrationHandler::getCalibSingle(const TranslationTable::csc_t &index) {

	vector<double> this_data;
	this_data = getCalib(index);
	if (this_data.size() == 1) {
//		jout<<index.slot<<" "<<index.channel<<" "<<this_data[0]<<endl;
		return this_data[0];
	} else {
		jerr << "DAQCalibrationHandler::getCalibSingle error: more than 1 entry, there are:" << this_data.size() << std::endl;
		return VALUE_OUT_OF_RANGE;
	}
}

