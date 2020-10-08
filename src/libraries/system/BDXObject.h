/*
 * BDXObject.h
 *
 *  Created on: Apr 3, 2016
 *      Author: celentan
 */

#ifndef SRC_LIBRARIES_SYSTEM_BDXOBJECT_H_
#define SRC_LIBRARIES_SYSTEM_BDXOBJECT_H_

#include <JANA/JObject.h>
#include <JANA/JFactory.h>

#include "TObject.h"

using namespace jana;
using namespace std;

class TCanvas;

class BDXObject: public jana::JObject {
public:
	BDXObject();
	virtual ~BDXObject();
	virtual TCanvas* Draw(int id = 0) const {
		return m_canvas;
	} //!

protected:
	mutable TCanvas *m_canvas; //!

	ClassDef(BDXObject,1);

};

#endif /* SRC_LIBRARIES_SYSTEM_BDXOBJECT_H_ */
