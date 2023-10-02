#pragma once

#include "Vector.hpp"
#include "Quaternion.hpp"

namespace smk
{
	/* 4x4 Row major Matrix

	Array Index	            Identity Matrix			Translation Matrix
	--------------------	--------------------	--------------------
	[01 , 02 , 03 , 04 ]	[1.0, 0.0, 0.0, 0.0]	[1.0, 0.0, 0.0, 0.0]
	[05 , 06 , 07 , 08 ]	[0.0, 1.0, 0.0, 0.0]	[0.0, 1.0, 0.0, 0.0]
	[09 , 10 , 11 , 12 ]	[0.0, 0.0, 1.0, 0.0]	[0.0, 0.0, 1.0, 0.0]
	[13 , 14 , 15 , 16 ]	[0.0, 0.0, 0.0, 1.0]	[T.x, T.y, T.z, 1.0]

	Vector * Matrix [matrix.preMult(vector)]
	-------------------------------------------
	[V.x, V.y, V.z, V.w] * [1.0, 0.0, 0.0, 0.0]
                           [0.0, 1.0, 0.0, 0.0]
                           [0.0, 0.0, 1.0, 0.0]
                           [T.x, T.y, T.z, 1.0]

	Matrix * Vector = Vector * Matrix-Transposed [matrix.postMult(vector)]
	-----------------------------------------------------------------------
	[V.x, V.y, V.z, V.w] * [1.0, 0.0, 0.0, T.x]
                           [0.0, 1.0, 0.0, T.y]
                           [0.0, 0.0, 1.0, T.z]
                           [0.0, 0.0, 0.0, 1.0]

	Matrix Multiplication
	---------------------
	M3 = M1 * M2     M1 = M1 * M2      M2 = M1 * M2
	~~~~~~~~~~~~~~~	 ~~~~~~~~~~~~~~	   ~~~~~~~~~~~~~~~
	M3.mult(M1, M2)	 M1 *= M2		   M2.preMult(M1);
	M3 = M1 * M2	 M1.postMult(M2)

	*/

	class Matrix44
	{
	public:
		static int const number_of_elements = 16;

		typedef float ValueType;
		typedef int SizeType;

		Matrix44();
		Matrix44(Matrix44 const& mat);
		Matrix44(Quaternion const& quaternion);

		template <typename T> Matrix44(T const* const matrix_ptr);

		bool operator == (Matrix44 const& matrix) const;
		bool operator != (Matrix44 const& matrix) const;

		ValueType& operator()(SizeType const row, SizeType const col);
		ValueType operator()(SizeType const row, SizeType const col) const;
		ValueType& operator[](SizeType const index);
		ValueType operator[](SizeType const index) const;
		Matrix44& operator = (Matrix44 const& rhs);

		void setRow(SizeType const row, ValueType const x, ValueType const y, ValueType const z, ValueType const w);

		template <typename T> void setRow(SizeType const row, Vector4<T> const& value);
		template <typename T> void set(T const* const matrix_ptr);

		void set(
			ValueType a00, ValueType a01, ValueType a02, ValueType a03,
			ValueType a10, ValueType a11, ValueType a12, ValueType a13,
			ValueType a20, ValueType a21, ValueType a22, ValueType a23,
			ValueType a30, ValueType a31, ValueType a32, ValueType a33);

		ValueType* ptr();
		ValueType const* ptr() const;

		bool isIdentity() const;
		void setAsIdentity();

		template <typename T> Vector3<T> preMult(Vector3<T> const& v) const;
		template <typename T> Vector3<T> postMult(Vector3<T> const& v) const;
		template <typename T> Vector3<T> operator * (Vector3<T> const& v) const;

		template <typename T> Vector4<T> preMult(Vector4<T> const& v) const;
		template <typename T> Vector4<T> postMult(Vector4<T> const& v) const;
		template <typename T> Vector4<T> operator * (Vector4<T> const& v) const;

		void mult(Matrix44 const& lhs, Matrix44 const& rhs);
        void preMult(Matrix44 const& rhs); // M2 = M1 * M2
        void postMult(Matrix44 const& rhs); // M1 = M1 * M2
        void operator *= (Matrix44 const & rhs); // M1 = M1 * M2  = this.postMult(rhs)
        Matrix44 operator * (Matrix44 const& rhs) const;
		void operator += (Matrix44 const & rhs);
		Matrix44 operator + (Matrix44 const& rhs) const;
		void operator -= (Matrix44 const & rhs);
		Matrix44 operator - (Matrix44 const& rhs) const;

