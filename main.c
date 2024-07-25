#include <windows.h>
#include <gl/gl.h>
#include <math.h>

#include "camera.h"


#define window_width	LOWORD(lParam)
#define window_height 	HIWORD(lParam)



#define GRIDW	256
#define GRIDH	256


LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);



float timer=0.0;
float morphGrid[GRIDW][GRIDH];
float vecGridX[GRIDW][GRIDH];
float vecGridY[GRIDW][GRIDH];

boolean	fluidsim_active=FALSE;
boolean	mouse_pressed=FALSE;
float mouse_x=0.0;
float mouse_y=0.0;


float distance(float x1,float y1,float x2,float y2) {x2-=x1;y2-=y1;return(sqrt(x2*x2+y2*y2));}

inline int wrap(int x) {if(x>255) return(0);if(x<0) return(255);return(x);}

void addto_world() {
	float mm,_dis;
	for(int yy=0;yy<GRIDH;yy++) {
	for(int xx=0;xx<GRIDW;xx++) {
		_dis=distance(mouse_x,mouse_y,(float)xx,(float)yy);
		if(_dis<16.0) {
			mm=cosf(_dis/16.0*1.57);
			morphGrid[xx][yy]+=mm*5.0;
		}
	}}
}


void update() {
	if(fluidsim_active) {
		//VELOCITY VECTORS
		for(int yy=0;yy<GRIDH;yy++) {
		for(int xx=0;xx<GRIDW;xx++) {
			vecGridX[xx][yy]+=(
				(morphGrid[wrap(xx-1)][yy]-morphGrid[xx][yy])
				+
				(morphGrid[wrap(xx+1)][yy]-morphGrid[xx][yy])
			)*0.35;

			vecGridY[xx][yy]+=(
				(morphGrid[xx][wrap(yy-1)]-morphGrid[xx][yy])
				+
				(morphGrid[xx][wrap(yy+1)]-morphGrid[xx][yy])
			)*0.35;
		}}

		//MORPH
		for(int yy=0;yy<GRIDH;yy++) {
		for(int xx=0;xx<GRIDW;xx++) {
			morphGrid[xx][yy]+=(vecGridX[xx][yy]+vecGridY[xx][yy]);
		}}
	}

	//MOUSE PRESS
	if(mouse_pressed) {
		addto_world();
		mouse_pressed=FALSE;
	}

	if(GetKeyState(VK_SPACE) & 1) fluidsim_active=!fluidsim_active;

	timer+=0.05f;
	Sleep(1.0);
}


void world_point(float xx, float yy) {
	int ix=(int)xx;
	int iy=(int)yy;
	float newz=morphGrid[ix][iy];
	float grayscale=0.5f+newz/24.0;
	glColor3f(grayscale,grayscale,grayscale*0.7f);
	glVertex3f(xx,yy,newz);
}


void draw_world() {
	glBegin(GL_TRIANGLES);
		for(float yy=0.0;yy<GRIDH;yy+=1.0) {for(float xx=0.0;xx<GRIDW;xx+=1.0) {
			world_point(xx,yy);
			world_point(xx+1.0,yy);
			world_point(xx,yy+1.0);
				world_point(xx,yy+1.0);
				world_point(xx+1.0,yy);
				world_point(xx+1.0,yy+1.0);

		}}
	glEnd();
}






int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow) {
    WNDCLASSEX wcex;
    HWND hwnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;
    float theta = 0.0f;

    /* register window class */
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "GLSample";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;


    if(!RegisterClassEx(&wcex)) return 0;

    /* create main window */
    hwnd = CreateWindowEx(0,"GLSample","OpenGL Sample",WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT,CW_USEDEFAULT,800,600,
                          NULL,NULL,hInstance,NULL);

    ShowWindow(hwnd, nCmdShow);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);

	camera_init();
	glEnable(GL_DEPTH_TEST);


    /* program main loop */
    while(!bQuit) {
        /* check for messages */
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            /* handle or dispatch messages */
            if(msg.message==WM_QUIT)	{bQuit=TRUE;}
            else						{TranslateMessage(&msg);DispatchMessage(&msg);}
        }
        else {
            /* OpenGL animation code goes here */
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

			glLoadIdentity();


			//DRAAW
			glFrustum(-1,1,-1,1,1.0, 256.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				camera_move();
				camera_set_z(morphGrid);
				draw_world();
            SwapBuffers(hDC);
			glFlush();

			update();
        }
    }


    DisableOpenGL(hwnd, hDC, hRC);	// shutdown OpenGL
    DestroyWindow(hwnd);			// destroy the window explicitly
    return msg.wParam;
}


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CLOSE: PostQuitMessage(0); break;

        case WM_DESTROY: return 0;

        case WM_KEYDOWN: {
            switch (wParam) {case VK_ESCAPE: PostQuitMessage(0); break;}
        }
        break;

		case WM_SIZE: glViewport(0,0,window_width,window_height); break;

		case WM_LBUTTONDOWN:
			mouse_pressed=TRUE;
			mouse_x=LOWORD(lParam);	mouse_y=HIWORD(lParam);
		break;

        default: return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}


void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC) {
    PIXELFORMATDESCRIPTOR pfd;

    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC(hwnd);

    /* set the pixel format for the DC */
    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
                  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iFormat = ChoosePixelFormat(*hDC, &pfd);

    SetPixelFormat(*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext(*hDC);

    wglMakeCurrent(*hDC, *hRC);
}


void DisableOpenGL (HWND hwnd, HDC hDC, HGLRC hRC) {
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}

