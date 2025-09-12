#ifndef __MOTOR_H
#define __MOTOR_H

typedef struct{

 float Vx,Vy,Vomega;
 float component[4][2];	
	
 float Absolute_Vx,Absolute_Vy,Absolute_Vomega;
	
	
 float Target_Position[4];

 float Target_Velocity[4];
	
}Control_Info_Typedef;
	
	
	
extern 	Control_Info_Typedef 	Control_Info;







#endif