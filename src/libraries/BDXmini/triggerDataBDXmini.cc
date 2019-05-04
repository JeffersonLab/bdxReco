#include "triggerDataBDXmini.h"

using namespace std;

triggerDataBDXmini::triggerDataBDXmini() {
	nTriggers = 0;

	for (int ii = 0; ii < nChansMAX; ii++) {
		channels[ii] = false;
		channelsTRG[ii] = false;
	}
	trgTimes.resize(nTriggersMAX);

}

unsigned int triggerDataBDXmini::getNTriggers() const {
	return nTriggers;
}
void triggerDataBDXmini::setNTriggers(unsigned int n) {
	nTriggers = n;
}

unsigned int triggerDataBDXmini::getNtriggers_single(int itrg) const {
	if ((itrg < 0) || (itrg >= nTriggersMAX)) {
		jout << "triggerDataBDXmini::getNtriggers_single error trigger number " << itrg << endl;
		return 0;
	}
	return trgTimes[itrg].size();
}

int triggerDataBDXmini::getTriggerTime_single(int itrg, int n) const {
	if ((itrg < 0) || (itrg >= nTriggersMAX)) {
		jout << "triggerDataBDXmini::getTriggerTime_single error trigger number" << itrg << endl;
		return 0;
	}
	if ((n < 0) || (n >= trgTimes[itrg].size())) {
		jout << "triggerDataBDXmini::getTriggerTime_single error on trigger count" << n << endl;
		return 0;
	}
	return trgTimes[itrg][n];
}

void triggerDataBDXmini::addTriggerTime_single(int itrg, int data) {
	if ((itrg < 0) || (itrg >= nTriggersMAX)) {
		jout << "triggerDataBDXmini::addTriggerTime_single error trigger number" << itrg << endl;
		return;
	}
	trgTimes[itrg].push_back(data);
}

bool triggerDataBDXmini::hasChannel(int ich) const {
	if ((ich < 0) || (ich > nChansMAX)) {
		jout << "triggerDataBDXmini::hasChannel error ch number " << ich << endl;
		return false;
	}
	return channels[ich];
}

bool triggerDataBDXmini::hasChannelTRG(int ich) const {
	if ((ich < 0) || (ich > nChansMAX)) {
		jout << "triggerDataBDXmini::hasChannelTRG error ch number " << ich << endl;
		return false;
	}
	return channelsTRG[ich];
}

void triggerDataBDXmini::setChannel(int ich) {
	if ((ich < 0) || (ich > nChansMAX)) {
		jout << "triggerDataBDXmini::setChannelTRG error ch number " << ich << endl;
		return;
	}
	channels[ich] = true;
}
void triggerDataBDXmini::setChannelTRG(int ich) {
	if ((ich < 0) || (ich > nChansMAX)) {
		jout << "triggerDataBDXmini::setChannelTRG error ch number " << ich << endl;
		return;
	}
	channelsTRG[ich] = true;
}
void triggerDataBDXmini::unsetChannel(int ich) {
	if ((ich < 0) || (ich > nChansMAX)) {
		jout << "triggerDataBDXmini::unsetChannel error ch number " << ich << endl;
		return;
	}
	channels[ich] = false;
}
void triggerDataBDXmini::unsetChannelTRG(int ich) {
	if ((ich < 0) || (ich > nChansMAX)) {
		jout << "triggerDataBDXmini::unsetChannelTRG error ch number " << ich << endl;
		return;
	}
	channelsTRG[ich] = false;
}
