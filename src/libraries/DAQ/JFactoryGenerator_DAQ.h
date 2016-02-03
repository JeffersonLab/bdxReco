// $Id$
//
//    File: JFactoryGenerator_DAQ.h
// Created: Tue Jan 12 11:28:22 CET 2016
// Creator: celentan (on Linux apcx4 2.6.32-504.30.3.el6.x86_64 x86_64)
//

#ifndef _JFactoryGenerator_DAQ_
#define _JFactoryGenerator_DAQ_

#include <JANA/jerror.h>
#include <JANA/JFactoryGenerator.h>

#include "fa250Mode1Hit.h"
#include "fa250Mode7Hit.h"
#include "triggerData.h"
#include "fa250Mode1CalibHit_factory.h"

class JFactoryGenerator_DAQ: public jana::JFactoryGenerator{
	public:
		JFactoryGenerator_DAQ(){}
		virtual ~JFactoryGenerator_DAQ(){}
		virtual const char* className(void){return static_className();}
		static const char* static_className(void){return "JFactoryGenerator_DAQ";}
		
		jerror_t GenerateFactories(jana::JEventLoop *loop){
			loop->AddFactory(new JFactory<fa250Mode1Hit>()    );
			loop->AddFactory(new JFactory<fa250Mode7Hit>()    );
			loop->AddFactory(new JFactory<triggerData  >()    );
			loop->AddFactory(new fa250Mode1CalibHit_factory() );
			return NOERROR;
		}

};

#endif // _JFactoryGenerator_DAQ_

