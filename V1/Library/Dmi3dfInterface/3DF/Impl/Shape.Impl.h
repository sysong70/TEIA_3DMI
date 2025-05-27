#pragma once

#include "../3DF.h"

#include "../Shape.h"

#include "DefinitionImpl.h"

namespace H3DF
{
	class ShapeElementImpl : public Impl
	{
	public:
		ShapeElementImpl() { m_eType = H3DF::Type::ShapeElement; }

		void Set(ShapeElementImpl * pcInThat)
		{
			if (pcInThat == nullptr) {
				DEBUG_STOP;
				return;
			}

			m_bFillFlag = pcInThat->m_bFillFlag;
			m_bDisjointedFlag = pcInThat->m_bDisjointedFlag;
		}

		bool Equals(ShapeElementImpl const * pcInThat) const;

		bool m_bFillFlag = false;
		bool m_bDisjointedFlag = false;
	};

	class PolygonShapeElementImpl : public ShapeElementImpl
	{
	public:
		PolygonShapeElementImpl() { m_eType = H3DF::Type::PolygonShapeElement; }

		void Set(PolygonShapeElementImpl * pcInThat)
		{
			if (pcInThat == nullptr) {
				DEBUG_STOP;
				return;
			}

			ShapeElementImpl::Set(pcInThat);

			m_arPoints = pcInThat->m_arPoints;
		}

		ShapePointArray m_arPoints;
	};

	class AnchorShapeElementImpl : public ShapeElementImpl
	{
	public:
		AnchorShapeElementImpl() { m_eType = H3DF::Type::AnchorShapeElement; }

		void Set(AnchorShapeElementImpl * pcInThat)
		{
			if (pcInThat == nullptr) {
				DEBUG_STOP;
				return;
			}

			ShapeElementImpl::Set(pcInThat);

			m_cAnchorPoint = pcInThat->m_cAnchorPoint;
			m_bSetAnchorPointFlag = pcInThat->m_bSetAnchorPointFlag;

			m_arIntermediatePoints = pcInThat->m_arIntermediatePoints;
			m_bSetIntermediatePointsFlag = pcInThat->m_bSetIntermediatePointsFlag;

			m_bConnectionFlag = pcInThat->m_bConnectionFlag;
			m_bSetConnectionFlag = pcInThat->m_bSetConnectionFlag;
		}

		ShapePoint m_cAnchorPoint;
		bool m_bSetAnchorPointFlag = false;

		ShapePointArray m_arIntermediatePoints;
		bool m_bSetIntermediatePointsFlag = false;

		bool m_bConnectionFlag = false;
		bool m_bSetConnectionFlag = false;
	};

	class ShapeKitImpl : public Impl
	{
	public:
		ShapeKitImpl() { m_eType = H3DF::Type::ShapeKit; }

		void Set(ShapeKitImpl * pcInThat)
		{
			if (pcInThat == nullptr) {
				DEBUG_STOP;
				return;
			}

			m_arElements = pcInThat->m_arElements;
			m_bSetElementsFlag = pcInThat->m_bSetElementsFlag;
		}
		
		bool Empty()
		{
			m_arElements.clear();
			m_bSetElementsFlag = false;

			return false;
		}

		bool Equals(ShapeKit const & cInKit) const
		{
			return false;
		}

		ShapeElementArray m_arElements;
		bool m_bSetElementsFlag = false;
	};

	class ShapeDefinitionImpl : public DefinitionImpl
	{
	public:
		ShapeDefinitionImpl() { m_eType = H3DF::Type::ShapeDefinition; }

		void Set(ShapeDefinitionImpl * pcInThat)
		{
			m_cShape = pcInThat->m_cShape;
		}

		ShapeKit m_cShape;
	};
}