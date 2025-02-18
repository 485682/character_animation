#pragma once

#include "application_header.h"

struct pascal_object;

struct application : public application_flags {

    application();

	bool init();

	void run();
    
	void clear();
    
	bool update();

    void onlostdevice();
    void onresetdevice();

    ID3DXFont* m_font;

    pascal_object* m_pascal;


	/*camera***************/
    _vec3      m_up;
    _vec3      m_look;
    _vec3      m_target;
    _vec3      m_position;
    _mat4      m_view;
    _mat4      m_projection;

    float      m_y_pos;
    float      m_x_pos;

	float      m_yaw;
    float      m_pitch;
    float      m_yaw_pos;
    float      m_pitch_pos;
    float      m_distance;
	/**********************/

	/*cursor***************/
	float      m_x_cursor_pos;
    float      m_y_cursor_pos;
	/**********************/

	/* animation keyframe updated in window class (winproc)*/
    int        m_keyframe;
	/**********************/

	/*timer***************/
	int        m_framespersecond;
	float      m_last_frame_seconds;
	float      m_last_frame_milliseconds;
	/**********************/

	/*application global static variables *********************/
    static _vec3           _up_axis;
    static application *  _instance;
    static HINSTANCE      _win32_instance;
	/**********************************************************/

	/***generates a mesh from a._mesh file **************/
    static bool readrtmeshfile(const char* path,_mesh* submeshes);
	/**********************************************************/

};
