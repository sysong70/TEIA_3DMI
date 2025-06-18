#include "StdAfx.h"

#include "Shape.h"
#include "Impl/Shape.Impl.h"

#include <limits>

using namespace H3DF;

//== ShapeCoordinate Class ========================================================================
H3DF::ShapeCoordinate::ShapeCoordinate()
{
	x = FLT_MAX;
	y = FLT_MAX;
	radius = FLT_MAX;
	margins.clear();
}

H3DF::ShapeCoordinate::ShapeCoordinate(float fInX, float fInY)
{
	x = fInX;
	y = fInY;
	radius = FLT_MAX;
	margins.clear();
}

H3DF::ShapeCoordinate::ShapeCoordinate(float fInX, float fInY, FloatArray const & afInMargins)
{
	x = fInX;
	y = fInY;
	radius = FLT_MAX;
	margins = afInMargins;
}

H3DF::ShapeCoordinate::ShapeCoordinate(float fInX, float fInY, size_t nInCount, float const afInMargins[])
{
	x = fInX;
	y = fInY;
	radius = FLT_MAX;

	margins.resize(nInCount);
	for (size_t nIndex = 0; nIndex < nInCount; nIndex++) {
		margins[nIndex] = afInMargins[nIndex];
	}
}

H3DF::ShapeCoordinate::ShapeCoordinate(float fInX, float fInY, float fInRadius)
{
	x = fInX;
	y = fInY;
	radius = fInRadius;
}

H3DF::ShapeCoordinate::ShapeCoordinate(float fInX, float fInY, float fInRadius, FloatArray const & afInMargins)
{
	x = fInX;
	y = fInY;
	radius = fInRadius;
	margins = afInMargins;
}

H3DF::ShapeCoordinate::ShapeCoordinate(float fInX, float fInY, float fInRadius, size_t nInCount, float const afInMargins[])
{
	x = fInX;
	y = fInY;
	radius = fInRadius;

	margins.resize(nInCount);
	for (size_t nIndex = 0; nIndex < nInCount; nIndex++) {
		margins[nIndex] = afInMargins[nIndex];
	}
}

bool H3DF::ShapeCoordinate::Equals(ShapeCoordinate const & cInThat) const
{
	if (x != cInThat.x) return false;
	if (y != cInThat.y) return false;
	if (radius != cInThat.radius) return false;
	if (margins.size() != cInThat.margins.size()) return false;

	for (size_t nIndex = 0; nIndex < margins.size(); nIndex++) {
		if (margins[nIndex] != cInThat.margins[nIndex]) return false;
	}

	return true;
}

ShapeCoordinate & H3DF::ShapeCoordinate::SetMargins(float fInMarginOne, float fInMarginTwo, float fInMarginThree, float fInMarginFour)
{
	margins.clear();

	// 중간에 0.0f가 있을 수 있어서 전체 갯수를 파악하도록 한다.
	int nCount = 1;
	if (0.0f != fInMarginTwo) nCount = 2;
	if (0.0f != fInMarginThree) nCount = 3;
	if (0.0f != fInMarginFour) nCount = 4;

	margins.push_back(fInMarginOne);

	if (2 == nCount) {
		margins.push_back(fInMarginTwo);
	}
	else if (3 == nCount) {
		margins.push_back(fInMarginTwo);
		margins.push_back(fInMarginThree);
	}
	else if (4 == nCount) {
		margins.push_back(fInMarginTwo);
		margins.push_back(fInMarginThree);
		margins.push_back(fInMarginFour);
	}

	return *this;
}

bool H3DF::ShapeCoordinate::ShowMargins(FloatArray & afOutMargins) const
{
	if (true == margins.empty()) {
		return false;
	}

	int nCount = 1;
	if (margins[1] != 0.0f) nCount = 2;
	if (margins[2] != 0.0f) nCount = 3;
	if (margins[3] != 0.0f) nCount = 4;

	for(int nIndex = 0; nIndex < nCount; nIndex++) {
		afOutMargins.push_back(margins[nIndex]);
	}

	return true;
}

ShapeCoordinate & H3DF::ShapeCoordinate::UnsetMargins()
{
	margins.clear();
	return *this;
}
//== ShapePoint Class =============================================================================

