// $Id$
//
//    File: JFactoryGenerator_PiastraSIPMCalibrationHit.h
// Created: Mon Aug 19 18:11:03 CEST 2019
// Creator: celentan (on Linux apcx4 3.10.0-957.el7.x86_64 x86_64)
//

#ifndef _JFactoryGenerator_PiastraSIPMCalibrationHit_
#define _JFactoryGenerator_PiastraSIPMCalibrationHit_

#include <JANA/jerror.h>
#include <JANA/JFactoryGenerator.h>

#include "../PiastraSIPMCalibrationHit/PiastraSIPMCalibrationHit_factory.h"

class JFactoryGenerator_PiastraSIPMCalibrationHit: public jana::JFactoryGenerator{
	public:
		JFactoryGenerator_PiastraSIPMCalibrationHit(){}
		virtual ~JFactoryGenerator_PiastraSIPMCalibrationHit(){}
		virtual const char* className(void){return static_className();}
		static const char* static_className(void){return "JFactoryGenerator_PiastraSIPMCalibrationHit";}
		
		jerror_t GenerateFactories(jana::JEventLoop *loop){
			loop->AddFactory(new PiastraSIPMCalibrationHit_factory());
			return NOERROR;
		}

};

#endif // _JFactoryGenerator_PiastraSIPMCalibrationHit_