		// An affine matrix is a 4x4 matrix with column 3 equal to (0, 0, 0, 1),
		bool isAffine() const;

		static ValueType innerProduct(Matrix44 const& lhs, Matrix44 const& rhs, SizeType const row, SizeType const col);

		bool invert();
		Matrix44 inverse() const;
		bool setAsInverseOf(Matrix44 const& rhs);
        bool setAsInverse4x3Of(Matrix44 const& rhs); // affine inverse
        bool setAsInverse4x4Of(Matrix44 const& rhs);

		void transpose();
		void setAsTransposeOf(Matrix44 const& rhs);

		void setAsTranslation(ValueType const x, ValueType const y, ValueType const z);
		template <typename T> void setAsTranslation(Vector3<T> const& v);

		void setAsScale(ValueType const x, ValueType const y, ValueType const z);
		template <typename T> void setAsScale(Vector3<T> const& v);

		void setAsRotation(Quaternion const& quaternion);
		void setAsRotation(ValueType const angle, ValueType const x, ValueType const y, ValueType const z);
		template <typename T> void setAsRotation(T const angle, Vector3<T> const& vector);
		template <typename T> void setAsRotation(Vector3<T> from, Vector3<T> to);

		Quaternion getRotation() const;

		static Matrix44 translate(ValueType const x, ValueType const y, ValueType const z);
		template <typename T> static Matrix44 translate(Vector3<T> const& v);
		static Matrix44 scale(ValueType const x, ValueType const y, ValueType const z);
		template <typename T> static Matrix44 scale(Vector3<T> const& v);
		static Matrix44 rotate(ValueType const angle, ValueType const x, ValueType const y, ValueType const z);
		template <typename T> static Matrix44 rotate(T const angle, Vector3<T> const& v);
		template <typename T> static Matrix44 rotate(Vector3<T> const& from, Vector3<T> const& to);

		//only changes the translation elements of the matrix
		template <typename T> void setTranslation(Vector3<T> const& translation);
		void setTranslation(ValueType const x, ValueType const y, ValueType const z);
		Vector3<ValueType> getTranslation() const;

		//
		// Perspective
		//
        void setAsOrtho(ValueType const left, ValueType const right, ValueType const bottom, ValueType const top, ValueType const z_near, ValueType const z_far);
        void setAsOrtho2D(ValueType const left, ValueType const right, ValueType const bottom, ValueType const top);
        void setAsFrustum(ValueType const left, ValueType const right, ValueType const bottom, ValueType const top, ValueType const z_near, ValueType const z_far);
        void setAsPerspective(ValueType const fovy, ValueType const aspect_ratio, ValueType const z_near, ValueType const z_far);

        static Matrix44 ortho(ValueType const left, ValueType const right, ValueType const bottom, ValueType const top, ValueType const z_near, ValueType const z_far);
        static Matrix44 ortho2D(ValueType const left, ValueType const right, ValueType const bottom, ValueType const top);
        static Matrix44 frustum(ValueType const left, ValueType const right, ValueType const bottom, ValueType const top, ValueType const z_near, ValueType const z_far);
        static Matrix44 perspective(ValueType const fovy, ValueType const aspect_ratio, ValueType const z_near, ValueType const z_far);

        //
        // lookAt
        //
        template <typename T> void setAsLookAt(Vector3<T> const& eye, Vector3<T> const& origin, Vector3<T> const& up);
        template <typename T> static Matrix44 lookAt(Vector3<T> const& eye, Vector3<T> const& origin, Vector3<T> const& up);

	private:
		ValueType _mat[4][4];
	};

	template <typename T> Vector3<T> operator * (Vector3<T> const& vector, Matrix44 const& matrix);
	template <typename T> Vector4<T> operator * (Vector4<T> const& vector, Matrix44 const& matrix);


	//
	// Implementation
	//

	inline Matrix44::Matrix44() { 
		setAsIdentity(); 
	}

	inline Matrix44::Matrix44(Matrix44 const& mat) { 
		set(mat.ptr()); 
	}

