#include "d3d_window.h"

#include "application.h"
#include "d3d_manager.h"

#include "pascal.h"

d3d_window* _window = NULL;

bool d3d_window::init(){

	WNDCLASSEX wincl;

	wincl.hInstance     = _application->_win32_instance;
	wincl.lpszClassName = "application_class";;
	wincl.lpfnWndProc   = winproc;
	wincl.style         = CS_HREDRAW | CS_VREDRAW;
	wincl.cbSize        = sizeof (WNDCLASSEX);
	wincl.hIcon         = LoadIcon (NULL, IDI_APPLICATION);
	wincl.hIconSm       = LoadIcon (NULL, IDI_APPLICATION);
	wincl.hCursor       = LoadCursor (NULL, IDC_ARROW);
	wincl.lpszMenuName  = NULL;
	wincl.cbClsExtra    = 0;
	wincl.cbWndExtra    = 0;
	wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

	if (!RegisterClassEx (&wincl)){ application_throw("registerclass"); }

	RECT R = {0, 0, application_width,application_height};
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	m_hwnd = CreateWindowEx (0,wincl.lpszClassName,application_title,WS_OVERLAPPEDWINDOW,
		50, 50, R.right, R.bottom,HWND_DESKTOP,NULL,_application->_win32_instance,NULL);
	if( !m_hwnd ) { application_throw("hwnd"); }
	ShowWindow(m_hwnd, SW_SHOW);

	return true;
}

bool d3d_window::update(){
	MSG  msg;
	application_zero(&msg,sizeof(MSG));
	while( PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) ) {
		TranslateMessage( &msg );
		DispatchMessage( &msg );
		if(msg.message == WM_QUIT){ _application->removeflags(application_running); }
	}
	if( _application->testflags(application_paused) ) { Sleep(20); }
	return true;
}

