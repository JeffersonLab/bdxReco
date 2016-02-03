// $Id$
//
//    File: fa250Mode1CalibHit.h
// Created: Wed Feb  3 07:38:15 EST 2016
// Creator: davidl (on Darwin harriet.jlab.org 13.4.0 i386)
//

#ifndef _fa250Mode1CalibHit_
#define _fa250Mode1CalibHit_

#include <JANA/JObject.h>
#include <JANA/JFactory.h>

#include "fa250Hit.h"

class fa250Mode1CalibHit:public fa250Hit{
	public:
		JOBJECT_PUBLIC(fa250Mode1CalibHit);
		
		vector <double> samples; // calibrated samples
		
		// This method is used primarily for pretty printing
		// the second argument to AddString is printf style format
		void toStrings(vector<pair<string,string> > &items)const{
			 AddString(items, "crate", "%4d", crate);
			 AddString(items, "slot", "%4d", slot);
			 AddString(items, "channel", "%4d", channel);
			 AddString(items, "nsamples", "%4d", samples.size());
		}
		
};

#endif // _fa250Mode1CalibHit_