	inline Matrix44::Matrix44(Quaternion const& quaternion) {
		setAsIdentity();
		setAsRotation(quaternion);
	}

	template <typename T> inline Matrix44::Matrix44(T const* const matrix_ptr) {
		set(matrix_ptr);
	}

	inline bool Matrix44::operator == (Matrix44 const& matrix) const {
		for (int i = 0; i != number_of_elements; ++i) 
			if (ptr()[i] != matrix.ptr()[i]) 
				return false;
		return true;
	}

	inline bool Matrix44::operator != (Matrix44 const& matrix) const {
		return ! ((*this) == matrix);
	}

	inline Matrix44::ValueType& Matrix44::operator()(SizeType const row, SizeType const col) {
        assert(row < 4);
        assert(col < 4);
		return _mat[row][col];
	}

	inline Matrix44::ValueType Matrix44::operator()(SizeType const row, SizeType const col) const {
        assert(row < 4);
        assert(col < 4);
		return _mat[row][col];
	}

	inline Matrix44::ValueType& Matrix44::operator[](SizeType const index){
        assert(index < number_of_elements);
		return ptr()[index];
	}

	inline Matrix44::ValueType Matrix44::operator[](SizeType const index) const
	{
        assert(index < number_of_elements);
		return ptr()[index];
	}

	inline Matrix44& Matrix44::operator = (Matrix44 const& rhs) {
		if (this != &rhs)
			set(rhs.ptr());
		return *this;
	}

	inline void Matrix44::setRow(SizeType const row, ValueType const x, ValueType const y, ValueType const z, ValueType const w) {
        assert(row < 4);
		_mat[row][0] = x;
		_mat[row][1] = y;
		_mat[row][2] = z;
		_mat[row][3] = w;
	}

	template <typename T> inline void Matrix44::setRow(SizeType const row, Vector4<T> const& value) {
		assert( (row >= 0) && (row < 4) );
		setRow(row, value.x(), value.y(), value.z(), value.w());
	}

	inline void Matrix44::setAsIdentity() {
		setRow(0, 1.0, 0.0, 0.0, 0.0);
		setRow(1, 0.0, 1.0, 0.0, 0.0);
		setRow(2, 0.0, 0.0, 1.0, 0.0);
		setRow(3, 0.0, 0.0, 0.0, 1.0);
	}

	template <typename T> inline void Matrix44::set(T const* matrix_ptr) {
		ptr()[0] = ValueType(matrix_ptr[0]);
		ptr()[1] = ValueType(matrix_ptr[1]);
		ptr()[2] = ValueType(matrix_ptr[2]);
		ptr()[3] = ValueType(matrix_ptr[3]);
		ptr()[4] = ValueType(matrix_ptr[4]);
		ptr()[5] = ValueType(matrix_ptr[5]);
		ptr()[6] = ValueType(matrix_ptr[6]);
		ptr()[7] = ValueType(matrix_ptr[7]);
		ptr()[8] = ValueType(matrix_ptr[8]);
		ptr()[9] = ValueType(matrix_ptr[9]);
		ptr()[10] = ValueType(matrix_ptr[10]);
		ptr()[11] = ValueType(matrix_ptr[11]);
		ptr()[12] = ValueType(matrix_ptr[12]);
		ptr()[13] = ValueType(matrix_ptr[13]);
		ptr()[14] = ValueType(matrix_ptr[14]);
		ptr()[15] = ValueType(matrix_ptr[15]);
	}

	inline void Matrix44::set(
		ValueType a00, ValueType a01, ValueType a02, ValueType a03,
		ValueType a10, ValueType a11, ValueType a12, ValueType a13,
		ValueType a20, ValueType a21, ValueType a22, ValueType a23,
		ValueType a30, ValueType a31, ValueType a32, ValueType a33)
	{
		setRow(0, a00, a01, a02, a03);
		setRow(1, a10, a11, a12, a13);
		setRow(2, a20, a21, a22, a23);
		setRow(3, a30, a31, a32, a33);
	}

	inline Matrix44::ValueType* Matrix44::ptr() { return (ValueType*)_mat; }
	inline Matrix44::ValueType const* Matrix44::ptr() const { return (ValueType const*) _mat; }

