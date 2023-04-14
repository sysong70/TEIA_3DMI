#pragma once

#include "3DF.h"

#include "3DF.Math.h"

OPEN_3DF_NAMESPACE

namespace Math {
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

		MatrixKit Multiply(MatrixKit const & cInRight) const;

		MatrixKit Multiply(float fInScalar) const;

		Point Transform(Point const & cInSource) const;

		void Transform(PointArray const & cInSource, PointArray & cOutPointArray) const;

		Vector XAxis() const;
		Vector YAxis() const;
		Vector ZAxis() const;
		Point Origin() const;
	};

	using Matrix = MatrixKit;
};

CLOSE_3DF_NAMESPACE