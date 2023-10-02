#include <UnitTest.hpp>
#include <math/Mathematics.hpp>
#include <math/Vector.hpp>
#include <math/Matrix.hpp>
#include <math/Easing.hpp>
#include <math/RunningStats.hpp>
#include <math/BoundingVolume.hpp>

using namespace smk;

template<typename T>
bool isVectorEquivalent(Vector3<T> const left, Vector3<T> const right, typename Vector3<T>::ValueType const epsilon = 1e-5) {
	typename Vector3<T>::ValueType delta = (right - left).length();
	return (delta < typename Vector3<T>::ValueType(0.0f)) ? (delta >= -epsilon) : (delta <= epsilon);
}


TEST_BEGIN(TestBaseMath)
{
    TEST_ASSERT( absolute(-1.5f) == 1.5f );
    TEST_ASSERT( absolute(-4.2) == 4.2 );

    TEST_ASSERT( equivalent(0.0000000001, 0.0) );
    TEST_ASSERT( equivalent(0.0, 0.0000000001) );

    TEST_ASSERT( !equivalent(0.0001, 0.0) );
    TEST_ASSERT( !equivalent(0.0f, 0.0001f) );

    TEST_ASSERT( minimum(10.2, 5.0) == 5.0 );
    TEST_ASSERT( minimum(5.0f, 10.2f) == 5.0f );

    TEST_ASSERT( maximum(10.2, 5.0) == 10.2 );
    TEST_ASSERT( maximum(5.0f, 10.2f) == 10.2f );


    TEST_ASSERT( 0.0000000001 != 0.0 );
    TEST_ASSERT( clampNear(0.0000000001, 0.0) == 0.0 );

    TEST_ASSERT( clampTo(12.0f, 5.0f, 10.2f) == 10.2f );
    TEST_ASSERT( clampTo(4.0f, 5.0f, 10.2f) == 5.0f );
    TEST_ASSERT( clampTo(7.0f, 5.0f, 10.2f) == 7.0f );

    TEST_ASSERT( clampBelow(12.0f, 5.0f) == 5.0f );
    TEST_ASSERT( clampBelow(1.0f, 5.0f) == 1.0f );

    TEST_ASSERT( clampAbove(4.0f, 5.0f) == 5.0f );
    TEST_ASSERT( clampAbove(6.0f, 5.0f) == 6.0f );

    TEST_ASSERT( sign(7) == 1 );
    TEST_ASSERT( sign(-2) == -1 );
    TEST_ASSERT( sign(0) == 0 );

    TEST_ASSERT( square(2.5) == 2.5 * 2.5 );
    TEST_ASSERT( signedSquare(2.5) == 2.5 * 2.5 );
    TEST_ASSERT( signedSquare(-2.5) == -2.5 * 2.5 );

    TEST_ASSERT( equivalent( radiansToDegrees(piAngle<double>()) , 180.0) );
    TEST_ASSERT( equivalent( degreesToRadians(180.0) , piAngle<double>()) );

    TEST_ASSERT( equivalent( round(1.5) , 2.0) );
    TEST_ASSERT( equivalent( round(1.2) , 1.0) );
    TEST_ASSERT( equivalent( round(-1.5) , -2.0) );
    TEST_ASSERT( equivalent( round(-1.2) , -1.0) );
    TEST_ASSERT( equivalent( round(1.5f) , 2.0f) );
    TEST_ASSERT( equivalent( round(1.2f) , 1.0f) );
}
TEST_END(TestBaseMath)

