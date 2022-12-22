#include "StdAfx.h"

#include "3DF.Math.h"

USING_3DF_NAMESPACE

MatrixKit::MatrixKit()
{
	memset(m_fData, 0, 16 * sizeof(float));
	m_fData[0] = 1.0;
	m_fData[5] = 1.0;
	m_fData[10] = 1.0;
	m_fData[15] = 1.0;
}

MatrixKit::MatrixKit(float const fInMatrixSource[])
{
	memcpy(m_fData, fInMatrixSource, 16 * sizeof(float));
}