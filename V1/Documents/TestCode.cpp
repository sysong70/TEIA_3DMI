
MbSurface * CreateConeSurface()
{
	MbCartPoint3D cOrigin(0.0, 0.0, 80.0);
	MbVector3D cXAxis(1.0, 0.0, 0.0);
	MbVector3D cYAxis(0.0, -1.0, 0.0);
	const MbPlacement3D cPlacement(cXAxis, cYAxis, cOrigin);

	double dSemiAngle = 0.55859931534356233;

	double dEdge = 94.339811320660388;
	double dRadius = 0.0; // From Hoops Exchange
	//double dRadius = dEdge * sin(dSemiAngle);
	double dHeight = dEdge * cos(dSemiAngle);

	// Cone Surface Create
	MbConeSurface * pcCone = new MbConeSurface(cPlacement, dRadius, dSemiAngle, dHeight);

	return (MbSurface *)pcCone;

}

MbSurface * CreatePlane()
{
	MbCartPoint3D cOrigin(0.0, 0.0, 80.0);
	MbVector3D cXAxis(1.0, 0.0, 0.0);
	MbVector3D cYAxis(0.0, -1.0, 0.0);

	const MbPlane * pcPlane = new MbPlane(cOrigin, cXAxis, cYAxis);

	return (MbSurface *) pcPlane;
}

void main()
{
	MbSurface * pcConeSurface = CreateConeSurface();
	MbSurface * pcPlane = CreatePlane();

	MbCartPoint cP1, cP2;

	// Create Line Segment 1 : Cone Bottom edge
	cP1.Set(5.2123889803846897, 0.99999999999900002);
	cP2.Set(8.3539816339744828, 0.99999999999900002);
	MbLineSegment * pcLine1 = new MbLineSegment(cP1, cP2);

	// Create Curve 2 : Plan Arc
	SArray<MbCartPoint> arcControlPointArray;
	SArray<double> ardWeightsArray;
	SArray<double> ardKnotsArray;

	arcControlPointArray.Add(MbCartPoint(23.971276930210145, -43.879128094518641));
	arcControlPointArray.Add(MbCartPoint(39.287936703579859, -35.511598724236549));
	arcControlPointArray.Add(MbCartPoint(51.319054482571445, -21.129041522144565));
	arcControlPointArray.Add(MbCartPoint(57.363005879799658, -2.2070376624617332));
	arcControlPointArray.Add(MbCartPoint(55.499795319349978, 18.368793753110062));
	arcControlPointArray.Add(MbCartPoint(45.443474355417273, 36.776765803188432));
	arcControlPointArray.Add(MbCartPoint(29.136206193043652, 49.461772587382058));
	arcControlPointArray.Add(MbCartPoint(9.9483280942201588, 54.599565170057843));
	arcControlPointArray.Add(MbCartPoint(-8.6546171568404304, 52.246657464800741));
	arcControlPointArray.Add(MbCartPoint(-23.971276930210060, 43.879128094518691));

	for(int i = 0; i < 10; i++)	{
		ardWeightsArray.Add(1.0);
	}

	ardKnotsArray.Add(0.26061944901923451);
	ardKnotsArray.Add(0.26061944901923451);
	ardKnotsArray.Add(0.26061944901923451);
	ardKnotsArray.Add(0.26061944901923451);
	ardKnotsArray.Add(0.26061944901923451);
	ardKnotsArray.Add(0.26061944901923451);
	ardKnotsArray.Add(0.26061944901923451);
	ardKnotsArray.Add(0.26061944901923451);
	ardKnotsArray.Add(0.26061944901923451);
	ardKnotsArray.Add(0.26061944901923451);
	ardKnotsArray.Add(0.41769908169872411);
	ardKnotsArray.Add(0.41769908169872411);
	ardKnotsArray.Add(0.41769908169872411);
	ardKnotsArray.Add(0.41769908169872411);
	ardKnotsArray.Add(0.41769908169872411);
	ardKnotsArray.Add(0.41769908169872411);
	ardKnotsArray.Add(0.41769908169872411);
	ardKnotsArray.Add(0.41769908169872411);
	ardKnotsArray.Add(0.41769908169872411);
	ardKnotsArray.Add(0.41769908169872411);

	ptrdiff_t nSplineOrder = nDegree + 1;
	MbNurbs * pcNurbs = MbNurbs::Create(nSplineOrder, false, arcControlPointArray, ardWeightsArray, ardKnotsArray);

	MbSurfaceIntersectionCurve * pcInterCurve1 = new MbSurfaceIntersectionCurve(pcConeSurface, pcLine1, pcPlane, pcNurbs, MbeCurveBuildType::cbt_Ordinary, true, false);

	MbVertex * pcStartVertex = new MbVertex(23.971276930210138, 43.879128094518627, 0.0);
	MbVertex * pcEndVertex = new MbVertex(-23.971276930210141, -43.879128094518627, 0.0);

	MbCurveEdge * pcCurveEdge1 = new MbCurveEdge(pcStartVertex, pcEndVertex, pcInterCurve1, true);

	MbOrientedEdge * pcOrientedEdge1 = new MbOrientedEdge(*pcCurveEdge1, false);

}



