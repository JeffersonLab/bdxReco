// $Id$
//
//    File: IntVetofa250Converter.h
// Created: Mon Jan 25 17:02:27 CET 2016
// Creator: celentan (on Linux apcx4 2.6.32-504.30.3.el6.x86_64 x86_64)
//

#ifndef _IntVetofa250Converter_
#define _IntVetofa250Converter_

#include <JANA/JObject.h>
#include <JANA/JFactory.h>

#include <IntVeto/IntVetoSiPMHit.h>
#include <DAQ/fa250Converter.h>
#include <TT/TranslationTable.h>

class fa250Mode1CalibHit;
class fa250Mode7Hit;


class IntVetofa250Converter:public fa250Converter<IntVetoSiPMHit>{
	public:
		JOBJECT_PUBLIC(IntVetofa250Converter);// Add data members here. For example:

		virtual IntVetoSiPMHit* convertHit(const fa250Hit *hit,const TranslationTable::ChannelInfo &m_channel) const;
		jerror_t convertMode1Hit(IntVetoSiPMHit* output,const fa250Mode1CalibHit *input) const;
		jerror_t convertMode7Hit(IntVetoSiPMHit* output,const fa250Mode7Hit *input) const;
};

#endif // _IntVetofa250Converter_

