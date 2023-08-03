#include "StdAfx.h"

#include "3DF.Math.Matrix.h"

#include "3DF.Point.h"

#include <hc.h>

const double EPSILON = 1e-10; // 미세한 값

USING_3DF_NAMESPACE

MatrixKit::MatrixKit()
{
	Reset();
}

MatrixKit::MatrixKit(MatrixKit const & cInOther)
{
	memcpy(m_fData, cInOther.m_fData, sizeof(m_fData));
}

MatrixKit::MatrixKit(float const pfInMatrixSource[])
{
	memcpy(m_fData, pfInMatrixSource, sizeof(m_fData));
}

MatrixKit::~MatrixKit() {}

MatrixKit MatrixKit::GetDefault()
{
	return MatrixKit();
}

bool MatrixKit::operator == (MatrixKit const & cInKit) const
{
	for (int i = 0; i < 16; ++i) {
		if (fabs(m_fData[i] - cInKit.m_fData[i]) > EPSILON) {
			return false;
		}
	}

	return true;
}

bool MatrixKit::operator != (MatrixKit const & cInKit) const
{
	return !(*this == cInKit);
}

MatrixKit & MatrixKit::operator = (MatrixKit const & cInKit)
{
	memcpy(m_fData, cInKit.m_fData, sizeof(m_fData));
	return *this;
}

MatrixKit MatrixKit::operator * (MatrixKit const & cInRight) const
{
	return Multiply(cInRight);
}

MatrixKit const & MatrixKit::operator *= (MatrixKit const & cInRight)
{
	HC_Compute_Matrix_Product(m_fData, cInRight.m_fData, m_fData);
	return *this;
}

MatrixKit MatrixKit::operator * (float fInScalar) const
{
	return Multiply(fInScalar);
}

MatrixKit const & MatrixKit::operator *= (float fInScalar)
{
	for (int i = 0; i < 16; ++i) {
		m_fData[i] = m_fData[i] * fInScalar;
	}
	return *this;
}

float * MatrixKit::operator[](int nIndex) {
	return m_fData + (nIndex * 4);
}

bool MatrixKit::ShowInverse(MatrixKit & cOutMatrix) const
{
	return HC_Compute_Matrix_Inverse(m_fData, cOutMatrix.m_fData);
}

void MatrixKit::Reset()
{
	memset(m_fData, 0, sizeof(m_fData));
	m_fData[0] = m_fData[5] = m_fData[10] = m_fData[15] = 1.0f;
}

void MatrixKit::Set(MatrixKit const & cOther)
{
	memcpy(m_fData, cOther.m_fData, sizeof(m_fData));
}


MatrixKit & MatrixKit::Rotate(double in_x, double in_y, double in_z)
{
	HC_Compute_Rotation_Matrix(in_x, in_y, in_z, m_fData);
	return *this;
}

MatrixKit & MatrixKit::RotateOffAxis(Vector const & cInVector, float cInTheta)
{
	float fData[16];
	HC_Compute_Offaxis_Rotation(cInVector.x, cInVector.y, cInVector.z, cInTheta, fData);

	HC_Compute_Matrix_Product(m_fData, fData, m_fData);

	//HC_Compute_Offaxis_Rotation(cInVector.x, cInVector.y, cInVector.z, cInTheta, m_fData);
	return *this;
}

MatrixKit & MatrixKit::Translate(float in_x, float in_y, float in_z)
{
	float fTranslationData[16];
	HC_Compute_Translation_Matrix(in_x, in_y, in_z, fTranslationData);

	HC_Compute_Matrix_Product(m_fData, fTranslationData, m_fData);

	return *this;
}

MatrixKit & MatrixKit::Translate(Vector const & cInTranslation)
{
	return Translate(cInTranslation.x, cInTranslation.y, cInTranslation.z);
}

MatrixKit & MatrixKit::Scale(float in_x, float in_y, float in_z)
{
	HC_Compute_Scale_Matrix(in_x, in_y, in_z, m_fData);
	return *this;
}

MatrixKit & MatrixKit::Normalize()
{
	HC_Compute_Normalized_Matrix(m_fData, m_fData);
	return *this;
}

MatrixKit & MatrixKit::Invert()
{
	HC_Compute_Matrix_Inverse(m_fData, m_fData);
	return *this;
}

bool MatrixKit::IsIdentity() const
{
	return *this == GetDefault();
}

MatrixKit MatrixKit::Multiply(MatrixKit const & cInRight) const
{
	MatrixKit cMatrix;
	HC_Compute_Matrix_Product(m_fData, cInRight.m_fData, cMatrix.m_fData);
	return cMatrix;
}

MatrixKit MatrixKit::Multiply(float fInScalar) const
{
	MatrixKit cMatrix;
	for (int i = 0; i < 16; ++i) {
		cMatrix.m_fData[i] = m_fData[i] * fInScalar;
	}
	return cMatrix;
}

Point MatrixKit::Transform(Point const & cInSource) const
{
	Point cPoint;
	HC_Compute_Transformed_Points(1, &cInSource, m_fData, &cPoint);
	return cPoint;
}

PointArray MatrixKit::Transform(PointArray const & cInSource) const
{
	PointArray cPointArray;
	cPointArray.resize(cInSource.size());

	HC_Compute_Transformed_Points((int)cInSource.size(), cInSource.data(), m_fData, cPointArray.data());

	return cPointArray;
}

WorldPointArray MatrixKit::Transform(WorldPointArray const & cInSource) const
{
	size_t nSize = cInSource.size();

	WorldPointArray cPointArray;
	cPointArray.resize(nSize);

	HC_Compute_Transformed_Points((int)cInSource.size(), cInSource.data(), m_fData, cPointArray.data());

	return cPointArray;
}

Vector MatrixKit::XAxis() const
{
	return Vector(m_fData[0], m_fData[1], m_fData[2]);
}

Vector MatrixKit::YAxis() const
{
	return Vector(m_fData[4], m_fData[5], m_fData[6]);
}

Vector MatrixKit::ZAxis() const
{
	return Vector(m_fData[8], m_fData[9], m_fData[10]);
}

Point MatrixKit::Origin() const
{
	return Point(m_fData[12], m_fData[13], m_fData[14]);
}