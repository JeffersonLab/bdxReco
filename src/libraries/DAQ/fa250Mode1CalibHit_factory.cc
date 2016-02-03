// $Id$
//
//    File: fa250Mode1CalibHit_factory.cc
// Created: Wed Feb  3 07:38:15 EST 2016
// Creator: davidl (on Darwin harriet.jlab.org 13.4.0 i386)
//


#include <iostream>
#include <iomanip>
using namespace std;

#include "fa250Mode1Hit.h"
#include "fa250Mode1CalibHit_factory.h"
using namespace jana;

//------------------
// init
//------------------
jerror_t fa250Mode1CalibHit_factory::init(void)
{
	return NOERROR;
}

//------------------
// brun
//------------------
jerror_t fa250Mode1CalibHit_factory::brun(jana::JEventLoop *eventLoop, int32_t runnumber)
{
	// Here, we would normally get this from the calibration DB.
	// For now, we hard code it...
	LSB = 0.4884;

	return NOERROR;
}

//------------------
// evnt
//------------------
jerror_t fa250Mode1CalibHit_factory::evnt(JEventLoop *loop, uint64_t eventnumber)
{
	vector<const fa250Mode1Hit*> hits;
	loop->Get(hits);

	for(uint32_t i=0; i<hits.size(); i++){
		
		const fa250Mode1Hit *hit = hits[i];
		
		// Create new fa250Mode1CalibHit
		fa250Mode1CalibHit *calibHit = new fa250Mode1CalibHit;
		
		// Copy the fa250Hit part (crate, slot, channel, ...)
		// doing it this way allow one to modify fa250 later and
		// not have to change this code.
		fa250Hit *a = calibHit;
		const fa250Hit *b = hit;
		*a = *b;
		
		// Copy all samples, applying calibration constant as we go
		for(uint32_t j=0; j<hit->samples.size(); j++){
			double sample = LSB*(double)hit->samples[j];
			calibHit->samples.push_back(sample);
		}
		
		// Add original as associated object 
		calibHit->AddAssociatedObject(hit);
		
		_data.push_back(calibHit);
	}

	return NOERROR;
}

//------------------
// erun
//------------------
jerror_t fa250Mode1CalibHit_factory::erun(void)
{
	return NOERROR;
}

//------------------
// fini
//------------------
jerror_t fa250Mode1CalibHit_factory::fini(void)
{
	return NOERROR;
}

