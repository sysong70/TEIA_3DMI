#pragma once

#include "3DF.h"

#include "3DF.Kit.h"
#include "3DF.Control.h"

OPEN_3DF_NAMESPACE

class VisualEffectsControl : public Control
{
public:
	VisualEffectsControl(SegmentKey & cInSegmentKey);
	VisualEffectsControl(VisualEffectsControl const & cInThat);

	void Set(VisualEffectsControl const & cInThat);
	VisualEffectsControl & operator = (VisualEffectsControl const & cInThat);

	TDF::Type ObjectType() const { return TDF::Type::VisualEffectsControl; };

	VisualEffectsControl & SetAntiAliasing(bool bInState);
	VisualEffectsControl & SetTextAntiAliasing(bool bInState);
	VisualEffectsControl & SetLineAntiAliasing(bool bInState);

	VisualEffectsControl & UnsetAntiAliasing();

	bool ShowAntiAliasing(bool & bOutState) const;
	bool ShowTextAntiAliasing(bool & bOutState) const;
	bool ShowLineAntiAliasing(bool & bOutState) const;
};

CLOSE_3DF_NAMESPACE

