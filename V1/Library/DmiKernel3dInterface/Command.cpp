#include "StdAfx.h"

#include "Command.h"

#include <3DF/3DF.h>
#include <3DF/Point.h>
#include <3DF/Window.h>

using namespace KERNEL;


Command::Result::Type KERNEL::Command::Result::Convert(H3DF::Operator::Result eInType)
{
	switch (eInType)
	{
	case H3DF::Operator::Result::Pass:
		return Command::Result::Type::Pass;

	case H3DF::Operator::Result::Consume:
		return Command::Result::Type::Consume;

	case H3DF::Operator::Result::Complete:
		return Result::Type::Complete;
	}

	return Result::Type::None;
}
