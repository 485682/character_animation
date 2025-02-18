#pragma once

#include <cmath>

template < typename T>
struct _vector2{

	_vector2():x(0),y(0) {}
	_vector2(const T& v):x(v),y(v) {}
	_vector2(const _vector2 &v ):x(v.x),y(v.y) {}
	_vector2(const T& x_,const T& y_ ):x(x_),y(y_) {}

	T& operator [](const int &index ){ return (&x)[ index ]; }
	const T& operator [](const int &index )const { return (&x)[ index ]; }

	void operator =(const _vector2 &v) { x=v.x; y=v.y; }

	void negate(){ x=-x; y=-y; }
	T x,y;
};

template < typename T>
struct _vector3{
	_vector3():x( T(0.0f) ),y( T(0.0f) ),z( T(0.0f) ) {}
	_vector3(const T& v):x(v),y(v),z(v) {}
	_vector3(const _vector3& v ):x(v.x),y(v.y),z(v.z) {}
	_vector3(const T& x_,const T& y_,const T& z_ ):x(x_),y(y_),z(z_) {}

	T& operator [](const int &index ){ return (&x)[ index ]; }
	const T& operator [](const int &index )const { return (&x)[ index ]; }

	void operator =(const _vector3 &v) { x=v.x; y=v.y; z=v.z; }

	void negate(){ x=-x; y=-y; z=-z; }

	void operator *=(const T& s){ x*=s; y*=s; z*=s; }
	_vector3 operator *(const T& s) const { return _vector3(x*s,y*s,z*s); }

	void operator +=(const _vector3& v) { x+=v.x; y+=v.y; z+=v.z; }
	_vector3 operator +(const  _vector3& v) const { return _vector3(x+v.x,y+v.y,z+v.z); }

	void operator -=(const _vector3& v) { x-=v.x; y-=v.y; z-=v.z; }
	_vector3 operator -(const  _vector3& v) const { return _vector3(x-v.x,y-v.y,z-v.z); }

	T length() const { return sqrt( (x*x)+(y*y)+(z*z) ); }

	_vector3 normal() const {
		const T len = length();
		return _vector3(x/len,y/len,z/len);
	}

	T x,y,z;
};

template <typename T>
T _dot(const _vector3<T>&v1,const _vector3<T>&v2) { return (v1.x*v2.x+v1.y*v2.y+v1.z*v2.z); }

template <typename T>
_vector3<T> _cross(const _vector3<T>&v1,const _vector3<T>&v2){
	return _vector3<T>(  v1.y*v2.z-v1.z*v2.y,  v1.z*v2.x-v1.x*v2.z,  v1.x*v2.y-v1.y*v2.x );
}
template <typename T>
_vector3<T> _normalize(const _vector3<T>&v){
	const T len = v.length();
	return _vector3<T>(v.x/len,v.y/len,v.z/len);
}
template <typename T>
T _distance(const _vector3<T>&v1,const _vector3<T>&v2){
	_vector3<T> dif = v1-v2;
	return dif.magnitude();
}

template < typename T>
struct _vector4{
	_vector4():x( T(0.0f) ),y( T(0.0f) ),z( T(0.0f) ),w( T(0.0f) ) {}
	_vector4(const T& v):x(v),y(v),z(v),w(v) {}
	_vector4(const _vector4& v ):x(v.x),y(v.y),z(v.z),w(v.w) {}
	_vector4(const T& x_,const T& y_,const T& z_,const T& w_ ):x(x_),y(y_),z(z_),w(w_) {}

	T& operator [](const int &index ){ return (&x)[ index ]; }
	const T& operator [](const int &index )const { return (&x)[ index ]; }

	void operator =(const _vector4 &v) { x=v.x; y=v.y; z=v.z; w=v.w; }

	void negate(){ x=-x; y=-y; z=-z; w=-w; }

	void operator *=(const T& s){ x*=s; y*=s; z*=s; w*=s; }
	_vector4 operator *(const T& s) const { return _vector4(x*s,y*s,z*s,w*s); }

	void operator +=(const _vector4& v) { x+=v.x; y+=v.y; z+=v.z; w+=v.w; }
	_vector4 operator +(const  _vector4& v) const { return _vector4(x+v.x,y+v.y,z+v.z,w+v.w); }

	T x,y,z,w;
};


template < typename T>
struct _matrix3{

	_matrix3(){ m_data[0][0]=m_data[1][1]=m_data[2][2]=1;}
	_matrix3(const T& s){ m_data[0][0]=m_data[1][1]=m_data[2][2]=s;}

	_matrix3(const _matrix3& m){  operator=(m); }

