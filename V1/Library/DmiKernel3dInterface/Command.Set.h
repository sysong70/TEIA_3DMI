#pragma once

#include "Command.h"

namespace KERNEL
{
	namespace Command
	{
		class Step;

		class SetIterator : public Object
		{
		public:
			SetIterator();
			SetIterator(SetIterator const & cInThat);

			void Set(SetIterator const & in_that);
			SetIterator & operator = (SetIterator const & cInThat);

			void Next();

			SetIterator & operator ++ ();
			SetIterator & operator ++ (int nInVal);

			bool operator == (SetIterator const & cInSearchResultsIterator);
			bool operator != (SetIterator const & cInSearchResultsIterator);

			bool IsValid() const;

			void Reset();

			Step * GetStep() const;

			Step * operator * () const;
		};

		class Set : public Object
		{
		public:
			Set(const Session * pcInSession);
			~Set();

			void Reset();
			void Reset() const;

			size_t GetCount() const;
			SetIterator GetIterator() const;

			Step * Front();
			Step * Front() const;

			void PushFront(Step * pcInStep);
			void PushBack(Step * pcInStep);

			virtual KERNEL::Command::Type GetType() const;
		};
	}
}