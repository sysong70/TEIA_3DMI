#pragma once

#include "Gi/GiDrawable.h"
#include "Gi/GiDrawableImpl.h"
#include "Gs/Gs.h"

//--------------------------------------------------------------------------------------------------

class TrackerBase
{
public:

	// addDrawable
	virtual void SetGsView(OdGsView* pView) = 0;
	// removeDrawable
	virtual void UnsetGsView(OdGsView* pView) = 0;
	// call UnsetGsView and clear
	virtual void Clear(OdGsView* pView) {}

public:

	virtual void SetValue(double value) {}

	virtual void SetValue(int value) {}

	virtual void SetValue(const OdGePoint3d& value) {}

	virtual void SetValue(CString value) {}
};

//--------------------------------------------------------------------------------------------------

class TrackerStack : TrackerBase
{
	std::vector<TrackerBase*> m_buffer;

public:

	TrackerStack() {}

	void SetGsView(OdGsView* pView) override;

	void UnsetGsView(OdGsView* pView) override;

	void Clear(OdGsView* pView) override;

public:

	void SetValue(double value) override;

	void SetValue(int value) override;

	void SetValue(const OdGePoint3d& value) override;

	void SetValue(CString value) override;

public:

	void Push(TrackerBase* pTracker);

	void Pop(TrackerBase* pTracker);
};

//--------------------------------------------------------------------------------------------------

class GeometryTrackerBase : public TrackerBase
{
};

//--------------------------------------------------------------------------------------------------

class GraphTrackerBase
	: public TrackerBase
	, public OdGiDrawableImpl<>
{
};

