#include "IntVetoDigiHit.h"
#include <DAQ/fa250Mode1CalibPedSubHit.h>
#include "TCanvas.h"
#include "TH1D.h"

TCanvas* IntVetoDigiHit::Draw(int id) const {

	int Nsamples;
	const fa250Mode1CalibPedSubHit* wave;

	vector<const fa250Mode1CalibPedSubHit*> waves; //use a vector since it is re-iterating!
	this->Get(waves);
	if (waves.size()!=1){
		jout<<"IntVetoDigiHit::Draw no wave associated to this "<<endl;
		wave=0;
		Nsamples = 1;
		return 0;
	}
	else{
		wave=waves[0];
		Nsamples = wave->samples.size();
	}

	cout<<Nsamples<<endl;
	hWave = new TH1D(Form("h%i_%i_%i_%i", m_channel.sector, m_channel.layer, m_channel.component, m_channel.readout), Form("h%i_%i_%i_%i", m_channel.sector, m_channel.layer, m_channel.component, m_channel.readout), Nsamples, -0.5*wave->m_dT, (Nsamples - 0.5)*wave->m_dT);
	if (wave!=0){
		wave->toHisto(hWave);
	}
	if (id >= 0) {
		if (m_canvas == 0) {
			m_canvas = new TCanvas(Form("c%i_%i_%i_%i", m_channel.sector, m_channel.layer, m_channel.component, m_channel.readout), 200, 200, id);
		}
		m_canvas->cd();
		m_canvas->Clear();
		hWave->Draw();
	}

	return m_canvas;

}