	void operator=(const _matrix3 &m){
		m_data[0]=m.m_data[0];
		m_data[1]=m.m_data[1];
		m_data[2]=m.m_data[2];
	}
	_matrix3 operator*(const T& s){
		_matrix3 result(*this);
		result[0]*=s;
		result[1]*=s;
		result[2]*=s;
		return result;
	}

	_vector3<T> operator * (const _vector3<T>& v){
		_vector3<T> result;
		result.x = v.x*m_data[0][0] + v.y*m_data[1][0] + v.z*m_data[2][0];
		result.y = v.x*m_data[0][1] + v.y*m_data[1][1] + v.z*m_data[2][1];
		result.z = v.x*m_data[0][2] + v.y*m_data[1][2] + v.z*m_data[2][2];
		return result;
	}
	_vector3<T>& operator [](const int &index ){ return m_data[ index ]; }
	const _vector3<T>& operator [](const int &index )const { return m_data[ index ]; }

	_vector3<T> m_data[3];
};

template <typename T>
_matrix3<T> _transpose(const _matrix3<T>& m){
	_matrix3<T> result;
	result[0][0]=m[0][0];  result[0][1]=m[1][0];  result[0][2]=m[2][0];
	result[1][0]=m[0][1];  result[1][1]=m[1][1];  result[1][2]=m[2][1];
	result[2][0]=m[0][2];  result[2][1]=m[1][2];  result[2][2]=m[2][2];
	return result;
}

template <typename T>
_matrix3<T> operator*(const _matrix3<T>& m1,const _matrix3<T>& m2){
	_matrix3<T> result;

	result[0][0] = m1[0][0]*m2[0][0] + m1[0][1]*m2[1][0] + m1[0][2]*m2[2][0];
	result[1][0] = m1[1][0]*m2[0][0] + m1[1][1]*m2[1][0] + m1[1][2]*m2[2][0];
	result[2][0] = m1[2][0]*m2[0][0] + m1[2][1]*m2[1][0] + m1[2][2]*m2[2][0];

	result[0][1] = m1[0][0]*m2[0][1] + m1[0][1]*m2[1][1] + m1[0][2]*m2[2][1];
	result[1][1] = m1[1][0]*m2[0][1] + m1[1][1]*m2[1][1] + m1[1][2]*m2[2][1];
	result[2][1] = m1[2][0]*m2[0][1] + m1[2][1]*m2[1][1] + m1[2][2]*m2[2][1];

	result[0][2] = m1[0][0]*m2[0][2] + m1[0][1]*m2[1][2] + m1[0][2]*m2[2][2];
	result[1][2] = m1[1][0]*m2[0][2] + m1[1][1]*m2[1][2] + m1[1][2]*m2[2][2];
	result[2][2] = m1[2][0]*m2[0][2] + m1[2][1]*m2[1][2] + m1[2][2]*m2[2][2];

	return result;
}

template < typename T>
struct _matrix4{

	_matrix4(){ m_data[0][0]=m_data[1][1]=m_data[2][2]=m_data[3][3]=1;}
	_matrix4(const T& s){ m_data[0][0]=m_data[1][1]=m_data[2][2]=m_data[3][3]=s;}

	_matrix4(const _matrix4& m){  operator=(m); }

	void operator=(const _matrix4 &m){
		m_data[0]=m.m_data[0];
		m_data[1]=m.m_data[1];
		m_data[2]=m.m_data[2];
		m_data[3]=m.m_data[3];
	}
	_matrix4 operator*(const T& s){
		_matrix4 result(*this);
		result[0]*=s;
		result[1]*=s;
		result[2]*=s;
		result[3]*=s;
		return result;
	}

	_vector4<T> operator * (const _vector4<T>& v){
		_vector4<T> result;
		result.x = v.x*m_data[0][0] + v.y*m_data[1][0] + v.z*m_data[2][0] + v.w*m_data[3][0];
		result.y = v.x*m_data[0][1] + v.y*m_data[1][1] + v.z*m_data[2][1] + v.w*m_data[3][1];
		result.z = v.x*m_data[0][2] + v.y*m_data[1][2] + v.z*m_data[2][2] + v.w*m_data[3][2];
		result.w = v.x*m_data[0][3] + v.y*m_data[1][3] + v.z*m_data[2][3] + v.w*m_data[3][3];
		return result;
	}

	_vector4<T>& operator [](const int &index ){ return m_data[ index ]; }
	const _vector4<T>& operator [](const int &index )const { return m_data[ index ]; }
	_vector4<T> m_data[4];
};
template <typename T>
_matrix4<T> _transpose(const _matrix4<T>& m){
	_matrix4<T> result;
	result[0][0]=m[0][0];  result[0][1]=m[1][0];  result[0][2]=m[2][0]; result[0][3]=m[3][0];
	result[1][0]=m[0][1];  result[1][1]=m[1][1];  result[1][2]=m[2][1]; result[1][3]=m[3][1];
	result[2][0]=m[0][2];  result[2][1]=m[1][2];  result[2][2]=m[2][2]; result[2][3]=m[3][2];
	result[3][0]=m[0][3];  result[3][1]=m[1][3];  result[3][2]=m[2][3]; result[3][3]=m[3][3];
	return result;
}

