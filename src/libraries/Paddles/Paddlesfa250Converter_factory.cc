// $Id$
//
//    File: ExtVetofa250Converter_factory.cc
// Created: Mon Jan 25 16:28:59 CET 2016
// Creator: celentan (on Linux apcx4 2.6.32-504.30.3.el6.x86_64 x86_64)
//


#include <iostream>
#include <iomanip>
using namespace std;

#include <Paddles/Paddlesfa250Converter_factory.h>
#include <system/CalibrationHandler.h>
using namespace jana;

//------------------
// init
//------------------
jerror_t Paddlesfa250Converter_factory::init(void)
{
	return NOERROR;
}

//------------------
// brun
//------------------
jerror_t Paddlesfa250Converter_factory::brun(jana::JEventLoop *eventLoop, int32_t runnumber)
{
	if (m_isFirstCallToBrun){
			m_isFirstCallToBrun=0;
			m_Paddlesfa250Converter=new Paddlesfa250Converter();

				/***** Reading Thresholds from CCDB *******/
				m_Paddlesfa250Converter->threshold=new CalibrationHandler<TranslationTable::PADDLES_Index_t>;
				vector<vector < double> > m_rawthreshold;
				eventLoop->GetCalib("/Paddles/Threshold", m_rawthreshold);
				m_Paddlesfa250Converter->threshold->fillCalib(m_rawthreshold);


//				/***** Reading Pedestals from CCDB *******/
//				m_Paddlesfa250Converter->pedestal_init=new CalibrationHandler<TranslationTable::PADDLES_Index_t>;
//				vector<vector < double> > m_rawpedestal_init;
//				eventLoop->GetCalib("/Paddles/Pedestal", m_rawpedestal_init);
//				m_Paddlesfa250Converter->pedestal_init->fillCalib(m_rawpedestal_init);

				/***** Reading Pedestals from CCDB - DAQ database *******/
				m_Paddlesfa250Converter->m_pedestals=new DAQCalibrationHandler();
				eventLoop->GetCalib("/DAQ/pedestals",m_Paddlesfa250Converter->m_rawpedestal);
				m_Paddlesfa250Converter->m_pedestals->fillCalib(m_Paddlesfa250Converter->m_rawpedestal);

/*
				for (int u = 0; u < m_Paddlesfa250Converter->m_rawpedestal.size(); u++) {
					for (int  v = 0; v < m_Paddlesfa250Converter->m_rawpedestal[u].size(); v++) {
						cout << u<< " "<<v<<" "<<m_Paddlesfa250Converter->m_rawpedestal[u][v] << " "<<cout << endl;
					}
				}
*/
			_data.push_back(m_Paddlesfa250Converter);
			SetFactoryFlag(PERSISTANT);
	}





	return NOERROR;
}

//------------------
// evnt
//------------------
jerror_t Paddlesfa250Converter_factory::evnt(JEventLoop *loop, uint64_t eventnumber)
{

	// Code to generate factory data goes here. Add it like:
	//
	// ExtVetofa250Converter *myExtVetofa250Converter = new ExtVetofa250Converter;
	// myExtVetofa250Converter->x = x;
	// myExtVetofa250Converter->y = y;
	// ...
	// _data.push_back(myExtVetofa250Converter);
	//
	// Note that the objects you create here will be deleted later
	// by the system and the _data vector will be cleared automatically.

	return NOERROR;
}

//------------------
// erun
//------------------
jerror_t Paddlesfa250Converter_factory::erun(void)
{
	return NOERROR;
}

//------------------
// fini
//------------------
jerror_t Paddlesfa250Converter_factory::fini(void)
{
	if (m_Paddlesfa250Converter){
		delete m_Paddlesfa250Converter;
	}
	_data.clear();
	return NOERROR;
}
