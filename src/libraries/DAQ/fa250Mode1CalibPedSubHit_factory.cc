// $Id$
//
//    File: fa250Mode1PedSubHit_factory.cc
// Created: Wed Feb  3 07:38:15 EST 2016
// Creator: davidl (on Darwin harriet.jlab.org 13.4.0 i386)
//

#include <iostream>
#include <iomanip>
using namespace std;

#include "fa250Mode1CalibPedSubHit_factory.h"
#include <TT/TranslationTable.h>
using namespace jana;

//------------------
// init
//------------------
jerror_t fa250Mode1CalibPedSubHit_factory::init(void) {
	m_pedestals = new DAQCalibrationHandler("/DAQ/pedestals");
	this->mapCalibrationHandler(m_pedestals);

	m_parms = new DAQCalibrationHandler("DAQ/parms");
	this->mapCalibrationHandler(m_parms);

	return NOERROR;
}

//------------------
// brun
//------------------
jerror_t fa250Mode1CalibPedSubHit_factory::brun(jana::JEventLoop *eventLoop, int32_t runnumber) {
	// Here, we would normally get this from the CalibPedSubration DB.
	LSB = 0.4884; //this is in any case the default

	this->updateCalibrationHandler(m_pedestals, eventLoop);
	this->updateCalibrationHandler(m_parms, eventLoop);

	return NOERROR;
}

//------------------
// evnt
//------------------
jerror_t fa250Mode1CalibPedSubHit_factory::evnt(JEventLoop *loop, uint64_t eventnumber) {

	vector<const fa250Mode1Hit*> hits;
	vector<const fa250WaveboardV1Hit*> wbhitsV1;

	vector<double> DAQdata, PARMSdata;
	double pedestal, RMS;
	double sample = 0;

	TranslationTable::csc_t index;

	//First get and process fa250Mode1Hit from JLab FADC
	loop->Get(hits);

	for (uint32_t i = 0; i < hits.size(); i++) {

		const fa250Mode1Hit *hit = hits[i];

		// Create new fa250Mode1PedSubHit
		fa250Mode1CalibPedSubHit *CalibPedSubHit = new fa250Mode1CalibPedSubHit;

		// Copy the fa250Hit part (crate, slot, channel, ...)
		// doing it this way allow one to modify fa250 later and
		// not have to change this code.
		fa250Hit *a = CalibPedSubHit;
		const fa250Hit *b = hit;
		*a = *b;

		// Copy all samples, applying PedSubration constant as we go
		DAQdata = m_pedestals->getCalib(hit->m_channel);
		pedestal = DAQdata[0];
		RMS = DAQdata[1];

		PARMSdata = m_parms->getCalib(hit->m_channel);
		LSB = PARMSdata[0];
		dT=0;
		if (PARMSdata.size()>=2){
			dT = PARMSdata[1];
		}

		if (dT==0) dT=4; //retro-compatibility

		for (uint32_t j = 0; j < hit->samples.size(); j++) {  //j=0
			sample = (double) hit->samples[j]; //get the sample
			sample = sample - pedestal; //subtract the pedestal (in FADC units)
			sample = sample * LSB; //convert to mV

			CalibPedSubHit->samples.push_back(sample);
		}
		CalibPedSubHit->m_dT = dT;
		CalibPedSubHit->m_ped = pedestal * LSB;
		CalibPedSubHit->m_RMS = fabs(RMS * LSB); //a.c. there are cases (v1725) where LSB is < 0, but RMS is >0!
		// Add original as associated object 
		CalibPedSubHit->AddAssociatedObject(hit);
		_data.push_back(CalibPedSubHit);
	}

	//Then get fa250Hit from waveboard V1
	loop->Get(wbhitsV1);
	for (uint32_t i = 0; i < wbhitsV1.size(); i++) {

		const fa250WaveboardV1Hit *hit = wbhitsV1[i];

		// Create new fa250Mode1PedSubHit
		fa250Mode1CalibPedSubHit *CalibPedSubHit = new fa250Mode1CalibPedSubHit;

		// Copy the fa250Hit part (crate, slot, channel, ...)
		// doing it this way allow one to modify fa250 later and
		// not have to change this code.
		fa250Hit *a = CalibPedSubHit;
		const fa250Hit *b = hit;
		*a = *b;

		// Copy all samples, applying PedSubration constant as we go
		DAQdata = m_pedestals->getCalib(hit->m_channel);
		pedestal = DAQdata[0];
		RMS = DAQdata[1];

		PARMSdata = m_parms->getCalib(hit->m_channel);
		LSB = PARMSdata[0];
		dT = PARMSdata[1];

		for (uint32_t j = 0; j < hit->samples.size(); j++) {  //j=0
			sample = (double) hit->samples[j]; //get the sample
			sample = sample - pedestal; //subtract the pedestal (in FADC units)
			sample = sample * LSB; //convert to mV

			CalibPedSubHit->samples.push_back(sample);
		}
		CalibPedSubHit->m_dT = dT;
		CalibPedSubHit->m_ped = pedestal * LSB;
		CalibPedSubHit->m_RMS = fabs(RMS * LSB);
		// Add original as associated object
		CalibPedSubHit->AddAssociatedObject(hit);
		_data.push_back(CalibPedSubHit);
	}

	return NOERROR;
}

//------------------
// erun
//------------------
jerror_t fa250Mode1CalibPedSubHit_factory::erun(void) {

	this->clearCalibrationHandler(m_pedestals);

	return NOERROR;
}

//------------------
// fini
//------------------
jerror_t fa250Mode1CalibPedSubHit_factory::fini(void) {

	return NOERROR;
}

