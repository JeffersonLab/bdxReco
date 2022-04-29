// $Id$
//
//    File: JFactoryGenerator_VETO_INT.h
// Created: Tue Jan 12 11:52:51 CET 2016
// Creator: celentan (on Linux apcx4 2.6.32-504.30.3.el6.x86_64 x86_64)
//

#ifndef _JFactoryGenerator_CALORIMETER_
#define _JFactoryGenerator_CALORIMETER_

#include <JANA/jerror.h>
#include <JANA/JFactoryGenerator.h>

#include "CalorimeterDigiHit_factory.h"
#include "CalorimeterDigiHit_factory_MC.h"
#include "CalorimeterHit_factory.h"
#include "CalorimeterCluster_factory.h"
#include "CalorimeterMCRealHit_factory.h"
#include "CalorimeterRawDataHit_factory.h"


class JFactoryGenerator_Calorimeter: public jana::JFactoryGenerator{
	public:
		JFactoryGenerator_Calorimeter(){
			VERBOSE=0;
			gPARMS->SetDefaultParameter("CALORIMETER:VERBOSE", VERBOSE,
						"Verbosity level for applying messages from CALORIMETER library."
						" 0=no messages, 10=all messages.");

		}
		virtual ~JFactoryGenerator_Calorimeter(){}
		virtual const char* className(void){return static_className();}
		static const char* static_className(void){return "JFactoryGenerator_CALORIMETER";}
		
		jerror_t GenerateFactories(jana::JEventLoop *loop){
			loop->AddFactory(new CalorimeterRawDataHit_factory());
			loop->AddFactory(new CalorimeterDigiHit_factory());
			loop->AddFactory(new CalorimeterDigiHit_factory_MC());
			loop->AddFactory(new CalorimeterHit_factory());
			loop->AddFactory(new CalorimeterMCRealHit_factory());
			loop->AddFactory(new CalorimeterCluster_factory());
			return NOERROR;

		}

	private:
		int VERBOSE;
};




#endif // _JFactoryGenerator_VETO_INT_
