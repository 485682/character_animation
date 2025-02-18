#include "d3d_manager.h"

#include "application.h"
#include "d3d_window.h"

d3d_manager* d3d_manager::_manager = NULL;

d3d_manager::d3d_manager(){

	m_fx     = NULL;

	m_d3dobject = NULL;
	m_d3ddevice = NULL;

	application_zero(&m_d3dpp,sizeof(m_d3dpp));

	m_hmvp      = (D3DXHANDLE)NULL;
	m_htex      = (D3DXHANDLE)NULL;
	m_htech     = (D3DXHANDLE)NULL;
	m_hworld    = (D3DXHANDLE)NULL;
	m_hbones    = (D3DXHANDLE)NULL;

}

bool d3d_manager::init(){

	m_d3dobject = Direct3DCreate9(D3D_SDK_VERSION);
	if( !m_d3dobject ) { application_throw("d3dobject"); }

	// Step 2: Verify hardware support for specified formats in windowed and full screen modes.
	D3DDISPLAYMODE mode;
	HRESULT HR = m_d3dobject->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mode);
	if(HR != D3D_OK){ application_throw("hr"); }

	application_throw_hr(m_d3dobject->CheckDeviceType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, mode.Format, mode.Format, true));
	application_throw_hr(m_d3dobject->CheckDeviceType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, D3DFMT_X8R8G8B8, false));

	// Step 3: Check for requested vertex processing and pure device.
	D3DCAPS9 caps;
	application_throw_hr(m_d3dobject->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps));

	DWORD devbehaviorflags = 0;
	if( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT ) { devbehaviorflags |= D3DCREATE_HARDWARE_VERTEXPROCESSING; }
	else { devbehaviorflags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING; }

	// If pure device and HW T&L supported
	if( caps.DevCaps & D3DDEVCAPS_PUREDEVICE && devbehaviorflags & D3DCREATE_HARDWARE_VERTEXPROCESSING) { devbehaviorflags |= D3DCREATE_PUREDEVICE; }

	// Step 4: Fill out the D3DPRESENT_PARAMETERS structure.
	m_d3dpp.BackBufferWidth            = 0;
	m_d3dpp.BackBufferHeight           = 0;
	m_d3dpp.BackBufferFormat           = D3DFMT_UNKNOWN;
	m_d3dpp.BackBufferCount            = 1;
	m_d3dpp.MultiSampleType            = D3DMULTISAMPLE_NONE;
	m_d3dpp.MultiSampleQuality         = 0;
	m_d3dpp.SwapEffect                 = D3DSWAPEFFECT_DISCARD;
	m_d3dpp.hDeviceWindow              = _window->m_hwnd;
	m_d3dpp.Windowed                   = true;
	m_d3dpp.EnableAutoDepthStencil     = true;
	m_d3dpp.AutoDepthStencilFormat     = D3DFMT_D24S8;
	m_d3dpp.Flags                      = 0;
	m_d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	m_d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;

	// Create the device.
	application_throw_hr(m_d3dobject->CreateDevice(

		D3DADAPTER_DEFAULT,// primary adapter
		D3DDEVTYPE_HAL,    // device type
		_window->m_hwnd,   // window associated with device
		devbehaviorflags,  // vertex processing
		&m_d3dpp,          // present parameters
		&m_d3ddevice       // return created device

		));

	// checking shader version 2.1 or greater required
	application_zero( &caps , sizeof(D3DCAPS9) );
	application_throw_hr(m_d3ddevice->GetDeviceCaps(&caps));
	if( caps.VertexShaderVersion < D3DVS_VERSION(2, 0) ) { application_throw("dev caps"); }
	if( caps.PixelShaderVersion  < D3DPS_VERSION(2, 0) ) { application_throw("dev caps"); }


	/* vertex_declaration ********************************************************/
	m_vertex_declaration = NULL;
	D3DVERTEXELEMENT9 vertexelements[] = {
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
		{0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		{0, 32, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0},
		{0, 48, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0},
		D3DDECL_END()
	};
	application_throw_hr(_api_manager->m_d3ddevice->CreateVertexDeclaration(vertexelements, &m_vertex_declaration));
	/*****************************************************************************/

	return buildfx();
}

void d3d_manager::clear(){

	application_releasecom(m_d3dobject);
	application_releasecom(m_d3ddevice);
	application_releasecom(m_fx);
}

bool d3d_manager::reset(){
	_application->onlostdevice();
	application_throw_hr(m_d3ddevice->Reset( &m_d3dpp) );
	_application->onresetdevice();
	return true;
}
 
bool d3d_manager::buildfx() {

	/* read effect file **********************************************************/
	ID3DXBuffer* errors = 0;
	HRESULT fx_result =D3DXCreateEffectFromFile(_api_manager->m_d3ddevice,"shader.fx",
		0, 0, D3DXSHADER_DEBUG, 0, &m_fx, &errors);
	if( errors )    { application_throw((char*)errors->GetBufferPointer()); }
	if( fx_result ) { application_throw("effect file"); }
	/*****************************************************************************/

	/* Obtain handles. **********************************************************/
	m_hmvp       = m_fx->GetParameterByName(0, "g_mvp");
	m_htex       = m_fx->GetParameterByName(0, "g_tex");
	m_htech      = m_fx->GetTechniqueByName("bone_tech");
	m_hworld     = m_fx->GetParameterByName(0, "g_world");
	m_hbones     = m_fx->GetParameterByName(0, "g_bones");
	m_hlight     = m_fx->GetParameterByName(0, "g_light");
	m_hmaterial  = m_fx->GetParameterByName(0, "g_material");


	if( m_hmvp      == (D3DXHANDLE)NULL ){ application_throw("mvp handle"); }
	if( m_htex      == (D3DXHANDLE)NULL ){ application_throw("texture handle"); }
	if( m_htech     == (D3DXHANDLE)NULL ){ application_throw("technique handle"); }
	if( m_hworld    == (D3DXHANDLE)NULL ){ application_throw("world handle"); }
	if( m_hbones    == (D3DXHANDLE)NULL ){ application_throw("bones handle"); }
	if( m_hlight    == (D3DXHANDLE)NULL ){ application_throw("light handle"); }
	if( m_hmaterial == (D3DXHANDLE)NULL ){ application_throw("material handle"); }	
	/****************************************************************************/

	/* single scene light  ***************************/
	m_light.ambient   = _vec4(0.98f,0.98f,0.98f,1.0f);
	m_light.diffuse   = _vec4(0.7f,0.7f,0.7f,1.0f);
	m_light.specular  = _vec4(0.3f,0.3f,0.3f,1.0f);
	m_light.direction = _vec3(1.0,-1.0f,0.0f);
	application_throw_hr(m_fx->SetValue(m_hlight, &m_light, sizeof(_light)));
	/***********************************************************************/

	return true;
}
