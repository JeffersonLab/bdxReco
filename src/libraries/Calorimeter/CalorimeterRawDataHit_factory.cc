// $Id$
//
//    File: CalorimeterRawDataHit_factory.cc
// Created: Wed Jan 27 16:39:49 CET 2016
// Creator: celentan (on Linux apcx4 2.6.32-504.30.3.el6.x86_64 x86_64)
//

#include <iostream>
#include <iomanip>
using namespace std;

//objects we need from the framework
#include <DAQ/fa250Mode1CalibPedSubHit.h>
#include <DAQ/fa250WaveboardV1Hit.h>
#include <TT/TranslationTable.h>
//objects we put in the framework
#include "CalorimeterRawDataHit.h"


#include "CalorimeterRawDataHit_factory.h"
using namespace jana;

//------------------
// init
//------------------
jerror_t CalorimeterRawDataHit_factory::init(void) {
	VERBOSE = 0;
	gPARMS->GetParameter("CALORIMETER:VERBOSE", VERBOSE);

	m_wb_gain = new CalibrationHandler<TranslationTable::CALO_Index_t>("/Calorimeter/wb_gain");
	this->mapCalibrationHandler(m_wb_gain);


	//Params
	m_ped=20;
	m_first=40;
	m_last=200;
	gPARMS->SetDefaultParameter("CALORIMETER:NPED",m_ped,"num of samples in ped calc.");
	gPARMS->SetDefaultParameter("CALORIMETER:FIRST",m_first,"first sample in int. window");
	gPARMS->SetDefaultParameter("CALORIMETER:LAST",m_last,"last sample in int. window");




	return NOERROR;
}

//------------------
// brun
//------------------
jerror_t CalorimeterRawDataHit_factory::brun(jana::JEventLoop *eventLoop, int32_t runnumber) {

	jout << "CalorimeterRawDataHit_factory::brun new run number: " << runnumber << endl;
	m_tt = 0;
	eventLoop->GetSingle(m_tt);
	if (m_tt == 0) {
		jerr << " unable to get the TranslationTable from this run!" << endl;
		return OBJECT_NOT_AVAILABLE;
	}



	this->updateCalibrationHandler(m_wb_gain, eventLoop);

	if (VERBOSE > 3) {
		std::map<TranslationTable::CALO_Index_t, std::vector<double> > gainCalibMap;
		std::map<TranslationTable::CALO_Index_t, std::vector<double> >::iterator gainCalibMap_it;
		gainCalibMap = m_wb_gain->getCalibMap();
		jout << "Got following wb_gain for run number: " << runnumber << endl;
		jout << "Rows: " << gainCalibMap.size() << endl;
		for (gainCalibMap_it = gainCalibMap.begin(); gainCalibMap_it != gainCalibMap.end(); gainCalibMap_it++) {
			jout << gainCalibMap_it->first.sector << " " << gainCalibMap_it->first.x << " " << gainCalibMap_it->first.y << " " << gainCalibMap_it->first.readout << " " << gainCalibMap_it->second.at(0) << endl;
		}
	}

	jout << "CalorimeterRawDataHit_factory::brun done" << endl;
	return NOERROR;
}

//------------------
// evnt
//------------------
jerror_t CalorimeterRawDataHit_factory::evnt(JEventLoop *loop, uint64_t eventnumber) {

	TranslationTable::ChannelInfo m_channel;
	TranslationTable::csc_t m_csc;
	double m_wb_calib;
	CalorimeterRawDataHit *m_CalorimeterRawDataHit = 0;

	//1: Here, we get from the framework the objects we need to process
	//1a: create vectors
	vector<const fa250Mode1CalibPedSubHit*> m_fa250Mode1CalibPedSubHit;

	vector<const fa250Mode1CalibPedSubHit*>::const_iterator it_fa250Mode1CalibPedSubHit;


	//1b: retrieve objects
	loop->Get(m_fa250Mode1CalibPedSubHit);


	/*2: Now we have the daq objects, still indexed as "crate-slot-channel"
	 *	 Use the translation table to produce the digitized hit of the inner veto
	 *	 Note that we can produce a single object type here, i.e. VetoIntDigiHit,
	 *	 but we have 2 possible source, mode 1 and mode 7.
	 *	 Therefore, we will use the TranslationTable ONLY to check if this crate-slot-channel
	 *	 combination refers to a Calorimeter hit and, in case, to determine which one, i.e. which ID in the InnerVeto scheme.
	 *	 Then, we will proceed in two different ways.
	 */

	/*First, mode 1*/
	/*Note that in this case we have to integrate the pulse - it is a mode 1 pulse! */
	for (it_fa250Mode1CalibPedSubHit = m_fa250Mode1CalibPedSubHit.begin(); it_fa250Mode1CalibPedSubHit != m_fa250Mode1CalibPedSubHit.end(); it_fa250Mode1CalibPedSubHit++) {

		const fa250Mode1CalibPedSubHit *hit=(*it_fa250Mode1CalibPedSubHit);
		m_channel = m_tt->getChannelInfo(hit->m_channel);



		if ((m_channel.det_sys == TranslationTable::CALORIMETER) && (m_channel.calorimeter->readout <= 2)) {
			m_CalorimeterRawDataHit = new CalorimeterRawDataHit();
			m_CalorimeterRawDataHit->m_channel=m_channel;

			//Here, do the integration

			double ped=0;
			double Q=0;
			double A=-9999;
			double T=0;

			for (int jj=0;jj<hit->samples.size();jj++){
				if (jj<m_ped) ped+=hit->samples[jj];
				if ((jj>=m_first)&&(jj<=m_last)){
				    Q+=hit->samples[jj];
				    if (hit->samples[jj]>A){
				        A=hit->samples[jj];
				        T=jj*hit->m_dT;
				    }
				}
			}
			Q=Q-ped*(m_last-m_first+1)/(m_ped);


			m_CalorimeterRawDataHit->pedMean=ped/(m_ped);
			m_CalorimeterRawDataHit->Qraw=Q;

			m_CalorimeterRawDataHit->A=A;
            m_CalorimeterRawDataHit->T=T;



			/*Apply a scaling factor if possible*/
			m_wb_calib = m_wb_gain->getCalibSingle(*(m_channel.calorimeter));
			if (m_wb_calib > 0) {
				m_CalorimeterRawDataHit->Q = m_CalorimeterRawDataHit->Qraw * m_wb_calib;
			}

			_data.push_back(m_CalorimeterRawDataHit);
		}
	}




	return NOERROR;
}

//------------------
// erun
//------------------
jerror_t CalorimeterRawDataHit_factory::erun(void) {

	this->clearCalibrationHandler(m_wb_gain);

	return NOERROR;
}

//------------------
// fini
//------------------
jerror_t CalorimeterRawDataHit_factory::fini(void) {
	return NOERROR;
}

