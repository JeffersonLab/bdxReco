// $Id$
//
//    File: VetoIntDigiHit_factory.cc
// Created: Tue Jan 12 11:52:41 CET 2016
// Creator: celentan (on Linux apcx4 2.6.32-504.30.3.el6.x86_64 x86_64)
//

#include <iostream>
#include <iomanip>
using namespace std;

//objects we need from the framework
#include <DAQ/fa250Mode1CalibPedSubHit.h>
#include <DAQ/fa250Mode7Hit.h>
#include <TT/TranslationTable.h>
//objects we put in the framework
#include <IntVeto/IntVetoSiPMHit.h>
#include <IntVeto/IntVetofa250Converter.h>

#include "IntVetoSiPMHit_factory.h"
using namespace jana;

IntVetoSiPMHit_factory::IntVetoSiPMHit_factory() :
		m_tt(0) {
	m_sipm_gain = 0;
	m_intVetofa250Converter = 0;
	VERBOSE = 0;
}

//------------------
// init
//------------------
jerror_t IntVetoSiPMHit_factory::init(void) {
	jout << "IntVetoSiPMHit_factory::init" << endl;
	VERBOSE = 0;
	m_sipm_gain = new CalibrationHandler<TranslationTable::INT_VETO_Index_t>("/InnerVeto/sipm_gain");
	this->mapCalibrationHandler(m_sipm_gain);

	m_sipm_ampl = new CalibrationHandler<TranslationTable::INT_VETO_Index_t>("/InnerVeto/sipm_ampl");
	this->mapCalibrationHandler(m_sipm_ampl);

	jout << "IntVetoSiPMHit_factory::init done" << endl;
	return NOERROR;

}

//------------------
// brun
//------------------
jerror_t IntVetoSiPMHit_factory::brun(jana::JEventLoop *eventLoop, int32_t runnumber) {
	jout << "IntVetoSiPMHit_factory::brun new run number: " << runnumber << endl;
	m_tt = 0;
	eventLoop->GetSingle(m_tt);
	if (m_tt == 0) {
		jerr << " unable to get the TranslationTable from this run!" << endl;
		return OBJECT_NOT_AVAILABLE;
	}

	m_intVetofa250Converter = 0;
	eventLoop->GetSingle(m_intVetofa250Converter);
	if (m_intVetofa250Converter == 0) {
		jerr << " unable to get the intVetofa250Converter!" << endl;
		return OBJECT_NOT_AVAILABLE;
	}

	this->updateCalibrationHandler(m_sipm_gain, eventLoop);
	this->updateCalibrationHandler(m_sipm_ampl, eventLoop);

	gPARMS->GetParameter("INTVETO:VERBOSE", VERBOSE);
	if (VERBOSE > 3) {
		std::map<TranslationTable::INT_VETO_Index_t, std::vector<double> > gainCalibMap;
		std::map<TranslationTable::INT_VETO_Index_t, std::vector<double> >::iterator gainCalibMap_it;
		gainCalibMap = m_sipm_gain->getCalibMap();
		jout << "Got following sipm_gain for run number: " << runnumber << endl;
		jout << "Rows: " << gainCalibMap.size() << endl;
		for (gainCalibMap_it = gainCalibMap.begin(); gainCalibMap_it != gainCalibMap.end(); gainCalibMap_it++) {
			jout << "sector: " << 1. * gainCalibMap_it->first.sector << " layer: " << 1. * gainCalibMap_it->first.layer << " component: " << 1. * gainCalibMap_it->first.component << " readout: " << 1. * gainCalibMap_it->first.readout << " gain: " << gainCalibMap_it->second.at(0)
					<< endl;
		}

		gainCalibMap = m_sipm_ampl->getCalibMap();
		jout << "Got following sipm_ampl for run number: " << runnumber << endl;
		jout << "Rows: " << gainCalibMap.size() << endl;
		for (gainCalibMap_it = gainCalibMap.begin(); gainCalibMap_it != gainCalibMap.end(); gainCalibMap_it++) {
			jout << "sector: " << 1. * gainCalibMap_it->first.sector << " layer: " << 1. * gainCalibMap_it->first.layer << " component: " << 1. * gainCalibMap_it->first.component << " readout: " << 1. * gainCalibMap_it->first.readout << " ampl: " << gainCalibMap_it->second.at(0)
					<< endl;
		}

	}
	jout << "IntVetoSiPMHit_factory::brun done" << endl;
	return NOERROR;
}

