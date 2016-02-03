#include "TranslationTable.h"

#include <JANA/JEventLoop.h>
#include <JANA/JParameterManager.h>
#include <JANA/JParameter.h>


#include <expat.h>
#include <sstream>


using namespace std;


// Use one translation table for all threads
pthread_mutex_t& TranslationTable::Get_TT_Mutex(void) const
{
	static pthread_mutex_t tt_mutex = PTHREAD_MUTEX_INITIALIZER;
	return tt_mutex;
}

bool& TranslationTable::Get_TT_Initialized(void) const
{
	static bool tt_initialized = false;
	return tt_initialized;
}

map<TranslationTable::csc_t, TranslationTable::ChannelInfo>& TranslationTable::Get_TT(void) const
{
	static map<TranslationTable::csc_t, TranslationTable::ChannelInfo> TT;
	return TT;
}

//...................................
// Less than operator for csc_t data types. This is used by
// the map<csc_t, XX> to order the entires by key
bool operator<(const TranslationTable::csc_t &a, const TranslationTable::csc_t &b) {
	if (a.rocid < b.rocid) return true;
	if (a.rocid > b.rocid) return false;
	if (a.slot < b.slot) return true;
	if (a.slot > b.slot) return false;
	if (a.channel < b.channel) return true;
	if (a.channel > b.channel) return false;
	return false;
}


TranslationTable::TranslationTable(JEventLoop *loop){


	// Default is to just read translation table from CCDB. If this fails,
	// then an attempt will be made to read from a file on the local disk.
	// The filename can be specified to be anything, but if the user specifies
	// this, then we assume that they want to use it and skip using the CCDB.
	// They may also specify that they want to skip checking the CCDB via
	// the "TT:NO_CCDB" parameter. This would only be useful if they want to
	// force the use of a local file named "tt.xml".
	NO_CCDB = false;
	XML_FILENAME = "tt.xml";
	VERBOSE = 0;
	SYSTEMS_TO_PARSE = "";
	gPARMS->SetDefaultParameter("TT:NO_CCDB", NO_CCDB,
			"Don't try getting translation table from CCDB and just look"
			" for file. Only useful if you want to force reading tt.xml."
			" This is automatically set if you specify a different"
			" filename via the TT:XML_FILENAME parameter.");
	JParameter *p = gPARMS->SetDefaultParameter("TT:XML_FILENAME", XML_FILENAME,
			"Fallback filename of translation table XML file."
			" If set to non-default, CCDB will not be checked.");
	if (p->GetDefault() != p->GetValue())
		NO_CCDB = true;
	gPARMS->SetDefaultParameter("TT:VERBOSE", VERBOSE,
			"Verbosity level for Applying Translation Table."
			" 0=no messages, 10=all messages.");

	ROCID_MAP_FILENAME = "rocid.map";
	gPARMS->SetDefaultParameter("TT:ROCID_MAP_FILENAME", ROCID_MAP_FILENAME,
			"Optional rocid to rocid conversion map for use with files"
			" generated with the non-standard rocid's");

	gPARMS->SetDefaultParameter("TT:SYSTEMS_TO_PARSE", SYSTEMS_TO_PARSE,
			"Comma separated list of systems to parse EVIO data for. "
			"Default is empty string which means to parse all. System "
			"names should be what is returned by DTranslationTable::DetectorName() .");

	// Initialize dedicated JStreamLog used for debugging messages
	ttout.SetTag("--- TT ---: ");
	ttout.SetTimestampFlag();
	ttout.SetThreadstampFlag();

	// Look for and read in an optional rocid <-> rocid translation table
	//ReadOptionalROCidTranslation();

	// Read in Translation table. This will create DChannelInfo objects
	// and store them in the "TT" map, indexed by csc_t objects
	ReadTranslationTable(loop->GetJCalibration());


}


//---------------------------------
// ~DTranslationTable    (Destructor)
//---------------------------------
TranslationTable::~TranslationTable()
{

}




//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//  The following routines access the translation table
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

static TranslationTable::Detector_t DetectorStr2DetID(string &type);
static void StartElement(void *userData, const char *xmlname, const char **atts);
static void EndElement(void *userData, const char *xmlname);




