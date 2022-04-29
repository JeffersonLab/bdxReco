// $Id$
//
//    File: fa250Mode1Hit.h
// Created: Mon Dec 28 11:04:04 CET 2015
// Creator: celentan (on Linux apcx4 2.6.32-504.30.3.el6.x86_64 x86_64)
//

#ifndef _fa250WaveboardV1Hit_
#define _fa250WaveboardV1Hit_

#include <JANA/JObject.h>
#include <JANA/JFactory.h>
#include "fa250Hit.h"


class TH1D;
class TCanvas;
class fa250WaveboardV1Hit:public fa250Hit{
	public:
		JOBJECT_PUBLIC(fa250WaveboardV1Hit);
		
		
		fa250WaveboardV1Hit();
		virtual ~fa250WaveboardV1Hit();

		vector <uint16_t> samples;
		int chargeFirstDBnopedsub;
		// This method is used primarily for pretty printing
		// the second argument to AddString is printf style format
		void toStrings(vector<pair<string,string> > &items)const{
			 AddString(items, "crate", "%4d",  m_channel.rocid);
			 AddString(items, "slot", "%4d",  m_channel.slot);
			 AddString(items, "channel", "%4d",  m_channel.channel);
			 AddString(items, "timestamp","%lld",(long long)timestamp);
			 AddString(items,"nsamples","%4d",samples.size());
			 AddString(items,"charge(firstDF,noPedSub)","%4d",chargeFirstDBnopedsub);

			// AddString(items, "E", "%f", E);
		}
		void toHisto(TH1D *h) const;
		
		virtual TCanvas* Draw(int id=0) const;

	protected:
		mutable TH1D *hWave;
};

#endif // _fa250Mode1Hit_

