#include "Quaternion.hpp"

namespace smk
{
	void Quaternion::setAsRotation(ValueType const angle, ValueType const x, ValueType const y, ValueType const z) {
		ValueType vector_length = sqrt(x * x + y * y + z * z);
		if (vector_length < epsilon<ValueType>()) {
			setAsIdentity();
		} else {
			ValueType half_angle = angle / ValueType(2.0);
			ValueType sin_halfangle = sin(half_angle);

			_v[0] = (x / vector_length) * sin_halfangle;
			_v[1] = (y / vector_length) * sin_halfangle;
			_v[2] = (z / vector_length) * sin_halfangle;
			_v[3] = cos(half_angle);

			//normalize();
		}
	}

	void Quaternion::getRotation(ValueType& angle, ValueType& x, ValueType& y, ValueType& z) {
		ValueType sqrt_length = sqrt(_v[0] * _v[0] + _v[1] * _v[1] + _v[2] * _v[2]);

		if (sqrt_length > ValueType(0.0)) {
			angle = ValueType(2.0) * acos( ValueType(_v[3]) );
			x = _v[0] / sqrt_length;
			y = _v[1] / sqrt_length;
			z = _v[2] / sqrt_length;
		} else {
			x = ValueType(0.0);
			y = ValueType(0.0);
			z = ValueType(1.0);
			angle = ValueType(0.0);
		}
	}

	void Quaternion::setAsRotation(Vector3<ValueType> from, Vector3<ValueType> to) {
		from.normalize();
		to.normalize();

		ValueType dot_plus_one = ValueType(1.0) + from * to;

		// if ( (cos(angle) + 1.0) <= 0.0) ) // opposite vectors 180-Turn
		// if ( (cos(angle) - 1.0) <= 0.0) ) // coincident vectors 0-Turn

		// check collinear vector with opposite directions
		if (dot_plus_one < epsilon<ValueType>()) {
			// Get an orthogonal vector of the given vector
			// in a plane with maximum vector coordinates.
			// Then use it as quaternion axis with pi angle
			// Trick is to realize one value at least is >0.6 for a normalized vector.
			if (fabs(from.x()) < ValueType(0.6)) {
				dot_plus_one = sqrt(ValueType(1.0) - from.x() * from.x()); // norm
				_v[0] = ValueType(0.0);
				_v[1] = from.z() / dot_plus_one;
				_v[2] = -from.y() / dot_plus_one;
				_v[3] = ValueType(0.0);
			} else if (fabs(from.y()) < 0.6) {
				dot_plus_one = sqrt(ValueType(1.0) - from.y() * from.y()); // norm
				_v[0] = -from.z() / dot_plus_one;
				_v[1] = ValueType(0.0);
				_v[2] = from.x() / dot_plus_one;
				_v[3] = ValueType(0.0);
			} else {
				dot_plus_one = sqrt(ValueType(1.0) - from.z() * from.z()); // norm
				_v[0] = from.y() / dot_plus_one;
				_v[1] = -from.x() / dot_plus_one;
				_v[2] = ValueType(0.0);
				_v[3] = ValueType(0.0);
			}
		} else {
			// calculate the axis of rotation (from ^ to)
			// calculate the angle of rotation ( acos( (from * to) / ( |from| * |to| ) ) )

			// the following equation supports the coincident vectors case.

			ValueType angle = sqrt(ValueType(0.5) * dot_plus_one);
			Vector3<ValueType> cross_product = from ^ to / (ValueType(2.0) * angle);
			_v[0] = cross_product.x();
			_v[1] = cross_product.y();
			_v[2] = cross_product.z();
			_v[3] = angle;
		}
	}

	void Quaternion::setAsSlerp(Quaternion const& from, Quaternion const& to, float const t) {
		double omega, cosomega, sinomega, scale_from, scale_to;
		Quaternion result(to);

		cosomega = from.x() * to.x() + from.y() * to.y() + from.z() * to.z() + from.w() * to.w();

		if (cosomega < ValueType(0.0)) {
			cosomega = -cosomega;
			result = -to;
		}

		if((ValueType(1.0) - cosomega) > epsilon<float>()) {
			omega = acos(cosomega);
			sinomega = sin(omega);
			scale_from = sin((ValueType(1.0) - t) * omega) / sinomega;
			scale_to = sin(t * omega) / sinomega;
		}
		else // Quaternions are very close, so we can linear interpolate
		{
			scale_from = ValueType(1.0) - t;
			scale_to = t;
		}

        *this = (from * ValueType(scale_from)) + (result * ValueType(scale_to));
	}

} // namespace rengine

