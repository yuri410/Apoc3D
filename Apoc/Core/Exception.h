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
		protected:
			String message;
		public:
			Exception()
			{
			}
			Exception(String message)
			{
				this->message = message;
			}
		};

		class ArgumentException : public Exception
		{

		}

		class ArgumentOutOfRangeException : public ArgumentException
		{
		private:
		public:
			ArgumentOutOfRangeException(String argument)
			{
				
			}
		}
	}
}
#endif