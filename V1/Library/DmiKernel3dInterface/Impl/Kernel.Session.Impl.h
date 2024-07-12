#pragma once

#include <Json.h>

#include "../Kernel.h"
#include "../Object.h"

#include "../Command.h"
#include "../Command.Step.h"
#include "../Command.HighlightObjectSnap.h"
#include "../Command.Camera.h"
#include "../Command.Select.h"
#include "../Command.ModelPanel.h"
#include "../Command.Attribute.h"

#include <Sprocket/3DF.Factory.h>
#include <Sprocket/3DF.Canvas.h>
#include <Sprocket/3DF.Model.h>
#include <Sprocket/3DF.View.h>
#include <3DF/Selection.h>
#include <3DF/Highlight.h>

#include <Sprocket/3DF.CADModel.h>

#include <unordered_map>

namespace KERNEL
{
	class SessionImpl : public Impl
	{
	public:
		SessionImpl();
		~SessionImpl();

		void Copy(const SessionImpl * pcInThat)
		{
			m_pcCanvas = pcInThat->m_pcCanvas;
			m_nViewId = pcInThat->m_nViewId;
			m_pcDelivery = pcInThat->m_pcDelivery;
			m_strFilePathName = pcInThat->m_strFilePathName;
		}

		int m_nViewId = -1;

		H3DF::Canvas & GetCanvas() { return *m_pcCanvas; }
		H3DF::Canvas * m_pcCanvas = nullptr;

		H3DF::WindowKey & Window();
		const H3DF::WindowKey & Window() const;

		H3DF::Model & GetModel();
		H3DF::CADModel & CADModel();
	
		H3DF::BaseView * GetBaseView();

		Signal::Delivery & Delivery();
		const Signal::Delivery & Delivery() const;
		void SetDelivery(const Signal::Delivery * pcInDelivery);

		void CancelCommands();

		//== Operator 관련 함수 ======================================================================
		void AllocationOperator(const Session * pcInSession);
		Command::Set * GetOperator(Command::Type eInType);

		Command::Attribute & Attribute();
		Command::Camera & Camera();
		Command::Select & Select();
		Command::ModelPanel & ModelPanel();

		//== Mouse 관련 함수 =========================================================================
		bool MouseMove(int nFlag, int x, int y);
		bool LButtonDown(int nFlag, int x, int y);
		bool LButtonUp(int nFlag, int x, int y);

		DWORD MouseMapFlags(DWORD nState);

		//== Visibility 관련 함수 ====================================================================
		void SetVisibility(int nId);

		//== Command 관련 함수 =======================================================================
		void CommandRequest(Json::Object & cInObject);
		void CommandChange(Json::Object & cInObject);

		void SetCommand(int nInCommandId);

	protected:
		bool IsCommandActive();
		bool CommandLButtonUp(int nFlag, int x, int y);

	protected:
		//== View Control 관련 함수 ==================================================================
		bool SelectViewControlMouseMove(int nFlag, int x, int y);
		bool SelectViewControlLButtonDown(int nFlag, int x, int y);
		Command::Step::InputType SelectViewControlLButtonUp(int nFlag, int x, int y);

		void RequestVisualEffects(Json::Object & cInObject);
		void ChangeVisualEffects(Json::Object & cInObject);

	private:
		// 화면 제어, Camera, Select, ModelPanel, Attribute등의 처리를 담당. 1회성 이벤트 처리
		std::unordered_map<Command::Type, Command::Set *> m_mpcCommandMap; 
		const Signal::Delivery * m_pcDelivery = nullptr;

		H3DF::CADModel m_cCADModel;
		
	public:
		//KERNEL::Command::InputManager m_cInputManager;
		std::vector<Command::Set *> m_vpcCommandSets;

		// 현재 선택된 요소들이 저장되는 변수
		H3DF::SelectionResults m_cSelectionResult;

		H3DF::Point2D m_cLButtonDownPosition;

		DWORD m_nMouseWhellStartTick;

		CString m_strFilePathName;
	};
}