TEST_BEGIN(TestVector)
{
	TEST_ASSERT(Vector2(1.0f, 1.0f) == Vector2<float>(1.0, 1.0));
	TEST_ASSERT(Vector2(1.0f, 1.0f) != Vector2<float>(1.0, 2.0));
	TEST_ASSERT(Vector2(1.1f, 3.0f).x() == 1.1f);
	TEST_ASSERT(Vector2(1.1f, 3.0f).y() == 3.0f);

	Vector2<float> as_float2;
	Vector2<double> as_double2(as_float2);
	Vector2<double> as_double2_ = as_float2;
	as_float2 = as_double2;

    TEST_ASSERT(as_double2_ != Vector2<double>(1.0, 1.0));

	as_float2.set(1.1f, 2.2f);

	TEST_ASSERT(Vector3(1.0f, 1.0f, 1.0f) == Vector3<float>(1.0, 1.0, 1.0));
	TEST_ASSERT(Vector3(1.0f, 1.0f, 1.0f) != Vector3<float>(1.0, 2.0, 1.0));
	TEST_ASSERT(Vector3(1.1f, 3.0f, 1.0f).x() == 1.1f);
	TEST_ASSERT(Vector3(1.1f, 3.0f, 1.0f).y() == 3.0f);
	TEST_ASSERT(Vector3(1.1f, 3.0f, 5.0f).z() == 5.0f);

	Vector3<float> as_float3;
	Vector3<double> as_double3(as_float3);
	Vector3<double> as_double3_ = as_float3;
	as_float3 = as_double3;

    TEST_ASSERT(as_double3_ != Vector3<double>(1.0, 1.0, 1.0));

	as_float3.set(1.1f, 2.2f, 3.3f);

	as_double3 = 2.0 * as_double3;
	as_double3 = as_double3 * 2.0;

	Vector2<float> vec2d_conversion;
	Vector2<double> vec2dd_conversion;
	Vector3 conversion(vec2d_conversion, 1.0f);
	Vector3<float> conversion_(vec2dd_conversion, 1.0f);

	conversion = conversion ^ conversion_;

	Vector3<double> vec3dd_0;
	Vector3<double> vec3dd_1;
	Vector3<double> vec3dd_2;

	vec3dd_2 = vec3dd_0 ^ vec3dd_1;

	TEST_ASSERT(Vector4(1.0f, 1.0f, 1.0f, 1.0f) == Vector4<float>(1.0, 1.0, 1.0, 1.0f));
	TEST_ASSERT(Vector4(1.0f, 1.0f, 1.0f, 1.0f) != Vector4<float>(1.0, 2.0, 1.0, 1.0f));
	TEST_ASSERT(Vector4(1.1f, 3.0f, 1.0f, 4.0f).x() == 1.1f);
	TEST_ASSERT(Vector4(1.1f, 3.0f, 1.0f, 5.0f).y() == 3.0f);
	TEST_ASSERT(Vector4(1.1f, 3.0f, 5.0f, 6.0f).z() == 5.0f);
	TEST_ASSERT(Vector4(1.1f, 3.0f, 5.0f, 7.0f).w() == 7.0f);
	TEST_ASSERT(Vector4(1.1f, 3.0f, 1.0f, 4.0f).r() == 1.1f);
	TEST_ASSERT(Vector4(1.1f, 3.0f, 1.0f, 5.0f).g() == 3.0f);
	TEST_ASSERT(Vector4(1.1f, 3.0f, 5.0f, 6.0f).b() == 5.0f);
	TEST_ASSERT(Vector4(1.1f, 3.0f, 5.0f, 7.0f).a() == 7.0f);

	Vector4 as_float4(1.1f, 2.2f, 3.3f, 4.4f);
}
TEST_END(TestVector)

