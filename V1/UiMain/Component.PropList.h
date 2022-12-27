#pragma once

#include "Component.h"



namespace Component
{
	class PropList : public CBCGPPropList
	{
	public:

		PropList();

		~PropList() override;

		void Initialize(CWnd* pParentWnd);
	};
}
