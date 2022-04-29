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


#include "eventData.h"
#include "epicsRawData.h"

#include "fa250Mode1Hit.h"
#include "fa250WaveboardV1Hit.h"

#include "fa250Mode1CalibPedSubHit_factory.h"

#include "fa250ConvertedHit.h"

class JFactoryGenerator_DAQ: public jana::JFactoryGenerator {
public:
	JFactoryGenerator_DAQ() {
	}
	virtual ~JFactoryGenerator_DAQ() {
	}
	virtual const char* className(void) {
		return static_className();
	}
	static const char* static_className(void) {
		return "JFactoryGenerator_DAQ";
	}

	jerror_t GenerateFactories(jana::JEventLoop *loop) {
		loop->AddFactory(new JFactory<fa250ConvertedHit>());
		loop->AddFactory(new JFactory<fa250Mode1Hit>());
		loop->AddFactory(new JFactory<fa250WaveboardV1Hit>());
		loop->AddFactory(new fa250Mode1CalibPedSubHit_factory());
		loop->AddFactory(new JFactory<eventData>());
		loop->AddFactory(new JFactory<epicsRawData>());
		return NOERROR;
	}

};

#endif // _JFactoryGenerator_DAQ_

