// bdx reconstruction software

// JANA headers
#include <JANA/JApplication.h>
using namespace jana;

// bdx headers

#include <system/JEventSourceEVIOGenerator.h>
#include <system/JEventSourceTRIDASGenerator.h>
#include <system/BDXEventProcessor.h>

//factory generators
#include <system/JFactoryGenerator_system.h>
#include <DAQ/JFactoryGenerator_DAQ.h>
#include <TT/JFactoryGenerator_TT.h>
#include <IntVeto/JFactoryGenerator_IntVeto.h>
#include <Calorimeter/JFactoryGenerator_Calorimeter.h>
#include <Paddles/JFactoryGenerator_Paddles.h>
#include <BDXmini/JFactoryGenerator_BDXmini.h>
#include <EventBuilder/JFactoryGenerator_EventBuilder.h>

#ifdef MC_SUPPORT_ENABLE
#include <MC/JFactoryGenerator_MC.h>
#endif


#include "TH1D.h"
// C++ headers
#include <iostream>
using namespace std;




int main(int narg, char *argv[])
{
	/*A.C. If I de-comment following, RootSPy won't work!*/
	//TH1::AddDirectory(kFALSE);

	JApplication app(narg, argv);

	if(narg==1){
		app.Usage();
		return 0;
	}



	app.AddEventSourceGenerator(new JEventSourceEvioGenerator());
	jout<<"JEventSourceEvioGenerator DONE"<<endl;

	app.AddEventSourceGenerator(new JEventSourceTRIDASGenerator());
	jout<<"JEventSourceTRIDASGenerator DONE"<<endl;

	app.AddFactoryGenerator(new JFactoryGenerator_system());
	jout<<"JFactoryGenerator_system DONE"<<endl;
	app.AddFactoryGenerator(new JFactoryGenerator_DAQ());
	jout<<"JFactoryGenerator_DAQ DONE"<<endl;
#ifdef MC_SUPPORT_ENABLE
	app.AddFactoryGenerator(new JFactoryGenerator_MC());
	jout<<"JFactoryGenerator_MC DONE"<<endl;
#endif
	app.AddFactoryGenerator(new JFactoryGenerator_TT());
	jout<<"JFactoryGenerator_TT DONE"<<endl;
	app.AddFactoryGenerator(new JFactoryGenerator_IntVeto());
	jout<<"JFactoryGenerator_IntVeto DONE"<<endl;
	app.AddFactoryGenerator(new JFactoryGenerator_Calorimeter());
	jout<<"JFactoryGenerator_Calorimeter DONE"<<endl;
	app.AddFactoryGenerator(new JFactoryGenerator_Paddles());
	jout<<"JFactoryGenerator_Paddles DONE"<<endl;
	app.AddFactoryGenerator(new JFactoryGenerator_BDXmini());
	jout<<"JFactoryGenerator_BDXmini DONE"<<endl;
	app.AddFactoryGenerator(new JFactoryGenerator_EventBuilder());
	jout<<"JFactoryGenerator_EventBuilder DONE"<<endl;
	app.AddProcessor(new BDXEventProcessor());
	jout<<"BDXEventProcessor DONE"<<endl;
	app.Run();

	return 1;
}


