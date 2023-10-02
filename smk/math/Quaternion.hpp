#pragma once

#include "Vector.hpp"
#include <cassert>

namespace smk
{
	class Quaternion
	{
	public:
		typedef float ValueType;

		Quaternion() { setAsIdentity(); }

		Quaternion(Quaternion const& quaternion) {
			_v[0] = quaternion[0];
			_v[1] = quaternion[1];
			_v[2] = quaternion[2];
			_v[3] = quaternion[3];
		}

		Quaternion(ValueType const x, ValueType const y, ValueType const z, ValueType const w) {
			_v[0] = x;
			_v[1] = y;
			_v[2] = z;
			_v[3] = w;
		}

		template <typename T> Quaternion(Vector4<T> const& v) {
			_v[0] = ValueType(v[0]);
			_v[1] = ValueType(v[1]);
			_v[2] = ValueType(v[2]);
			_v[3] = ValueType(v[3]);
		}

		template <typename T> operator Vector4<T>() {
			return Vector4<T>(_v[0], _v[1], _v[2], _v[3]);
		}

		ValueType* ptr() { return _v; }
		ValueType const * ptr() const {	return _v; }

		void set(ValueType const x, ValueType const y, ValueType const z, ValueType const w) {
			_v[0] = x;
			_v[1] = y;
			_v[2] = z;
			_v[3] = w;
		}

		template <typename T> void set(Vector4<T> const& v) {
			_v[0] = ValueType(v[0]);
			_v[1] = ValueType(v[1]);
			_v[2] = ValueType(v[2]);
			_v[3] = ValueType(v[3]);
		}

		ValueType& x() { return _v[0]; }
		ValueType& y() { return _v[1]; }
		ValueType& z() { return _v[2]; }
		ValueType& w() { return _v[3]; }

		ValueType x() const { return _v[0]; }
		ValueType y() const { return _v[1]; }
		ValueType z() const { return _v[2]; }
		ValueType w() const { return _v[3]; }

		bool operator == (Quaternion const& v) const {
			return _v[0] == v._v[0] && _v[1] == v._v[1] && _v[2] == v._v[2] && _v[3] == v._v[3];
		}

		bool operator != (Quaternion const& v) const {
			return _v[0] != v._v[0] || _v[1] != v._v[1] || _v[2] != v._v[2] || _v[3] != v._v[3];
		}

		bool operator < (Quaternion const& v) const {
			if (_v[0] < v._v[0]) return true;
			else if (_v[0] > v._v[0]) return false;
			else if (_v[1] < v._v[1]) return true;
			else if (_v[1] > v._v[1]) return false;
			else if (_v[2] < v._v[2]) return true;
			else if (_v[2] > v._v[2]) return false;
			else return (_v[3] < v._v[3]);
		}

		Quaternion& operator = (Quaternion const& quaternion) {
			_v[0] = quaternion[0];
			_v[1] = quaternion[1];
			_v[2] = quaternion[2];
			_v[3] = quaternion[3];
			return *this;
		}

		ValueType& operator [] (int const i) { return _v[i]; }
		ValueType operator [] (int const i) const { return _v[i]; }

		Quaternion operator * (ValueType const rhs) const {
			return Quaternion(_v[0] * rhs, _v[1] * rhs, _v[2] * rhs, _v[3] * rhs);
		}

		Quaternion& operator *= (ValueType const rhs) {
			_v[0] *= rhs;
			_v[1] *= rhs;
			_v[2] *= rhs;
			_v[3] *= rhs;
			return *this;
		}

		Quaternion operator / (ValueType const rhs) const {
			return Quaternion(_v[0] / rhs, _v[1] / rhs, _v[2] / rhs, _v[3] / rhs);
		}

		Quaternion& operator /= (ValueType const rhs) {
			_v[0] /= rhs;
			_v[1] /= rhs;
			_v[2] /= rhs;
			_v[3] /= rhs;
			return *this;
		}

		Quaternion operator + (Quaternion const& rhs) const {
			return Quaternion(_v[0] + rhs._v[0], _v[1] + rhs._v[1], _v[2] + rhs._v[2], _v[3] + rhs._v[3]);
		}

		Quaternion& operator += (Quaternion const& rhs) {
			_v[0] += rhs._v[0];
			_v[1] += rhs._v[1];
			_v[2] += rhs._v[2];
			_v[3] += rhs._v[3];
			return *this;
		}

		Quaternion operator - (Quaternion const& rhs) const {
			return Quaternion(_v[0] - rhs._v[0], _v[1] - rhs._v[1], _v[2] - rhs._v[2], _v[3] - rhs._v[3]);
		}

		Quaternion& operator -= (Quaternion const& rhs) {
			_v[0] -= rhs._v[0];
			_v[1] -= rhs._v[1];
			_v[2] -= rhs._v[2];
			_v[3] -= rhs._v[3];
			return *this;
		}

