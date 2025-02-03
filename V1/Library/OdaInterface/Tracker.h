#pragma once

#include "RxObjectImpl.h"
#include "Ed/EdUserIO.h"
#include "Gi/GiDrawableImpl.h"
#include "Gi/GiViewportDraw.h"
#include "Gi/GiWorldDraw.h"
#include "StaticRxObject.h"

#include "OSnapManager.h"

class Renderer;

//--------------------------------------------------------------------------------------------------

namespace Tracker
{
	class Base
		: public OdEdPointTracker
		, public OdGiDrawableImpl<>
	{
	protected:

		ODRX_USING_HEAP_OPERATORS(OdEdPointTracker);

		Base() {}

		bool m_bValid = false;
		OdGsView* m_pView = nullptr;
		OdGsModelPtr m_pModel;

		mutable OdGePoint3dArray m_points;
		UINT m_nPointMax = 0;
		UINT m_nPointIndex = 0;
		OdGePoint3d m_lastPoint;
		const OdGePoint3d* m_pLastPoint = nullptr;
		OSnapManagerStatic m_osnapManager;

	public: // OdEdInputTracker

		int addDrawables(OdGsView* pView) override;

		void removeDrawables(OdGsView* pView) override;

	public: // OdEdPointTracker

		// WARNING - ignore, replace to SetValue()
		void setValue(const OdGePoint3d& point) override {}

	protected: // OdGiDrawable

		OdUInt32 subSetAttributes(OdGiDrawableTraits* pDt) const override;

		void subViewportDraw(OdGiViewportDraw* pVd) const override;

		bool subWorldDraw(OdGiWorldDraw* pWd) const override;

	public:

		virtual void Initialize(const OdGePoint3d& base, int pointMax = 0);

		virtual void SetValue(UINT index, const OdGePoint3d& point, bool lastPoint = false);

	public:

		UINT AddValue(const OdGePoint3d& point);

		void ClearValues();

		UINT GetPointIndex();

		OdGePoint3d GetValue(UINT index);

		OdGePoint3dArray& GetValues();

		bool IsValid();

		void IsValid(bool value);

		bool Snap(OdGePoint3d& point);

	protected:

		OdGsModel* GetGsModel() const;

		bool HasGsModel() const;

		void InitializeGsModel(OdGsView* pView, bool force = false);

		void InvalidateGsModel();

		void ResetGsModel();

		void SetGsModel(OdGsModel* pModel);

		static OdDbStub* GetVisualStyleOverride(OdDbBaseDatabase* pDb);
	};

	typedef OdSmartPtr<Base> BasePtr;

//--------------------------------------------------------------------------------------------------

	class Line : public Base
	{
	public:

		static OdEdInputTrackerPtr Create();

		static OdEdInputTrackerPtr Create(const OdGePoint3d& base, Renderer* pRenderer = nullptr);

	private: // OdGiDrawable

		void subViewportDraw(OdGiViewportDraw* pVd) const override;

	protected:

		ODRX_USING_HEAP_OPERATORS(Base);
	};

//--------------------------------------------------------------------------------------------------

	class Polyline : public Base
	{
	public:

		static OdEdInputTrackerPtr Create();

		static OdEdInputTrackerPtr Create(const OdGePoint3d& base, Renderer* pRenderer = nullptr);

	private: // OdGiDrawable

		void subViewportDraw(OdGiViewportDraw* pVd) const override;

	protected:

		ODRX_USING_HEAP_OPERATORS(Base);
	};

//--------------------------------------------------------------------------------------------------

	class RubberBand : public Base
	{
	public:

		static OdEdInputTrackerPtr Create(const OdGePoint3d& base, Renderer* pRenderer = nullptr);

	private: // OdGiDrawable

		void subViewportDraw(OdGiViewportDraw* pVd) const override;

	protected:

		ODRX_USING_HEAP_OPERATORS(Base);
	};

//--------------------------------------------------------------------------------------------------

	class RectFrame : public Base
	{
	public:

		static OdEdInputTrackerPtr Create(const OdGePoint3d& base, Renderer* pRenderer = nullptr);

	private: // OdGiDrawable

		OdUInt32 subSetAttributes(OdGiDrawableTraits* pDt) const override;

		void subViewportDraw(OdGiViewportDraw* pVd) const override;

	protected:

		ODRX_USING_HEAP_OPERATORS(Base);
	};

//--------------------------------------------------------------------------------------------------

	class Combo : public Base
	{
	public:

		OdEdInputTrackerPtr First;
		OdEdInputTrackerPtr Second;

		static OdEdInputTrackerPtr Create(const OdGePoint3d& base, Renderer* pRenderer = nullptr);

	public: // OdEdInputTracker

		int addDrawables(OdGsView* pView) override;

		void removeDrawables(OdGsView* pView) override;

	public:

		void SetValue(UINT index, const OdGePoint3d& point, bool lastPoint = false) override;
	};
}

