#include "platform.h"

#include "font_render.h"
#include "console.h"

void error_message(const char *message)
{
	MessageBox(NULL, message, "Error!!!", MB_OK);
	exit(EXIT_FAILURE);
}

bool WGLExtensionSupported(const char *extension_name)
{
	// this is pointer to function which returns pointer to string with list of all wgl extensions
	PFNWGLGETEXTENSIONSSTRINGEXTPROC _wglGetExtensionsStringEXT = NULL;

	// determine pointer to wglGetExtensionsStringEXT function
	_wglGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC)wglGetProcAddress("wglGetExtensionsStringEXT");

	if (strstr(_wglGetExtensionsStringEXT(), extension_name) == NULL)
	{
		// string was not found
		error_message("String not found in wgl extensions buffer!");
		return(false);
	}

	// extension is supported
	return(true);
}

System *sys = NULL;
PFNWGLSWAPINTERVALEXTPROC SwapInterval = NULL;

System init_system(HINSTANCE instance)
{
    System system = {};
    
    WNDCLASS wc = {};
	wc.style = CS_OWNDC;
	wc.hInstance = instance;
	wc.lpfnWndProc = window_proc;
	wc.lpszClassName = "Win32Class";
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	if(!RegisterClass(&wc))
		error_message("Window class not registered!");
	system.window = CreateWindowEx(0, wc.lpszClassName,
		"Terminal", WS_SYSMENU & ~WS_CAPTION,
		CW_USEDEFAULT, CW_USEDEFAULT, system.width, system.height,
			NULL, NULL, instance, NULL);

	if(!system.window)
		error_message("Window not created");
	ShowWindow(system.window, SW_SHOW);
	//init opengl
	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
	system.dc = GetDC(system.window);
	if(!system.dc) 
		error_message("Coundn't retrieve a valid device context!");
	
	int err = DescribePixelFormat(system.dc, 
								  PFD_SUPPORT_OPENGL | 
								  PFD_DOUBLEBUFFER | 
								  PFD_DRAW_TO_WINDOW, 
								  sizeof(PIXELFORMATDESCRIPTOR), 
								  &pfd);
						
	if(!err) 
		error_message("DescribePixelFormat() failed!");
	
	int pixel_format = ChoosePixelFormat(system.dc, &pfd);
	if(!pixel_format) 
		error_message("No pixel format available");
	
	if(!SetPixelFormat(system.dc, pixel_format, &pfd))
		error_message("Couldn't set pixel format!");
	
	system.gl_context = wglCreateContext(system.dc);
	if(!system.gl_context) 
		error_message("Couldn't create GL context!");
	
	if(!wglMakeCurrent(system.dc, system.gl_context)) 
		error_message("GL context not current!");
	
	
	if (WGLExtensionSupported("WGL_EXT_swap_control"))
		SwapInterval = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
	
	if(SwapInterval)
		SwapInterval(1);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 200, 100, 0, -1, 1);

    return system;
}

void release_system(System *system)
{
    wglMakeCurrent(system->dc, NULL);
	wglDeleteContext(system->gl_context);
	ReleaseDC(system->window, system->dc);
	DestroyWindow(system->window);
}

void resize_window(System *system, FontRender *fr, Console *console)
{		
	system->width = fr->width * (console->screen_width+1) + 10;
	system->height = fr->height * (console->screen_height+3) + 10;
	
	RECT client_area = {};
	client_area.right = system->width;
	client_area.bottom = system->height;
	AdjustWindowRect(&client_area, WS_OVERLAPPEDWINDOW, false);
	
	int nx = client_area.right - system->width;
	int ny = client_area.bottom - system->height;

	SetWindowPos(system->window, HWND_NOTOPMOST, 0, 0, client_area.right, client_area.bottom, SWP_NOMOVE);
	ShowWindow(system->window, SW_SHOW);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, system->width, system->height, 0, -1, 1);
	
	GetClientRect(system->window, &client_area);

	glViewport(client_area.left, client_area.top, client_area.right, client_area.bottom);
}