//------------------
// evnt
//------------------
jerror_t IntVetoSiPMHit_factory::evnt(JEventLoop *loop, uint64_t eventnumber) {
	TranslationTable::ChannelInfo m_channel;
	TranslationTable::csc_t m_csc;
	double m_q_gain;
	double m_q_ped;

	double m_ampl;

	IntVetoSiPMHit *m_IntVetoSiPMHit = 0;

	//1: Here, we get from the framework the objects we need to process
	//1a: create vectors
	vector<const fa250Mode1CalibPedSubHit*> m_fa250Mode1CalibPedSubHit;
	vector<const fa250Mode7Hit*> m_fa250Mode7Hit;
	vector<const fa250Mode1CalibPedSubHit*>::const_iterator it_fa250Mode1CalibPedSubHit;
	vector<const fa250Mode7Hit*>::const_iterator it_fa250Mode7Hit;

	//1b: retrieve objects
	loop->Get(m_fa250Mode1CalibPedSubHit);
	loop->Get(m_fa250Mode7Hit);

	/*2: Now we have the daq objects, still indexed as "crate-slot-channel"
	 *	 Use the translation table to produce the digitized hit of the inner veto
	 *	 Note that we can produce a single object type here, i.e. VetoIntDigiHit,
	 *	 but we have 2 possible source, mode 1 and mode 7.
	 *	 Therefore, we will use the TranslationTable ONLY to check if this crate-slot-channel
	 *	 combination refers to a InnerVeto hit and, in case, to determine which one, i.e. which ID in the InnerVeto scheme.
	 *	 Then, we will proceed in two different ways.
	 */

	/*First, mode 1*/
	/*Note that in this case we have to integrate the pulse - it is a mode 1 pulse! */
	for (it_fa250Mode1CalibPedSubHit = m_fa250Mode1CalibPedSubHit.begin(); it_fa250Mode1CalibPedSubHit != m_fa250Mode1CalibPedSubHit.end(); it_fa250Mode1CalibPedSubHit++) {

		m_channel = m_tt->getChannelInfo((*it_fa250Mode1CalibPedSubHit)->m_channel);

		if (m_channel.det_sys == TranslationTable::INT_VETO) {
			//A.C. do not touch these

			m_IntVetoSiPMHit = m_intVetofa250Converter->convertHit((fa250Hit*) *it_fa250Mode1CalibPedSubHit, m_channel, eventnumber);
			if (m_IntVetoSiPMHit == 0){
				continue;
			}
			/*Apply phe conversion if possible*/
			m_q_gain = m_sipm_gain->getCalib(*m_channel.int_veto)[0];
			m_q_ped = m_sipm_gain->getCalib(*m_channel.int_veto)[1];

			m_IntVetoSiPMHit->Qphe = m_IntVetoSiPMHit->Qraw - m_q_ped;
			if (m_q_gain > 0) {
				m_IntVetoSiPMHit->Qphe /= m_q_gain;
			}
			m_IntVetoSiPMHit->Aphe = m_IntVetoSiPMHit->Araw;
			m_ampl = m_sipm_ampl->getCalib(*m_channel.int_veto)[0];
			if (m_ampl > 0){
				m_IntVetoSiPMHit->Aphe /=m_ampl;
			}

			_data.push_back(m_IntVetoSiPMHit);
		}
	}

	/*Then, mode 7*/
	/*Note that in this case we do not have to integrate the pulse - it is a mode 7 pulse! */
	for (it_fa250Mode7Hit = m_fa250Mode7Hit.begin(); it_fa250Mode7Hit != m_fa250Mode7Hit.end(); it_fa250Mode7Hit++) {
		m_channel = m_tt->getChannelInfo((*it_fa250Mode7Hit)->m_channel);
		if (m_channel.det_sys == TranslationTable::INT_VETO) {
			//A.C. do not touch these
			m_IntVetoSiPMHit = m_intVetofa250Converter->convertHit((fa250Hit*) *it_fa250Mode7Hit, m_channel, eventnumber);

			/*Apply phe conversion if possible*/
			m_q_gain = m_sipm_gain->getCalib(*m_channel.int_veto)[0];
			m_q_ped = m_sipm_gain->getCalib(*m_channel.int_veto)[1];

			m_IntVetoSiPMHit->Qphe = m_IntVetoSiPMHit->Qraw - m_q_ped;
			if (m_q_gain > 0) {
				m_IntVetoSiPMHit->Qphe /= m_q_gain;
			}

			_data.push_back(m_IntVetoSiPMHit);
		}
	}

	return NOERROR;
}

//------------------
// erun
//------------------
jerror_t IntVetoSiPMHit_factory::erun(void) {
	this->clearCalibrationHandler(m_sipm_gain);
	return NOERROR;
}

//------------------
// fini
//------------------
jerror_t IntVetoSiPMHit_factory::fini(void) {
	return NOERROR;
}

