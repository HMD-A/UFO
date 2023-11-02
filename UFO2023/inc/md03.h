//COM, ID, POW
__declspec(dllimport) void MD03SetMotor(int,unsigned char,int);

//COM, ID, DEG, POW
__declspec(dllimport) void MD03SetMotorDeg(int,unsigned char,int,int);
//COM, ID, DEG, SPD
__declspec(dllimport) void MD03SetMotorDegSpd(int,unsigned char,int,int);
//COM, ID, SPD
__declspec(dllimport) void MD03SetMotorSpd(int,unsigned char,int);

//P, I, D
__declspec(dllimport) void MD03SetPIDDeg(int,unsigned char,float,float,float);
//P, I, D
__declspec(dllimport) void MD03SetPIDSpd(int,unsigned char,float,float,float);

//Invert enable
__declspec(dllimport) void MD03SetEncInv(int,unsigned char,unsigned char);