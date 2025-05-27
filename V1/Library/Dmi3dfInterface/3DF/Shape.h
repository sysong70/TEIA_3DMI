#pragma once

#include "3DF.h"
#include "Kit.h"
#include "Definition.h"
#include "Math.h"
#include "Object.h"

#pragma warning(push)
#pragma warning(disable: 4251) // C4251: '클래스' : 클래스 'std::vector<float>' 클라이언트에서 DLL 인터페이스를 사용하도록 지정해야 합니다.

namespace H3DF
{
    class API_3DF ShapeCoordinate
    {
    public:
        ShapeCoordinate();

        ShapeCoordinate(float fInX, float fInY);

        ShapeCoordinate(float fInX, float fInY, FloatArray const & afInMargins);
        ShapeCoordinate(float fInX, float fInY, size_t nInCount, float const afInMargins[]);
        ShapeCoordinate(float fInX, float fInY, float fInRadius);
        ShapeCoordinate(float fInX, float fInY, float fInRadius, FloatArray const & afInMargins);
        ShapeCoordinate(float fInX, float fInY, float fInRadius, size_t nInCount, float const afInMargins[]);

		bool Equals(ShapeCoordinate const & cInThat) const;
        bool operator==(ShapeCoordinate const & cInThat) const { return Equals(cInThat); }
		bool operator!=(ShapeCoordinate const & cInThat) const { return !Equals(cInThat); }

        ShapeCoordinate & SetMargins(float fInMarginOne, float fInMarginTwo = 0.0f, float fInMarginThree = 0.0f, float fInMarginFour = 0.0f);
        bool ShowMargins(FloatArray & afOutMargins) const;

        ShapeCoordinate & UnsetMargins();

        float x;
        float y;
        float radius;
		FloatArray margins;
    };

    class API_3DF ShapePoint 
    {
    public:
        ShapePoint();
        ShapePoint(ShapeCoordinate const & cInX, ShapeCoordinate const & cInY);
        ShapePoint(float fInX, float fInY);

        bool Equals(ShapePoint const & cInThat) const { return (x == cInThat.x && y == cInThat.y); }
        bool operator==(ShapePoint const & cInThat) const { return Equals(cInThat); }
        bool operator!=(ShapePoint const & cInThat) const { return !Equals(cInThat); }

        ShapeCoordinate x;
        ShapeCoordinate y;
    };

    using ShapePointArray = std::vector<ShapePoint>;

	class API_3DF ShapeElement : public Object
	{
	public:
        ShapeElement();
        ShapeElement(ShapeElement const & cInThat);
        
        virtual ~ShapeElement();

        H3DF::Type ObjectType() const override { return H3DF::Type::ShapeElement; }

        ShapeElement(ShapeElement && cInThat) noexcept;
        ShapeElement & operator = (ShapeElement && cInThat) noexcept;

		void Set(ShapeElement const & cInThat);
        ShapeElement const & operator = (ShapeElement const & cInThat);

        bool Equals(ShapeElement const & cInThat) const;
        bool operator==(ShapeElement const & cInThat) const;
        bool operator!=(ShapeElement const & cInThat) const;

        // Two shape elements of the same type are automatically joined together if they appear consecutively in the shape
        // element array. LineShapeElement objects are always disjointed.
        ShapeElement & SetDisjointed(bool bInState);

        // Two shape elements of the same type are automatically joined together if they appear consecutively in the shape
        // element array. LineShapeElement objects are always disjointed.
        bool ShowDisjointed(bool & bOutState) const;

        // All shape elements default to being filled, except for Lines which cannot be filled.
        ShapeElement & SetFill(bool bInState);

        bool ShowFill(bool & bOutState) const;
	};

	using ShapeElementArray = std::vector<ShapeElement>;

    class API_3DF PolygonShapeElement: public ShapeElement {
      public:
        PolygonShapeElement();
        PolygonShapeElement(ShapeElement const & cInThat);
        PolygonShapeElement(PolygonShapeElement const& cInThat);
        explicit PolygonShapeElement(ShapePointArray const& arInPoints);
        PolygonShapeElement(size_t nInCount, ShapePoint const arInPoints[]);