	inline bool Matrix44::isIdentity() const { 
		return
			_mat[0][0] == ValueType(1.0) && _mat[0][1] == ValueType(0.0) && _mat[0][2] == ValueType(0.0) &&  _mat[0][3] == ValueType(0.0) &&
			_mat[1][0] == ValueType(0.0) && _mat[1][1] == ValueType(1.0) && _mat[1][2] == ValueType(0.0) &&  _mat[1][3] == ValueType(0.0) &&
			_mat[2][0] == ValueType(0.0) && _mat[2][1] == ValueType(0.0) && _mat[2][2] == ValueType(1.0) &&  _mat[2][3] == ValueType(0.0) &&
			_mat[3][0] == ValueType(0.0) && _mat[3][1] == ValueType(0.0) && _mat[3][2] == ValueType(0.0) &&  _mat[3][3] == ValueType(1.0);
	}

	template <typename T> inline Vector3<T> Matrix44::preMult(Vector3<T> const& v) const {
		T d = T(1.0) / (T(_mat[0][3]) * v.x() + T(_mat[1][3]) * v.y() + T(_mat[2][3]) * v.z() + T(_mat[3][3])) ;
		return Vector3<T>(
			(T(_mat[0][0]) * v.x() + T(_mat[1][0]) * v.y() + T(_mat[2][0]) * v.z() + T(_mat[3][0])) * d,
			(T(_mat[0][1]) * v.x() + T(_mat[1][1]) * v.y() + T(_mat[2][1]) * v.z() + T(_mat[3][1])) * d,
			(T(_mat[0][2]) * v.x() + T(_mat[1][2]) * v.y() + T(_mat[2][2]) * v.z() + T(_mat[3][2])) * d
			);
	}

	template <typename T> inline Vector3<T> Matrix44::postMult(Vector3<T> const& v) const {
		T d = T(1.0) / (T(_mat[3][0]) * v.x() + T(_mat[3][1]) * v.y() + T(_mat[3][2]) * v.z() + T(_mat[3][3]));

		return Vector3<T>(
			(T(_mat[0][0]) * v.x() + T(_mat[0][1]) * v.y() + T(_mat[0][2]) * v.z() + T(_mat[0][3])) * d,
			(T(_mat[1][0]) * v.x() + T(_mat[1][1]) * v.y() + T(_mat[1][2]) * v.z() + T(_mat[1][3])) * d,
			(T(_mat[2][0]) * v.x() + T(_mat[2][1]) * v.y() + T(_mat[2][2]) * v.z() + T(_mat[2][3])) * d
			);
	}

	template <typename T> inline Vector3<T> Matrix44::operator * (Vector3<T> const& v) const { return postMult(v); }

	template <typename T> inline Vector4<T> Matrix44::preMult(Vector4<T> const& v) const {
		return Vector4<T>(
			(T(_mat[0][0]) * v.x() + T(_mat[1][0]) * v.y() + T(_mat[2][0]) * v.z() + T(_mat[3][0]) * v.w()),
			(T(_mat[0][1]) * v.x() + T(_mat[1][1]) * v.y() + T(_mat[2][1]) * v.z() + T(_mat[3][1]) * v.w()),
			(T(_mat[0][2]) * v.x() + T(_mat[1][2]) * v.y() + T(_mat[2][2]) * v.z() + T(_mat[3][2]) * v.w()),
			(T(_mat[0][3]) * v.x() + T(_mat[1][3]) * v.y() + T(_mat[2][3]) * v.z() + T(_mat[3][3]) * v.w())
			);
	}

	template <typename T> inline Vector4<T> Matrix44::postMult(Vector4<T> const& v) const {
		return Vector4<T>(
			(T(_mat[0][0]) * v.x() + T(_mat[0][1]) * v.y() + T(_mat[0][2]) * v.z() + T(_mat[0][3]) * v.w()),
			(T(_mat[1][0]) * v.x() + T(_mat[1][1]) * v.y() + T(_mat[1][2]) * v.z() + T(_mat[1][3]) * v.w()),
			(T(_mat[2][0]) * v.x() + T(_mat[2][1]) * v.y() + T(_mat[2][2]) * v.z() + T(_mat[2][3]) * v.w()),
			(T(_mat[3][0]) * v.x() + T(_mat[3][1]) * v.y() + T(_mat[3][2]) * v.z() + T(_mat[3][3]) * v.w())
			);
	}

