// $Id$
//
//    File: JEventProcessor_NA64EcalHits.cc
// Created: Thu Oct  3 17:27:30 CEST 2019
// Creator: celentan (on Linux apcx4 3.10.0-957.el7.x86_64 x86_64)
//

#include "../NA64EcalHits/JEventProcessor_NA64EcalHits.h"
using namespace jana;

// Routine used to create our JEventProcessor
#include <JANA/JApplication.h>
#include <JANA/JFactory.h>

#include "Calorimeter/CalorimeterHit.h"
#include "DAQ/eventData.h"

#include <system/JROOTOutput.h>
#include <system/BDXEventProcessor.h>

#include "TROOT.h"
// Routine used to create our JEventProcessor
#include <JANA/JApplication.h>
#include <JANA/JFactory.h>

/*Here goes the histograms*/
static const int nECAL = 9;

//Amplitude
static TH1D *hNA64EcalHitsEnergy[nECAL] = { 0 };

extern "C" {
void InitPlugin(JApplication *app) {
    InitJANAPlugin(app);
    app->AddProcessor(new JEventProcessor_NA64EcalHits());
}
} // "C"

//------------------
// JEventProcessor_NA64EcalHits (Constructor)
//------------------
JEventProcessor_NA64EcalHits::JEventProcessor_NA64EcalHits() :
        m_ROOTOutput(0), m_isMC(0) {
    m_isFirstCallToBrun = 1;

}

//------------------
// ~JEventProcessor_NA64EcalHits (Destructor)
//------------------
JEventProcessor_NA64EcalHits::~JEventProcessor_NA64EcalHits() {

}

//------------------
// init
//------------------
jerror_t JEventProcessor_NA64EcalHits::init(void) {
    // This is called once at program startup. If you are creating
    // and filling historgrams in this plugin, you should lock the
    // ROOT mutex like this:
    //
    // japp->RootWriteLock();
    //  ... fill historgrams or trees ...
    // japp->RootUnLock();
    //
    m_isFirstCallToBrun = 1;
    m_isMC = 0;
    gPARMS->GetParameter("MC", m_isMC);

    /*Hard-coded XY -> id geometry*/
    geometry[make_pair(0, 0)] = 1;
    geometry[make_pair(0, 1)] = 2;
    geometry[make_pair(0, 2)] = 3;
    geometry[make_pair(1, 0)] = 4;
    geometry[make_pair(1, 1)] = 5;
    geometry[make_pair(1, 2)] = 6;
    geometry[make_pair(2, 0)] = 7;
    geometry[make_pair(2, 1)] = 8;
    geometry[make_pair(2, 2)] = 9;

    japp->RootWriteLock();

    if (hNA64EcalHitsEnergy[0] != NULL) {
        japp->RootUnLock();
        return NOERROR;
    }

    gROOT->cd();
    TDirectory *main = gDirectory;
    gDirectory->mkdir("NA64EcalHits")->cd();

    /*Create all the histograms*/
    map<pair<int, int>, int>::iterator geometry_it;
    int iX, iY, id;

    double Emin, Emax;
    int NE;

    NE = 500;
    Emin = 100;
    Emax = 600; //600

    gPARMS->SetDefaultParameter("NA64EcalHits:Emin", Emin, "Emin for histo");
    gPARMS->SetDefaultParameter("NA64EcalHits:Emax", Emax, "Emax for histo");
    gPARMS->SetDefaultParameter("NA64EcalHits:NE", NE, "NE for histo");

    for (geometry_it = geometry.begin(); geometry_it != geometry.end(); geometry_it++) {
        //Do whatever you want. To access the first part of pair(pair is the key of the map here) you should write

        iX = (geometry_it->first).first;
        iY = (geometry_it->first).second;
        id = (geometry_it->second);

        id = id - 1;

        hNA64EcalHitsEnergy[id] = new TH1D(Form("hE_x%i_y%i", iX, iY), Form("hE_x%i_y%i", iX, iY), NE, Emin, Emax);
        hNA64EcalHitsEnergy[id]->GetXaxis()->SetTitle("E");

    }

    // back to main dir
    main->cd();
    japp->RootUnLock();

    return NOERROR;

}