TEST_BEGIN(TestMatrix)
{
    	float myf_matrix[16] =
	{
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0
	};

	Matrix44 matrix;
	TEST_ASSERT( matrix.isIdentity() );

	matrix.set(myf_matrix);
	TEST_ASSERT( matrix.isIdentity() );

	myf_matrix[0] = 12.0f;
	matrix.set(myf_matrix);
	TEST_ASSERT( !matrix.isIdentity() );

	matrix.set(
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0
		);
	TEST_ASSERT( matrix.isIdentity() );

	Matrix44 second;
	TEST_ASSERT(matrix == second);

	matrix.set(
		1.0, 2.0, 3.0, 4.0,
		5.0, 6.0, 7.0, 8.0,
		9.0, 10.0, 11.0, 12.0,
		13.0, 14.0, 15.0, 16.0
		);

	second = matrix;
	TEST_ASSERT(matrix == second);

	matrix.set(
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0
		);

	second = matrix;

	second(2, 3) = 14.5f;
	TEST_ASSERT(matrix != second);

	second[2 * 4 + 3] = matrix[2 * 4 + 3];
	TEST_ASSERT(matrix == second);

	second.setAsIdentity();
	matrix(2, 3) = 14.5f;
	second = matrix;
	TEST_ASSERT(matrix == second);
//	std::cout << "Matrix : " << std::endl << matrix;
//	std::cout << "Second : " << std::endl << second;

	// testar isto
	matrix.setAsIdentity();
	Vector3 x_axis(1.0f, 0.0f, 0.0f);
	Vector3 y_axis(1.0f, 0.0f, 0.0f);
	Vector3 z_axis(1.0f, 0.0f, 0.0f);

	Vector3 result;

	result = matrix.preMult(x_axis);
	result = matrix.postMult(x_axis);
	result = x_axis * matrix;
	result = matrix * x_axis;

	Matrix44 result_matrix;
	result_matrix += matrix;
	result_matrix = matrix + second;
	result_matrix -= matrix;
	result_matrix += matrix - second;


	//
	// Matrix Multiplication
	//

	Matrix44 one, two, three, four;

	one.postMult(two);
	one.preMult(two);
	one.mult(two, three);

	one(0, 0) = 1.0;
	one(1, 0) = 2.0;
	one(2, 0) = 3.0;
	one(3, 0) = 1.0;

	two(0, 0) = 5.0;
	two(1, 0) = 6.0;
	two(2, 0) = 7.0;
	two(3, 0) = 9.0;

	three(0, 1) = 15.0;
	three(1, 2) = 63.0;
	three(0, 3) = 37.0;
	three(1, 2) = 79.0;

	matrix = one * two;
	three = two * one;
	TEST_ASSERT(matrix != three);

	four = one;
	four *= two;
	TEST_ASSERT(four == matrix);

	four = one;
	four.postMult(two);
	TEST_ASSERT(four == matrix);

	four = two;
	four.preMult(one);
	TEST_ASSERT(four == matrix);

	matrix = one * two * three;
	four = one;
	four.postMult(two);
	four.postMult(three);
	TEST_ASSERT(four == matrix);

	one.setAsIdentity();
	two.setAsIdentity();

	one.setTranslation( Vector3(2.0f, 3.0f, 4.0f) );
	two.setTranslation(2.0f, 3.0f, 4.0f);
	TEST_ASSERT(one == two);
	result = one.getTranslation();
	TEST_ASSERT(result == Vector3(2.0f, 3.0f, 4.0f));


	one(0, 0) =  1.0; one(0, 1) = 02.0; one(0, 2) =  3.0; one(0, 3) =  4.0;
	one(1, 0) =  5.0; one(1, 1) =  6.0; one(1, 2) =  7.0; one(1, 3) =  8.0;
	one(2, 0) =  9.0; one(2, 1) = 10.0; one(2, 2) = 11.0; one(2, 3) = 12.0;
	one(3, 0) = 13.0; one(3, 1) = 14.0; one(3, 2) = 15.0; one(3, 3) = 16.0;

	matrix.setAsTransposeOf(one);
//	std::cout << "Transpose : " << std::endl << one << std::endl << matrix << std::endl;


	matrix.setAsIdentity();
	TEST_ASSERT(matrix.isAffine());
	matrix(0, 3) = 2.0;
	TEST_ASSERT(!matrix.isAffine());

	one.setAsInverseOf(matrix);
	one.setAsInverseOf(two);
	one.invert();
	matrix = one.inverse();

	// translation
	matrix.setAsIdentity();
	Vector3 translation(12.0f, 10.20f, 123.0f);

	matrix.setAsTranslation(translation);
	one = Matrix44::translate(translation);
	TEST_ASSERT(matrix == one);

	Vector3 point(5.0f, 10.0f, 2.0f);

	result = point * matrix;
	//std::cout << "Translation : " << std::endl << matrix << std::endl;
	TEST_ASSERT(result == (translation + point));
	matrix.invert();
	TEST_ASSERT((point * matrix) == (-translation + point));
//	std::cout << "Translation : " << std::endl << matrix << std::endl;

	// scale
	Vector3 scale(2.0f, 4.0f, 5.0f);
	point.set(10.0f, 10.0f, 10.0f);
	matrix.setAsScale(scale);
	TEST_ASSERT(matrix == Matrix44::scale(scale));
//	std::cout << "Scale : " << std::endl << matrix << std::endl;

	result = point * matrix;
	TEST_ASSERT(result == Vector3(scale.x() * point.x(), scale.y() * point.y(), scale.z() * point.z()));

	matrix = Matrix44::translate(translation) * Matrix44::scale(scale);
	one = Matrix44::scale(scale) * Matrix44::translate(translation);
//	std::cout << "T * S : " << std::endl << matrix << std::endl;
//	std::cout << "S * T : " << std::endl << one << std::endl;
}
TEST_END(TestMatrix)

