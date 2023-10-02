#include "Matrix.hpp"

namespace smk
{
	template<typename T> inline void fastSwap(T& left, T& right, T& tmp) { tmp = left; left = right; right = tmp; }

	bool Matrix44::setAsInverse4x3Of(Matrix44 const& rhs) {
		assert(rhs.isAffine());

		if (this == &rhs) {
			Matrix44 matrix(rhs);
			return setAsInverse4x3Of(matrix);
		}

		// invert rotation 3x3
		ValueType r00, r01, r02, r10, r11, r12, r20, r21, r22;
		r00 = rhs._mat[0][0]; r01 = rhs._mat[0][1]; r02 = rhs._mat[0][2];
		r10 = rhs._mat[1][0]; r11 = rhs._mat[1][1]; r12 = rhs._mat[1][2];
		r20 = rhs._mat[2][0]; r21 = rhs._mat[2][1]; r22 = rhs._mat[2][2];

		_mat[0][0] = r11 * r22 - r12 * r21;
		_mat[0][1] = r02 * r21 - r01 * r22;
		_mat[0][2] = r01 * r12 - r02 * r11;

		ValueType invd = ValueType(1.0) / (r00 * _mat[0][0] + r10 * _mat[0][1] + r20 * _mat[0][2]);
		r00 *= invd;
		r10 *= invd;
		r20 *= invd;

		_mat[0][0] *= invd;
		_mat[0][1] *= invd;
		_mat[0][2] *= invd;
		_mat[0][3] = ValueType(0.0);
		_mat[1][0] = r12 * r20 - r10 * r22;
		_mat[1][1] = r00 * r22 - r02 * r20;
		_mat[1][2] = r02 * r10 - r00 * r12;
		_mat[1][3] = ValueType(0.0);
		_mat[2][0] = r10 * r21 - r11 * r20;
		_mat[2][1] = r01 * r20 - r00 * r21;
		_mat[2][2] = r00 * r11 - r01 * r10;
		_mat[2][3] = ValueType(0.0);
		_mat[3][3] = ValueType(1.0);

		// invert translation
		r10 = rhs._mat[3][0]; r11 = rhs._mat[3][1]; r12 = rhs._mat[3][2];
		_mat[3][0] = - (r10 * _mat[0][0] + r11 * _mat[1][0] + r12 * _mat[2][0]);
		_mat[3][1] = - (r10 * _mat[0][1] + r11 * _mat[1][1] + r12 * _mat[2][1]);
		_mat[3][2] = - (r10 * _mat[0][2] + r11 * _mat[1][2] + r12 * _mat[2][2]);

		return true;
	}

	bool Matrix44::setAsInverse4x4Of(Matrix44 const& rhs) {
		if (this == &rhs) {
			Matrix44 matrix(rhs);
			return setAsInverse4x4Of(matrix);
		}

		int indxc[4], indxr[4], ipiv[4];
		int i, j, k, l, ll;
		int icol = 0;
		int irow = 0;
		ValueType temp, pivinv, dum, big;

		*this = rhs;

		for (j = 0; j < 4; j++) {
			ipiv[j] = 0;
		}

		for(i = 0; i < 4; i++) {
			big = ValueType(0.0);
			for (j = 0; j < 4; j++) {
				if (ipiv[j] != 1) {
					for (k = 0; k < 4; k++)	{
						if (ipiv[k] == 0) {
							if (absolute(operator()(j, k)) >= big) {
								big = absolute(operator()(j, k));
								irow = j;
								icol = k;
							}
						} else if (ipiv[k] > 1) {
							return false;
						}
					}
				}
			}

			++(ipiv[icol]);
			if (irow != icol) {
				for (l = 0; l < 4; l++) {
					fastSwap(operator()(irow, l), operator()(icol, l), temp);
				}
			}

			indxr[i] = irow;
			indxc[i] = icol;
			if (operator()(icol, icol) == 0) {
				return false;
			}

			pivinv = ValueType(1.0) / operator()(icol, icol);
			operator()(icol, icol) = 1;

			for (l = 0; l < 4; l++) {
				operator()(icol, l) *= pivinv;
			}

			for (ll = 0; ll < 4; ll++) {
				if (ll != icol) {
					dum = operator()(ll, icol);
					operator()(ll, icol) = 0;
					for (l = 0; l < 4; l++) {
						operator()(ll, l) -= operator()(icol, l) * dum;
					}
				}
			}
		}

		for (int lx = 4; lx > 0; --lx) {
			if (indxr[lx - 1] != indxc[lx - 1])
				for (k = 0; k < 4; k++) {
					fastSwap(operator()(k, indxr[lx - 1]), operator()(k, indxc[lx - 1]), temp);
				}
		}

		return true;
	}

	Quaternion Matrix44::getRotation() const {
		Quaternion quaternion;
		ValueType trace = ValueType(1.0) + _mat[0][0] + _mat[1][1] + _mat[2][2];
		if (trace > epsilon<ValueType>()) {
			ValueType s = ValueType(0.5) / sqrt(trace);
			quaternion.w() = ValueType(0.25) / s;
			quaternion.x() = (_mat[1][2] - _mat[2][1]) * s;
			quaternion.y() = (_mat[2][0] - _mat[0][2]) * s;
			quaternion.z() = (_mat[0][1] - _mat[1][0]) * s;
		} else {
			if (_mat[0][0] > _mat[1][1] && _mat[0][0] > _mat[2][2]) {
				ValueType s = ValueType(2.0) * sqrt(ValueType(1.0) + _mat[0][0] - _mat[1][1] - _mat[2][2]);
				quaternion.w() = (_mat[1][2] - _mat[2][1]) / s;
				quaternion.x() = ValueType(0.25) * s;
				quaternion.y() = (_mat[0][1] + _mat[1][0]) / s;
				quaternion.z() = (_mat[2][0] + _mat[0][2]) / s;
			} else if (_mat[1][1] > _mat[2][2]) {
				ValueType s = ValueType(2.0) * sqrt(ValueType(1.0) - _mat[0][0] + _mat[1][1] - _mat[2][2]);
				quaternion.w() = (_mat[2][0] - _mat[0][2]) / s;
				quaternion.x() = (_mat[0][1] + _mat[1][0]) / s;
				quaternion.y() = ValueType(0.25) * s;
				quaternion.z() = (_mat[1][2] + _mat[2][1]) / s;
			} else {
				ValueType s = ValueType(2.0) * sqrt(ValueType(1.0) - _mat[0][0] - _mat[1][1] + _mat[2][2]);
				quaternion.w() = (_mat[0][1] - _mat[1][0]) / s;
				quaternion.x() = (_mat[2][0] + _mat[0][2]) / s;
				quaternion.y() = (_mat[1][2] + _mat[2][1]) / s;
				quaternion.z() = ValueType(0.25) * s;
			}
		}

		return quaternion;
	}

} // namespace rengine

