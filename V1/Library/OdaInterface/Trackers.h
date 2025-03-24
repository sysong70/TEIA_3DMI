#pragma once

#include "DbPolyline.h"
#include "DbText.h"
#include "Gi/GiDrawable.h"
#include "Gi/GiDrawableImpl.h"
#include "Gi/GiViewportDraw.h"
#include "Gs/Gs.h"

#include <vector>

class Renderer;
class UserIO;

//--------------------------------------------------------------------------------------------------

class TrackerParams
{
public:

	UserIO* UserIoPtr = nullptr;
	Renderer* RendererPtr = nullptr;
	OdDbDatabase* DatabasePtr = nullptr;

	void Set(UserIO* pIo);
};

//--------------------------------------------------------------------------------------------------

class TrackerBase
{
public:

	TrackerParams Params;
	bool Valid = false;
	// WARNING - prevent SetGsView, UnsetGsView on UserIO::LockProcess()
	bool Protect = false;

public:

	static OdGePoint2d PixelDensity;

	static void SetPixelDensity(OdGsView* pView);

	static double GetPixelSize(int pixel);

	static bool HasDynamicInput() { return false; }

	enum class EType
	{
		General,
		RubberBand,
		RubberRect,
	};

public:

	virtual EType GetType() { return EType::General; }
	// addDrawable and
	virtual void Initialize(OdGsView* pView);
	// removeDrawable and
	virtual void Terminate(OdGsView* pView);

	virtual void Invalidate() {}

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

	OdGsView* GsViewPtr = nullptr;
	OdGsModel* GsModelPtr = nullptr;

public: // WARNING - Skip OdRxObject memory management

	void addRef() override {}

	void release() override {}

protected: // OdGiDrawable

	OdUInt32 subSetAttributes(OdGiDrawableTraits* pDt) const override;

	bool subWorldDraw(OdGiWorldDraw* pWd) const override;

public:

	// addDrawable and
	void Initialize(OdGsView* pView) override;
	// removeDrawable and
	void Terminate(OdGsView* pView) override;

	void Invalidate() override;
};

//--------------------------------------------------------------------------------------------------

class RubberBandTracker : public GraphTracker
{
public:

	OdGePoint3d StartPoint;
	OdGePoint3d EndPoint;

	struct
	{
		// [0] StartPoint, [2] Length point, [4] EndPoint
		OdGePoint3d LinePoints[5];
		// [0] Center, [2] Ange point
		OdGePoint3d ArcPoints[4];
		// Or Radius
		double Length = 0;
		double Angle = 0;
	}
	Result;

protected: // OdGiDrawable

	void subViewportDraw(OdGiViewportDraw* pVd) const override;

public:

	bool BandLine = false;
	bool LengthGuide = false;
	bool AngleGuide = false;

	EType GetType() override { return EType::RubberBand; }

	void Initialize(OdGsView* pView) override;

	void Terminate(OdGsView* pView) override;

	void SetBasePoint(const OdGePoint3d& value);

	void SetValue(const OdGePoint3d& value) override;	

protected:

	OdDbTextPtr LengthText;
	OdDbTextPtr AngleText;

	void DrawLengthText(OdGiViewportDraw* pVd);

	void DrawAngleText(OdGiViewportDraw* pVd);
};

//--------------------------------------------------------------------------------------------------

class RubberRectTracker : public RubberBandTracker
{
protected:

	void subViewportDraw(OdGiViewportDraw* pVd) const override;

public:

	RubberRectTracker();

	EType GetType() override { return EType::RubberRect; }
};

//--------------------------------------------------------------------------------------------------

class TrackerStack
{
public:

	bool UseDynamicInput = false;
	std::vector<TrackerBase*> Buffer;

public:

	TrackerStack() {}

	void Initialize(OdGsView* pView);

	void Terminate(OdGsView* pView);

	void Invalidate(Renderer* pRenderer);

public:

	void SetValue(double value);

	void SetValue(int value);

	void SetValue(const OdGePoint3d& value);

	void SetValue(CString value);

public:

	void Push(TrackerBase* pTracker);

	void Pop(TrackerBase* pTracker);

	RubberBandTracker* GetRubberBand();
};