template <typename T>
_matrix4<T> operator*(const _matrix4<T>& m1,const _matrix4<T>& m2){
	_matrix4<T> result;

	result[0][0] = m1[0][0]*m2[0][0] + m1[0][1]*m2[1][0] + m1[0][2]*m2[2][0]  + m1[0][3]*m2[3][0];
	result[1][0] = m1[1][0]*m2[0][0] + m1[1][1]*m2[1][0] + m1[1][2]*m2[2][0]  + m1[1][3]*m2[3][0];
	result[2][0] = m1[2][0]*m2[0][0] + m1[2][1]*m2[1][0] + m1[2][2]*m2[2][0]  + m1[2][3]*m2[3][0];
	result[3][0] = m1[3][0]*m2[0][0] + m1[3][1]*m2[1][0] + m1[3][2]*m2[2][0]  + m1[3][3]*m2[3][0];

	result[0][1] = m1[0][0]*m2[0][1] + m1[0][1]*m2[1][1] + m1[0][2]*m2[2][1]  + m1[0][3]*m2[3][1];
	result[1][1] = m1[1][0]*m2[0][1] + m1[1][1]*m2[1][1] + m1[1][2]*m2[2][1]  + m1[1][3]*m2[3][1];
	result[2][1] = m1[2][0]*m2[0][1] + m1[2][1]*m2[1][1] + m1[2][2]*m2[2][1]  + m1[2][3]*m2[3][1];
	result[3][1] = m1[3][0]*m2[0][1] + m1[3][1]*m2[1][1] + m1[3][2]*m2[2][1]  + m1[3][3]*m2[3][1];

	result[0][2] = m1[0][0]*m2[0][2] + m1[0][1]*m2[1][2] + m1[0][2]*m2[2][2]  + m1[0][3]*m2[3][2];
	result[1][2] = m1[1][0]*m2[0][2] + m1[1][1]*m2[1][2] + m1[1][2]*m2[2][2]  + m1[1][3]*m2[3][2];
	result[2][2] = m1[2][0]*m2[0][2] + m1[2][1]*m2[1][2] + m1[2][2]*m2[2][2]  + m1[2][3]*m2[3][2];
	result[3][2] = m1[3][0]*m2[0][2] + m1[3][1]*m2[1][2] + m1[3][2]*m2[2][2]  + m1[3][3]*m2[3][2];

	result[0][3] = m1[0][0]*m2[0][3] + m1[0][1]*m2[1][3] + m1[0][2]*m2[2][3]  + m1[0][3]*m2[3][3];
	result[1][3] = m1[1][0]*m2[0][3] + m1[1][1]*m2[1][3] + m1[1][2]*m2[2][3]  + m1[1][3]*m2[3][3];
	result[2][3] = m1[2][0]*m2[0][3] + m1[2][1]*m2[1][3] + m1[2][2]*m2[2][3]  + m1[2][3]*m2[3][3];
	result[3][3] = m1[3][0]*m2[0][3] + m1[3][1]*m2[1][3] + m1[3][2]*m2[2][3]  + m1[3][3]*m2[3][3];
	return result;
}
template <typename T> /*glm*/
_matrix4<T> _lookatrh (const _vector3<T>& eye,const _vector3<T>& center,const _vector3<T>& up) {
	const _vector3<T> f(_normalize(center - eye));
	const _vector3<T> s(_normalize(_cross(f, up)));
	const _vector3<T> u(_cross(s, f));

	_matrix4<T> result;
	result[0][0] = s.x;
	result[1][0] = s.y;
	result[2][0] = s.z;
	result[0][1] = u.x;
	result[1][1] = u.y;
	result[2][1] = u.z;
	result[0][2] =-f.x;
	result[1][2] =-f.y;
	result[2][2] =-f.z;
	result[3][0] =-_dot(s, eye);
	result[3][1] =-_dot(u, eye);
	result[3][2] = _dot(f, eye);
	return result;
}
template <typename T> /*glm*/
_matrix4<T> _perspectivefovrh(T fov,T width,T height,T zNear,T zFar ) {

	T const rad = fov;
	T const h = cos(T(0.5) * rad) / sin(T(0.5) * rad);
	T const w = h * height / width; ///todo max(width , Height) / min(width , Height)?

	_matrix4<T> result(T(0));
	result[0][0] = w;
	result[1][1] = h;
	result[2][2] = - (zFar + zNear) / (zFar - zNear);
	result[2][3] = - T(1);
	result[3][2] = - (T(2) * zFar * zNear) / (zFar - zNear);
	return result;
}
template <typename T> /*glm*/
_matrix4<T> _translate(const _vector3<T> & v ){
	_matrix4<T> result;
	result[3] = result[0]*v[0] + result[1]*v[1] + result[2]*v[2] + result[3];
	return result;
}
template <typename T> /*glm*/
_matrix4<T> _scale ( const _vector3<T>& v ){
	_matrix4<T> result,m;
	result[0] = m[0] * v[0];
	result[1] = m[1] * v[1];
	result[2] = m[2] * v[2];
	result[3] = m[3];
	return result;
}
template <typename T> /*glm*/
_matrix4<T> _rotate ( T angle, const _vector3<T>& v ){

	const T a = angle;
	const T c = cos(a);
	const T s = sin(a);

	_vector3<T> axis(_normalize(v));
	_vector3<T> temp( axis*(T(1) - c));

	_matrix4<T> result;
	result[0][0] = c + temp[0] * axis[0];
	result[0][1] = 0 + temp[0] * axis[1] + s * axis[2];
	result[0][2] = 0 + temp[0] * axis[2] - s * axis[1];

	result[1][0] = 0 + temp[1] * axis[0] - s * axis[2];
	result[1][1] = c + temp[1] * axis[1];
	result[1][2] = 0 + temp[1] * axis[2] + s * axis[0];

	result[2][0] = 0 + temp[2] * axis[0] + s * axis[1];
	result[2][1] = 0 + temp[2] * axis[1] - s * axis[0];
	result[2][2] = c + temp[2] * axis[2];

	return result;
}
template <typename T> /*glm*/
_matrix4<T> _rotate ( const _matrix4<T>& m, T angle, const _vector3<T> & v ){
	T const a = angle;
	T const c = cos(a);
	T const s = sin(a);

	_vector3<T> axis(_normalize(v));
	_vector3<T> temp(axis* (T(1) - c) );

	_matrix4<T> Rotate(0);
	Rotate[0][0] = c + temp[0] * axis[0];
	Rotate[0][1] = 0 + temp[0] * axis[1] + s * axis[2];
	Rotate[0][2] = 0 + temp[0] * axis[2] - s * axis[1];

	Rotate[1][0] = 0 + temp[1] * axis[0] - s * axis[2];
	Rotate[1][1] = c + temp[1] * axis[1];
	Rotate[1][2] = 0 + temp[1] * axis[2] + s * axis[0];

	Rotate[2][0] = 0 + temp[2] * axis[0] + s * axis[1];
	Rotate[2][1] = 0 + temp[2] * axis[1] - s * axis[0];
	Rotate[2][2] = c + temp[2] * axis[2];

	_matrix4<T> Result(0);
	Result[0] = m[0] * Rotate[0][0] + m[1] * Rotate[0][1] + m[2] * Rotate[0][2];
	Result[1] = m[0] * Rotate[1][0] + m[1] * Rotate[1][1] + m[2] * Rotate[1][2];
	Result[2] = m[0] * Rotate[2][0] + m[1] * Rotate[2][1] + m[2] * Rotate[2][2];
	Result[3] = m[3];
	return Result;
}
template <typename T> /*glm*/
_matrix4<T> _yawpitchroll (const T&yaw, const T&pitch,const T&roll){
	T tmp_ch = cos(yaw);
	T tmp_sh = sin(yaw);
	T tmp_cp = cos(pitch);
	T tmp_sp = sin(pitch);
	T tmp_cb = cos(roll);
	T tmp_sb = sin(roll);

	_matrix4<T> Result;
	Result[0][0] = tmp_ch * tmp_cb + tmp_sh * tmp_sp * tmp_sb;
	Result[0][1] = tmp_sb * tmp_cp;
	Result[0][2] = -tmp_sh * tmp_cb + tmp_ch * tmp_sp * tmp_sb;
	Result[0][3] = static_cast<T>(0);
	Result[1][0] = -tmp_ch * tmp_sb + tmp_sh * tmp_sp * tmp_cb;
	Result[1][1] = tmp_cb * tmp_cp;
	Result[1][2] = tmp_sb * tmp_sh + tmp_ch * tmp_sp * tmp_cb;
	Result[1][3] = static_cast<T>(0);
	Result[2][0] = tmp_sh * tmp_cp;
	Result[2][1] = -tmp_sp;
	Result[2][2] = tmp_ch * tmp_cp;
	Result[2][3] = static_cast<T>(0);
	Result[3][0] = static_cast<T>(0);
	Result[3][1] = static_cast<T>(0);
	Result[3][2] = static_cast<T>(0);
	Result[3][3] = static_cast<T>(1);
	return Result;
}