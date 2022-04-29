// $Id$
//
//    File: fa250Mode1Hit.h
// Created: Mon Dec 28 11:04:04 CET 2015
// Creator: celentan (on Linux apcx4 2.6.32-504.30.3.el6.x86_64 x86_64)
//

#ifndef _fa250Mode1Hit_
#define _fa250Mode1Hit_

#include <JANA/JObject.h>
#include <JANA/JFactory.h>
#include "fa250Hit.h"
#include "TH1D.h"

class TCanvas;

class fa250Mode1Hit: public fa250Hit {
public:
	JOBJECT_PUBLIC(fa250Mode1Hit)
	;

	fa250Mode1Hit();
	virtual ~fa250Mode1Hit();

	vector<double> samples;

	// This method is used primarily for pretty printing
	// the second argument to AddString is printf style format
	void toStrings(vector<pair<string, string> > &items) const {
		AddString(items, "crate", "%4d", m_channel.rocid);
		AddString(items, "slot", "%4d", m_channel.slot);
		AddString(items, "channel", "%4d", m_channel.channel);
		AddString(items, "timestamp", "%lld", (long long) timestamp);

		// AddString(items, "E", "%f", E);
	}
	void toHisto(TH1D *h) const;

	virtual TCanvas* Draw(int id = 0) const;

	double m_dT; //sampling time
	double m_RMS, m_ped;

protected:
	mutable TH1D *hWave;

ClassDef(fa250Mode1Hit,1);
};

#endif // _fa250Mode1Hit_
