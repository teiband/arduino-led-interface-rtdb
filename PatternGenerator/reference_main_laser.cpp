/* asctime example */
#include <stdio.h>      /* printf */
#include <time.h>       /* time_t, struct tm, time, localtime, asctime */
#include <fstream>
#include <Pos2d.h>
#include <Pose2d.h>
#include <LaserData.h>
#include <MoCapData.h>
#include <signal.h>

using namespace std;
using namespace KogniMobil;

#define NOT_VALID nan	
volatile sig_atomic_t stop = 0;

void
inthand(int signum)
{
    stop = 1;
}


int main ()
{
	RTDBConn DBC("LoggingEffortlessDW", 1, "");
	string logPath = getenv("PROJECTROOT") ;
	logPath = logPath + "/log/";
	string separator(",");

	//Dataypes
	Pos2d ptu_data(DBC);
	
	Pose2d tcp_velocity(DBC);
	Pose2d desired_tcp_velocity(DBC);
	
	MoCapData head(DBC);
	MoCapData shoulder(DBC);
	MoCapData table_red(DBC);
	MoCapData wrist(DBC);

	LaserData laser_front(DBC);
	LaserData laser_rear(DBC);
	
	//RTDB Names
	string ptu_data_rtdbname("PTU_data");
	
	string tcp_velocity_rtdbname("tcpVelocity");
	string desired_tcp_velocity_rtdbname("desiredTcpVelocity");
	
	string head_rtdbname("cap");
	string shoulder_rtdbname("shoulder");
	string table_red_rtdbname("table");
	string wrist_rtdbname("wrist");

	string laser_front_rtdbname("laser_front");
	string laser_rear_rtdbname("laser_rear");
	
	// // Timing variables
	// char *str_now;
 //  	time_t secs_now;
 //  	size_t length;

	// time(&secs_now);

	// str_now = ctime(&secs_now);
 //  	length = strlen(str_now);
 //  	str_now[length-1]=NULL;


  	//Filenames
	time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
  	// Construct Filename with date/time info
    std::string fileStamp(asctime (timeinfo));
    fileStamp="_" + fileStamp;
	//Delete unneccessary symbols from the filename!!
    while ( fileStamp.find(" ") < fileStamp.size() ){
       fileStamp.replace(fileStamp.find(" "),1,"-");
   }
    while ( fileStamp.find("--") < fileStamp.size() ){
       fileStamp.replace(fileStamp.find("--"),2,"-");
   }
    while ( fileStamp.find(":") < fileStamp.size() ){
        fileStamp.replace(fileStamp.find(":"),1,"-");
    }
    fileStamp.replace(fileStamp.find("\n"),1,"");
	// End Delete Unneccessary symbols!

	// use relative path for the file to data/...  
    string fileNamePTUData=logPath + ptu_data_rtdbname;
    fileNamePTUData=fileNamePTUData + fileStamp;
    fileNamePTUData = fileNamePTUData + ".csv";
    
    string fileNameTcpVelocity=logPath + tcp_velocity_rtdbname;
    fileNameTcpVelocity=fileNameTcpVelocity + fileStamp;
    fileNameTcpVelocity = fileNameTcpVelocity + ".csv";

    string fileNameDesiredTcpVelocity=logPath + desired_tcp_velocity_rtdbname;
    fileNameDesiredTcpVelocity=fileNameDesiredTcpVelocity + fileStamp;
    fileNameDesiredTcpVelocity=fileNameDesiredTcpVelocity + ".csv";

    string fileNameLaserRear=logPath + laser_rear_rtdbname;
    fileNameLaserRear=fileNameLaserRear + fileStamp;
    fileNameLaserRear=fileNameLaserRear + ".csv";

    string fileNameLaserFront=logPath + laser_front_rtdbname;
    fileNameLaserFront=fileNameLaserFront + fileStamp;
    fileNameLaserFront=fileNameLaserFront + ".csv";

	string fileNameHead=logPath + head_rtdbname;
    fileNameHead=fileNameHead + fileStamp;
    fileNameHead=fileNameHead + ".csv";

	string fileNameShoulder=logPath + shoulder_rtdbname;
    fileNameShoulder=fileNameShoulder + fileStamp;
    fileNameShoulder=fileNameShoulder + ".csv";

    string fileNameTableRed=logPath + table_red_rtdbname;
    fileNameTableRed=fileNameTableRed + fileStamp;
    fileNameTableRed=fileNameTableRed + ".csv";
	
    string fileNameWrist=logPath + wrist_rtdbname;
    fileNameWrist=fileNameWrist + fileStamp;
    fileNameWrist=fileNameWrist + ".csv";

    std::cout << fileNamePTUData << std::endl;
	std::cout << fileNameTcpVelocity << std::endl;
	std::cout << fileNameDesiredTcpVelocity << std::endl;
	std::cout << fileNameLaserRear << std::endl;
	std::cout << fileNameLaserFront << std::endl;
	std::cout << fileNameHead << std::endl;
	std::cout << fileNameShoulder << std::endl;
	std::cout << fileNameTableRed << std::endl;
	std::cout << fileNameWrist << std::endl;
	

	ofstream myfile_ptu_data;
	ofstream myfile_tcp_velocity;
	ofstream myfile_desired_tcp_velocity;
	ofstream myfile_laser_rear;
	ofstream myfile_laser_front;
	ofstream myfile_head;
	ofstream myfile_shoulder;
	ofstream myfile_table_red;
	ofstream myfile_wrist;

	// Open the file in order to write important data
    myfile_ptu_data.open(fileNamePTUData.c_str());
    myfile_tcp_velocity.open(fileNameTcpVelocity.c_str());
    myfile_desired_tcp_velocity.open(fileNameDesiredTcpVelocity.c_str());
    myfile_laser_rear.open(fileNameLaserRear.c_str());
    myfile_laser_front.open(fileNameLaserFront.c_str());
    myfile_head.open(fileNameHead.c_str());
    myfile_shoulder.open(fileNameShoulder.c_str());
    myfile_table_red.open(fileNameTableRed.c_str());
    myfile_wrist.open(fileNameWrist.c_str());

	myfile_ptu_data << "x()" << separator;
	myfile_ptu_data << "y() ";
	myfile_ptu_data << "\n";

	myfile_tcp_velocity << "x()" << separator;
	myfile_tcp_velocity << "y()" << separator;
	myfile_tcp_velocity << "phi()";
	myfile_tcp_velocity << "\n";

	myfile_desired_tcp_velocity << "x()" << separator;
	myfile_desired_tcp_velocity << "y()" << separator;
	myfile_desired_tcp_velocity << "phi()";
	myfile_desired_tcp_velocity << "\n";

	myfile_head << "t.x" << separator << "t.y" << separator << "t.z" << separator;
	myfile_head << "o.qw" << separator << "o.qx" << separator << "o.qy" << separator << "o.qz";
	myfile_head << "\n";
	
	myfile_shoulder << "t.x" << separator << "t.y" << separator << "t.z" << separator;
	myfile_shoulder << "o.qw" << separator << "o.qx" << separator << "o.qy" << separator << "o.qz";
	myfile_shoulder << "\n";
	
	myfile_table_red << "t.x" << separator << "t.y" << separator << "t.z" << separator;
	myfile_table_red << "o.qw" << separator << "o.qx" << separator << "o.qy" << separator << "o.qz";
	myfile_table_red << "\n";
	
	myfile_wrist << "t.x" << separator << "t.y" << separator << "t.z" << separator;
	myfile_wrist << "o.qw" << separator << "o.qx" << separator << "o.qy" << separator << "o.qz";
	myfile_wrist << "\n";


	myfile_laser_front << "local(i).x" << separator << "local(i).y" ;
	myfile_laser_front << "\n";

	myfile_laser_rear << "local(i).x" << separator << "local(i).y" ;
	myfile_laser_rear << "\n";

    signal(SIGINT, inthand);
	
	while(!stop) {
		
		// PTU Data
		try {
			ptu_data.RTDBRead();
			myfile_ptu_data << ptu_data.x() << separator;
			myfile_ptu_data << ptu_data.y() ;
			myfile_ptu_data << "\n";
		} catch(DBError err) {
			std::cerr << "Cannot read RTDB Object." << std::endl << err.what() << std::endl;	
			try {
				ptu_data.RTDBSearch(ptu_data_rtdbname.c_str());
				ptu_data.RTDBRead();
				std::cout << "Successfully read " << ptu_data_rtdbname << std::endl;
			} catch (DBError err) {
				std::cerr << "Object not found." << std::endl;
				std::cout << err.what();
				//usleep(1000000);
				//continue;
			}
		}
		

		try {
			tcp_velocity.RTDBRead();
			myfile_tcp_velocity << tcp_velocity.x() << separator;
			myfile_tcp_velocity << tcp_velocity.y() << separator;
			myfile_tcp_velocity << tcp_velocity.phi();
			myfile_tcp_velocity << "\n";
		} catch(DBError err) {
			std::cerr << "Cannot read RTDB Object." << std::endl << err.what() << std::endl;	
			try {
				tcp_velocity.RTDBSearch(tcp_velocity_rtdbname.c_str());
				tcp_velocity.RTDBRead();
				std::cout << "Successfully read " << tcp_velocity_rtdbname << std::endl;
			} catch (DBError err) {
				std::cerr << "Object not found." << std::endl;
				std::cout << err.what();
				//usleep(1000000);
				//continue;
			}
		}

		try {
			desired_tcp_velocity.RTDBRead();
			myfile_desired_tcp_velocity << desired_tcp_velocity.x() << separator;
			myfile_desired_tcp_velocity << desired_tcp_velocity.y() << separator;
			myfile_desired_tcp_velocity << desired_tcp_velocity.phi();
			myfile_desired_tcp_velocity << "\n";
		} catch(DBError err) {
			std::cerr << "Cannot read RTDB Object." << std::endl << err.what() << std::endl;	
			try {
				desired_tcp_velocity.RTDBSearch(desired_tcp_velocity_rtdbname.c_str());
				desired_tcp_velocity.RTDBRead();
				std::cout << "Successfully read " << desired_tcp_velocity_rtdbname << std::endl;
			} catch (DBError err) {
				std::cerr << "Object not found." << std::endl;
				std::cout << err.what();
				//usleep(1000000);
				//continue;
			}
		}
		

		try {
			head.RTDBRead();
			myfile_head << head.getPose3d().t.x << separator << head.getPose3d().t.y << separator << head.getPose3d().t.z << separator;
			myfile_head << head.getPose3d().o.qw << separator << head.getPose3d().o.qx << separator << head.getPose3d().o.qy << separator << head.getPose3d().o.qz;
			myfile_head << "\n";
		} catch(DBError err) {
			std::cerr << "Cannot read RTDB Object." << std::endl << err.what() << std::endl;	
			try {
				head.RTDBSearch(head_rtdbname.c_str());
				head.RTDBRead();
				std::cout << "Successfully read " << head_rtdbname << std::endl;
			} catch (DBError err) {
				std::cerr << "Object not found." << std::endl;
				std::cout << err.what();
				//usleep(1000000);
				//continue;
			}
		}
		

		try {
			shoulder.RTDBRead();
			myfile_shoulder << shoulder.getPose3d().t.x << separator << shoulder.getPose3d().t.y << separator << shoulder.getPose3d().t.z << separator;
			myfile_shoulder << shoulder.getPose3d().o.qw << separator << shoulder.getPose3d().o.qx << separator << shoulder.getPose3d().o.qy << separator << shoulder.getPose3d().o.qz;
			myfile_shoulder << "\n";
		} catch(DBError err) {
			std::cerr << "Cannot read RTDB Object." << std::endl << err.what() << std::endl;	
			try {
				shoulder.RTDBSearch(shoulder_rtdbname.c_str());
				shoulder.RTDBRead();
				std::cout << "Successfully read " << shoulder_rtdbname << std::endl;
			} catch (DBError err) {
				std::cerr << "Object not found." << std::endl;
				std::cout << err.what();
				//usleep(1000000);
				//continue;
			}
		}
		


		try {
			table_red.RTDBRead();
			myfile_table_red << table_red.getPose3d().t.x << separator << table_red.getPose3d().t.y << separator << table_red.getPose3d().t.z << separator;
			myfile_table_red << table_red.getPose3d().o.qw << separator << table_red.getPose3d().o.qx << separator << table_red.getPose3d().o.qy << separator << table_red.getPose3d().o.qz;
			myfile_table_red << "\n";
		} catch(DBError err) {
			std::cerr << "Cannot read RTDB Object." << std::endl << err.what() << std::endl;	
			try {
				table_red.RTDBSearch(table_red_rtdbname.c_str());
				table_red.RTDBRead();
				std::cout << "Successfully read " << table_red_rtdbname << std::endl;
			} catch (DBError err) {
				std::cerr << "Object not found." << std::endl;
				std::cout << err.what();
				//usleep(1000000);
				//continue;
			}
		}
		

		try {
			wrist.RTDBRead();
			myfile_wrist << wrist.getPose3d().t.x << separator << wrist.getPose3d().t.y << separator << wrist.getPose3d().t.z << separator;
			myfile_wrist << wrist.getPose3d().o.qw << separator << wrist.getPose3d().o.qx << separator << wrist.getPose3d().o.qy << separator << wrist.getPose3d().o.qz;
			myfile_wrist << "\n";
		} catch(DBError err) {
			std::cerr << "Cannot read RTDB Object." << std::endl << err.what() << std::endl;	
			try {
				wrist.RTDBSearch(table_red_rtdbname.c_str());
				wrist.RTDBRead();
				std::cout << "Successfully read " << wrist_rtdbname << std::endl;
			} catch (DBError err) {
				std::cerr << "Object not found." << std::endl;
				std::cout << err.what();
				//usleep(1000000);
				//continue;
			}
		}
	
		try {
			laser_rear.RTDBRead();
			for (unsigned int i=0;i<(laser_rear.size()-1) ;i++){
				myfile_laser_rear << laser_rear.local(i).x << separator << laser_rear.local(i).y << separator ;
			}
			myfile_laser_rear << laser_rear.local(laser_rear.size()-1).x << separator << laser_rear.local(laser_rear.size()-1).y ;
			myfile_laser_rear << "\n";
		} catch(DBError err) {
			std::cerr << "Cannot read RTDB Object." << std::endl << err.what() << std::endl;	
			try {
				laser_rear.RTDBSearch(laser_rear_rtdbname.c_str());
				laser_rear.RTDBRead();
				std::cout << "Successfully read " << laser_rear_rtdbname << std::endl;
			} catch (DBError err) {
				std::cerr << "Object not found." << std::endl;
				std::cout << err.what();
				//usleep(1000000);
				//continue;
			}
		}


		try {
			laser_front.RTDBRead();
			for (unsigned int i=0;i<(laser_front.size()-1) ;i++){
				myfile_laser_front << laser_front.local(i).x << separator << laser_front.local(i).y << separator ;
			}
			myfile_laser_front << laser_front.local(laser_front.size()-1).x << separator << laser_front.local(laser_front.size()-1).y ;
			myfile_laser_front << "\n";
		} catch(DBError err) {
			std::cerr << "Cannot read RTDB Object." << std::endl << err.what() << std::endl;	
			try {
				laser_front.RTDBSearch(laser_front_rtdbname.c_str());
				laser_front.RTDBRead();
				std::cout << "Successfully read " << laser_front_rtdbname << std::endl;
			} catch (DBError err) {
				std::cerr << "Object not found." << std::endl;
				std::cout << err.what();
				//usleep(1000000);
				//continue;
			}
		}
		
		myfile_ptu_data.flush();
		myfile_tcp_velocity.flush();
		myfile_desired_tcp_velocity.flush();
		myfile_laser_rear.flush();
		myfile_laser_front.flush();
		myfile_head.flush();
		myfile_shoulder.flush();
		myfile_table_red.flush();
		myfile_wrist.flush();

		
		usleep(1e6/20);
	}

	myfile_ptu_data.close();
	
	myfile_tcp_velocity.close();
	myfile_desired_tcp_velocity.close();

	myfile_head.close();
	myfile_shoulder.close();
	myfile_table_red.close();
	myfile_wrist.close();

	myfile_laser_rear.close();
	myfile_laser_front.close();

}
