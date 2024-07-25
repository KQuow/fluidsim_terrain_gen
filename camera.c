#include <windows.h>
#include <gl/gl.h>
#include <math.h>

#include <stddef.h>
#include <stdlib.h>

#include "camera.h"


float lerp(float a,float b,float t) {return(a+(b-a)*t);}


struct CAM {
	float x;float y;float z;
	float yaw;float pitch;float roll;
} cam;

void camera_init() {
	cam.x=32.0;cam.y=32.0;cam.z=4.0;
	cam.yaw=45.0;cam.pitch=90.0;cam.roll=0.0;
}

void camera_move() {
	float spd=0.1;

	if(GetKeyState(VK_DOWN)		<0)	cam.pitch	=--cam.pitch<0		? 0	: cam.pitch;
	if(GetKeyState(VK_UP)		<0)	cam.pitch	=++cam.pitch>180	? 180	: cam.pitch;
	if(GetKeyState(VK_RIGHT)	<0)	cam.yaw		=--cam.yaw<0		? 360	: cam.yaw;
	if(GetKeyState(VK_LEFT)		<0)	cam.yaw		=++cam.yaw>360		? 0		: cam.yaw;

	float ang=-cam.yaw/180*M_PI;
	float halfP=M_PI*0.5;

	if(GetKeyState('W')<0)	{
		cam.x+=spd*sin(ang);
		cam.y+=spd*cos(ang);
	}
	if(GetKeyState('S')<0)	{
		cam.x-=spd*sin(ang);
		cam.y-=spd*cos(ang);
	}
	if(GetKeyState('A')<0)	{
		cam.x+=spd*sin(ang-halfP);
		cam.y+=spd*cos(ang-halfP);
	}
	if(GetKeyState('D')<0)	{
		cam.x+=spd*sin(ang+halfP);
		cam.y+=spd*cos(ang+halfP);
	}

	if(GetKeyState(VK_SPACE)<0)		cam.z+=spd;
	if(GetKeyState(VK_CONTROL)<0)	cam.z-=spd;

	glRotatef(-cam.pitch,1,0,0);
	glRotatef(-cam.yaw,0,0,1);
	glTranslatef(-cam.x,-cam.y,-cam.z);
}


void camera_set_z(float grid[256][256]) {
	float _xoff=cam.x-floor(cam.x);
	float _yoff=cam.y-floor(cam.y);

	float highg=(
		lerp(grid[(int)(floor(cam.x))][(int)(floor(cam.y))], grid[(int)(ceil(cam.x))][(int)(floor(cam.y))], _xoff)+
		lerp(grid[(int)(floor(cam.x))][(int)(floor(cam.y))], grid[(int)(floor(cam.x))][(int)(ceil(cam.y))], _yoff)
	)*0.5+5.0;

	cam.z=lerp(cam.z,highg,0.3);
}




