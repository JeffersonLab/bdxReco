// hnamepath: /NA64EcalHits/hE_x0_y0
// hnamepath: /NA64EcalHits/hE_x0_y1
// hnamepath: /NA64EcalHits/hE_x0_y2
// hnamepath: /NA64EcalHits/hE_x1_y0
// hnamepath: /NA64EcalHits/hE_x1_y1
// hnamepath: /NA64EcalHits/hE_x1_y2
// hnamepath: /NA64EcalHits/hE_x2_y0
// hnamepath: /NA64EcalHits/hE_x2_y1
// hnamepath: /NA64EcalHits/hE_x2_y2

{
	TDirectory *dir = (TDirectory*)gDirectory->FindObjectAny("NA64EcalHits");
	if(dir) dir->cd();

	const int nChannels=9;

	TH1D *h[nChannels];
	h[0]= (TH1D*)gDirectory->FindObjectAny("hE_x0_y0");
	h[1]= (TH1D*)gDirectory->FindObjectAny("hE_x0_y1");
	h[2]= (TH1D*)gDirectory->FindObjectAny("hE_x0_y2");
	h[3]= (TH1D*)gDirectory->FindObjectAny("hE_x1_y0");
	h[4]= (TH1D*)gDirectory->FindObjectAny("hE_x1_y1");
	h[5]= (TH1D*)gDirectory->FindObjectAny("hE_x1_y2");
	h[6]= (TH1D*)gDirectory->FindObjectAny("hE_x2_y0");
	h[7]= (TH1D*)gDirectory->FindObjectAny("hE_x2_y1");
	h[8]= (TH1D*)gDirectory->FindObjectAny("hE_x2_y2");



	// Just for testing
	if(gPad == NULL){
		TCanvas *c1 = new TCanvas("NA64ECAL");
		c1->cd(0);
		c1->Draw();
		c1->Update();
	}

	if(!gPad) return;
	TCanvas *c1 = gPad->GetCanvas();

	c1->Divide(3, 3);
	c1->cd(7);
	gPad->SetTicks();
	gPad->SetGrid();
	h[0]->Draw();
	c1->cd(4);
	gPad->SetTicks();
	gPad->SetGrid();
	h[1]->Draw();
	c1->cd(1);
	gPad->SetTicks();
	gPad->SetGrid();
	h[2]->Draw();

	c1->cd(8);
	gPad->SetTicks();
	gPad->SetGrid();
	h[3]->Draw();
	c1->cd(5);
	gPad->SetTicks();
	gPad->SetGrid();
	h[4]->Draw();
	c1->cd(2);
	gPad->SetTicks();
	gPad->SetGrid();
	h[5]->Draw();

	c1->cd(9);
	gPad->SetTicks();
	gPad->SetGrid();
	h[6]->Draw();

	c1->cd(6);
	gPad->SetTicks();
	gPad->SetGrid();
	h[7]->Draw();

	c1->cd(3);
	gPad->SetTicks();
	gPad->SetGrid();
	h[8]->Draw();
}





