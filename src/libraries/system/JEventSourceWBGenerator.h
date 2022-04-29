#ifndef _JEventSourceWBGenerator_
#define _JEventSourceWBGenerator_

// JANA headers
#include "JANA/JEventSourceGenerator.h"
using namespace jana;

class JEventSourceWBGenerator:public JEventSourceGenerator
{
private:
	int isMC;
	public:
		 JEventSourceWBGenerator();
		~JEventSourceWBGenerator(){}


		const char* className(void)               {return static_className();}
		static const char* static_className(void) {return "JEventSourceWBGenerator";}
		
		const char* Description(void);
		
		double CheckOpenable(string source);
		
		JEventSource* MakeJEventSource(string source);
		

};

#endif // _JEventSourceWBGenerator_

