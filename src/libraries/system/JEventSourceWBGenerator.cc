// C++ headers
#include <string>
using namespace std;

#include <JANA/JParameterManager.h>

// bdx headers
#include <system/JEventSourceWBGenerator.h>
#include <DAQ/JEventSourceWBDAQ.h>

#ifdef MC_SUPPORT_ENABLE
#include <MC/JEventSourceWBMC.h>
#endif
JEventSourceWBGenerator::JEventSourceWBGenerator() :
		isMC(0) {
#ifdef MC_SUPPORT_ENABLE
	gPARMS->SetDefaultParameter("MC", isMC, "Flag to select if analysis is runned on a MC file. 0 Means real data (default). >=1 means MC, according to: \n 1 CataniaProtoV1,\n 2 -> CataniaProtoV2,\n 4-> JLabFlux, \n 10->FullMC");
#endif
}

// Describes the file format
const char* JEventSourceWBGenerator::Description(void) {
	return "WB";
}

// Check if it's a valid file
double JEventSourceWBGenerator::CheckOpenable(string source) {
	/*Return 1.0 if it contains "WB" string or the "ET:" string
	 */
	if ((source.find("wb") != std::string::npos)||( (source.find("WB") != std::string::npos))) {
		std::cout << "JEventSourceWBGenerator: source name " << source
				<< " contains \"WB\" substring. Open it" << endl;
		return 1.0;
	} else if (source.find("ET:") != std::string::npos) {
		std::cout << "JEventSourceWBGenerator: source name " << source
				<< " contains \"ET:\" substring . Open it" << endl;
		return 1.0;
	} else {
		std::cout << "JEventSourceWBGenerator failed on source " << source
				<< endl;
		return 0.0;
	}
}

// Make the file a JEventSource
JEventSource* JEventSourceWBGenerator::MakeJEventSource(string source) {
	/*Here is the trick for now. Since we do not have the same file format for MC and for Data,
	 * but both are WB, let's look at the option MC, and see if it is 0 or 1
	 */
	int m_isMC, m_verbose;
	m_isMC = 0;
	m_verbose = 0;
	gPARMS->GetParameter("MC", m_isMC);
	gPARMS->GetParameter("SYSTEM:VERBOSE", m_verbose);

	if (m_isMC == 0) {
		if (m_verbose > 2)
			jout << "JEventSourceWBGenerator::MakeJEventSource for DAQ "
					<< endl;
		return new JEventSourceWBDAQ(source.c_str());
	} else if (m_isMC >= 1) {

#ifdef  MC_SUPPORT_ENABLE
		if (m_verbose>2) jout<<" JEventSourceWBGenerator::MakeJEventSource for MC "<<endl;
		return new JEventSourceWBMC(source.c_str());
#else
		cerr
				<< "=== ERROR: MC source specified and this was compiled without    ==="
				<< endl;
		cerr
				<< "===        MC support. You need to recompile setting MC=1 ,i.e. ==="
				<< endl;
		cerr << "===        scons MC=1 								             ===" << endl;
		throw JException("Failed to open MC WB file - no MC support enabled");
#endif

	}

}

