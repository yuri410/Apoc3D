#ifndef EXCEPTION_H
#define EXCEPTION_H

#pragma once

#include "..\String.h"

namespace Apoc
{
	namespace Core
	{
		class Exception
		{
		private:
			String message;
		public:
			Exception()
			{
			}

		};

		class ArgumentException : public Exception
		{

		}

		class ArgumentOutOfRangeException : public ArgumentException
		{

		}
	}
}
#endif