TEST_BEGIN(TestQuaternion)
{
	Quaternion quaternion;
	Quaternion quaternion0(1.0f, 2.0f, 3.0f, 4.0f);
	Quaternion quaternion1(6.0f, 215.0f, 311.0f, 241.0f);
	Quaternion quaternion2(9.0f, 21.0f, 32.0f, 344.0f);
	Quaternion quaternion3(12.0f, 28.0f, 369.0f, 574.0f);

	TEST_ASSERT(quaternion0 == Quaternion(1.0f, 2.0f, 3.0f, 4.0f));
	TEST_ASSERT(quaternion1 != quaternion0);
	TEST_ASSERT(quaternion0.x() == 1.0f);
	TEST_ASSERT(quaternion0.y() == 2.0f);
	TEST_ASSERT(quaternion0.z() == 3.0f);
	TEST_ASSERT(quaternion0.w() == 4.0f);

	TEST_ASSERT( equivalent(sqrtf(quaternion0.lengthSquared()), quaternion0.length()) );


	quaternion = 3.0 * quaternion;
	quaternion *= 3.0;

	quaternion = quaternion / 3.0;
	quaternion /= 3.0;

	quaternion.set(6.0f, 215.0f, 311.0f, 241.0f);

	quaternion = quaternion3;
	quaternion = quaternion2 / quaternion3;

	//std::cout << "Quaternion : " << quaternion << std::endl;

	Vector3 point(10.0f, 0.0f, 0.0f);

    quaternion0.setAsRotation(Quaternion::ValueType(degreesToRadians(90.0)), Quaternion::ValueType(0.0), Quaternion::ValueType(0.0), Quaternion::ValueType(1.0));
    quaternion1.setAsRotation(Quaternion::ValueType(degreesToRadians(90.0)), Quaternion::ValueType(0.0), Quaternion::ValueType(1.0), Quaternion::ValueType(0.0));
    quaternion2.setAsRotation(Quaternion::ValueType(degreesToRadians(90.0)), Quaternion::ValueType(1.0), Quaternion::ValueType(0.0), Quaternion::ValueType(0.0));

	Vector3 vector_00;
	float angle;
	quaternion0.getRotation(angle, vector_00);
	TEST_ASSERT( equivalent(angle, degreesToRadians(90.0f)) );
	TEST_ASSERT( isVectorEquivalent( vector_00 , Vector3(0.0f, 0.0f, 1.0f)) );

	quaternion1.getRotation(angle, vector_00);
	TEST_ASSERT( equivalent(angle, degreesToRadians(90.0f)) );
	TEST_ASSERT( isVectorEquivalent( vector_00 , Vector3(0.0f, 1.0f, 0.0f)) );

	quaternion2.getRotation(angle, vector_00);
	TEST_ASSERT( equivalent(angle, degreesToRadians(90.0f)) );
	TEST_ASSERT( isVectorEquivalent( vector_00 , Vector3(1.0f, 0.0f, 0.0f)) );


	quaternion = quaternion0;
	TEST_ASSERT( isVectorEquivalent( point * quaternion , quaternion * point) );

	quaternion = quaternion0;
	TEST_ASSERT( isVectorEquivalent(point * quaternion , Vector3(0.0f, 10.0f, 0.0f)) );

	quaternion = quaternion1;
	TEST_ASSERT( isVectorEquivalent(point * quaternion , Vector3(0.0f, 0.0f, -10.0f)) );

	quaternion = quaternion2;
	TEST_ASSERT( isVectorEquivalent(point * quaternion , Vector3(10.0f, 0.0f, 0.0f)) );


	TEST_ASSERT(equivalent(quaternion0.length(), 1.0f) );
	TEST_ASSERT(equivalent(quaternion1.length(), 1.0f) );
	TEST_ASSERT(equivalent(quaternion2.length(), 1.0f) );

	quaternion = quaternion0 * quaternion1;
	TEST_ASSERT( isVectorEquivalent(point * quaternion , Vector3(0.0f, 10.0f, 0.0f)) );

	quaternion = quaternion0 * quaternion1 * quaternion2;
	TEST_ASSERT( isVectorEquivalent(point * quaternion ,  Vector3(0.0f, 0.0f, 10.0f)) );

	quaternion = quaternion0 * quaternion1 * quaternion2 * quaternion1;
	TEST_ASSERT( isVectorEquivalent(point * quaternion , point ) );


	quaternion = quaternion0 * quaternion1;
	TEST_ASSERT( isVectorEquivalent(point * quaternion , Vector3(0.0f, 10.0f, 0.0f) ) );
	//std::cout << " " << point << " -> " << point * quaternion << std::endl;


	//std::cout << " " << point << " -> " << point * quaternion << std::endl;
}
TEST_END(TestQuaternion)



