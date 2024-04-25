#pragma once

#include "3DF.h"
#include "Kit.h"
#include "Geometry.h"

#include "Math.h"
#include "Color.h"

namespace H3DF
{
    class API_3DF CuttingSection {
    public:
        // Enumerates the modes for cutting sections.  These are the shapes (if any) of the visualization geometry that will be
        // drawn to represent cutting planes within cutting sections.
        enum class Mode : uint32_t {
            None,   // No visualization geometry will be drawn for cutting planes in a cutting sections.
            Round,  // A circular plane will be drawn for cutting planes in a cutting section.  The size of the planes will be
                    // based on the size of the geometry being cut and the visualization scale.
            Square, // A square plane will be drawn for cutting planes in a cutting section.  The size of the planes will be
                    // based on the size of the geometry being cut and the visualization scale.
            Plane   // An infinitely large plane will be drawn for cutting planes in a cutting section.
        };

        // Enumerates the level at which capping geometry should be generated for cutting sections. */
        enum class CappingLevel : uint32_t {
            Entity,     // Each piece of geometry should be treated as if it encloses a volume.
            Segment,    // All geometry in a segment (but not subsegments) should be treated as if it encloses a single volume.
            SegmentTree // All geometry in a segment tree should be treated as if it encloses a single volume.
        };

        // Enumerates the choices for whether geometry (faces) are cut and processed for capping geometry. */
        enum class CappingUsage : uint32_t {
            Off,        // Faces will not be used for capping
            On,         // Faces will be used for capping generation
            Visibility  // Faces will be used for capping generation if they are visible
        };

        // Enumerates the how tolerance should be interpreted. */
        enum class ToleranceUnits : uint32_t {
            Percent,    // Tolerance specified is interpreted as a percentage of current view (camera field).
            WorldSpace, // Tolerance specified is interpreted in world space.
        };

        // Enumerates the level at which cutting planes should operate. */
        enum class CuttingLevel : uint32_t {
            Global, // Cutting sections will cut all geometry in the scene graph.
            Local,  // Cutting sections will only cut geometry contained in the segment and subsegments where the cutting section
                    // is inserted.
        };

        // Enumerates which material settings should be used for cut geometry generated for cutting sections. */
        enum class MaterialPreference : uint32_t {
            Explicit, // Use the effective materials set for cut faces and cut edges in the segment tree.
            Implicit, // Use the effective materials set for (regular) faces and (regular) edges in the segment tree. r)</span>
        };

        // Enumerates the level at which capping geometry should be gathered. */
        enum class GatheringLevel : uint32_t {
            Segment, // Capping geometry will be gathered only from a single segment.
            SegmentTree // Capping geometry will be gathered from the segment and all its subsegments and includes.
        };

    private:
        CuttingSection() {}
    };


    //== CuttingSectionKit Class ===================================================================
    class API_3DF CuttingSectionKit : public H3DF::Kit {
    public:
        CuttingSectionKit();
        CuttingSectionKit(CuttingSectionKit const & cInKit);

        void Set(CuttingSectionKit const & cInThat);
        CuttingSectionKit & operator = (CuttingSectionKit const & cInThat);

        H3DF::Type ObjectType() const { return H3DF::Type::CuttingSectionKit; };

        void Show(CuttingSectionKit & cOutKit) const;
        bool Empty() const;

        bool Equals(CuttingSectionKit const & cInKit) const;
        bool operator==(CuttingSectionKit const & cInKit) const;
        bool operator!=(CuttingSectionKit const & cInKit) const;

        // Sets a single cutting plane for this CuttingSectionKit.
        CuttingSectionKit & SetPlanes(H3DF::Plane const & cInPlane);
        CuttingSectionKit & SetPlanes(H3DF::PlaneArray const & cInPlanes);

        //CuttingSectionKit & SetVisualization(CuttingSection::Mode eInMode, RGBAColor const & cInColor, float fInScale = 1.0f);
        CuttingSectionKit & SetVisualization(RGBAColor const & cInColor);

/*
        CuttingSectionKit & UnsetPlanes();
        CuttingSectionKit & UnsetVisualization();
        CuttingSectionKit & UnsetEverything();

        size_t GetPlaneCount() const;
        bool ShowPlanes(H3DF::PlaneArray & out_planes) const;
*/
    };


    //== CuttingSectionKey Class ===================================================================
    class API_3DF CuttingSectionKey : public H3DF::GeometryKey {
    public:
        CuttingSectionKey();
        explicit CuttingSectionKey(Key const & cInThat);
        CuttingSectionKey(CuttingSectionKey const & cInThat);

        void Set(CuttingSectionKey const & cInThat);
        CuttingSectionKey & operator = (CuttingSectionKey const & cInThat);

        H3DF::Type ObjectType() const { return H3DF::Type::CuttingSectionKey; };

/*
        void Set(CuttingSectionKit const & cInKit);
        void Show(CuttingSectionKit & cOutKit) const;

        CuttingSectionKey & SetPlanes(H3DF::Plane const & cInPlane);
        CuttingSectionKey & SetPlanes(H3DF::PlaneArray const & cInPlanes);
        CuttingSectionKey & SetPlanes(size_t nInCount, Plane const pcInPlanes[]);

        CuttingSectionKey & SetVisualization(CuttingSection::Mode eInMode, RGBAColor const & cInColor, float fInScale = 1.0f);

        size_t GetPlaneCount() const;

        bool ShowPlanes(H3DF::PlaneArray & cOutPlanes) const;
        bool ShowVisualization(CuttingSection::Mode & cOutMode, RGBAColor & cOutColor, float & fOutScale) const;
*/
    };
}