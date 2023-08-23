#pragma once

#include "3DF.h"

#include <HBaseView.h>
#include <HUtility.h>

OPEN_3DF_NAMESPACE

class NavigationCube;

enum class ViewMode
{
	top,
	bottom,
	front,
	back,
	left,
	right,

	py_nz,
	py_pz,
	ny_pz,
	ny_nz,

	nx_nz,
	nx_pz,
	px_pz,
	px_nz,

	nx_py,
	px_py,
	px_ny,
	nx_ny,

	nx_py_nz,
	nx_py_pz,
	nx_ny_pz,
	nx_ny_nz,

	px_py_pz, // ISO
	px_py_nz,
	px_ny_nz,
	px_ny_pz,

	Count,
	Unknown
};

class BaseView : public HBaseView
{
public:
	BaseView(HBaseModel * model,
		const char * alias = 0,
		const char * driver_type = 0,
		const char * instance_name = 0,
		void * window_handle = 0,
		void * colormap = 0,
		void * clip_override = 0,
		void * window_handle_2 = 0,
		const char * driver_path = 0);

	void UpdateInternal(bool antialias = false, bool force_update = false) override;

	void SetViewMode(H3DF::ViewMode mode, bool bFitWorld = true);

public:
	void SetNavigationCube(NavigationCube * pcNaviCube);

	

private:
	NavigationCube * m_pcNaviCube = nullptr;

	H3DF::ViewMode m_eViewMode = H3DF::ViewMode::Unknown;
};

CLOSE_3DF_NAMESPACE