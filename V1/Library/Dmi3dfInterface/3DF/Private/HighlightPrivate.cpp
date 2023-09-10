#include "StdAfx.h"

#include "../3DF.Highlight.h"

#include "SelectionPrivate.h"

#include "../Window.h"
#include "../BaseView.h"

#include "../Line.h"
#include "../3DF.LineAttribute.h"

#include <vhash.h>
#include <vlist.h>

#include <atlcoll.h>

#include <HBaseOperator.h>
#include <HMarkupManager.h>
#include <HEventManager.h>
#include <HConstantFrameRate.h>

#define		SEGMENT_TYPE		1
#define		ENTITY_TYPE			2
#define		SUBENTITY_TYPE		3
#define		REGION_TYPE			4

USING_3DF_NAMESPACE

