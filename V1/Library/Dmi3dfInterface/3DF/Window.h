#pragma once

#include "3DF.h"

#include "Segment.h"
// #include "Selection.h"
// #include "Highlight.h"

namespace H3DF
{
	class BaseView;

	class API_3DF Window
	{
	public:
		// Enumerates the types of supported display drivers. We recommend that you use the Default3D driver type,
		// and provide end-users with the ability to manually select from a range of 3d drivers in a driver-pulldown list.
		// This will allow them to potentially work around graphics-card-specific problems that occur with a specific driver type.
		enum class Driver : uint32_t
		{
			Default3D,			// Visualize will perform runtime query of the 3D capabilities of the Operating System and graphics card and automatically choose a display driver.
			// Under Windows, preference will be given to DirectX API, followed by OpenGL. Under all other platforms, only the OpenGL API is supported. 
			OpenGL,				// This is an alias for OpenGL2
			OpenGL2,			// Shader-based driver which supports OpenGL Core Profile on Windows, Linux, and macOS. OpenGL ES 2.0+ on iOS and Android.
			DirectX11,			// Shader-based driver which supports DirectX 10.X+   Available on Windows. 
			OpenGL2Mesa,
			Metal,
		};

		/*! \enum UpdateStatus
		 *	Enumerates the status that can be returned when performing an update. */
		enum class UpdateStatus : uint32_t
		{
			InProgress,			// Update is not done yet.
			Completed,			// Update was successfully completed.
			TimedOut,			// Update timed out before it could be completed.
			Interrupted,		// Update was interrupted.
			Failed				// Update failed.
		};

		// Controls the type of update that Visualize will attempt to perform. For information regarding static model regeneration for each specific UpdateType, 
		// please see <a href="../../prog_guide/0703_performance_considerations.html">Performance Considerations</a> in the %Rendering section of the programming guide.
		enum class UpdateType : uint32_t
		{
			// Makes sure that the screen is up-to-date with respect to the internal scene graph. Allows Visualize to automatically perform optimizations.
			Default,

			// Redraws the entire scene regardless of whether any changes occurred in the scene-graph which may have triggered (required) an update.
			// It should only be used in specific situations where an update is required due to factors external to the scene-graph.
			// An example would be the situation where GUI event-handling logic encounters a 'GUI window expose' event which Visualize is not aware of,
			// but the scene needs to be forcefully redrawn to repair (via a complete redraw) the exposed portion of the window.
			Complete,

			// Pulls the back buffer onto the screen. This is a useful way to update the screen after an expose event like when a window that was occluding your scene is moved away.
			Refresh,

			// Only compiles static trees and display lists.
			CompileOnly,

			// Will perform a complete update and also rebuild static trees and display lists that are enabled in the scene-graph.
			// This should typically only be performed for the first update following a file load, or the effective equivalent, such as loading/creating another model
			// inside an existing scene-graph that has the static setting.
			Exhaustive,
		};

		// Enumerates the mobility modes (i.e., resizing and positioning behaviors) for stand-alone windows.
		enum class Mobility : uint32_t
		{
			Locked,				// The stand-alone window cannot be moved or resized.
			Floating,			// The stand-alone window can be moved, but cannot be resized.
			FixedRatio,			// The stand-alone window can be moved and resized, but the aspect ratio cannot be changed.
			Free				// The stand-alone window can be moved and resized, and the aspect ratio can be changed.
		};

		// Dimensions of the Window Frame
		enum class FrameSize : uint32_t
		{
			Single,				// Window Frame Size
			Double				// Window Frame Size
		};

		//Window Frame Positioning
		enum class FrameOptions : uint32_t
		{
			Inset,				// Window Frame Positioning
			Overlay				// Window Frame Positioning
		};

		// The native image format for off-screen windows
		enum class ImageFormat : uint32_t
		{
			Default,			// Image Format
			RGBA,				// Image Format
			RGB,				// Image Format
			Jpeg,				// Image Format
			Png					// Image Format
		};

	private:
		Window() {}
	};


	class API_3DF WindowKey : public SegmentKey
	{
	public:
		WindowKey();
		WindowKey(WindowKey const & cInThat);
		~WindowKey();

		static const H3DF::Type staticType = H3DF::Type::WindowKey;
		H3DF::Type ObjectType() const { return staticType; };

		WindowKey const & operator = (WindowKey const & cInThat);

		const H3DF::BaseView * GetBaseView() const;
		H3DF::BaseView * GetBaseView();

		void Update();
		void ForceUpdate();

		int ViewId();
		const int ViewId() const;
		void SetViewId(int nViewId);

		WindowKey & SetSelectionOptions(SelectionOptionsKit const & cInKit);
		bool ShowSelectionOptions(SelectionOptionsKit & cOutKit) const;

		// Returns a control that allows the user to manipulate and query details of the selection options on this window.
		SelectionOptionsControl & GetSelectionOptionsControl();
		SelectionOptionsControl const & GetSelectionOptionsControl() const;

		// Returns a control that allows the user to perform selections in this window.
		SelectionControl & GetSelectionControl();
		SelectionControl const & GetSelectionControl() const;

		// Returns a control that allows the user to highlight segments or geometry in this window.
		HighlightControl & GetHighlightControl();
		HighlightControl const & GetHighlightControl() const;

		NavigationCube & GetNavigationCube();
	};
}