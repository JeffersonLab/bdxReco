// $Id$
//
//    File: JEventProcessor_DAQWaveformsMode1.cc
// Created: Mon Nov 13 10:53:49 CET 2017
// Creator: celentan (on Linux apcx4 2.6.32-696.13.2.el6.x86_64 x86_64)
//

#include "JEventProcessor_DAQWaveformsMode1.h"
using namespace jana;

#include <TDirectory.h>
#include <TH2.h>
#include <TH1.h>
#include <TProfile2D.h>
#include <TStyle.h>

#include "DAQ/fa250WaveboardV1Hit.h"
#include "DAQ/fa250Mode1CalibPedSubHit.h"
#include <DAQ/eventData.h>

// Routine used to create our JEventProcessor
#include <JANA/JApplication.h>
#include <JANA/JFactory.h>

#include "TROOT.h"
/*Here goes the histograms*/
static const int nCrates = 1;
static const int nSlots = 6;
static const int nChannels = 16;
static const int nSamples = 2000;
// root hist pointers
static TH1D *hDAQWaveform[nCrates][nSlots][nChannels];
static int NsamplesWfm[nCrates][nSlots][nChannels] = { 0 };
extern "C" {
void InitPlugin(JApplication *app) {
    InitJANAPlugin(app);
    app->AddProcessor(new JEventProcessor_DAQWaveformsMode1());
}
} // "C"

//------------------
// JEventProcessor_DAQWaveformsMode1 (Constructor)
//------------------
JEventProcessor_DAQWaveformsMode1::JEventProcessor_DAQWaveformsMode1() {

}

//------------------
// ~JEventProcessor_DAQWaveformsMode1 (Destructor)
//------------------
JEventProcessor_DAQWaveformsMode1::~JEventProcessor_DAQWaveformsMode1() {

}

//------------------
// init
//------------------
jerror_t JEventProcessor_DAQWaveformsMode1::init(void) {
    // This is called once at program startup. If you are creating
    // and filling historgrams in this plugin, you should lock the
    // ROOT mutex like this:
    //
    // japp->RootWriteLock();
    //  ... fill historgrams or trees ...
    // japp->RootUnLock();
    //

    // lock all root operations
    japp->RootWriteLock();

    // First thread to get here makes all histograms. If one pointer is
    // already not NULL, assume all histograms are defined and return now
    if (hDAQWaveform[0][0][0] != NULL) {
        japp->RootUnLock();
        return NOERROR;
    }
    gROOT->cd();
    TDirectory *main = gDirectory;
    gDirectory->mkdir("DAQWaveformsMode1")->cd();

    /*Create all the histograms*/
    for (int icrate = 0; icrate < nCrates; icrate++) {
        for (int islot = 0; islot < nSlots; islot++) {
            for (int ichannel = 0; ichannel < nChannels; ichannel++) {
                hDAQWaveform[icrate][islot][ichannel] = new TH1D(Form("hDAQWaveform_c%i_s%i_c%i", icrate, islot, ichannel),
                        Form("hDAQWaveform_c%i_s%i_c%i", icrate, islot, ichannel), nSamples, -0.5, nSamples - 0.5);
                hDAQWaveform[icrate][islot][ichannel]->GetXaxis()->SetTitle("Sample N.");
                hDAQWaveform[icrate][islot][ichannel]->GetYaxis()->SetTitle("Ampl. (V)");

            }
        }
    }

    // back to main dir
    main->cd();

    // unlock
    japp->RootUnLock();
    return NOERROR;

}

//------------------
// brun
//------------------
jerror_t JEventProcessor_DAQWaveformsMode1::brun(JEventLoop *eventLoop, int32_t runnumber) {
    // This is called whenever the run number changes
    return NOERROR;
}