	template <typename T> inline Vector4<T> Matrix44::operator * (Vector4<T> const& v) const { return postMult(v); }
	template <typename T> inline Vector3<T> operator * (Vector3<T> const& vector, Matrix44 const& matrix) { return matrix.preMult(vector); }
	template <typename T> inline Vector4<T> operator * (Vector4<T> const& vector, Matrix44 const& matrix) { return matrix.preMult(vector); }

	inline Matrix44 Matrix44::operator * (Matrix44 const& rhs) const {
		Matrix44 matrix;
		matrix.mult(*this, rhs);
		return matrix;
	}

	inline void Matrix44::operator *= (Matrix44 const & rhs) { postMult(rhs); }

	inline Matrix44 Matrix44::operator + (Matrix44 const& rhs) const {
		Matrix44 matrix;
		matrix += rhs;
		return matrix;
	}

	inline Matrix44 Matrix44::operator - (Matrix44 const& rhs) const {
		Matrix44 matrix;
		matrix -= rhs;
		return matrix;
	}

	inline void Matrix44::mult(Matrix44 const& lhs, Matrix44 const& rhs) {
		if (&lhs == this) {
			postMult(rhs);
		} else if (&rhs == this) {
			preMult(lhs);
		} else {
			_mat[0][0] = innerProduct(lhs, rhs, 0, 0);
			_mat[0][1] = innerProduct(lhs, rhs, 0, 1);
			_mat[0][2] = innerProduct(lhs, rhs, 0, 2);
			_mat[0][3] = innerProduct(lhs, rhs, 0, 3);
			_mat[1][0] = innerProduct(lhs, rhs, 1, 0);
			_mat[1][1] = innerProduct(lhs, rhs, 1, 1);
			_mat[1][2] = innerProduct(lhs, rhs, 1, 2);
			_mat[1][3] = innerProduct(lhs, rhs, 1, 3);
			_mat[2][0] = innerProduct(lhs, rhs, 2, 0);
			_mat[2][1] = innerProduct(lhs, rhs, 2, 1);
			_mat[2][2] = innerProduct(lhs, rhs, 2, 2);
			_mat[2][3] = innerProduct(lhs, rhs, 2, 3);
			_mat[3][0] = innerProduct(lhs, rhs, 3, 0);
			_mat[3][1] = innerProduct(lhs, rhs, 3, 1);
			_mat[3][2] = innerProduct(lhs, rhs, 3, 2);
			_mat[3][3] = innerProduct(lhs, rhs, 3, 3);
		}
	}

	inline void Matrix44::preMult(Matrix44 const& rhs) {
		ValueType t[4];
		for(int col = 0; col < 4; ++col) {
			t[0] = innerProduct(rhs, *this, 0, col);
			t[1] = innerProduct(rhs, *this, 1, col);
			t[2] = innerProduct(rhs, *this, 2, col);
			t[3] = innerProduct(rhs, *this, 3, col);
			_mat[0][col] = t[0];
			_mat[1][col] = t[1];
			_mat[2][col] = t[2];
			_mat[3][col] = t[3];
		}
	}

	inline void Matrix44::postMult(Matrix44 const& rhs) {
		ValueType t[4];
		for(int row = 0; row < 4; ++row) {
			t[0] = innerProduct(*this, rhs, row, 0);
			t[1] = innerProduct(*this, rhs, row, 1);
			t[2] = innerProduct(*this, rhs, row, 2);
			t[3] = innerProduct(*this, rhs, row, 3);
			setRow(row, t[0], t[1], t[2], t[3]);
		}
	}

	inline void Matrix44::operator += (Matrix44 const & rhs) {
		_mat[0][0] += rhs._mat[0][0];
		_mat[0][1] += rhs._mat[0][1];
		_mat[0][2] += rhs._mat[0][2];
		_mat[0][3] += rhs._mat[0][3];
		_mat[1][0] += rhs._mat[1][0];
		_mat[1][1] += rhs._mat[1][1];
		_mat[1][2] += rhs._mat[1][2];
		_mat[1][3] += rhs._mat[1][3];
		_mat[2][0] += rhs._mat[2][0];
		_mat[2][1] += rhs._mat[2][1];
		_mat[2][2] += rhs._mat[2][2];
		_mat[2][3] += rhs._mat[2][3];
		_mat[3][0] += rhs._mat[3][0];
		_mat[3][1] += rhs._mat[3][1];
		_mat[3][2] += rhs._mat[3][2];
		_mat[3][3] += rhs._mat[3][3];
	}

