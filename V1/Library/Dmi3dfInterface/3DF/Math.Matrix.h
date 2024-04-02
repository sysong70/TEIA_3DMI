#pragma once

#include "3DF.h"

#include "Math.h"

namespace H3DF
{
	class API_3DF MatrixKit
	{
	public:
		float m_fData[16];

		MatrixKit();
		MatrixKit(MatrixKit const & cInOther);
		MatrixKit(float const pfInMatrixSource[]);
		virtual ~MatrixKit();

		static MatrixKit GetDefault();

		bool operator == (MatrixKit const & cInKit) const;
		bool operator != (MatrixKit const & cInKit) const;
		MatrixKit & operator = (MatrixKit const & cInKit);
		MatrixKit operator * (MatrixKit const & cInRight) const;
		MatrixKit const & operator *= (MatrixKit const & cInRight);
		MatrixKit operator * (float fInScalar) const;
		MatrixKit const & operator *= (float fInScalar);
		float * operator[] (int nIndex);

		bool ShowInverse(MatrixKit & cOutMatrix) const;

		void Reset();

		void Set(MatrixKit const & cOther);

		MatrixKit & Rotate(double in_x, double in_y, double in_z);

		MatrixKit & RotateOffAxis(Vector const & cInVector, float cInTheta);

		MatrixKit & Translate(float in_x, float in_y, float in_z);

		MatrixKit & Translate(Vector const & cInTranslation);

		MatrixKit & Scale(float in_x, float in_y, float in_z);

		MatrixKit & Normalize();

		MatrixKit & Invert();

		bool IsIdentity() const;
		;;;
		MatrixKit Multiply(MatrixKit const & cInRight) const;

		MatrixKit Multiply(float fInScalar) const;

		MatrixKit & SetElement(size_t nInRow, size_t nInColumn, float fInValue);

		Point Transform(Point const & cInSource) const;
		PointArray Transform(PointArray const & cInSource) const;
		WorldPointArray Transform(WorldPointArray const & cInSource) const;

		Vector XAxis() const;
		Vector YAxis() const;
		Vector ZAxis() const;
		Point Origin() const;

		MatrixKit & SetXAxis(Vector cInVector);
		MatrixKit & SetYAxis(Vector cInVector);
		MatrixKit & SetZAxis(Vector cInVector);
		MatrixKit & SetOrigin(Point cInOrigin);

		MatrixKit & SetXAxis(float fInX, float fInY, float fInZ);
		MatrixKit & SetYAxis(float fInX, float fInY, float fInZ);
		MatrixKit & SetZAxis(float fInX, float fInY, float fInZ);
		MatrixKit & SetOrigin(float fInX, float fInY, float fInZ);
	};

	using Matrix = MatrixKit;
}