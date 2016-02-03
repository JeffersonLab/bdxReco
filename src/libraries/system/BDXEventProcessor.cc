// C++ headers
#include <cstdio>
#include <iostream>
using namespace std;

// bdx headers
#include "BDXEventProcessor.h"
#include "string_utilities.h"
//DAQ

// TT
#include <TT/TranslationTable.h>
//#include "ctofHitR.h"
//#include "marcoCluster.h"


#include "JOutput.h"
#include "JROOTOutput.h"

// Constructor
BDXEventProcessor::BDXEventProcessor(goptions bdxOpt):
m_output(0),m_tt(0)
{
	/* Opens output file if specified
	the string should be of the form
	type filename
	with type: TXT EVIO ROOT
	and filename anything
	 */
	string type,name,fname;
	optf  = bdxOpt.optMap["OUTPUT"].args;


	outType.assign(optf, 0, optf.find(",")) ;
	outFile.assign(optf,    optf.find(",") + 1, optf.size()) ;




	if(optf!= "none")
	{
		jout<<"Out file type is: "<<outType<<endl;
		if(outType == "root"){
			fname=TrimSpaces(outFile);
			m_output=new JROOTOutput(outFile);
		}
		else if (outType == "evio"){
			jerr<<"evio not yet implemented"<<endl;
		}
		else if (outType == "txt"){
			jerr<<"txt not yet implemented"<<endl;
		}
		else{
			jerr<<"file type not recognized"<<endl;
		}

	}

	//string hd_msg = bdxOpt.optMap["LOG_MSG"].args + " Event: >> " ;


}

// Destructor
BDXEventProcessor::~BDXEventProcessor()
{

}

// init
jerror_t BDXEventProcessor::init(void)
{

	jout<<"BDXEventProcessor::init is called"<<endl;
	// Create histograms here
	return NOERROR;
}

// brun
jerror_t BDXEventProcessor::brun(JEventLoop *eventLoop, int32_t runnumber)
{
	eventLoop->GetSingle(m_tt);
	return NOERROR;
}

// evnt
jerror_t BDXEventProcessor::evnt(JEventLoop *loop, uint64_t eventnumber)
{
	//
	//	vector<const marcoCluster*> marcoC;
	//	loop->Get(marcoC);

	//vector<const ctofHitR*> marcoC;
	//loop->Get(marcoC);

//	jout<<"Event: "<<eventnumber<<std::endl;


	return NOERROR;
}

// erun
jerror_t BDXEventProcessor::erun(void)
{
	// Any final calculations on histograms (like dividing them)
	// should be done here. This may get called more than once.
	return NOERROR;
}

// fini
jerror_t BDXEventProcessor::fini(void)
{
	// If another DEventProcessor is in the list ahead of this one, then
	// it will have finished before this is called. e.g. closed the
	// ROOT file!
	if (m_output){
		m_output->CloseOutput();
	}


	return NOERROR;
}

