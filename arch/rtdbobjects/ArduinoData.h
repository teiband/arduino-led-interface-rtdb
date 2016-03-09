#ifndef __ARDUINODATA_H__
#define __ARDUINODATA_H__



#ifdef __cplusplus
    #include <kogmo_rtdb.hxx>
	namespace KogniMobil {
	extern "C" {
#endif

#include <kogmo_rtdb.h>
#include <array>
#include "../../InterfaceLEDStrip/arduino_config.h"

/** Subobject of RTDB object, which contains payload
*/
typedef struct {
    std::array< std::array<char, PATTERN_DEPTH>, NUMPIXELS> pattern;
    bool emergency_state = false;
} subobj_arduino_t;

/** RTDB object, which contains the subobject and additional information for the database
*/
typedef struct {
    kogmo_rtdb_subobj_base_t base;
    subobj_arduino_t arduino_data;
} obj_arduino_t;

#ifdef __cplusplus
}; /* extern "C" */

    /**
 * @brief ArduinoData_T is the parent class of ArduinoData
 */
typedef RTDBObj_T < subobj_arduino_t, TYPEID_arduino_DATA, RTDBObj > ArduinoData_T;

/** Data class to handle the RTDB object and writing to the RTDB
 */
class ArduinoData : public ArduinoData_T {
	private:

	protected:
		subobj_arduino_t *data;
	public:
	ArduinoData (class RTDBConn& DBC, const char* name = "", const int& otype = TYPEID_ARDUINO_DATA);

	~ArduinoData();

	/**
	 * @brief Dump function, which converts the RTDB object's data to a legible string.
	 * @return string, which is displayed in the ARCADE inspector (GUI of the RTDB)
	 */
		std::string dump(void) 		const;
		
};

inline std::ostream& operator<< (std::ostream& out, const ArduinoData& O) {
        return out << O.dump() << std::endl;
}

};     // namespace KogniMobil
#endif //__cplusplus

#endif //__ARDUINODATA_H__