//---------------------------------
// ReadTranslationTable
//---------------------------------
void TranslationTable::ReadTranslationTable(JCalibration *jcalib)
{
	// It seems expat is not thread safe so we lock a mutex here and
	// read in the translation table just once
	pthread_mutex_lock(&Get_TT_Mutex());
	if (Get_TT_Initialized()) {
		pthread_mutex_unlock(&Get_TT_Mutex());
		return;
	}

	// String to hold entire XML translation table
	string tt_xml;

	// Try getting it from CCDB first
	if (jcalib && !NO_CCDB) {
		map<string,string> tt;
		string namepath = "Translation/DAQ2detector";
		jout << "Reading translation table from calib DB: " << namepath << " ..." << std::endl;
		jcalib->GetCalib(namepath, tt);
		if (tt.size() != 1) {
			jerr << " Error: Unexpected translation table format!" << std::endl;
			jerr << "        tt.size()=" << tt.size() << " (expected 1)" << std::endl;
		}else{
			// Copy table into tt string
			map<string,string>::iterator iter = tt.begin();
			tt_xml = iter->second;
		}
	}

	// If getting from CCDB fails, try just reading in local file
	if (tt_xml.size() == 0) {
		if (!NO_CCDB) jout << "Unable to get translation table from CCDB." << std::endl;
		jout << "Will try reading TT from local file: " << XML_FILENAME << std::endl;

		// Open file
		ifstream ifs(XML_FILENAME.c_str());
		if (! ifs.is_open()) {
			jerr << " Error: Cannot open file! Translation table unavailable." << std::endl;
			pthread_mutex_unlock(&Get_TT_Mutex());
			return;
		}

		// read lines into stringstream object
		stringstream ss;
		while (ifs.good()) {
			char line[4096];
			ifs.getline(line, 4096);
			ss << line;
		}

		// Close file
		ifs.close();

		// Copy from stringstream to tt
		tt_xml = ss.str();
	}

	// create parser and specify element handlers
	XML_Parser xmlParser = XML_ParserCreate(NULL);
	if (xmlParser == NULL) {
		jerr << "readTranslationTable...unable to create parser" << std::endl;
		exit(EXIT_FAILURE);
	}
	XML_SetElementHandler(xmlParser,StartElement,EndElement);
	XML_SetUserData(xmlParser, &Get_TT());

	// Parse XML string
	int status=XML_Parse(xmlParser, tt_xml.c_str(), tt_xml.size(), 1); // "1" indicates this is the final piece of XML
	if (status == 0) {
		jerr << "  ?readTranslationTable...parseXMLFile parse error for " << XML_FILENAME << std::endl;
		jerr << XML_ErrorString(XML_GetErrorCode(xmlParser)) << std::endl;
	}

	jout << Get_TT().size() << " channels defined in translation table" << std::endl;
	XML_ParserFree(xmlParser);

	pthread_mutex_unlock(&Get_TT_Mutex());
	Get_TT_Initialized() = true;
	if (VERBOSE > 6){
		std::map<TranslationTable::csc_t, TranslationTable::ChannelInfo>::const_iterator it;
		for (it=Get_TT().begin();it!=Get_TT().end();it++){
			jout<<"Crate: "<<(it)->first.rocid<<" Slot: "<<(it)->first.slot<<" "<<" Channel: "<<(it)->first.channel<<" Detector: "<<DetectorName((it)->second.det_sys)<<endl;
		}
	}

}




TranslationTable::ChannelInfo TranslationTable::getChannelInfo(const csc_t &csc) const{
	TranslationTable::ChannelInfo m_channel;
	/*Get the map, search for csc, return an iterator*/
	map<csc_t, ChannelInfo>::const_iterator iter = Get_TT().find(csc);
	if (iter == Get_TT().end()) {
		if (VERBOSE > 6){
			ttout << csc.rocid << " " << csc.slot << " " <<csc.channel << " ";
			ttout << "     - Didn't find it" << std::endl;
		}
		m_channel.det_sys=UNKNOWN_DETECTOR;
		return m_channel;
	}
	const ChannelInfo &chaninfo = iter->second;
	if (VERBOSE > 6){
		ttout << csc.rocid << " " << csc.slot << " " <<csc.channel << " ";
		ttout << "     - Found entry for: " << DetectorName(chaninfo.det_sys)<< std::endl;
	}
	m_channel=chaninfo;
	return m_channel;
}


TranslationTable::ChannelInfo TranslationTable::getChannelInfo(int crate,int slot,int channel) const{
	TranslationTable::csc_t csc = {crate,slot,channel};
	return this->getChannelInfo(csc);
}


//---------------------------------
// DetectorStr2DetID
//---------------------------------
TranslationTable::Detector_t DetectorStr2DetID(string &type)
{
	if ( type == "ext_veto" ) {
		return TranslationTable::EXT_VETO;
	} else if ( type == "int_veto" ) {
		return TranslationTable::INT_VETO;
	} else if ( type == "calo" ) {
		return TranslationTable::CALO;
	} else if ( type == "other" ) {
		return TranslationTable::OTHER;
	} else {
		return TranslationTable::UNKNOWN_DETECTOR;
	}
}












