#include "IntVetofa250Converter.h"

#include <DAQ/fa250Mode1CalibPedSubHit.h>
#include <DAQ/fa250Mode7Hit.h>

#include "TFile.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TApplication.h"

IntVetoSiPMHit* IntVetofa250Converter::convertHit(const fa250Hit *hit, const TranslationTable::ChannelInfo &m_channel, int eventN) const {
	IntVetoSiPMHit *m_IntVetoSiPMHit = new IntVetoSiPMHit;
	m_IntVetoSiPMHit->m_channel = m_channel;

	jerror_t ret;

	if (strcmp(hit->className(), "fa250Mode1CalibPedSubHit") == 0) {
		ret = this->convertMode1Hit(m_IntVetoSiPMHit, (const fa250Mode1CalibPedSubHit*) hit, eventN);
	} else if (strcmp(hit->className(), "fa250Mode7Hit") == 0) {
		ret = this->convertMode7Hit(m_IntVetoSiPMHit, (const fa250Mode7Hit*) hit, eventN);
	} else {
		jerr << "IntVetofa250Converter::convertHit unsupported class name: " << hit->className() << std::endl;
		return 0;
	}
	if (ret == NOERROR) {
		return m_IntVetoSiPMHit;
	} else {
		return 0;
	}
}

jerror_t IntVetofa250Converter::convertMode1Hit(IntVetoSiPMHit *output, const fa250Mode1CalibPedSubHit *input, int eventN) const {
	int size = input->samples.size();

	// Copy the fa250Hit part (crate, slot, channel, ...)
	// doing it this way allow one to modify fa250 later and
	// not have to change this code.

	int N, n, idx, imax;
	int istart, istartmin, icheck;
	double min, max, xmin, xmax, prev_xmin, prev_xmax, rms, Tmax;
	int ixmin, ixmax;
	double ped, pedRMS, thr, pedRMSmin, pedmin;
	bool found;
	std::pair<int, int> m_thisCrossingTime;

	std::vector<std::pair<int, int> > m_crossingTimes;
	std::vector<int> m_crossingTimesDelta;

	std::vector<int> m_singleCrossingIndexes;
	//std::vector<int> m_signalCrossingIndexs;

	double m_NPEDs, m_NSBs, m_NSAs;

	m_NPEDs = (int) (m_NPED / input->m_dT);
	m_NSAs = (int) (m_NSA / input->m_dT);
	m_NSBs = (int) (m_NSB / input->m_dT);

	vector<double> m_waveform;
	m_waveform.clear();

	output->AddAssociatedObject(input);
	output->nSingles = 0;
	output->average = 0;

	ped = 0;
	pedRMS = 0;

	if (size < m_NPEDs) {
		return OBJECT_NOT_AVAILABLE;
	}

	for (int ii = 0; ii < m_NPEDs; ii++) {
		ped += input->samples[ii];
		pedRMS += input->samples[ii] * input->samples[ii];

	}
	ped /= m_NPEDs;
	pedRMS /= m_NPEDs;
	pedRMS = sqrt(pedRMS - ped * ped);

	for (int ii = 0; ii < size; ii++) {
		m_waveform.push_back(input->samples[ii] - ped);
	}

	output->pedMean = ped;
	output->pedRMS = pedRMS;
	output->RMSflag = found;

	//1: compute the average
	for (int ii = 0; ii < size; ii++) {
		output->average += m_waveform[ii];
	}
	output->average /= m_waveform.size();

	thr = m_thrCalib->getCalibSingle(*output->m_channel.int_veto); //this is the 1 phe ampl FROM DB

	thr = thr * m_thr;   //put a very low thr at this level

	//2: find thr crossings
	m_thisCrossingTime.first = -1;
	m_thisCrossingTime.second = -1;
	if (m_waveform[0] > thr) m_thisCrossingTime.first = 0;

	for (int ii = 1; ii < size; ii++) {

		if ((m_waveform[ii] > thr) && (m_waveform[ii - 1] < thr)) m_thisCrossingTime.first = ii;

		else if ((m_waveform[ii] < thr) && (m_waveform[ii - 1] > thr) && (m_thisCrossingTime.first != -1)) {

			m_thisCrossingTime.second = ii;
			m_crossingTimes.push_back(m_thisCrossingTime);
			m_thisCrossingTime.first = -1;
			m_thisCrossingTime.second = -1;

		}

	}
	//It may happen that the last sample is still over thr!
	if (m_waveform[size - 1] > thr) {
		m_thisCrossingTime.second = size;
		m_crossingTimes.push_back(m_thisCrossingTime);
	}

	//3: deltas
	//3a: if no crossing times are found
	/*Compute ToT */
	for (int itime = 0; itime < m_crossingTimes.size(); itime++) {
		m_crossingTimesDelta.push_back(m_crossingTimes[itime].second - m_crossingTimes[itime].first);
	}

	/*Verify the ToT for each pulse*/
	for (int itime = 0; itime < m_crossingTimes.size(); itime++) {
		if (m_crossingTimesDelta[itime] < 0) {
			jerr << "IntVetofa20Converter::convertMode1Hit error, negative ToT?" << std::endl;
		} else if ((m_crossingTimesDelta[itime] > (m_minTot / input->m_dT)) || (m_crossingTimes[itime].second) == (size) || (m_crossingTimes.at(itime).first) == (0)) {
			m_singleCrossingIndexes.push_back(itime);
		}
	}
	output->nSingles = m_singleCrossingIndexes.size();
	if ((output->nSingles) == 0) {
		output->m_type = IntVetoSiPMHit::noise;
		output->Qraw = this->sumSamples((m_NSAs + m_NSBs > m_waveform.size()) ? m_waveform.size() : m_NSAs + m_NSBs, &(m_waveform[0]));  //for compatibility with case 1
		output->Araw = this->getMaximum(m_waveform.size(), &(m_waveform[0]), output->T);
		output->T = -1;
		return NOERROR;
	}

	else {
		output->m_type = IntVetoSiPMHit::real_signal;
		output->Araw = -9999;
		/*Loop over the thr crossings and select the highest pulse*/
		for (int iphe = 0; iphe < output->nSingles; iphe++) {
			idx = m_singleCrossingIndexes.at(iphe);
			xmin = m_crossingTimes.at(idx).first;
			xmax = m_crossingTimes.at(idx).second;
			ixmin = (int) xmin;
			ixmax = (int) xmax;
			if (ixmin < 0) ixmin = 0;
			if (ixmax >= m_waveform.size()) ixmax = m_waveform.size() - 1;

			max = this->getMaximum(ixmin, ixmax, &(m_waveform[0]), Tmax);
			if ((output->Araw) < max) {
				output->Araw = max;
				imax = idx;
			}
		}

		xmin = m_crossingTimes[imax].first;
		xmax = m_crossingTimes[imax].second;

		ixmin = (int) xmin;
		ixmax = (int) xmax;

		if (ixmin < 0) ixmin = 0;
		if (ixmax >= m_waveform.size()) ixmax = m_waveform.size() - 1;

		max = this->getMaximum(ixmin, ixmax, &(m_waveform[0]), Tmax);

		xmin = Tmax - m_NSAs;
		if (xmin < 0) xmin = 0;
		xmax = Tmax + m_NSBs;
		if (xmax >= size) (xmax = size - 1);
		output->Qraw = this->sumSamples(xmin, xmax, &(m_waveform[0]));

		/*Now do timing. Keep it simple: 4 ns resolution is enough for what we need*/
		max = this->getMaximum(m_waveform.size(), &(m_waveform[0]), Tmax);
		output->T = Tmax;
		output->T *= input->m_dT; //in NS!!!

		return NOERROR;
	}

	return NOERROR;
}

jerror_t IntVetofa250Converter::convertMode7Hit(IntVetoSiPMHit *output, const fa250Mode7Hit *input, int eventN) const {
	output->AddAssociatedObject(input);

	return NOERROR;
}
