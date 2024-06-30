#pragma once

#include "../Kernel.h"

#include <vector>
#include <deque>

#include <3DF/Math.h>

#include "../Signal/Signal.h"
#include "../Common/Common_Define.h"

namespace KERNEL
{
	namespace Command
	{
		class Step;

		class SetImpl : public Impl
		{
		public:
			SetImpl(const Session * pcInSession);

			void Copy(SetImpl * pcInThat);

			H3DF::WindowKey & Window();
			const H3DF::WindowKey & Window() const;

			H3DF::View & View();
			const H3DF::View & View() const;

			Signal::Delivery & Delivery();
			const Signal::Delivery & Delivery() const;

			Session & GetSession();
			const Session & GetSession() const;

			H3DF::Model & GetModel();
			const H3DF::Model & GetModel() const;

			// Update하기전에 Hightlight된 것들을 모두 Unhighlight하고, SnapItem을 모두 Reset한다.
			void PrepareUpdate();
			// Update가 완료되면, View를 Update한다.
			void Updated();

		private:
			const Session * m_pcSession = nullptr;

		public:
			// 명령어 단계를 저장하는 queue
			std::deque<Step *> m_deStep;

			std::vector<CString> m_vstrTexts;
			std::vector<H3DF::Point> m_vcPoints;
			std::vector<float> m_vfValues;

			H3DF::Point2D m_cPoint;
		};

		class SetIteratorImpl : public Impl
		{
		public:
			void Copy(SetIteratorImpl * pcInThat) {
				pcIterator = pcInThat->pcIterator;
				pcBeginIterator = pcInThat->pcBeginIterator;
				pcEndIterator = pcInThat->pcEndIterator;
			}

			std::deque<Step *>::iterator pcIterator;
			std::deque<Step *>::iterator pcBeginIterator;
			std::deque<Step *>::iterator pcEndIterator;
		};
	}
}