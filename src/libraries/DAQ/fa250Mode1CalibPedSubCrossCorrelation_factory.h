// $Id$
//
//    File: fa250Mode1CalibPedSubCrossCorrelation_factory.h
// Created: Wed Feb 10 15:01:00 CET 2021
// Creator: mspreafico (on Linux apcx18 3.10.0-514.el7.x86_64 x86_64)
//

#ifndef _fa250Mode1CalibPedSubCrossCorrelation_factory_
#define _fa250Mode1CalibPedSubCrossCorrelation_factory_

#include <JANA/JFactory.h>
#include "fa250Mode1CalibPedSubCrossCorrelation.h"

class TF1;

class fa250Mode1CalibPedSubCrossCorrelation_factory:public jana::JFactory<fa250Mode1CalibPedSubCrossCorrelation>{
	public:
		fa250Mode1CalibPedSubCrossCorrelation_factory(){};
		~fa250Mode1CalibPedSubCrossCorrelation_factory(){};


	private:
		jerror_t init(void);						///< Called once at program start.
		jerror_t brun(jana::JEventLoop *eventLoop, int32_t runnumber);	///< Called everytime a new run number is detected.
		jerror_t evnt(jana::JEventLoop *eventLoop, uint64_t eventnumber);	///< Called every event.
		jerror_t erun(void);						///< Called everytime run number changes, provided brun has been called.
		jerror_t fini(void);						///< Called after last event of last event source has been processed.

		double T1, T2;
		double tao, A;
		TF1 *f1;
		TF1* f2;
		TF1 *f3;

};

#endif // _fa250Mode1CalibPedSubCrossCorrelation_factory_

