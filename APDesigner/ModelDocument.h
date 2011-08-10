#ifndef MODELDOCUMENT_H
#define MODELDOCUMENT_H

#include "Document.h"

namespace APDesigner
{
	class ModelDocument : public Document
	{
	public:
		ModelDocument(MainWindow* window)
			: Document(window)
		{

		}
		virtual void LoadRes() { }
		virtual void SaveRes() { };
		virtual bool IsReadOnly() { return false; };
	};
}

#endif