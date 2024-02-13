#pragma once

//--------------------------------------------------------------------------------------------------

namespace Dialog
{
	class Base;			// CBCGPDialog
	class Standard;		// Dialog::Base
	// Modal
	class AppOptions;	// Dialog::Standard
	// Modaless
	class DebugTracer;	// Dialog::Standard
	class ObjectSnaps;	// Dialog::Standard
	class ProgressLog;	// Dialog::Standard



	using Controls = std::list<CWnd*>;



	class Instances
	{
	public:

		Instances();

		~Instances();

	public:

		void Add(Base* pValue);

		Base* Get(int id);

		void Remove(int id);

	private:

		std::list<Base*> m_buffer;
	};



	// control frame padding
	CSize FramePadding();
	// for non-frame widnow
	CSize WindowPadding();
}
