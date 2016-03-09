#include "ArduinoData.h"
#include <string>
#include <iomanip>

using namespace KogniMobil;
using namespace std;

ArduinoData::
ArduinoData (	class RTDBConn& DBC,
		const char* name,
		const int& otype) 
            : ArduinoData_T(DBC, name, otype) {
	data = subobj_p;
}

ArduinoData::~ArduinoData(void) {

}



std::string ArduinoData::dump(void) const {

    std::ostringstream ostr;

    ostr << "* LED Pattern (On='X' / Off='0')" << endl;
    for (int i=0; i < NUMPIXELS; i++) {
        ostr << (data->pattern != 0 ? "X" : "0");
        ostr << (i%10 == 0 ? endl : "");
    }

    ostr << endl;

    // do not touch :-)
    ostr << std::endl << ArduinoData_T::dump() << std::endl;
	return ostr.str();
}

