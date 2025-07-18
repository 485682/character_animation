#include "pascal.h"

#include "application.h"

#include "d3d_window.h"
#include "d3d_manager.h"

#include "resource.h"

pascal_object::pascal_object(){

	m_animation_second = 0.0f;
	m_animation_length = 0.0f;
	m_current_keyframe = 0;
	m_start_keyframe   = 0;
	m_end_keyframe     = 0;

	m_texture = NULL;
}

bool pascal_object::init(){

	m_animation_length = 0.25f;

	//* due to blender's up axis being Z
	m_world = _rotate(float(_radians(-90.0f)),_vec3(1.0f,0.0f,0.0f));
	//**********************************

	m_world = m_world*_translate(_vec3(0.0f,-3.0f,0.0f) );

	/* load mesh from rc resource */
	LPVOID data = application::getresourcedata( IDR_PASCAL );
	if(!application::loadmeshfile(data,&m_mesh) ){ application_throw("loadmesh"); }
	application::freeresourcedata();

	if(m_mesh.m_bones.m_count){ m_keyframe_buffer.allocate(m_mesh.m_bones.m_count); }


	for(uint32_t i=0;i<m_mesh.m_submeshes.m_count;i++){

		application_throw_hr(_api_manager->m_d3ddevice->CreateVertexBuffer(
			m_mesh.m_submeshes[i].m_vertices.m_count * sizeof(_vertex),
			D3DUSAGE_WRITEONLY,0, D3DPOOL_MANAGED,	&(m_mesh.m_submeshes[i].m_vertex_buffer), 0));
		if(!m_mesh.m_submeshes[i].m_vertex_buffer){ application_throw("vertex buffer"); }

		_vertex * v = 0;
		application_throw_hr(m_mesh.m_submeshes[i].m_vertex_buffer->Lock(0, 0, (void**)&v, 0));
		for(uint32_t ii=0;ii<m_mesh.m_submeshes[i].m_vertices.m_count;ii++){
			v[ii] = m_mesh.m_submeshes[i].m_vertices[ii];
			//st to uv************************
			v[ii].m_uv.y = 1.0f-v[ii].m_uv.y;
			//********************************
		}
		application_throw_hr(m_mesh.m_submeshes[i].m_vertex_buffer->Unlock());

		application_throw_hr(_api_manager->m_d3ddevice->CreateIndexBuffer(
			m_mesh.m_submeshes[i].m_indices.m_count * sizeof(WORD),
			D3DUSAGE_WRITEONLY,D3DFMT_INDEX16,
			D3DPOOL_MANAGED, &(m_mesh.m_submeshes[i].m_index_buffer), 0));
		if(!m_mesh.m_submeshes[i].m_index_buffer){ application_throw("index_buffer"); }

		WORD* indices = 0;
		application_throw_hr(m_mesh.m_submeshes[i].m_index_buffer->Lock(0, 0, (void**)&(indices), 0));
		for(uint32_t ii=0;ii<m_mesh.m_submeshes[i].m_indices.m_count/3;ii++){

			//* conversion from right hand( opengl ) to left hand( direct3d ) Coordinate Systems
			//* requires clockwise rotation of triangles
			/*https://learn.microsoft.com/en-us/windows/win32/direct3d9/coordinate-systems*/
			uint32_t pos = ii*3;
			indices[pos  ] = WORD(m_mesh.m_submeshes[i].m_indices[pos]);
			indices[pos+1] = WORD(m_mesh.m_submeshes[i].m_indices[pos+2]);
			indices[pos+2] = WORD(m_mesh.m_submeshes[i].m_indices[pos+1]);

		}
		application_throw_hr(m_mesh.m_submeshes[i].m_index_buffer->Unlock());
		 
		if(i==0){
			m_mesh.m_submeshes[i].m_material.ambient = _vec4(0.9f,0.9f,0.9f,1.0f);
			m_mesh.m_submeshes[i].m_material.diffuse = _vec4(0.5f,0.5f,0.5f,1.0f);
			m_mesh.m_submeshes[i].m_material.specular = _vec4(0.0f,0.0f,0.0f,1.0f);
			m_mesh.m_submeshes[i].m_material.specular_power =0.0f;
		}//dress
		if(i==1){
			m_mesh.m_submeshes[i].m_material.ambient = _vec4(0.98f,0.98f,0.98f,1.0f);
			m_mesh.m_submeshes[i].m_material.diffuse = _vec4(0.1f,0.1f,0.1f,1.0f);
			m_mesh.m_submeshes[i].m_material.specular = _vec4(0.15f,0.15f,0.15f,1.0f);
			m_mesh.m_submeshes[i].m_material.specular_power =0.1f;
		}//accessories
		if(i==2){
			m_mesh.m_submeshes[i].m_material.ambient = _vec4(1.0f,1.0f,1.0f,1.0f);
			m_mesh.m_submeshes[i].m_material.diffuse = _vec4(1.0f,1.0f,1.0f,1.0f);
			m_mesh.m_submeshes[i].m_material.specular = _vec4(1.0f,1.0f,1.0f,1.0f);
			m_mesh.m_submeshes[i].m_material.specular_power =0.0f;
		}//eyes
		if(i==3){
			m_mesh.m_submeshes[i].m_material.ambient = _vec4(1.0f,1.0f,1.0f,1.0f);
			m_mesh.m_submeshes[i].m_material.diffuse = _vec4(0.15f,0.15f,0.15f,1.0f);
			m_mesh.m_submeshes[i].m_material.specular = _vec4(0.0f,0.0f,0.0f,1.0f);
			m_mesh.m_submeshes[i].m_material.specular_power =0.0f;
		}//skin
		if(i==4){
			m_mesh.m_submeshes[i].m_material.ambient = _vec4(1.0f,1.0f,1.0f,1.0f);
			m_mesh.m_submeshes[i].m_material.diffuse = _vec4(1.0f,1.0f,1.0f,1.0f);
			m_mesh.m_submeshes[i].m_material.specular = _vec4(1.0f,1.0f,1.0f,1.0f);
			m_mesh.m_submeshes[i].m_material.specular_power =0.0f;
		}//hair
	}
	application_throw_hr( D3DXCreateTextureFromResource( _api_manager->m_d3ddevice, NULL, MAKEINTRESOURCE( IDB_PASCAL_UV ), &m_texture ) );
	return true;
}

