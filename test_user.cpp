#include <cassert>

#include <test.h>
#include <test_space.h>
#include <test_manager.h>
#include <test_service.h>
#include <test_draw.h>
#include <test_window.h>
#include <test_samples.h>

#include <action_general.h>
#include <action_point.h>
#include <action_curve.h>
#include <action_curve3d.h>
#include <action_surface_curve.h>
#include <action_surface.h>
#include <action_direct.h>
#include <action_mesh.h>
#include <action_phantom.h>
#include <action_shell.h>
#include <action_sheet.h>
#include <action_solid.h>

#include <templ_s_array.h>
#include <templ_rp_array.h>
#include <templ_array2.h>
#include <templ_sptr.h>
#include <templ_dptr.h>

#include <mb_cart_point.h>
#include <mb_cart_point3d.h>
#include <mb_matrix.h>
#include <mb_matrix3d.h>
#include <mb_matrixnn.h>
#include <mb_placement.h>
#include <mb_placement3d.h>
#include <mb_axis3d.h>
#include <mb_thread.h>
#include <mb_nurbs_function.h>

#include <function.h>
#include <function_factory.h>

#include <curve.h>
#include <cur_line_segment.h>
#include <cur_arc.h>
#include <cur_nurbs.h>
#include <cur_hermit.h>
#include <cur_reparam_curve.h>
#include <cur_trimmed_curve.h>
#include <cur_character_curve.h>
#include <cur_polyline.h>
#include <cur_projection_curve.h>
#include <cur_contour.h>

#include <region.h>
#include <contour_graph.h>

#include <curve3d.h>
#include <cur_line_segment3d.h>
#include <cur_arc3d.h>
#include <cur_nurbs3d.h>
#include <cur_hermit3d.h>
#include <cur_trimmed_curve3d.h>
#include <cur_reparam_curve3d.h>
#include <cur_character_curve3d.h>
#include <cur_contour3d.h>
#include <cur_plane_curve.h>
#include <cur_surface_curve.h>
#include <cur_contour_on_surface.h>
#include <cur_silhouette_curve.h>
#include <cur_surface_intersection.h>
#include <cur_spiral.h>

#include <surface.h>
#include <surf_plane.h>
#include <surf_cylinder_surface.h>
#include <surf_cone_surface.h>
#include <surf_sphere_surface.h>
#include <surf_torus_surface.h>
#include <surf_extrusion_surface.h>
#include <surf_revolution_surface.h>
#include <surf_lofted_surface.h>
#include <surf_evolution_surface.h>
#include <surf_spiral_surface.h>
#include <surf_spline_surface.h>
#include <surf_offset_surface.h>
#include <surf_curve_bounded_surface.h>

#include <attribute_item.h>
#include <attr_color.h>
#include <attr_common_attribute.h>
#include <attr_dencity.h>
#include <attr_identifier.h>
#include <attr_selected.h>
#include <attr_user_attribute.h>
#include <attribute_container.h>

#include <name_item.h>
#include <name_check.h>

#include <topology.h>
#include <topology_faceset.h>
#include <check_geometry.h>

#include <point3d.h>
#include <point_frame.h>
#include <wire_frame.h>
#include <creator.h>
#include <cr_simple_creator.h>
#include <solid.h>
#include <instance_item.h>
#include <plane_instance.h>
#include <space_instance.h>
#include <assembly.h>

#include <mesh.h>
#include <mesh_grid.h>
#include <mesh_primitive.h>
#include <mesh_polygon.h>

#include <map_create.h>
#include <map_lump.h>
#include <map_section.h>
#include <map_section_complex.h>
#include <map_vestige.h>
#include <map_thread.h>

#include <tri_face.h>

#include <mip_curve_properties.h>
#include <mip_solid_area_volume.h>
#include <mip_solid_mass_inertia.h>

#include <cdet_bool.h>
#include <cdet_data.h>
#include <cdet_utility.h>
#include <part_solid.h>

#include <alg_base.h>
#include <alg_draw.h>
#include <generic_utility.h>
#include <conv_model_exchange.h>
#include <algorithm>
#include <ctime>
#include <functional>
#include <map>
#include <vector>
#include <utility>

#include <math_namespace.h>
#include <last.h>

using namespace c3d;

