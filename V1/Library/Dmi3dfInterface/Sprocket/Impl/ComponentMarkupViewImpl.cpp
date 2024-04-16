#include "StdAfx.h"

#include "ComponentMarkupViewImpl.h"


#include "../../3DF/Segment.h"
#include "../../3DF/Reference.h"
#include "../../3DF/3DF.Utility.h"
#include "../../3DF/KeyPath.h"
#include "../../3DF/Selection.h"
#include "../../3DF/Impl/SelectionImpl.h"

#include <Common_Define.h>

#include <ranges>

using namespace H3DF;

H3DF::ComponentMarkupViewImpl::ComponentMarkupViewImpl()
{
}

H3DF::ComponentMarkupViewImpl::~ComponentMarkupViewImpl()
{
}

void H3DF::ComponentMarkupViewImpl::Copy(ComponentMarkupViewImpl * pcInThat)
{
	ComponentImpl::Copy(pcInThat);
}