	inline void Matrix44::operator -= (Matrix44 const & rhs) {
		_mat[0][0] -= rhs._mat[0][0];
		_mat[0][1] -= rhs._mat[0][1];
		_mat[0][2] -= rhs._mat[0][2];
		_mat[0][3] -= rhs._mat[0][3];
		_mat[1][0] -= rhs._mat[1][0];
		_mat[1][1] -= rhs._mat[1][1];
		_mat[1][2] -= rhs._mat[1][2];
		_mat[1][3] -= rhs._mat[1][3];
		_mat[2][0] -= rhs._mat[2][0];
		_mat[2][1] -= rhs._mat[2][1];
		_mat[2][2] -= rhs._mat[2][2];
		_mat[2][3] -= rhs._mat[2][3];
		_mat[3][0] -= rhs._mat[3][0];
		_mat[3][1] -= rhs._mat[3][1];
		_mat[3][2] -= rhs._mat[3][2];
		_mat[3][3] -= rhs._mat[3][3];
	}

	inline void Matrix44::setAsTransposeOf(Matrix44 const& rhs) {
		_mat[0][0] = rhs(0, 0);
		_mat[0][1] = rhs(1, 0);
		_mat[0][2] = rhs(2, 0);
		_mat[0][3] = rhs(3, 0);
		_mat[1][0] = rhs(0, 1);
		_mat[1][1] = rhs(1, 1);
		_mat[1][2] = rhs(2, 1);
		_mat[1][3] = rhs(3, 1);
		_mat[2][0] = rhs(0, 2);
		_mat[2][1] = rhs(1, 2);
		_mat[2][2] = rhs(2, 2);
		_mat[2][3] = rhs(3, 2);
		_mat[3][0] = rhs(0, 3);
		_mat[3][1] = rhs(1, 3);
		_mat[3][2] = rhs(2, 3);
		_mat[3][3] = rhs(3, 3);
	}

	template <typename T> inline void Matrix44::setTranslation(Vector3<T> const& translation) {
		_mat[3][0] = translation.x();
		_mat[3][1] = translation.y();
		_mat[3][2] = translation.z();
	}

	inline void Matrix44::setTranslation(Matrix44::ValueType const x, Matrix44::ValueType const y, Matrix44::ValueType const z) {
		_mat[3][0] = x;
		_mat[3][1] = y;
		_mat[3][2] = z;
	}

	inline Vector3<Matrix44::ValueType> Matrix44::getTranslation() const {
		return Vector3<ValueType>(_mat[3][0], _mat[3][1], _mat[3][2]);
	}

	inline bool Matrix44::isAffine() const {
		return (_mat[0][3] == ValueType(0.0) && _mat[1][3] == ValueType(0.0) && _mat[2][3] == ValueType(0.0) && _mat[3][3] == ValueType(1.0));
	}

	inline Matrix44::ValueType Matrix44::innerProduct(Matrix44 const& lhs, Matrix44 const& rhs, SizeType const row, SizeType const col) {
		return (
			lhs._mat[row][0] * rhs._mat[0][col] +
			lhs._mat[row][1] * rhs._mat[1][col] +
			lhs._mat[row][2] * rhs._mat[2][col] +
			lhs._mat[row][3] * rhs._mat[3][col]
		);
	}

	inline bool Matrix44::invert() {
		return setAsInverseOf(*this);
	}

	inline Matrix44 Matrix44::inverse() const {
		Matrix44 matrix;
		matrix.setAsInverseOf(*this);
		return matrix;
	}

	inline bool Matrix44::setAsInverseOf(Matrix44 const& rhs) {
		return (rhs.isAffine() ? setAsInverse4x3Of(rhs) : setAsInverse4x4Of(rhs));
	}

	inline void Matrix44::transpose() { setAsTransposeOf(*this); }

