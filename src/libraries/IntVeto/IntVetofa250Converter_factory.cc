// $Id$
//
//    File: IntVetofa250Converter_factory.cc
// Created: Mon Jan 25 17:02:27 CET 2016
// Creator: celentan (on Linux apcx4 2.6.32-504.30.3.el6.x86_64 x86_64)
//


#include <iostream>
#include <iomanip>
using namespace std;

#include "IntVetofa250Converter_factory.h"
using namespace jana;

IntVetofa250Converter_factory::IntVetofa250Converter_factory():
				m_isFirstCallToBrun(1),m_intVetofa250Converter(0){
	jout<<"IntVetofa250Converter_factory::creator"<<endl;


	m_minTot=12;
	m_thr=1.5;
	m_NSB=20;
	m_NSA=30;
	m_NPED=10;
	m_RMSTHRscale=5;

	m_TOTTHR=100;

	gPARMS->SetDefaultParameter("INTVETO:MIN_TOT",m_minTot,"Min ToT (in ns) for a pulse to be considered");
	gPARMS->SetDefaultParameter("INTVETO:THR",m_thr,"Min amplitude (in number of phe) for a pulse to be considered");
	gPARMS->SetDefaultParameter("INTVETO:NSB",m_NSB,"Samples before the maximum (in ns) to include in integration");
	gPARMS->SetDefaultParameter("INTVETO:NSA",m_NSA,"Samples after the maximum  (in ns) to include in integration");
	gPARMS->SetDefaultParameter("INTVETO:NPED",m_NPED,"Number of samples (in ns) in pedestal evaluation");
	gPARMS->SetDefaultParameter("INTVETO:RMSSCALE",m_RMSTHRscale,"Scale factor for RMS in ped. subtraction");
	gPARMS->SetDefaultParameter("INTVETO:TOT_THR",m_TOTTHR,"THR for TOT, in mV");
}

//------------------
// init
//------------------
jerror_t IntVetofa250Converter_factory::init(void)
{
	jout<<"IntVetofa250Converter_factory::init"<<endl;
	m_intVetofa250Converter=new IntVetofa250Converter();
	m_intVetofa250Converter->m_thrCalib=new CalibrationHandler<TranslationTable::INT_VETO_Index_t>("/InnerVeto/sipm_ampl");
	this->mapCalibrationHandler(m_intVetofa250Converter->m_thrCalib);
	jout<<"IntVetofa250Converter_factory::init done"<<endl;
	return NOERROR;
}

//------------------
// brun
//------------------
jerror_t IntVetofa250Converter_factory::brun(jana::JEventLoop *eventLoop, int32_t runnumber)
{
	jout<<"IntVetofa250Converter_factory::brun"<<endl;
	this->updateCalibrationHandler(	m_intVetofa250Converter->m_thrCalib,eventLoop);

	gPARMS->GetParameter("INTVETO:VERBOSE",m_intVetofa250Converter->verbose());

	m_intVetofa250Converter->m_minTot=m_minTot;
	m_intVetofa250Converter->m_thr=m_thr;
	m_intVetofa250Converter->m_NSB=m_NSB;
	m_intVetofa250Converter->m_NSA=m_NSA;
	m_intVetofa250Converter->m_NPED=m_NPED;
	m_intVetofa250Converter->m_NPED=m_NPED;
	m_intVetofa250Converter->m_TOTTHR=m_TOTTHR;

	if (m_isFirstCallToBrun){
		_data.push_back(m_intVetofa250Converter);
		m_isFirstCallToBrun=0;
	}
	SetFactoryFlag(PERSISTANT);

	return NOERROR;
}

//------------------
// evnt
//------------------
jerror_t IntVetofa250Converter_factory::evnt(JEventLoop *loop, uint64_t eventnumber)
{

	// Code to generate factory data goes here. Add it like:
	//
	// IntVetofa250Converter *myIntVetofa250Converter = new IntVetofa250Converter;
	// myIntVetofa250Converter->x = x;
	// myIntVetofa250Converter->y = y;
	// ...
	// _data.push_back(myIntVetofa250Converter);
	//
	// Note that the objects you create here will be deleted later
	// by the system and the _data vector will be cleared automatically.

	return NOERROR;
}

//------------------
// erun
//------------------
jerror_t IntVetofa250Converter_factory::erun(void)
{

	this->clearCalibrationHandler(m_intVetofa250Converter->m_thrCalib);
	/*if (m_intVetofa250Converter!=0){
		delete m_intVetofa250Converter;
	}*/
	_data.clear();
	return NOERROR;
}

//------------------
// fini
//------------------
jerror_t IntVetofa250Converter_factory::fini(void)
{
/*	if (m_intVetofa250Converter!=0){
		delete m_intVetofa250Converter;
	}*/
	_data.clear();
	return NOERROR;
}

