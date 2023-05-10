#pragma once

#include "3DF.h"

#include <HBaseView.h>
#include <HUtility.h>

OPEN_3DF_NAMESPACE

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
};

CLOSE_3DF_NAMESPACE