	inline void Matrix44::setAsTranslation(ValueType const x,  ValueType const y, ValueType const z) {
		setRow(0, 1.0, 0.0, 0.0, 0.0);
		setRow(1, 0.0, 1.0, 0.0, 0.0);
		setRow(2, 0.0, 0.0, 1.0, 0.0);
		setRow(3,   x,   y,   z, 1.0);
	}

	template <typename T> inline void Matrix44::setAsTranslation(Vector3<T> const& v) { setAsTranslation(v.x(), v.y(), v.z()); }

	inline void Matrix44::setAsScale(ValueType const x,  ValueType const y, ValueType const z) {
		setRow(0,   x, 0.0, 0.0, 0.0);
		setRow(1, 0.0,   y, 0.0, 0.0);
		setRow(2, 0.0, 0.0,   z, 0.0);
		setRow(3, 0.0, 0.0, 0.0, 1.0);
	}

	inline void Matrix44::setAsRotation(Quaternion const& quaternion) {
		ValueType wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

		x2 = quaternion.x() + quaternion.x();
		y2 = quaternion.y() + quaternion.y();
		z2 = quaternion.z() + quaternion.z();

		xx = quaternion.x() * x2;
		xy = quaternion.x() * y2;
		xz = quaternion.x() * z2;

		yy = quaternion.y() * y2;
		yz = quaternion.y() * z2;
		zz = quaternion.z() * z2;

		wx = quaternion.w() * x2;
		wy = quaternion.w() * y2;
		wz = quaternion.w() * z2;

		_mat[0][0] = ValueType(1.0) - (yy + zz);
		_mat[1][0] = xy - wz;
		_mat[2][0] = xz + wy;

		_mat[0][1] = xy + wz;
		_mat[1][1] = ValueType(1.0) - (xx + zz);
		_mat[2][1] = yz - wx;

		_mat[0][2] = xz - wy;
		_mat[1][2] = yz + wx;
		_mat[2][2] = ValueType(1.0) - (xx + yy);
	}

	inline void Matrix44::setAsRotation(ValueType const angle, ValueType const x, ValueType const y, ValueType const z) {
		setAsRotation( Quaternion::rotate(angle, x, y, z) );
	}

	template <typename T> inline void Matrix44::setAsRotation(T const angle, Vector3<T> const& vector) {
		setAsRotation( Quaternion::rotate(angle, vector) );
	}

	template <typename T> inline void Matrix44::setAsRotation(Vector3<T> from, Vector3<T> to) {
		setAsRotation( Quaternion::rotate(from, to) );
	}

	template <typename T> inline void Matrix44::setAsScale(Vector3<T> const& v) {
		setAsScale(v.x(), v.y(), v.z());
	}

	inline Matrix44 Matrix44::translate(ValueType const x, ValueType const y, ValueType const z) {
		Matrix44 matrix;
		matrix.setAsTranslation(x, y, z);
		return matrix;
	}

	template <typename T> inline Matrix44 Matrix44::translate(Vector3<T> const& v) {
		return translate(v.x(), v.y(), v.z());
	}

	inline Matrix44 Matrix44::scale(ValueType const x, ValueType const y, ValueType const z) {
		Matrix44 matrix;
		matrix.setAsScale(x, y, z);
		return matrix;
	}

	template <typename T> inline Matrix44 Matrix44::scale(Vector3<T> const& v) {
		return scale(v.x(), v.y(), v.z());
	}

	inline Matrix44 Matrix44::rotate(ValueType const angle, ValueType const x, ValueType const y, ValueType const z) {
		Matrix44 matrix;
		matrix.setAsRotation(angle, x, y, z);
		return matrix;
	}

	template <typename T> inline Matrix44 Matrix44::rotate(T const angle, Vector3<T> const& v) {
		Matrix44 matrix;
		matrix.setAsRotation(angle, v);
		return matrix;
	}

	template <typename T> inline Matrix44 Matrix44::rotate(Vector3<T> const& from, Vector3<T> const& to) {
		Matrix44 matrix;
		matrix.setAsRotation(from, to);
		return matrix;
	}

