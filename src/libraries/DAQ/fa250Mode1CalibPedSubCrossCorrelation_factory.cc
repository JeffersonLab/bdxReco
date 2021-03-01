// $Id$
//
//    File: fa250Mode1CalibPedSubCrossCorrelation_factory.cc
// Created: Wed Feb 10 15:01:00 CET 2021
// Creator: mspreafico (on Linux apcx18 3.10.0-514.el7.x86_64 x86_64)
//

#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <array>
#include <sstream>
#include <string>
#include "TROOT.h"
#include <vector>
#include <TMath.h>
#include <TF1.h>
using namespace std;

#include "fa250Mode1CalibPedSubCrossCorrelation_factory.h"
#include "fa250Mode1CalibPedSubHit.h"


#include <JANA/JApplication.h>
using namespace jana;

double fa250Mode1CalibPedSubCrossCorrelation_factory::signal(double *x, double *par) {
	//Fit parameters:
	//par[0]=starting time of signal
	//par[1]= tao
	//par[2]= amplitude
	if (x[0] < par[0])
		return 0;
	else
		return TMath::Power((x[0] - par[0]), 2)
				* TMath::Exp(-(x[0] - par[0]) / par[1]) * par[2]
				* (1 / (4 * TMath::Exp(-2) * TMath::Power(par[1], 2)));
}


double fa250Mode1CalibPedSubCrossCorrelation_factory::f380(double *x, double *par) {

	return -par[0]*cos(par[1]*(x[0]-par[2]))+par[3];
}

double fa250Mode1CalibPedSubCrossCorrelation_factory::f600(double *x, double *par) {

	return -par[0]*cos(par[1]*(x[0]-par[2]))+par[3];
}

//------------------
// init
//------------------
jerror_t fa250Mode1CalibPedSubCrossCorrelation_factory::init(void) {
	//period of sine
	T1 = 380;
	T2 = 600;
	tao = 30;
	A = 2;
	japp->RootWriteLock();
	f1 = new TF1("f380",this->f380,0,700,4);
	f1->FixParameter(0, 1);
	f1->FixParameter(1, 2 * TMath::Pi() / T1);
	f1->FixParameter(3, 0);

	f2 = new TF1("f600",this->f600,0,700,4);
	f2->FixParameter(0, 1);
	f2->FixParameter(1, 2 * TMath::Pi() / T2);
	f2->FixParameter(3, 0);

	f3 = new TF1("signal", this->signal, 0, 700, 3);
	f3->SetParameter(1, tao);
	f3->SetParameter(2, A);

	japp->RootUnLock();
	return NOERROR;
}

//------------------
// brun
//------------------
jerror_t fa250Mode1CalibPedSubCrossCorrelation_factory::brun(
		jana::JEventLoop *eventLoop, int32_t runnumber) {
	return NOERROR;
}