//------------------------------------------------------------------------------
// Sample how to project the boundary 3D curves onto their surface to get boundary 2D contour
// ---
void TestSample()
{
  double cylRadius = 20;
  double cylHeight = 100;

  // build whole cylinder or not;
  bool doWholeCylinder = true;// false;
  // bound curves rotation angle
  double rotationAngle = -M_PI / 3; // you can change it to test different angle disposition

  c3d::SurfaceSPtr surface;
  c3d::SpaceCurvesSPtrVector outerBoundCurves; // outer bound curves in space

  if ( cylRadius > METRIC_ACCURACY && cylHeight > METRIC_ACCURACY ) {
    MbCartPoint3D arcOrigin;
    MbVector3D arcZ( 0, 0, 1 );
    MbVector3D arcX( 1, 0, 0 );

    SPtr<MbCurve3D> arc0( new MbArc3D( arcOrigin, arcZ, arcX, cylRadius, cylRadius, M_PI ) ); // arc (half of circle)
    SPtr<MbCurve3D> arc1( static_cast<MbArc3D *>( &arc0->Duplicate() ) );
    SPtr<MbCurve3D> arc2( static_cast<MbArc3D *>( &arc0->Duplicate() ) );

    double cylHalfHeight = 0.5 * cylHeight;
    MbVector3D to( 0, 0, cylHalfHeight );

    arc2->Move( to );
    to.Invert();
    arc1->Move( to );

    MbCartPoint3D lsBeg, lsEnd;
    arc1->GetLimitPoint( 1, lsBeg );
    arc2->GetLimitPoint( 1, lsEnd );
    SPtr<MbCurve3D> lineSegment1( new MbLineSegment3D( lsBeg, lsEnd ) );
    arc1->GetLimitPoint( 2, lsBeg );
    arc2->GetLimitPoint( 2, lsEnd );
    SPtr<MbCurve3D> lineSegment2( new MbLineSegment3D( lsBeg, lsEnd ) );

    outerBoundCurves.reserve( 4 );
    outerBoundCurves.push_back( arc1 );
    outerBoundCurves.push_back( arc2 );
    outerBoundCurves.push_back( lineSegment1 );
    outerBoundCurves.push_back( lineSegment2 );

    MbPlacement3D cylPlace;
    if ( arc1->GetPlacement( cylPlace ) ) {
      surface = new MbCylinderSurface( cylPlace, cylRadius, cylHeight );

      if ( !doWholeCylinder ) {
        surface->SetLimit( 0.0, 0.0, M_PI, 1.0 ); // half of cylinder
      }
      if ( ::fabs( rotationAngle ) > ANGLE_EPSILON ) {
        MbAxis3D rotationAxis;
        if ( surface->GetCylinderAxis( rotationAxis ) ) {
          for ( const auto & outerBoundCurve : outerBoundCurves ) {
            outerBoundCurve->Rotate( rotationAxis, rotationAngle );
          }
        }
      }
    }
  }

  if ( surface != nullptr && (outerBoundCurves.size() == 4) ) {
    SpaceContourSPtr outerBoundContour;
    {
      RPArray<MbCurve3D> tmpCurves;
      for ( const auto & outerBoundCurve : outerBoundCurves ) {
        tmpCurves.push_back( outerBoundCurve );
      }
      // we need to build whole bound contour for projection
      CreateContours( tmpCurves, METRIC_PRECISION, false );
      C3D_ASSERT( tmpCurves.size() == 1 );

      if ( tmpCurves.size() == 1 && tmpCurves.front()->Type() == st_Contour3D ) {
        outerBoundContour = static_cast<MbContour3D *>(tmpCurves.front());
      }
    }

    MbItem * surfaceItem = nullptr;
    { // for C3D Test Application only
      Style surfaceStyle( 1, RGB2uint32( TRGB_AZURE ) );
      surfaceItem = TestVariables::viewManager->AddObject( *surface );

      Style curveStyle( 2, RGB2uint32( TRGB_AZURE ) );
      TestVariables::viewManager->AddObject( curveStyle, outerBoundContour );
    }

    if ( outerBoundContour != nullptr ) {
      MbVector3D * generalProjectionDirection = nullptr; // null pointer because that we need projection by normal to surface
      bool createExact = true; // we guess that exact boundaries are better in any case
      bool truncateByBounds = false; // here we must not use cutting to get single contour

      c3d::PlaneContourSPtr uvContour;
      {
        c3d::SpaceCurvesSPtrVector spatialSurfaceCurves;

        // you should project whole bound contour to get correct projection contour on periodic surface
        MbResultType res = CurveProjection( *surface, *outerBoundContour, generalProjectionDirection, createExact, truncateByBounds, spatialSurfaceCurves );

        if ( res == rt_Success && spatialSurfaceCurves.size() == 1 ) {
          c3d::PlaneCurveSPtr uvCurve;
          if ( spatialSurfaceCurves.front()->GetSurfaceCurve( uvCurve, surface ) ) {
            if ( uvCurve->Type() == pt_Contour ) {
              uvContour = &static_cast<MbContour &>(*uvCurve);
              double uvArea = uvContour->GetArea();
              if ( uvArea < 0 ) { // area of outer contour have be positive
                uvContour->Inverse(); // so we have to invert contour if the ares is negative
              }
            }
          }
        }
      }

      if ( uvContour != nullptr ) {
        surface = new MbCurveBoundedSurface( *surface );
        static_cast<MbCurveBoundedSurface &>(*surface).AddContour( *uvContour );

        { // for C3D Test Application only
          TestVariables::viewManager->DeleteObject( surfaceItem );
          Style surfaceStyle( 1, RGB2uint32( TRGB_AZURE ) );
          surfaceItem = TestVariables::viewManager->AddObject( *surface );
        }

        // if you want to stitch a set of surfaces into a shell
        // you can create shells by surfaces, then stitch them using the function StitchShells

        c3d::SolidSPtr surfaceShell;
        {
          MbSolid * shellRawPtr = nullptr;
          MbSNameMaker dummyNameMaker;
          SurfaceShell( *surface, dummyNameMaker, shellRawPtr );
          if ( shellRawPtr != nullptr ) {
            surfaceShell = shellRawPtr;
          }
        }
        if ( surfaceShell != nullptr ) {
          { // for C3D Test Application only
            TestVariables::viewManager->DeleteObject( surfaceItem );
            Style surfaceStyle( 1, RGB2uint32( TRGB_OLIVE ) );
            surfaceItem = surfaceItem = TestVariables::viewManager->AddObject( surfaceStyle, surfaceShell );
          }
        }
      }
    }
  }
}