//------------------
// evnt
//------------------
jerror_t JEventProcessor_DAQWaveformsMode1::evnt(JEventLoop *loop, uint64_t eventnumber) {

    int crate, slot, channel, N;
//	vector<const fa250WaveboardV1Hit*> m_waveforms;
//	vector<const fa250WaveboardV1Hit*>::const_iterator it;

    vector<const fa250Mode1CalibPedSubHit*> m_waveforms;
    vector<const fa250Mode1CalibPedSubHit*>::const_iterator it;

    loop->Get(m_waveforms);

    const eventData *tData;

    try {
        loop->GetSingle(tData);
    } catch (unsigned long e) {
        jout << "JEventProcessor_Trigger::evnt no eventData bank this event" << std::endl;
        return OBJECT_NOT_AVAILABLE;
    }
    /*If this is an EPICS event, do nothing.*/
    if (tData->eventType == eventSource::EPICS) {
        return NOERROR;
    }

    // Lock ROOT
    japp->RootWriteLock();
    for (it = m_waveforms.begin(); it != m_waveforms.end(); it++) {

        crate = (*it)->m_channel.rocid;
        slot = (*it)->m_channel.slot;
        channel = (*it)->m_channel.channel;
        N = (*it)->samples.size();

        /*Check for crate-slot-channel numbers*/
        if (crate < 0) {
            jerr << "ERROR in DAQWaveformsMode1 plugin, crate number is <0 " << endl;
            japp->RootUnLock();
            return VALUE_OUT_OF_RANGE;
        } else if (crate >= nCrates) {
            jerr << "ERROR in DAQWaveformsMode1 plugin, crate number too high - change nCrates and recompile " << crate << " " << nCrates << endl;
            japp->RootUnLock();
            return VALUE_OUT_OF_RANGE;
        }

        if (slot < 0) {
            jerr << "ERROR in DAQWaveformsMode1 plugin, slot number is <0 " << endl;
            japp->RootUnLock();
            return VALUE_OUT_OF_RANGE;
        } else if (slot >= nSlots) {
            jerr << "ERROR in DAQWaveformsMode1 plugin, slot number too high - change nSlots and recompile " << slot << " " << nSlots << endl;
            japp->RootUnLock();
            return VALUE_OUT_OF_RANGE;
        }

        if ((channel < 0) || (channel > 15)) {
            jerr << "ERROR in DAQWaveformsMode1 plugin, channel number is out of range " << channel << endl;
            japp->RootUnLock();
            return VALUE_OUT_OF_RANGE;
        }

        /*if (N != NsamplesWfm[crate][slot][channel]) {
         jerr << "ERROR in DAQWaveformsMode1 plugin nsamples " << N << " " << NsamplesWfm[crate][slot][channel] << endl;
         }*/

        if (NsamplesWfm[crate][slot][channel] != N) {
            /*Check for samples number*/
            if (NsamplesWfm[crate][slot][channel] > nSamples) {
                jerr << "ERROR in DAQWaveformsMode1 plugin, number of samples is too high " << NsamplesWfm[crate][slot][channel] << " max set in plugin: "
                        << nSamples << endl;
                japp->RootUnLock();
                return VALUE_OUT_OF_RANGE;
            } else {
                NsamplesWfm[crate][slot][channel] = N;
                /*Reset histogram*/
                hDAQWaveform[crate][slot][channel]->Reset();
                hDAQWaveform[crate][slot][channel]->GetXaxis()->SetRangeUser(0, NsamplesWfm[crate][slot][channel] - 1);
                hDAQWaveform[crate][slot][channel]->GetYaxis()->SetRangeUser(0,16000);
            }
        }
        for (int isample = 0; isample < NsamplesWfm[crate][slot][channel]; isample++) {
            hDAQWaveform[crate][slot][channel]->SetBinContent(isample, (*it)->samples[isample]);
        }

    }

    // Unlock ROOT
    japp->RootUnLock();
    return NOERROR;
}

//------------------
// erun
//------------------
jerror_t JEventProcessor_DAQWaveformsMode1::erun(void) {
    // This is called whenever the run number changes, before it is
    // changed to give you a chance to clean up before processing
    // events from the next run number.
    return NOERROR;
}

//------------------
// fini
//------------------
jerror_t JEventProcessor_DAQWaveformsMode1::fini(void) {
    // Called before program exit after event processing is finished.
    return NOERROR;
}