//------------------
// evnt
//------------------
jerror_t fa250Mode1CalibPedSubCrossCorrelation_factory::evnt(JEventLoop *loop,
		uint64_t eventnumber) {

	//code to read waveforms
	vector<const fa250Mode1CalibPedSubHit*> waves;
	loop->Get(waves);
	if (waves.size() == 0) {
		return OBJECT_NOT_AVAILABLE;
	}

	double xcorr_tot = 0;
	double xcorr_tot_s = 0;
	double xcorr_tot_5mV = 0;
	double xcorr_tot_s_5mV = 0;
	double xcorr_tot_5mV_calo = 0;
	double xcorr_tot_s_5mV_calo = 0;
	double xcorr_tot_calo = 0;
	double xcorr_tot_s_calo = 0;

	const fa250Mode1CalibPedSubHit *wave = 0;
	int crate, slot, channel;
	vector<double> data;
	double dT = 0;
	//begin the cycle over the channels
	for (int ii = 0; ii < waves.size(); ii++) {
		wave = waves[ii];
		crate = wave->m_channel.rocid;
		slot = wave->m_channel.slot;
		channel = wave->m_channel.channel;
		data = wave->samples;
		dT = wave->m_dT;
		//find sine parameters
		double t_min = 0, f_min = data[0];
		double t_max = 0, f_max = data[0];

		for (int jj = 0; jj < data.size(); jj++) {
			if (data[jj] > f_max) {
				f_max = data[jj];
				t_max = jj * dT;
			}
			if (data[jj] < f_min) {
				f_min = data[jj];
				t_min = jj * dT;
				;
			}
		}

		double mid = (f_max + f_min) / 2;

		f_max = f_max - mid;

		for (int jj = 0; jj < data.size(); jj++) {
			if (f_max == 0)
				break;
			data[jj] = (data[jj] - mid) / f_max;
		}

		f1->FixParameter(2, t_min);

		f2->FixParameter(2, t_min);

		//find parameters for signal
		double ped = (f_min - mid) / f_max;

		double t_0 = t_max - 2 * tao;
		f3->SetParameter(0, t_0);

		//evaluating crosscorrelation
		double crosscorrelation1 = 0;
		double crosscorrelation2 = 0;
		double crosscorrelation3 = 0;
		double crosscorrelation4 = 0;
		double crosscorrelation5 = 0;
		double crosscorrelation6 = 0;

		for (int jj = 0; jj < data.size(); jj++) {
			//excluding broken channels ???
			if (slot == 0 && channel == 2)
				break;
			if (slot == 2 && channel == 14)
				break;
			if (slot == 2 && channel == 15)
				break;
			if (slot == 1 && channel == 15)
				break;
			crosscorrelation3 += (data[jj] - ped) * f3->Eval(jj * dT) * dT;
			if (slot == 1 && channel > 4) {
				crosscorrelation6 += (data[jj] - ped) * f3->Eval(jj * dT) * dT;
			} else if (slot > 1) {
				crosscorrelation6 += (data[jj] - ped) * f3->Eval(jj * dT) * dT;
			}
		}

		xcorr_tot_s += (crosscorrelation3);
		xcorr_tot_s_calo += (crosscorrelation6);
		if (f_max > 5) {
			xcorr_tot_s_5mV += (crosscorrelation3);
			xcorr_tot_s_5mV_calo += crosscorrelation6;
		}

		//evaluating crosscorrelation with sine over a period
		int deltaT1, deltaT2;
		deltaT1 = T1 / dT;
		deltaT2 = T2 / dT;
		for (int jj = 0; jj < deltaT1; jj++) {
			if (slot == 0 && channel == 2)
				break;
			if (slot == 2 && channel == 14)
				break;
			if (slot == 2 && channel == 15)
				break;
			if (slot == 1 && channel == 15)
				break;
			crosscorrelation1 += data[jj] * f1->Eval(jj * dT) * dT;
			if (slot == 1 && channel > 4) {
				crosscorrelation4 += data[jj] * f1->Eval(jj * dT) * dT;
			} else if (slot > 1) {
				crosscorrelation4 += data[jj] * f1->Eval(jj * dT) * dT;
			}
		}

		for (int jj = 0; jj < deltaT2; jj++) {
			if (slot == 0 && channel == 2)
				break;
			if (slot == 2 && channel == 14)
				break;
			if (slot == 2 && channel == 15)
				break;
			if (slot == 1 && channel == 15)
				break;
			crosscorrelation2 += data[jj] * f2->Eval(jj * dT) * dT;
			if (slot == 1 && channel > 4) {
				crosscorrelation5 += data[jj] * f2->Eval(jj * dT) * dT;
			} else if (slot > 1) {
				crosscorrelation5 += data[jj] * f2->Eval(jj * dT) * dT;
			}
		}

		//we use the maximum between the two periods
		double crosscorrelation = 0;
		double crosscorrelation_calo = 0;
		if (crosscorrelation1 > crosscorrelation2)
			crosscorrelation = crosscorrelation1;
		else
			crosscorrelation = crosscorrelation2;

		if (crosscorrelation4 > crosscorrelation5)
			crosscorrelation_calo = crosscorrelation4;
		else
			crosscorrelation_calo = crosscorrelation5;

		xcorr_tot += (crosscorrelation);
		xcorr_tot_calo += crosscorrelation_calo;
		if (f_max > 5) {
			xcorr_tot_5mV += (crosscorrelation);
			xcorr_tot_5mV_calo += crosscorrelation_calo;
		}
	}

	fa250Mode1CalibPedSubCrossCorrelation *myfa250Mode1CalibPedSubCrossCorrelation =
			new fa250Mode1CalibPedSubCrossCorrelation;

	myfa250Mode1CalibPedSubCrossCorrelation->crossCorrSine_tot = xcorr_tot;
	myfa250Mode1CalibPedSubCrossCorrelation->crossCorrSignal_tot = xcorr_tot_s;
	myfa250Mode1CalibPedSubCrossCorrelation->crossCorrSine_5mVth =
			xcorr_tot_5mV;
	myfa250Mode1CalibPedSubCrossCorrelation->crossCorrSignal_5mVth =
			xcorr_tot_s_5mV;
	myfa250Mode1CalibPedSubCrossCorrelation->crossCorrSine_calo =
			xcorr_tot_calo;
	myfa250Mode1CalibPedSubCrossCorrelation->crossCorrSignal_calo =
			xcorr_tot_s_calo;
	myfa250Mode1CalibPedSubCrossCorrelation->crossCorrSine_5mVth_calo =
			xcorr_tot_5mV_calo;
	myfa250Mode1CalibPedSubCrossCorrelation->crossCorrSignal_5mVth_calo =
			xcorr_tot_s_5mV_calo;

	_data.push_back(myfa250Mode1CalibPedSubCrossCorrelation);
	//
	// Note that the objects you create here will be deleted later
	// by the system and the _data vector will be cleared automatically.

	return NOERROR;
}

//------------------
// erun
//------------------
jerror_t fa250Mode1CalibPedSubCrossCorrelation_factory::erun(void) {
	return NOERROR;
}

//------------------
// fini
//------------------
jerror_t fa250Mode1CalibPedSubCrossCorrelation_factory::fini(void) {
	return NOERROR;
}

