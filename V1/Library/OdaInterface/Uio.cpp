#include "stdafx.h"

#include "Uio.h"

//**************************************************************************************************

double Uio::ToRadian(double degree)
{
	if (degree > 360) {
		degree = fmod(degree, 360);
	}

	if (degree < 0.0) {
		degree = 360 - degree;
	}

	return OdaToRadian(degree);
}



double Uio::ToDegree(double radian)
{
	return OdaToDegree(radian);
}