        ~PolygonShapeElement() noexcept override;
        
        H3DF::Type ObjectType() const override { return H3DF::Type::PolygonShapeElement; }
        
        PolygonShapeElement(PolygonShapeElement && cInThat) noexcept;
        PolygonShapeElement& operator=(PolygonShapeElement&& cInThat) noexcept;

		void Set(PolygonShapeElement const & cInThat);
        PolygonShapeElement const & operator = (PolygonShapeElement const & cInThat);

        PolygonShapeElement& SetPoints(ShapePointArray const& arInPoints);
        PolygonShapeElement& SetPoints(size_t nInCount, ShapePoint const arInPoints[]);
        bool ShowPoints(ShapePointArray & arOutPoints) const;
    };

    class API_3DF AnchorShapeElement : public ShapeElement {
    public:
        AnchorShapeElement();
        AnchorShapeElement(ShapeElement const & cInThat);
        AnchorShapeElement(AnchorShapeElement const & cInThat);
        explicit AnchorShapeElement(ShapePoint const & cInAnchorPoint);
        explicit AnchorShapeElement(ShapePoint const & cInAnchorPoint, ShapePointArray const & arInIntermediatePoints, bool bInConnection);

        ~AnchorShapeElement();

        H3DF::Type ObjectType() const override { return H3DF::Type::AnchorShapeElement; }

        AnchorShapeElement(AnchorShapeElement && cInThat);
        AnchorShapeElement & operator=(AnchorShapeElement && cInThat);

		void Set(AnchorShapeElement const & cInThat);
        AnchorShapeElement const & operator = (AnchorShapeElement const & cInThat);

        AnchorShapeElement & SetAnchor(ShapePoint const & in_anchor);

        bool ShowAnchor(ShapePoint & out_anchor) const;

        AnchorShapeElement & SetIntermediatePoints(ShapePointArray const & arInIntermediatePoints);

        bool ShowIntermediatePoints(ShapePointArray & arOutIntermediatePoints) const;

        AnchorShapeElement & UnsetIntermediatePoints();

        AnchorShapeElement & SetConnection(bool bInConnection);

        bool ShowConnection(bool & bOutConnection) const;

        AnchorShapeElement & UnsetConnection();
    };

	class API_3DF ShapeKit : public Kit {
	public:
		ShapeKit();
		ShapeKit(ShapeKit const & cInKit);

		virtual ~ShapeKit();

        H3DF::Type ObjectType() const override { return H3DF::Type::ShapeKit; }

		ShapeKit(ShapeKit && cInThat) noexcept;
		ShapeKit & operator=(ShapeKit && cInThat) noexcept;

		void Set(ShapeKit const & cInKit);
		ShapeKit & operator=(ShapeKit const & cInKit);

		void Show(ShapeKit & cOutKit) const;

		bool Empty() const;

		bool Equals(ShapeKit const & cInKit) const;

		bool operator==(ShapeKit const & cInKit) const;
		bool operator!=(ShapeKit const & cInKit) const;

		ShapeKit & SetElements(ShapeElementArray const & cInDef);

		ShapeKit & SetElements(size_t in_count, ShapeElement const cInDef[]);

		ShapeKit & SetElement(ShapeElement const & cInElement);

		ShapeKit & UnsetElements();

		ShapeKit & UnsetEverything();

		bool ShowElements(ShapeElementArray & cOutDef) const;
	};

    class API_3DF ShapeDefinition : public Definition {
    public:
        ShapeDefinition();
        ShapeDefinition(Definition const & cInThat);
        ShapeDefinition(ShapeDefinition const & cInThat);

        ~ShapeDefinition();

        H3DF::Type ObjectType() const override { return H3DF::Type::ShapeDefinition; }
        
        ShapeDefinition(ShapeDefinition && cInThat) noexcept;
        ShapeDefinition & operator = (ShapeDefinition && cInThat) noexcept;

		void Set(ShapeDefinition const & cInThat);
        ShapeDefinition & operator = (ShapeDefinition const & cInThat);

        void Set(ShapeKit const & cInKit);
        void Show(ShapeKit & cOutKit) const;
    };
}

#pragma warning(pop)