TEST_BEGIN(TestBoundingVolume)
{
	float x_min = -1.0f;
	float y_min = -2.0f;
	float z_min = -3.0f;
	float x_max = 1.0f;
	float y_max = 2.0f;
	float z_max = 3.0f;

	BoundingBox bounding_box0(x_min, y_min, z_min, x_max, y_max, z_max);
	TEST_ASSERT(bounding_box0.xMin() == x_min);
	TEST_ASSERT(bounding_box0.yMin() == y_min);
	TEST_ASSERT(bounding_box0.zMin() == z_min);
	TEST_ASSERT(bounding_box0.xMax() == x_max);
	TEST_ASSERT(bounding_box0.yMax() == y_max);
	TEST_ASSERT(bounding_box0.zMax() == z_max);
	TEST_ASSERT(bounding_box0.minimum() == Vector3(x_min, y_min, z_min));
	TEST_ASSERT(bounding_box0.maximum() == Vector3(x_max, y_max, z_max));

	y_max = 4.0f;

	bounding_box0.set(x_min, y_min, z_min, x_max, y_max, z_max);
	TEST_ASSERT(bounding_box0.xMin() == x_min);
	TEST_ASSERT(bounding_box0.yMin() == y_min);
	TEST_ASSERT(bounding_box0.zMin() == z_min);
	TEST_ASSERT(bounding_box0.xMax() == x_max);
	TEST_ASSERT(bounding_box0.yMax() == y_max);
	TEST_ASSERT(bounding_box0.zMax() == z_max);
	TEST_ASSERT(bounding_box0.minimum() == Vector3(x_min, y_min, z_min));
	TEST_ASSERT(bounding_box0.maximum() == Vector3(x_max, y_max, z_max));


	x_min = -10.0f;
	y_min = -20.0f;
	z_min = -30.0f;
	x_max = 10.0f;
	y_max = 20.0f;
	z_max = 30.0f;

	bounding_box0.merge(Vector3(x_min, y_min, z_min));
	bounding_box0.merge(Vector3(x_max, y_max, z_max));
	TEST_ASSERT(bounding_box0.xMin() == x_min);
	TEST_ASSERT(bounding_box0.yMin() == y_min);
	TEST_ASSERT(bounding_box0.zMin() == z_min);
	TEST_ASSERT(bounding_box0.xMax() == x_max);
	TEST_ASSERT(bounding_box0.yMax() == y_max);
	TEST_ASSERT(bounding_box0.zMax() == z_max);
	TEST_ASSERT(bounding_box0.minimum() == Vector3(x_min, y_min, z_min));
	TEST_ASSERT(bounding_box0.maximum() == Vector3(x_max, y_max, z_max));

	x_min = -5.0f;
	y_min = -21.0f;
	z_min = 0.0f;
	x_max = 1.0f;
	y_max = 22.0f;
	z_max = 32.0f;

	bounding_box0.merge(Vector3(x_min, y_min, z_min));
	bounding_box0.merge(Vector3(x_max, y_max, z_max));

	TEST_ASSERT(bounding_box0.xMin() != x_min);
	TEST_ASSERT(bounding_box0.yMin() == y_min);
	TEST_ASSERT(bounding_box0.zMin() != z_min);
	TEST_ASSERT(bounding_box0.xMax() != x_max);
	TEST_ASSERT(bounding_box0.yMax() == y_max);
	TEST_ASSERT(bounding_box0.zMax() == z_max);
}

TEST_END(TestBoundingVolume)