H3DF::ShapePoint::ShapePoint()
{
}

H3DF::ShapePoint::ShapePoint(ShapeCoordinate const & cInX, ShapeCoordinate const & cInY)
{
	x = cInX;
	y = cInY;
}

H3DF::ShapePoint::ShapePoint(float fInX, float fInY)
{
	x = ShapeCoordinate(fInX, fInY);
	y = ShapeCoordinate(fInX, fInY);
}

//== ShapeElement Class ===========================================================================

H3DF::ShapeElement::ShapeElement()
{
	DEBUG_VALID(SetImpl(H3DF::Type::ShapeElement, std::make_unique<ShapeElementImpl>()));
}

H3DF::ShapeElement::ShapeElement(ShapeElement const & cInThat)
{
	m_pcImpl = (nullptr != cInThat.m_pcImpl) ? cInThat.m_pcImpl->Clone() : nullptr;
}

H3DF::ShapeElement::ShapeElement(ShapeElement && cInThat) noexcept :
	Object(std::move(cInThat))
{

}

ShapeElement & H3DF::ShapeElement::operator=(ShapeElement && cInThat)  noexcept
{
	this->Object::operator = (std::move(cInThat));
	return *this;
}

H3DF::ShapeElement::~ShapeElement()
{

}

ShapeElement const & H3DF::ShapeElement::operator = (ShapeElement const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

bool H3DF::ShapeElement::Equals(ShapeElement const & cInThat) const
{
	auto pcImpl = static_cast<ShapeElementImpl const *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	auto pccInImpl = static_cast<ShapeElementImpl const *>(cInThat.GetImpl());
	DEBUG_VALID(pccInImpl);

	pcImpl->Equals(pccInImpl);
	
	return true;
}

bool H3DF::ShapeElement::operator==(ShapeElement const & cInThat) const
{
	return Equals(cInThat);
}

bool H3DF::ShapeElement::operator!=(ShapeElement const & cInThat) const
{
	return !Equals(cInThat);
}

ShapeElement & H3DF::ShapeElement::SetDisjointed(bool bInState)
{
	auto pcImpl = static_cast<ShapeElementImpl *> (m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_bDisjointedFlag = bInState;
	
	return *this;
}

bool H3DF::ShapeElement::ShowDisjointed(bool & bOutState) const
{
	auto pcImpl = static_cast<ShapeElementImpl *> (m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	bOutState = pcImpl->m_bDisjointedFlag;

	return true;
}

ShapeElement & H3DF::ShapeElement::SetFill(bool bInState)
{
	auto pcImpl = static_cast<ShapeElementImpl *> (m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_bFillFlag = bInState;

	return *this;
}

bool H3DF::ShapeElement::ShowFill(bool & bOutState) const
{
	auto pcImpl = static_cast<ShapeElementImpl *> (m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	bOutState = pcImpl->m_bFillFlag;

	return true;
}

//== PolygonShapeElement Class ====================================================================

H3DF::PolygonShapeElement::PolygonShapeElement()
{
	DEBUG_VALID(SetImpl(H3DF::Type::PolygonShapeElement, std::make_unique<PolygonShapeElementImpl>()));
}

H3DF::PolygonShapeElement::PolygonShapeElement(ShapeElement const & cInThat)
{
	m_pcImpl = cInThat.GetImpl()->Clone();
	m_pcImpl->SetType(H3DF::Type::PolygonShapeElement);

	return;

	// PolygonShapeElementImpl 생성
	m_pcImpl = std::make_unique<PolygonShapeElementImpl>();
	auto pcImpl = static_cast<PolygonShapeElementImpl *>(m_pcImpl.get());

	auto pcInThatImpl = static_cast<const ShapeElementImpl *>(cInThat.GetImpl());

	if (nullptr != pcImpl && nullptr != pcInThatImpl) {
		pcImpl->ShapeElementImpl::Copy(pcInThatImpl);
	}
	else {
		DEBUG_STOP;
	}

	
}

H3DF::PolygonShapeElement::PolygonShapeElement(PolygonShapeElement const & cInThat)
{
	m_pcImpl = (nullptr != cInThat.m_pcImpl) ? cInThat.m_pcImpl->Clone() : nullptr;
}

H3DF::PolygonShapeElement::PolygonShapeElement(ShapePointArray const & arInPoints)
{
	m_pcImpl = std::make_unique<PolygonShapeElementImpl>();
	DEBUG_VALID(m_pcImpl);
	m_pcImpl->SetType(H3DF::Type::PolygonShapeElement);

	auto pcImpl = static_cast<PolygonShapeElementImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_arPoints = arInPoints;
}

H3DF::PolygonShapeElement::PolygonShapeElement(size_t nInCount, ShapePoint const arInPoints[])
{
	m_pcImpl = std::make_unique<PolygonShapeElementImpl>();
	DEBUG_VALID(m_pcImpl);
	m_pcImpl->SetType(H3DF::Type::PolygonShapeElement);

	auto pcImpl = static_cast<PolygonShapeElementImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_arPoints.resize(nInCount);

	for (size_t nIndex = 0; nIndex < nInCount; nIndex++) {
		pcImpl->m_arPoints[nIndex] = arInPoints[nIndex];
	}
}

H3DF::PolygonShapeElement::~PolygonShapeElement()
{
}

H3DF::PolygonShapeElement::PolygonShapeElement(PolygonShapeElement && cInThat) noexcept :
	ShapeElement(std::move(cInThat))
{
	m_pcImpl->SetType(H3DF::Type::PolygonShapeElement);
}

H3DF::PolygonShapeElement & H3DF::PolygonShapeElement::operator = (PolygonShapeElement && cInThat) noexcept
{
	this->ShapeElement::operator = (std::move(cInThat));
	return *this;
}

PolygonShapeElement const & H3DF::PolygonShapeElement::operator = (PolygonShapeElement const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

PolygonShapeElement & H3DF::PolygonShapeElement::SetPoints(ShapePointArray const & arInPoints)
{
	auto pcImpl = static_cast<PolygonShapeElementImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_arPoints = arInPoints;

	return *this;
}

PolygonShapeElement & H3DF::PolygonShapeElement::SetPoints(size_t nInCount, ShapePoint const arInPoints[])
{
	auto pcImpl = static_cast<PolygonShapeElementImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_arPoints.resize(nInCount);
	for (size_t nIndex = 0; nIndex < nInCount; nIndex++) {
		pcImpl->m_arPoints[nIndex] = arInPoints[nIndex];
	}

	return *this;
}

bool H3DF::PolygonShapeElement::ShowPoints(ShapePointArray & arOutPoints) const
{
	auto pcImpl = static_cast<PolygonShapeElementImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	arOutPoints = pcImpl->m_arPoints;

	return true;
}

//== PolygonShapeElement Class ====================================================================

H3DF::AnchorShapeElement::AnchorShapeElement()
{
	DEBUG_VALID(SetImpl(H3DF::Type::AnchorShapeElement, std::make_unique<AnchorShapeElementImpl>()));
}

H3DF::AnchorShapeElement::AnchorShapeElement(ShapeElement const & cInThat)
{
	// PolygonShapeElementImpl 생성
	m_pcImpl = std::make_unique<AnchorShapeElementImpl>();
	auto pcImpl = static_cast<AnchorShapeElementImpl *>(m_pcImpl.get());

	auto pcInThatImpl = static_cast<const ShapeElementImpl *>(cInThat.GetImpl());

	if (nullptr != pcImpl && nullptr != pcInThatImpl) {
		pcImpl->ShapeElementImpl::Copy(pcInThatImpl);
	}
	else {
		DEBUG_STOP;
	}

	m_pcImpl->SetType(H3DF::Type::AnchorShapeElement);
}

H3DF::AnchorShapeElement::AnchorShapeElement(AnchorShapeElement const & cInThat)
{
	m_pcImpl = (nullptr != cInThat.m_pcImpl) ? cInThat.m_pcImpl->Clone() : nullptr;
}


H3DF::AnchorShapeElement::AnchorShapeElement(ShapePoint const & cInAnchorPoint)
{
	m_pcImpl = std::make_unique<AnchorShapeElementImpl>();
	auto pcImpl = static_cast<AnchorShapeElementImpl *>(m_pcImpl.get());

	pcImpl->m_cAnchorPoint = cInAnchorPoint;
	pcImpl->m_bSetAnchorPointFlag = true;
}

H3DF::AnchorShapeElement::AnchorShapeElement(ShapePoint const & cInAnchorPoint,	ShapePointArray const & arInIntermediatePoints, bool bInConnection)
{
	m_pcImpl = std::make_unique<AnchorShapeElementImpl>();
	DEBUG_VALID(m_pcImpl);
	auto pcImpl = static_cast<AnchorShapeElementImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_cAnchorPoint = cInAnchorPoint;
	pcImpl->m_bSetAnchorPointFlag = true;

	pcImpl->m_arIntermediatePoints = arInIntermediatePoints;
	pcImpl->m_bSetIntermediatePointsFlag = true;

	pcImpl->m_bConnectionFlag = bInConnection;
	pcImpl->m_bSetConnectionFlag = true;
}

H3DF::AnchorShapeElement::~AnchorShapeElement()
{

}

H3DF::AnchorShapeElement::AnchorShapeElement(AnchorShapeElement && cInThat) noexcept :
	ShapeElement(std::move(cInThat))
{

}

AnchorShapeElement & H3DF::AnchorShapeElement::operator=(AnchorShapeElement && cInThat) noexcept
{
	this->ShapeElement::operator = (std::move(cInThat));
	return *this;
}

AnchorShapeElement const & H3DF::AnchorShapeElement::operator = (AnchorShapeElement const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

AnchorShapeElement & H3DF::AnchorShapeElement::SetAnchor(ShapePoint const & in_anchor)
{
	auto pcImpl = static_cast<AnchorShapeElementImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_cAnchorPoint = in_anchor;
	pcImpl->m_bSetAnchorPointFlag = true;

	return *this;
}

bool H3DF::AnchorShapeElement::ShowAnchor(ShapePoint & cOutAnchor) const
{
	auto pcImpl = static_cast<AnchorShapeElementImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	if (false == pcImpl->m_bSetAnchorPointFlag) {
		return false;
	}

	cOutAnchor = pcImpl->m_cAnchorPoint;

	return true;
}

AnchorShapeElement & H3DF::AnchorShapeElement::SetIntermediatePoints(ShapePointArray const & arInIntermediatePoints)
{
	auto pcImpl = static_cast<AnchorShapeElementImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_arIntermediatePoints = arInIntermediatePoints;
	pcImpl->m_bSetIntermediatePointsFlag = true;

	return *this;
}

bool H3DF::AnchorShapeElement::ShowIntermediatePoints(ShapePointArray & arOutIntermediatePoints) const
{
	auto pcImpl = static_cast<AnchorShapeElementImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	if (false == pcImpl->m_bSetIntermediatePointsFlag) {
		return false;
	}

	arOutIntermediatePoints = pcImpl->m_arIntermediatePoints;

	return true;
}

AnchorShapeElement & H3DF::AnchorShapeElement::UnsetIntermediatePoints()
{
	auto pcImpl = static_cast<AnchorShapeElementImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_arIntermediatePoints.clear();
	pcImpl->m_bSetIntermediatePointsFlag = false;

	return *this;
}

AnchorShapeElement & H3DF::AnchorShapeElement::SetConnection(bool bInConnection)
{
	auto pcImpl = static_cast<AnchorShapeElementImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_bConnectionFlag = bInConnection;
	pcImpl->m_bSetConnectionFlag = true;

	return *this;
}

bool H3DF::AnchorShapeElement::ShowConnection(bool & bOutConnection) const
{
	auto pcImpl = static_cast<AnchorShapeElementImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);
	
	if (false == pcImpl->m_bSetConnectionFlag) {
		return false;
	}

	bOutConnection = pcImpl->m_bConnectionFlag;

	return true;
}

AnchorShapeElement & H3DF::AnchorShapeElement::UnsetConnection()
{
	auto pcImpl = static_cast<AnchorShapeElementImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_bSetConnectionFlag = false;

	return *this;
}

// == ShapeKit Class ==============================================================================

H3DF::ShapeKit::ShapeKit()
{
	DEBUG_VALID(SetImpl(H3DF::Type::ShapeKit, std::make_unique<ShapeKitImpl>()));
}

H3DF::ShapeKit::ShapeKit(ShapeKit const & cInThat)
{
	m_pcImpl = (nullptr != cInThat.m_pcImpl) ? cInThat.m_pcImpl->Clone() : nullptr;
}

H3DF::ShapeKit::~ShapeKit()
{

}

H3DF::ShapeKit::ShapeKit(ShapeKit && cInThat)  noexcept :
	Kit(std::move(cInThat))
{
}

ShapeKit & H3DF::ShapeKit::operator=(ShapeKit && cInThat) noexcept 
{
	this->Kit::operator = (std::move(cInThat));
	return *this;
}

ShapeKit & H3DF::ShapeKit::operator = (ShapeKit const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

void H3DF::ShapeKit::Show(ShapeKit & cOutKit) const
{
	cOutKit = *this;
}

bool H3DF::ShapeKit::Empty() const
{
	auto pcImpl = static_cast<ShapeKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	return pcImpl->Empty();
}

bool H3DF::ShapeKit::Equals(ShapeKit const & cInKit) const
{
	auto pcImpl = static_cast<ShapeKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	return pcImpl->Equals(cInKit);
}

bool H3DF::ShapeKit::operator == (ShapeKit const & cInKit) const
{
	return Equals(cInKit);
}

bool H3DF::ShapeKit::operator != (ShapeKit const & cInKit) const
{
	return !Equals(cInKit);
}

ShapeKit & H3DF::ShapeKit::SetElements(ShapeElementArray const & cInDef)
{
	auto pcImpl = static_cast<ShapeKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_arElements = cInDef;
	pcImpl->m_bSetElementsFlag = true;

	return *this;
}

ShapeKit & H3DF::ShapeKit::SetElements(size_t nInCount, ShapeElement const cInDef[])
{
	auto pcImpl = static_cast<ShapeKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	for (size_t nIndex = 0; nIndex < nInCount; nIndex++) {
		pcImpl->m_arElements.emplace_back(cInDef[nIndex]);
	}

	pcImpl->m_bSetElementsFlag = true;

	return *this;
}

ShapeKit & H3DF::ShapeKit::SetElement(ShapeElement const & cInElement)
{
	auto pcImpl = static_cast<ShapeKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_arElements.clear();
	pcImpl->m_arElements.emplace_back(cInElement);
	pcImpl->m_bSetElementsFlag = true;

	return *this;
}

ShapeKit & H3DF::ShapeKit::UnsetElements()
{
	auto pcImpl = static_cast<ShapeKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_arElements.clear();
	pcImpl->m_bSetElementsFlag = false;

	return *this;
}

ShapeKit & H3DF::ShapeKit::UnsetEverything()
{
	UnsetElements();
	
	return *this;
}

bool H3DF::ShapeKit::ShowElements(ShapeElementArray & cOutDef) const
{
	auto pcImpl = static_cast<ShapeKitImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	if (false == pcImpl->m_bSetElementsFlag) {
		return false;
	}

	cOutDef = pcImpl->m_arElements;

	return true;
}

// == ShapeDefinition Class =======================================================================

H3DF::ShapeDefinition::ShapeDefinition()
{
	DEBUG_VALID(SetImpl(H3DF::Type::ShapeDefinition, std::make_unique<ShapeDefinitionImpl>()));
}

H3DF::ShapeDefinition::ShapeDefinition(Definition const & cInThat)
{
	// PolygonShapeElementImpl 생성
	m_pcImpl = std::make_unique<ShapeDefinitionImpl>();
	auto pcImpl = static_cast<ShapeDefinitionImpl *>(m_pcImpl.get());

	auto pcInThatImpl = static_cast<const DefinitionImpl *>(cInThat.GetImpl());

	if (nullptr != pcImpl && nullptr != pcInThatImpl) {
		pcImpl->DefinitionImpl::Copy(pcInThatImpl);
	}
	else {
		DEBUG_STOP;
	}

	m_pcImpl->SetType(H3DF::Type::ShapeDefinition);
}

H3DF::ShapeDefinition::ShapeDefinition(ShapeDefinition const & cInThat)
{
	m_pcImpl = (nullptr != cInThat.m_pcImpl) ? cInThat.m_pcImpl->Clone() : nullptr;
}

H3DF::ShapeDefinition::~ShapeDefinition()
{
}

H3DF::ShapeDefinition::ShapeDefinition(ShapeDefinition && cInThat) noexcept :
	Definition(std::move(cInThat))
{
}

ShapeDefinition & H3DF::ShapeDefinition::operator=(ShapeDefinition && cInThat) noexcept
{
	this->Definition::operator = (std::move(cInThat));
	return *this;
}

ShapeDefinition & H3DF::ShapeDefinition::operator = (ShapeDefinition const & cInThat)
{
	if (nullptr != cInThat.m_pcImpl) {
		m_pcImpl = cInThat.m_pcImpl->Clone();
	}
	else {
		m_pcImpl.reset();
	}

	return *this;
}

void H3DF::ShapeDefinition::Set(ShapeKit const & cInKit)
{
	auto pcImpl = static_cast<ShapeDefinitionImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	pcImpl->m_cShape = cInKit;
}

void H3DF::ShapeDefinition::Show(ShapeKit & cOutKit) const
{
	auto pcImpl = static_cast<ShapeDefinitionImpl *>(m_pcImpl.get());
	DEBUG_VALID(pcImpl);

	cOutKit = pcImpl->m_cShape;
}


//== Test Function ================================================================================

#include "Segment.h"
#include "Portfolio.h"
// #include "Text.h"

void Test() 
{
	

	// define the vertices of the textbox rectangle
	H3DF::ShapePoint leftBottom(-1, -1);
	H3DF::ShapePoint leftTop(-1, 1);
	H3DF::ShapePoint rightBottom(1, -1);
	H3DF::ShapePoint rightTop(1, 1);

	H3DF::ShapePoint textBoxRectanglePoints[4] = { leftBottom, rightBottom, rightTop, leftTop };

	// use a PolygonShapeElement to contain our textbox rectangle
	H3DF::PolygonShapeElement rectangleBackgroundShape(4, textBoxRectanglePoints);

	// 5.4 Define Leader Line Anchor and Intermediate Points=====
	// set the anchor at the top right vertex of our textbox rectangle
	H3DF::AnchorShapeElement lineAnchor(rightTop);

	// define the intermediate point where the leader line will bend
	H3DF::ShapeCoordinate intermediatePoint_1(1.5f, 2.5f);
	H3DF::ShapePointArray intermediatePoints;
	intermediatePoints.emplace_back(intermediatePoint_1.x, intermediatePoint_1.y);

	// and set the intermediate point
	lineAnchor.SetIntermediatePoints(intermediatePoints);
	
	// 5.5 Define a ShapeKit and Add Text

	// add our textbox background and line anchors to a ShapeKit
	H3DF::ShapeKit rectangle_shape;
	H3DF::ShapeElement rectangle_elements[] = { rectangleBackgroundShape, lineAnchor };

	rectangle_shape.SetElements(2, rectangle_elements);

	// 5.3 Define the Annotation Shape
	H3DF::SegmentKey rectangleSegmentKey;// = GetCanvas().GetFrontView().GetAttachedModel().GetSegmentKey();

	H3DF::PortfolioKey portfolio;
	rectangleSegmentKey.GetPortfolioControl().ShowTop(portfolio);

	// define the rectangle_shape in our portfolio and add to the rectangle segment
	portfolio.DefineShape("anchored_leader_line_rectangle", rectangle_shape);
	//rectangleSegmentKey.GetTextAttributeControl().SetBackground("anchored_leader_line_rectangle");
/*
	// create a TextKey and insert the text into it
	H3DF::TextKey rectangle_text =
		rectangleSegmentKey.InsertText(H3DF::Point(2, -2, 0), "Vertex is 0.5, 0.5, -0.5\nin world space.");
	rectangle_text.SetColor(RGBColor(0.1, 0.1, 0.1)).SetFont("stroked").SetBold(true).SetSize(26, H3DF::Text::SizeUnits::Points);

	// put an outline around the text box
	rectangleSegmentKey.GetEdgeAttributeControl().SetWeight(2, H3DF::Edge::SizeUnits::Pixels);

	// define where the leader line will point, just a little bit offset from the cube vertex of 0.5, 0.5, -0.5
	H3DF::Point leader_line_position(0.47f, 0.5f, -0.57f);
	rectangle_text.SetLeaderLine(leader_line_position);* /*/
}