//------------------------------------------------------------------------------
// \ru Построение пользователя. \en User creation command.
// ---
void MakeUserCommand0() 
{ 
  TestSample();
  //::PutErrorMessage( 0, RT_MAKE_USER_COMAND_0 );
}


//------------------------------------------------------------------------------
// \ru Построение пользователя. \en User creation command.
// ---
void MakeUserCommand1()
{
  ::PutErrorMessage( 0, RT_MAKE_USER_COMAND_1 );
}


//------------------------------------------------------------------------------
// \ru Построение пользователя. \en User creation command.
// ---
void MakeUserCommand2()
{
  ::PutErrorMessage( 0, RT_MAKE_USER_COMAND_2 );
}


//------------------------------------------------------------------------------
// \ru Построение пользователя. \en User creation command.
// ---
void MakeUserCommand3() 
{
  ::PutErrorMessage( 0, RT_MAKE_USER_COMAND_3 );
}


//------------------------------------------------------------------------------
// \ru Построение пользователя. \en User creation command.
// ---
void MakeUserCommand4() 
{
  ::PutErrorMessage( 0, RT_MAKE_USER_COMAND_4 );
}


//------------------------------------------------------------------------------
// \ru Построение пользователя. \en User creation command.
// ---
void MakeUserCommand5() 
{
  ::PutErrorMessage( 0, RT_MAKE_USER_COMAND_5 );
}


//------------------------------------------------------------------------------
// \ru Построение пользователя. \en User creation command.
// ---
void MakeUserCommand6() 
{
  ::PutErrorMessage( 0, RT_MAKE_USER_COMAND_6 );
}


//------------------------------------------------------------------------------
// \ru Построение пользователя. \en User creation command.
// ---
void MakeUserCommand7() 
{
  ::PutErrorMessage( 0, RT_MAKE_USER_COMAND_7 );
}


//------------------------------------------------------------------------------
// \ru Построение пользователя. \en User creation command.
// ---
void MakeUserCommand8() 
{
  ::PutErrorMessage( 0, RT_MAKE_USER_COMAND_8 );
}


//------------------------------------------------------------------------------
// \ru Построение пользователя. \en User creation command.
// ---
void MakeUserCommand9() 
{
  ::PutErrorMessage( 0, RT_MAKE_USER_COMAND_9 );
}