		Quaternion operator * (Quaternion const& rhs) const {
			return Quaternion(
				rhs._v[3] * _v[0] + rhs._v[0] * _v[3] + rhs._v[1] * _v[2] - rhs._v[2] * _v[1],
				rhs._v[3] * _v[1] - rhs._v[0] * _v[2] + rhs._v[1] * _v[3] + rhs._v[2] * _v[0],
				rhs._v[3] * _v[2] + rhs._v[0] * _v[1] - rhs._v[1] * _v[0] + rhs._v[2] * _v[3],
				rhs._v[3] * _v[3] - rhs._v[0] * _v[0] - rhs._v[1] * _v[1] - rhs._v[2] * _v[2]
			);
		}

		Quaternion& operator *= (Quaternion const& rhs) {
			(*this) = (*this) * rhs;
			return *this;
		}

		Quaternion operator - () const {
			return Quaternion(-_v[0], -_v[1], -_v[2], -_v[3]);
		}

		ValueType length() const {
			return sqrt(_v[0] * _v[0] + _v[1] * _v[1] + _v[2] * _v[2] + _v[3] * _v[3]);
		}

		ValueType lengthSquared() const {
			return _v[0] * _v[0] + _v[1] * _v[1] + _v[2] * _v[2] + _v[3] * _v[3];
		}

		ValueType normalize() {
			ValueType norm = Quaternion::length();
			if (norm > ValueType(0.0)) {
				ValueType inv = ValueType(1.0) / norm;
				_v[0] *= inv;
				_v[1] *= inv;
				_v[2] *= inv;
				_v[3] *= inv;
			}
			return norm;
		}

		Quaternion conjugate() const {
			return Quaternion(-_v[0], -_v[1], -_v[2], _v[3]);
		}

		Quaternion unitInverse() const {
			assert( equivalent(length(), ValueType(1.0)) );
			return conjugate();
		}

		Quaternion inverse() const {
			assert( !equivalent(length(), ValueType(0.0)) );
			return conjugate() / lengthSquared();
		}

		Quaternion operator / (Quaternion const& denominator) const {
			return (*this) * denominator.inverse();
		}

        Quaternion& operator /= (Quaternion const& denominator) {
            (*this) = (*this) * denominator.inverse();
            return *this;
        }

		bool isIdentity() const {
			return _v[0] == 0.0 && _v[1] == 0.0 && _v[2] == 0.0 && _v[3] == 1.0;
		}

		bool isZeroRotation() const {
			return isIdentity();
		}

		void setAsIdentity() {
			_v[0] = ValueType(0.0);
			_v[1] = ValueType(0.0);
			_v[2] = ValueType(0.0);
			_v[3] = ValueType(1.0);
		}

		//
		// rotation methods
		//
		void setAsRotation(ValueType const angle, ValueType const x, ValueType const y, ValueType const z);
		template <typename T> void setAsRotation(T const angle, Vector3<T> const& vector) {
			setAsRotation(ValueType(angle), ValueType(vector.x()), ValueType(vector.y()), ValueType(vector.z()));
		}
		void setAsRotation(Vector3<ValueType> from, Vector3<ValueType> to);
		void setAsSlerp(Quaternion const& from, Quaternion const& to, float const t);
		void getRotation(ValueType& angle, ValueType& x, ValueType& y, ValueType& z);

		template <typename T> void getRotation(T& angle, Vector3<T>& vector)
		{
			ValueType x, y, z, conversion_angle;
			getRotation(conversion_angle, x, y, z);
			vector.set( T(x), T(y) , T(z) );
			angle = T(conversion_angle);
		}

		static Quaternion rotate(ValueType const angle, ValueType const x, ValueType const y, ValueType const z) {
			Quaternion quaternion;
			quaternion.setAsRotation(angle, x, y, z);
			return quaternion;
		}

		template <typename T> static Quaternion rotate(T const angle, Vector3<T> const& vector) {
			Quaternion quaternion;
			quaternion.setAsRotation(angle, vector);
			return quaternion;
		}

		template <typename T> static Quaternion rotate(Vector3<ValueType> from, Vector3<ValueType> to)	{
			Quaternion quaternion;
			quaternion.setAsRotation(from, to);
			return quaternion;
		}

		static Quaternion slerp(Quaternion const& from, Quaternion const& to, float const t) {
			Quaternion quaternion;
			quaternion.setAsSlerp(from, to, t);
			return quaternion;
		}
	private:
		ValueType  _v[4];
	};

	inline Quaternion operator * (Quaternion::ValueType const lhs, Quaternion rhs) {
		rhs *= lhs;
		return rhs;
	}

	inline Quaternion operator / (Quaternion::ValueType const lhs, Quaternion rhs) {
		rhs /= lhs;
		return rhs;
	}

	// Rotate a vector by a quaternion
	template <typename T> inline Vector3<T> operator * (Quaternion  const& rhs, Vector3<T> v) {
		// nVidia SDK implementation
		Vector3<T> uv, uuv;
		Vector3<T> qvec( T(rhs.x()), T(rhs.y()), T(rhs.z()));
		uv = qvec ^ v;
		uuv = qvec ^ uv;
		uv *= (T(2.0) * T(rhs.w()));
		uuv *= T(2.0);
		return v + uv + uuv;
	}

	template <typename T> inline Vector3<T> operator * (Vector3<T> lhs, Quaternion  const& rhs)	{
		lhs = rhs * lhs;
		return lhs;
	}

} // namespace 
