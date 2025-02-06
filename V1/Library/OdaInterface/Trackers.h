#pragma once

#include "Gi/GiDrawable.h"
#include "Gi/GiDrawableImpl.h"
#include "Gi/GiViewportDraw.h"
#include "Gs/Gs.h"

//--------------------------------------------------------------------------------------------------

class TrackerBase
{
public:

	// WARNING - prevent SetGsView, UnsetGsView on UserIO::LockProcess()
	bool Protect = false;

public:

	// addDrawable and
	virtual void Initialize(OdGsView* pView) {}
	// removeDrawable and
	virtual void Terminate(OdGsView* pView) {}

	virtual void SetValue(double value) {}

	virtual void SetValue(int value) {}

	virtual void SetValue(const OdGePoint3d& value) {}

	virtual void SetValue(CString value) {}
};

//--------------------------------------------------------------------------------------------------

class GraphTracker
	: public TrackerBase
	, public OdGiDrawableImpl<>
{
protected:

	OdGsModel* m_pGsModel = nullptr;

public: // WARNING - Skip OdRxObject memory management

	void addRef() override {}

	void release() override {}

protected: // OdGiDrawable

	OdUInt32 subSetAttributes(OdGiDrawableTraits* pDt) const override;

	bool subWorldDraw(OdGiWorldDraw* pWd) const override;

protected:

	// Call by SetValue()
	void Invalidate();

public:

	// addDrawable and
	void Initialize(OdGsView* pView) override;
	// removeDrawable and
	virtual void Terminate(OdGsView* pView) override;
};

//--------------------------------------------------------------------------------------------------

class RubberBand : public GraphTracker
{
protected:

	OdGePoint3d m_start;
	OdGePoint3d m_end;

protected: // OdGiDrawable

	void subViewportDraw(OdGiViewportDraw* pVd) const override;

public:

	void SetBasePoint(const OdGePoint3d& value);

	void SetValue(const OdGePoint3d& value) override;
};

//--------------------------------------------------------------------------------------------------

class RubberRect : public RubberBand
{
protected:

	void subViewportDraw(OdGiViewportDraw* pVd) const override;
};

//--------------------------------------------------------------------------------------------------

class TrackerStack
{
	std::vector<TrackerBase*> m_buffer;

public:

	TrackerStack() {}

	void Initialize(OdGsView* pView);

	void Clear(OdGsView* pView);

	void SetValue(double value);

	void SetValue(int value);

	void SetValue(const OdGePoint3d& value);

	void SetValue(CString value);

public:

	void Push(TrackerBase* pTracker);

	void Pop(TrackerBase* pTracker);
};