void d3d_window::enablefullscreenmode(bool enable) {

	if( enable ) {
		// Are we already in fullscreen mode?
		if( !_api_manager->m_d3dpp.Windowed ) { return; }

		int width  = GetSystemMetrics(SM_CXSCREEN);
		int height = GetSystemMetrics(SM_CYSCREEN);

		_api_manager->m_d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
		_api_manager->m_d3dpp.BackBufferWidth  = width;
		_api_manager->m_d3dpp.BackBufferHeight = height;
		_api_manager->m_d3dpp.Windowed         = false;

		// Change the window style to a more fullscreen friendly style.
		SetWindowLongPtr(m_hwnd, GWL_STYLE, WS_POPUP);
		// If we call SetWindowLongPtr, MSDN states that we need to call
		// SetWindowPos for the change to take effect.  In addition, we
		// need to call this function anyway to update the window dimensions.
		SetWindowPos(m_hwnd, HWND_TOP, 0, 0, width, height, SWP_NOZORDER | SWP_SHOWWINDOW);
	}else {
		if( _api_manager->m_d3dpp.Windowed ) { return; }
		RECT R = {0, 0, application_width, application_height};
		AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
		_api_manager->m_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
		_api_manager->m_d3dpp.BackBufferWidth  = application_width;
		_api_manager->m_d3dpp.BackBufferHeight = application_height;

		_api_manager->m_d3dpp.Windowed         = true;
		// Change the window style to a more windowed friendly style.
		SetWindowLongPtr(m_hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
		// If we call SetWindowLongPtr, MSDN states that we need to call
		// SetWindowPos for the change to take effect.  In addition, we
		// need to call this function anyway to update the window dimensions.
		SetWindowPos(m_hwnd, HWND_TOP, 100, 100, R.right, R.bottom, SWP_NOZORDER | SWP_SHOWWINDOW);
	}
	_api_manager->reset();
}

LRESULT CALLBACK d3d_window::winproc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){

	// Is the application in a minimized or maximized state?
	static bool minormaxed = false;

	RECT clientrect = {0, 0, 0, 0};
	switch( msg )
	{

		// WM_ACTIVE is sent when the window is activated or deactivated.
		// We pause  when the main window is deactivated and
		// unpause it when it becomes active.
	case WM_ACTIVATE:{
		if( LOWORD(wParam) == WA_INACTIVE ) { _application->addflags(application_paused); }
		else{ _application->removeflags(application_paused); }
		break;
					 }
	case WM_SIZE:{
		if( _api_manager->m_d3ddevice ){

			_api_manager->m_d3dpp.BackBufferWidth  = LOWORD(lParam);
			_api_manager->m_d3dpp.BackBufferHeight = HIWORD(lParam);

			if( wParam == SIZE_MINIMIZED ){
				_application->addflags(application_paused);
				minormaxed = true;
			}else if( wParam == SIZE_MAXIMIZED ){
				_application->removeflags(application_paused);
				minormaxed = true;
				_api_manager->reset();
			}
			// Restored is any resize that is not a minimize or maximize.
			// For example, restoring the window to its default size
			// after a minimize or maximize, or from dragging the resize
			// bars.
			else if( wParam == SIZE_RESTORED ){
				_application->removeflags(application_paused);

				// Are we restoring from a mimimized or maximized state,
				// and are in windowed mode?  Do not execute this code if
				// we are restoring to full screen mode.
				if( minormaxed && _api_manager->m_d3dpp.Windowed ) { _api_manager->reset(); }
				else {
					// No, which implies the user is resizing by dragging
					// the resize bars.  However, we do not reset the device
					// here because as the user continuously drags the resize
					// bars, a stream of WM_SIZE messages is sent to the window,
					// and it would be pointless (and slow) to reset for each
					// WM_SIZE message received from dragging the resize bars.
					// So instead, we reset after the user is done resizing the
					// window and releases the resize bars, which sends a
					// WM_EXITSIZEMOVE message.
				}
				minormaxed = false;
			}
		}
		
				 }break;
				 // WM_EXITSIZEMOVE is sent when the user releases the resize bars.
				 // Here we reset everything based on the new window dimensions.
	case WM_EXITSIZEMOVE:{
		GetClientRect(_window->m_hwnd, &clientrect);
		_api_manager->m_d3dpp.BackBufferWidth  = clientrect.right;
		_api_manager->m_d3dpp.BackBufferHeight = clientrect.bottom;
		_api_manager->reset();
		break;
						 }
	case WM_CLOSE:{
		DestroyWindow(_window->m_hwnd);
		break;
				  }
	case WM_DESTROY:{
		PostQuitMessage(0);
		break;
					}
	case WM_KEYDOWN:{
		if( wParam == VK_ESCAPE ){
			DestroyWindow(_window->m_hwnd);
		}else if( wParam == 'V' ){//vsync
			if( _application->testflags(application_vsync)){
				_api_manager->m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
				_application->removeflags(application_vsync);
			}else{
				_api_manager->m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
				_application->addflags(application_vsync);
			}
			_api_manager->reset();

		}else if( wParam == 'F' ){//fullscreen
			if( _application->testflags(application_fullscreen)){
				_window->enablefullscreenmode(false);
				_application->removeflags(application_fullscreen);
			}else{
				_window->enablefullscreenmode(true);
				_application->addflags(application_fullscreen);
			}
		}else if(wParam == 'N'){//switch animation
			if( _application->m_keyframe>=3 ){ _application->m_keyframe=0; }
			else{ _application->m_keyframe++; }
			if( _application->m_keyframe ==0 ){ _application->m_pascal->setbindpose(); }
			if( _application->m_keyframe ==1 ){
				/*walk animation*/
				_application->m_pascal->keyframe(0,5);
				/*seconds per frame*/
				_application->m_pascal->m_animation_length = 0.16f;
			}
			if( _application->m_keyframe ==2 ){
				/*wave animation*/
				_application->m_pascal->keyframe(7,31);
				/*seconds per frame*/
				_application->m_pascal->m_animation_length = 0.1f;
			}
			if( _application->m_keyframe ==3 ){
				/*stretch animation*/
				_application->m_pascal->keyframe(32,72);
				/*seconds per frame*/
				_application->m_pascal->m_animation_length = 0.1f;
			}
		}
		break;
					}


					//camera update
	case WM_MOUSEMOVE:{

		if( _application->testflags(application_lmousedown) ){
			float pitch = _application->m_y_pos - _application->m_y_cursor_pos;
			float yaw   = _application->m_x_pos - _application->m_x_cursor_pos;
			_application->m_pitch = _application->m_pitch_pos + pitch;
			_application->m_yaw   = _application->m_yaw_pos   + yaw  ;
		}
		break;
					  }
	case WM_LBUTTONDOWN:{
		_application->addflags(application_lmousedown);
		_application->m_x_pos = _application->m_x_cursor_pos;
		_application->m_y_pos = _application->m_y_cursor_pos;
		break;
						}
	case WM_RBUTTONDOWN:{
		_application->addflags(application_rmousedown);
		break;
						}
	case WM_LBUTTONUP:  {
		_application->m_pitch_pos = _application->m_pitch;
		_application->m_yaw_pos   = _application->m_yaw;
		_application->removeflags(application_lmousedown);
		break;
						}
	case WM_RBUTTONUP:  {
		_application->removeflags(application_rmousedown);
		break;
						}
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}
