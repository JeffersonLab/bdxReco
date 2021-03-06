#include <dlfcn.h>
#include <iostream>
#include <string>
using namespace std;
static string macro_data=""
"\n"
"\n"
"// hnamepath: /IntVetoSipm/hSipmCharge_s0_l0_c1_r1\n"
"// hnamepath: /IntVetoSipm/hSipmCharge_s0_l0_c2_r1\n"
"// hnamepath: /IntVetoSipm/hSipmCharge_s0_l0_c3_r1\n"
"// hnamepath: /IntVetoSipm/hSipmCharge_s0_l0_c4_r1\n"
"// hnamepath: /IntVetoSipm/hSipmCharge_s0_l0_c5_r1\n"
"// hnamepath: /IntVetoSipm/hSipmCharge_s0_l0_c6_r1\n"
"// hnamepath: /IntVetoSipm/hSipmCharge_s0_l0_c7_r1\n"
"// hnamepath: /IntVetoSipm/hSipmCharge_s0_l0_c8_r1\n"
"// hnamepath: /IntVetoSipm/hSipmCharge_s0_l0_c9_r1\n"
"// hnamepath: /IntVetoSipm/hSipmCharge_s0_l0_c10_r1\n"
"// hnamepath: /IntVetoSipm/hSipmCharge_s0_l0_c11_r1\n"
"// hnamepath: /IntVetoSipm/hSipmCharge_s0_l0_c12_r1\n"
"// hnamepath: /IntVetoSipm/hSipmCharge_s0_l0_c13_r1\n"
"\n"
"// hnamepath: /IntVetoSipm/hSipmAmpl_s0_l0_c1_r1\n"
"// hnamepath: /IntVetoSipm/hSipmAmpl_s0_l0_c2_r1\n"
"// hnamepath: /IntVetoSipm/hSipmAmpl_s0_l0_c3_r1\n"
"// hnamepath: /IntVetoSipm/hSipmAmpl_s0_l0_c4_r1\n"
"// hnamepath: /IntVetoSipm/hSipmAmpl_s0_l0_c5_r1\n"
"// hnamepath: /IntVetoSipm/hSipmAmpl_s0_l0_c6_r1\n"
"// hnamepath: /IntVetoSipm/hSipmAmpl_s0_l0_c7_r1\n"
"// hnamepath: /IntVetoSipm/hSipmAmpl_s0_l0_c8_r1\n"
"// hnamepath: /IntVetoSipm/hSipmAmpl_s0_l0_c9_r1\n"
"// hnamepath: /IntVetoSipm/hSipmAmpl_s0_l0_c10_r1\n"
"// hnamepath: /IntVetoSipm/hSipmAmpl_s0_l0_c11_r1\n"
"// hnamepath: /IntVetoSipm/hSipmAmpl_s0_l0_c12_r1\n"
"// hnamepath: /IntVetoSipm/hSipmAmpl_s0_l0_c13_r1\n"
"\n"
"{\n"
"\n"
"	TDirectory *dir = (TDirectory*)gDirectory->FindObjectAny(\"IntVetoSipm\");\n"
"	if(dir) dir->cd();\n"
"\n"
"	const int nChannels=13;\n"
"\n"
"	TH1D *hQ[nChannels];\n"
"	TH1D *hA[nChannels];\n"
"\n"
"	for (int ii=0;ii<nChannels;ii++) {\n"
"		hQ[ii]= (TH1D*)gDirectory->FindObjectAny(Form(\"hSipmCharge_s%i_l%i_c%i_r%i\",0,0,ii+1,1));\n"
"		hA[ii]= (TH1D*)gDirectory->FindObjectAny(Form(\"hSipmAmpl_s%i_l%i_c%i_r%i\",0,0,ii+1,1));\n"
"\n"
"	}\n"
"\n"
"\n"
"	if(gPad == NULL) {\n"
"		TCanvas *c1 = new TCanvas(\"cIntVetoSipm\");\n"
"		c1->cd(0);\n"
"		c1->Draw();\n"
"		c1->Update();\n"
"	}\n"
"\n"
"	if(!gPad) return;\n"
"	TCanvas *c1 = gPad->GetCanvas();\n"
"	c1->Divide(4, 4);\n"
"\n"
"	for (int ii=0;ii<nChannels;ii++) {\n"
"		c1->cd(ii+1);\n"
"		gPad->SetTicks();\n"
"		gPad->SetGrid();\n"
"		if (hQ[ii]){\n"
"			hQ[ii]->SetLineWidth(2);\n"
"			hQ[ii]->Draw(\"HIST\");\n"
"		}\n"
"		if (hA[ii]) {\n"
"			hA[ii]->SetLineWidth(2);\n"
"			hA[ii]->SetLineColor(2);\n"
"			hA[ii]->Draw(\"HISTSAME\");\n"
"		}\n"
"	}\n"
"}\n"
;
class IntVetoSipm_JLabFlux_rootspy_class{
   public:
   typedef void rmfcn(string, string, string);
   IntVetoSipm_JLabFlux_rootspy_class(){
      rmfcn* fcn = (rmfcn*)dlsym(RTLD_DEFAULT, "REGISTER_ROOTSPY_MACRO");
      if(fcn) (*fcn)("IntVetoSipm_JLabFlux","/", macro_data);
   }
};
static IntVetoSipm_JLabFlux_rootspy_class tmp;