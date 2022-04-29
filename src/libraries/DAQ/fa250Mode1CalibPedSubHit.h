// $Id$
//
//    File: fa250Mode1CalibHit.h
// Created: Wed Feb  3 07:38:15 EST 2016
// Creator: davidl (on Darwin harriet.jlab.org 13.4.0 i386)
//

#ifndef _fa250Mode1CalibPedSubHit_
#define _fa250Mode1CalibPedSubHit_

#include <JANA/JObject.h>
#include <JANA/JFactory.h>

#include "fa250Mode1Hit.h"

class fa250Mode1CalibPedSubHit:public fa250Mode1Hit{
	public:
		JOBJECT_PUBLIC(fa250Mode1CalibPedSubHit);
		fa250Mode1CalibPedSubHit();
		virtual ~fa250Mode1CalibPedSubHit();



		// This method is used primarily for pretty printing
		// the second argument to AddString is printf style format
		void toStrings(vector<pair<string,string> > &items)const{
			 AddString(items, "crate", "%4d", m_channel.rocid);
			 AddString(items, "slot", "%4d", m_channel.slot);
			 AddString(items, "channel", "%4d", m_channel.channel);
			 AddString(items, "nsamples", "%4d", samples.size());
		}

		double m_ped,m_RMS; //from DB


		virtual TCanvas* Draw(int id=0) const;
		void toHisto(TH1D *h) const;

		ClassDef(fa250Mode1CalibPedSubHit,1);
};

#endif // _fa250Mode1CalibHit_