	inline void Matrix44::setAsOrtho(ValueType const left, ValueType const right, ValueType const bottom, ValueType const top, ValueType const z_near, ValueType const z_far) {
        setRow(0, ValueType(2.0 / (right - left)), ValueType(0.0), ValueType(0.0), ValueType(0.0));
        setRow(1, ValueType(0.0), ValueType(2.0 / (top - bottom)), ValueType(0.0), ValueType(0.0));
        setRow(2,ValueType(0.0), ValueType(0.0), ValueType(-2.0 / (z_far - z_near)), ValueType(0.0));
        setRow(3, ValueType(-(right + left) / (right - left)), ValueType(-(top + bottom) / (top - bottom)), ValueType(-(z_far + z_near) / (z_far - z_near)), ValueType(1.0));
	}

	inline void Matrix44::setAsOrtho2D(ValueType const left, ValueType const right, ValueType const bottom, ValueType const top) {
		setAsOrtho(left, right, bottom, top, ValueType(-1.0), ValueType(1.0));
	}

	inline void Matrix44::setAsFrustum(ValueType const left, ValueType const right, ValueType const bottom, ValueType const top, ValueType const z_near, ValueType const z_far) {
        ValueType A = ValueType( (right + left) / (right - left) );
        ValueType B = ValueType( (top + bottom) / (top - bottom) );
        ValueType C = ValueType( -(z_far + z_near) / (z_far - z_near) );
        ValueType D = ValueType( -2.0 * z_far * z_near / (z_far - z_near) );

        setRow(0, ValueType(2.0 * z_near / (right - left)), ValueType(0.0), ValueType(0.0), ValueType(0.0));
        setRow(1, ValueType(0.0), ValueType(2.0 * z_near / (top - bottom)), ValueType(0.0), ValueType(0.0));
        setRow(2, A, B, C, ValueType(-1.0));
        setRow(3, ValueType(0.0), ValueType(0.0), D, ValueType(0.0));
	}

	inline void Matrix44::setAsPerspective(ValueType const fovy, ValueType const aspect_ratio, ValueType const z_near, ValueType const z_far) {
        ValueType tan_fovy = ValueType(tan(degreesToRadians(fovy * 0.5)));
		ValueType right = tan_fovy * aspect_ratio * z_near;
		ValueType left = -right;
		ValueType top = tan_fovy * z_near;
		ValueType bottom = -top;
		setAsFrustum(left, right, bottom, top, z_near, z_far);
	}

	inline Matrix44 Matrix44::ortho(ValueType const left, ValueType const right, ValueType const bottom, ValueType const top, ValueType const z_near, ValueType const z_far) {
		Matrix44 matrix;
		matrix.setAsOrtho(left, right, bottom, top, z_near, z_far);
		return matrix;
	}

	inline Matrix44 Matrix44::ortho2D(ValueType const left, ValueType const right, ValueType const bottom, ValueType const top) {
		Matrix44 matrix;
		matrix.setAsOrtho2D(left, right, bottom, top);
		return matrix;
	}

	inline Matrix44 Matrix44::frustum(ValueType const left, ValueType const right, ValueType const bottom, ValueType const top, ValueType const z_near, ValueType const z_far) {
		Matrix44 matrix;
		matrix.setAsFrustum(left, right, bottom, top, z_near, z_far);
		return matrix;
	}

	inline Matrix44 Matrix44::perspective(ValueType const fovy, ValueType const aspect_ratio, ValueType const z_near, ValueType const z_far) {
		Matrix44 matrix;
		matrix.setAsPerspective(fovy, aspect_ratio, z_near, z_far);
		return matrix;
	}

	template <typename T> inline void Matrix44::setAsLookAt(Vector3<T> const& eye, Vector3<T> const& origin, Vector3<T> const& up) {
		Vector3<T> forward = origin - eye;
		forward.normalize();

		Vector3<T> side = forward ^ up;
		side.normalize();

		Vector3<T> calculated_up = side ^ forward;
		calculated_up.normalize();

		set(side[0] , calculated_up[0], -forward[0], T(0.0),
	        side[1] , calculated_up[1], -forward[1], T(0.0),
			side[2] , calculated_up[2], -forward[2], T(0.0),
			T(0.0)  , T(0.0)          , T(0.0)     , T(1.0));

		preMult( Matrix44::translate(-eye) );
    }

	template <typename T> inline Matrix44 Matrix44::lookAt(Vector3<T> const& eye, Vector3<T> const& origin, Vector3<T> const& up) {
		Matrix44 matrix;
		matrix.setAsLookAt(eye, origin, up);
		return matrix;
	}
} // namespace