//------------------
// brun
//------------------
jerror_t JEventProcessor_NA64EcalHits::brun(JEventLoop *eventLoop, int32_t runnumber) {
    // This is called whenever the run number changes
    jout << m_isFirstCallToBrun << endl;

    if (m_isFirstCallToBrun) {

        jout << "JEventProcessor_BDXMiniClusters::brun searching m_RootOutput" << endl;
        string class_name, this_class_name;
        string joutput_name;
        BDXEventProcessor *m_BDXEventProcessor;
        vector<JEventProcessor*> m_processors = app->GetProcessors();
        vector<JEventProcessor*>::iterator m_processors_it;

        m_isFirstCallToBrun = 0;
        class_name = "BDXEventProcessor";
        joutput_name = "JROOTOutput";
        //Now I need to determine which processor is the one holding the output. Discussing with David, he suggested just to check the class name, since
        //a dynamic cast may not work with plugins
        for (m_processors_it = m_processors.begin(); m_processors_it != m_processors.end(); m_processors_it++) {
            if ((*m_processors_it) != 0) {
                this_class_name = string((*m_processors_it)->className());
                if (this_class_name == class_name) {
                    m_BDXEventProcessor = (BDXEventProcessor*) (*m_processors_it);
                    if (m_BDXEventProcessor->getOutput() == 0) {
                        jerr << "BDXEventProcessor JOutput is null!" << endl;
                        break;
                    }
                    if (string(m_BDXEventProcessor->getOutput()->className()) == joutput_name) {
                        m_ROOTOutput = (JROOTOutput*) (m_BDXEventProcessor->getOutput());
                        jout << "Got JROOTOutput!" << endl;
                    } else {
                        jerr << "BDXEventProcessor JOutput is not null BUT class is: " << m_BDXEventProcessor->getOutput()->className() << endl;
                    }
                }
            }
        }
    }
    return NOERROR;
}

//------------------
// evnt
//------------------
jerror_t JEventProcessor_NA64EcalHits::evnt(JEventLoop *loop, uint64_t eventnumber) {
    // This is called for every event. Use of common resources like writing
    // to a file or filling a histogram should be mutex protected. Using
    // loop->Get(...) to get reconstructed objects (and thereby activating the
    // reconstruction algorithm) should be done outside of any mutex lock
    // since multiple threads may call this method at the same time.
    // Here's an example:
    //
    // vector<const MyDataClass*> mydataclasses;
    // loop->Get(mydataclasses);
    //
    // japp->RootWriteLock();
    //  ... fill historgrams or trees ...
    // japp->RootUnLock();

    vector<const CalorimeterHit*> hits;
    vector<const CalorimeterHit*>::iterator hits_it;
    const CalorimeterHit *m_hit;
    const eventData *tData;
    if (!m_isMC) {
        try {
            loop->GetSingle(tData);
        } catch (unsigned long e) {
            jout << "JEventProcessor_NA64EcalHits::evnt no eventData bank this event" << std::endl;
            return OBJECT_NOT_AVAILABLE;
        }
    }

    loop->Get(hits);
    int sector, X, Y, id;

    for (hits_it = hits.begin(); hits_it != hits.end(); hits_it++) {
        m_hit = *hits_it;

        sector = m_hit->m_channel.sector;
        X = m_hit->m_channel.x;
        Y = m_hit->m_channel.y;

        if ((X < 0) || (X > 2) || (Y < 0) || (Y > 2))
            continue;
        id = geometry[make_pair(X, Y)];
        id = id - 1;

        japp->RootWriteLock();
        hNA64EcalHitsEnergy[id]->Fill(m_hit->E); //E
        japp->RootUnLock();
    }

    return NOERROR;
}

//------------------
// erun
//------------------
jerror_t JEventProcessor_NA64EcalHits::erun(void) {
    // This is called whenever the run number changes, before it is
    // changed to give you a chance to clean up before processing
    // events from the next run number.

    if (m_ROOTOutput != 0) {
        jout << "JEventProcessor_BDXMiniCalorimeterEnergyCalibartion adding histos to m_ROOTOutput" << endl;
        for (int ii = 0; ii < nECAL; ii++) {
            m_ROOTOutput->AddObject(hNA64EcalHitsEnergy[ii]);
        }
    }
    return NOERROR;
}

//------------------
// fini
//------------------
jerror_t JEventProcessor_NA64EcalHits::fini(void) {
    // Called before program exit after event processing is finished.
    return NOERROR;
}

