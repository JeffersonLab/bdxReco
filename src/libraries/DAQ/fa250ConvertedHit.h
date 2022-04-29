/*
 * faConvertedHit.h
 *
 *  Created on: Jan 21, 2016
 *      Author: celentan
 */

#ifndef FAINTEGRATEDHIT_H_
#define FAINTEGRATEDHIT_H_

#include <JANA/JObject.h>
#include <JANA/JFactory.h>
#include "TObject.h"

/*A common class for fad250 converted hits. Can be useful*/
class fa250ConvertedHit :public jana::JObject {
public:
	JOBJECT_PUBLIC(fa250ConvertedHit);
	fa250ConvertedHit(){};
	virtual ~fa250ConvertedHit(){};

	ClassDef(fa250ConvertedHit,1);

};



#endif /* FAINTEGRATEDHIT_H_ */