void pascal_object::clear(){
	for(uint32_t i=0;i<m_mesh.m_submeshes.m_count;i++){
		application_releasecom(m_mesh.m_submeshes[i].m_index_buffer);
		application_releasecom(m_mesh.m_submeshes[i].m_vertex_buffer);
	}
}
bool pascal_object::update(){

	static _mat4 m_mat;

	/* proccess keyframes */
	/* animation_second( time elapsed ), animation_length(time between keyframes ) */
	if( ((m_end_keyframe - m_start_keyframe)>0) ){
		if(m_animation_second>=m_animation_length){
			m_animation_second=0;
			if(m_current_keyframe<m_end_keyframe){ m_current_keyframe++; }
			else{ m_current_keyframe = m_start_keyframe; }
		}
		else { m_animation_second += _application->m_last_frame_seconds;}
	}

	/* model view projection (g_mvp in shader ) matrix */
	m_mat = m_world*_application->m_view*_application->m_projection;
	application_throw_hr(_fx->SetMatrix(_api_manager->m_hmvp,(D3DXMATRIX*)&m_mat ));

	application_throw_hr(_fx->SetMatrix(_api_manager->m_hworld, (D3DXMATRIX*)&m_world));

	application_throw_hr(_fx->SetTexture(_api_manager->m_htex, m_texture));

	application_throw_hr(_api_manager->m_d3ddevice->SetVertexDeclaration(_api_manager->m_vertex_declaration));
	application_throw_hr(_fx->SetMatrixArray(_api_manager->m_hbones,currentkeyframe(), m_mesh.m_bones.m_count));

	/* iterate through the submeshes .. and render*/
	for(uint32_t i=0;i<m_mesh.m_submeshes.m_count;i++){

		application_throw_hr(_api_manager->m_d3ddevice->SetStreamSource(0, m_mesh.m_submeshes[i].m_vertex_buffer, 0, sizeof(_vertex)));
		application_throw_hr(_api_manager->m_d3ddevice->SetIndices(m_mesh.m_submeshes[i].m_index_buffer));

		application_throw_hr(_fx->SetValue(_api_manager->m_hmaterial, &(m_mesh.m_submeshes[i].m_material), sizeof(_material)));

		application_throw_hr(_fx->Begin(NULL, 0));
		application_throw_hr(_fx->BeginPass(0));
		application_throw_hr(_api_manager->m_d3ddevice->DrawIndexedPrimitive(
			D3DPT_TRIANGLELIST, 0, 0,
			m_mesh.m_submeshes[i].m_vertices.m_count, 0,
			m_mesh.m_submeshes[i].m_indices.m_count/3));
		application_throw_hr(_fx->EndPass());
		application_throw_hr(_fx->End());
	}
	return true;
} 

void  pascal_object::keyframe(const uint32_t&start,const uint32_t&end){
	if( ( end<m_mesh.m_keyframes.m_count ) && (end>start) ){
		m_start_keyframe = m_current_keyframe = start;
		m_end_keyframe   = end;
	}
}

D3DXMATRIX*  pascal_object::currentkeyframe(){

	if( ((m_end_keyframe - m_start_keyframe)>0) ){

		int32_t current_key;
		int32_t previous_key;

		if(m_current_keyframe == m_end_keyframe ){
			current_key  = m_start_keyframe;
			previous_key = m_current_keyframe;
		}
		else{
			current_key  = m_current_keyframe+1;
			previous_key = m_current_keyframe;
		}

		/* linear interpolation of bone transforms */
		for(uint32_t i=0;i<m_mesh.m_bones.m_count;i++){
			float* buffer   = (float*) &( m_keyframe_buffer[i] );
			float* previous = (float*) &( m_mesh.m_keyframes[previous_key][i] );
			float* current  = (float*) &( m_mesh.m_keyframes[current_key][i] );
			for(uint32_t ii=0;ii<16;ii++){
				buffer[ii] = _lerp(previous[ii],current[ii],  m_animation_second/m_animation_length );
			}
		}

		return (D3DXMATRIX*)&(m_keyframe_buffer[0]);

	}
	return (D3DXMATRIX*)&(m_mesh.m_bones[0]);
}