//---------------------------------
// StartElement
//---------------------------------
void StartElement(void *userData, const char *xmlname, const char **atts)
{
	static int crate=0, slot=0;

	static string type,Type;
//	int mc2codaType= 0;
	int channel = 0;
	string Detector, locSystem;
//	int end=0;
	int row=0,column=0,module=0,sector=0,layer=0,component=0,readout=0;
	int id=0;
//	int side=0;

	// This complicated line just recasts the userData pointer into
	// a reference to the "TT" member of the TranslationTable object
	// that called us.
	map<TranslationTable::csc_t, TranslationTable::ChannelInfo> &TT = *((map<TranslationTable::csc_t, TranslationTable::ChannelInfo>*)userData);

	// store crate summary info, fill both maps
	if (strcasecmp(xmlname,"bdx_translation_table") == 0) {
		// do nothing

	} else if (strcasecmp(xmlname,"crate") == 0) {
		for (int i=0; atts[i]; i+=2) {
			if (strcasecmp(atts[i],"number") == 0) {
				crate = atoi(atts[i+1]);
				break;
			}
		}

	} else if (strcasecmp(xmlname,"slot") == 0) {
		for (int i=0; atts[i]; i+=2) {
			if (strcasecmp(atts[i],"number") == 0) {
				slot = atoi(atts[i+1]);
			} else if (strcasecmp(atts[i],"type") == 0) {
				Type = string(atts[i+1]);
				type = string(atts[i+1]);
				std::transform(type.begin(), type.end(), type.begin(), (int(*)(int)) tolower);
			}
		}

		// The detID value set here shows up in the header of the Data Block Bank
		// of the output file. It should be set to one if this crate has JLab
		// made modules that output in the standard format (see document:
		// "VME Data Format Standards for JLAB Modules"). These would include
		// f250ADC, f125ADC, F1TDC, .... Slots containing other types of modules
		// (e.g. CAEN1290) should have their own unique detID. We use detID of
		// zero for non-digitizing modules like CPUs nd TIDs even though potentially,
		// one could read data from these.
		//      mc2codaType = ModuleStr2ModID(type);

	} else if (strcasecmp(xmlname,"channel") == 0) {

		for (int i=0; atts[i]; i+=2) {
			string tag(atts[i+0]);
			string sval(atts[i+1]);


			int ival = atoi(atts[i+1]);

			if (tag == "number")
				channel = ival;
			else if (tag == "detector")
				Detector = sval;
			else if (tag == "sector")
				sector = ival;
			else if (tag == "readout")
				readout = ival;
			else if ((tag == "row")||(tag == "iy"))
				row = ival;
			else if ((tag == "col")|| (tag == "column")||(tag == "ix"))
				column = ival;
			else if (tag == "module")
				module = ival;
			else if (tag == "sector")
				sector = ival;
			else if (tag == "layer")
				layer = ival;
			else if (tag == "component")
				component = ival;
			else if (tag == "id")
				id = ival;
		}


		TranslationTable::csc_t csc = {(uint32_t)crate,(uint32_t)slot,(uint32_t)channel};
		string detector = Detector;
		std::transform(detector.begin(), detector.end(), detector.begin(), (int(*)(int)) tolower);

		//string s="unknown::";

		// Common indexes
		TranslationTable::ChannelInfo &ci = TT[csc];
		ci.CSC = csc;
		//      ci.module_type = (DModuleType::type_id_t)mc2codaType;
		ci.det_sys = DetectorStr2DetID(detector);
		//     TranslationTable::Get_ROCID_By_System()[ci.det_sys].insert(crate);

		// detector-specific indexes
		//This is the part that - probably - has to be modified
		switch (ci.det_sys) {
		case TranslationTable::EXT_VETO:
			ci.ext_veto.sector = sector;
			ci.ext_veto.layer = layer;
			ci.ext_veto.component = component;
			ci.ext_veto.readout = readout;
			break;
		case TranslationTable::INT_VETO:
			ci.int_veto.sector = sector;
			ci.int_veto.layer = layer;
			ci.int_veto.component = component;
			ci.int_veto.readout = readout;
			break;
		case TranslationTable::CALO:
			ci.calo.sector = sector;
			ci.calo.x = column;  //A.C. fine, x is a column and y is a row
			ci.calo.y = row;
			ci.calo.readout = readout;
			break;
		case TranslationTable::OTHER:
			ci.other.id = id;
			break;
		case TranslationTable::UNKNOWN_DETECTOR:
		default:
			break;
		}

	} else {
		jerr << std::endl << std::endl
				<< "?startElement...unknown xml tag " << xmlname
				<< std::endl << std::endl;
	}

}


//--------------------------------------------------------------------------


void EndElement(void *userData, const char *xmlname) {
	// nothing to do yet...
}


//--------------------------------------------------------------------------





