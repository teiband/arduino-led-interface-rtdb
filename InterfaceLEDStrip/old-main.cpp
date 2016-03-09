/*
 * main.cpp
 *
 *  Created on: 21 Oct 2015
 *      Author: Thomas Eiband
 *
 *      thomas.eiband@tum.de
 */

#include "arduino.h"
#include <getopt.h>
#include <iostream>
#include <unistd.h> // for sleep()

using namespace std;

/*
static struct option long_options[] = {
   {"device", required_argument, 0, 'd'},
   {"verbose", no_argument, 0, 'v'},
   {0,0,0,0}
};
*/

int main(int argc, char *argv[])
{
    char *deviceName = "/dev/ttyACM0";
    arduino ard(deviceName);


    char color[3];

    ard.createColorTriple(0x00, 0x06, 0x00, color);

    ard.fillColor(color);

    ard.write();

    // ard.read();





    /*
    char data[] = {0x00, 0x00, 0xFF};  //Random data we want to send
    FILE *file;
    file = fopen("/dev/ttyACM0","w");  //Opening device file
    int i = 0;
    for(i = 0 ; i < 3 ; i++)
    {
        fprintf(file,"%d",data[i]); //Writing to the file
        // sleep(1);
    }
    fclose(file);
    */


    /*
    int data[] = {10,5,13};  //Random data we want to send
    FILE *file;
    file = fopen("/dev/ttyACM0","w");  //Opening device file
    int i = 0;
    for(i = 0 ; i < 3 ; i++)
    {
        fprintf(file,"%d",data[i]); //Writing to the file
        fprintf(file,"%c",','); //To separate digits
        sleep(1);
    }
    fclose(file);
    */


    ///////////////////////////////////
    // Multiplus stuff as template:
//    int o;
//    int optionindex;
//    std::string device_name = "/dev/multiplus";
//    bool verbose_mode = false;

//    while ((o = getopt_long(argc, argv, "d:v", long_options, &optionindex)) != -1) {
//        switch(o) {
//            case 'd':
//                device_name = optarg;
//                break;
//	    case 'v':
//		verbose_mode = true;
//		break;
//            case '?':
//                std::cerr << "Usage: " << argv[0] << " [--device /dev/multiplus]" << std::endl;
//                exit(1);
//                break;
//            default:
//                abort();
//        }

//    }


//    char *device_name_c = new char[device_name.size() + 1];
//    std::copy(device_name.begin(), device_name.end(), device_name_c);
//    device_name_c[device_name.size()] = '\0';

//    cout << "Establishing database connection ..." << endl;
//    RTDBConn DBC("MultiPlus", 5.0);
//    if (verbose_mode)
//    cout << "Database connection established!" << endl << endl;

//    // Create rtdb database object with name MultiPlusData
//    MultiplusData rtdb_obj(DBC, "MultiPlusData");

//    // create Multiplus object with serial connection to device and pointer to database subobject
//    cout << "Create Multiplus object and establish serial connection ..." << endl;
//    Multiplus mp((device_name_c), rtdb_obj.subobj_p);
//    cout << "Connected to Multiplus" << endl << endl;

//    rtdb_obj.RTDBInsert();

//    cout << "Start infinite loop and write periodically to database ..." << endl << endl;
//    for(;;) {

//        cout << "Multiplus.update() ..." << endl << endl;
//        mp.update();
//        cout << endl << "MultiplusData object updated!" << endl << endl;

//        /*
//        //dummy for database testing
//        rtdb_obj.subobj_p->Blink.mains = false;
//        rtdb_obj.subobj_p->On.mains = !rtdb_obj.subobj_p->On.mains;
//        */

//        cout << "Write to RTDB ..." << endl << endl;
//        rtdb_obj.RTDBWrite();
//        cout << "Write to RTDB successfull!" << endl << endl;
//        sleep(1);

//        // wait until cycle done
//        DBC.CycleDone();
//    }


    return 0;
}
