#include "Scope.h"
#include "Datum.h"
#include "Exception.h"

namespace os
{
	Scope::Scope()
		: m_lookupTable(DefaultCapacity, true), m_indexer(DefaultCapacity),
		m_parent(nullptr)
	{

	}

	Scope::Scope(uint32 capacity)
		: m_lookupTable(capacity, true), m_indexer(capacity),
		m_parent(nullptr)
	{

	}

	Scope::Scope(const Scope& scope)
		: m_lookupTable(scope.m_lookupTable.TableCapacity(), true)
	{
		this->operator=(scope);
	}

	Scope::~Scope()
	{
		Clear();

		if (m_parent)
		{
			m_parent->Orphan(this);
		}
	}

	void Scope::Orphan(Scope* ch)
	{
		assert(ch);

		// shallow orphan
		bool found = false;

		for (StorageList::Iterator iter = m_storage.Begin(); iter != m_storage.End() && !found; ++iter)
		{
			Datum& dat = (*iter).second;
			if (dat.GetType() == Datum::DAT_Table)
			{
				uint32 actuallScopeCount = GetActuallDatumScopeCount(dat);

				bool onlyElementEquals = false;
				if (actuallScopeCount==1)
				{
					for (uint32 i=0;i<dat.Size();i++)
					{
						if (dat.Get<Scope*>(i) == ch)
						{
							onlyElementEquals = true;
						}
					}
				}

				if (dat == ch || onlyElementEquals)
				{
					found = true;
					
					// in this case remove it
					bool r = m_indexer.Remove(iter);
					assert(r);

					m_lookupTable.Remove((*iter).first);
					r = m_storage.Remove(*iter);
					assert(r);
					break;
				}
				else
				{
					for (uint32 i=0;i<dat.Size();i++)
					{
						if (dat.Get<Scope*>(i) == ch)
						{
							dat.Set((Scope*)nullptr, i);
							found = true;
							break;
						}
					}
				}
			}
		}
		assert(found);
		
		m_parent = nullptr;
	}


	Scope& Scope::operator=(const Scope& rhs)
	{
		if (&rhs == this)
			return *this;

		Clear();

		m_parent = rhs.m_parent;
		m_storage = rhs.m_storage;
		for (StorageList::Iterator iter = m_storage.Begin(); iter != m_storage.End(); ++iter)
		{
			m_indexer.PushBack(iter);

			// deal with sub scope copying
			Datum& dat = (*iter).second;
			if (dat.GetType() != Datum::DAT_Table || (dat.GetType() == Datum::DAT_Table && dat.IsExternal()))
			{
				m_lookupTable.Insert(LookupTable::PairType((*iter).first, &dat));
			}
			else
			{
				for (uint32 i=0;i<dat.Size(); i++)
				{
					Scope* s = dat.Get<Scope*>(i);
					if (s)
					{
						Scope* newScope = new Scope(*s);
						newScope->m_parent = this;
						dat.Set(newScope, i);
					}
					else 
						dat.Set((Scope*)nullptr, i);
				}
				m_lookupTable.Insert(LookupTable::PairType((*iter).first, &dat));
			}
		}

		return *this;
	}

	void Scope::Clear()
	{
		Vector<Scope*> deadList;
		for (StorageList::Iterator iter = m_storage.Begin(); iter != m_storage.End(); ++iter)
		{
			Datum& dat = (*iter).second;
			if (dat.GetType() == Datum::DAT_Table && !dat.IsExternal())
			{
				for (uint32 i=0;i<dat.Size(); i++)
				{
					Scope* s = dat.Get<Scope*>(i);
					if (s)
					{
						deadList.PushBack(s);
						//delete s;
					}
				}
			}
		}

		for (uint32 i=0;i<deadList.Size();i++)
		{
			delete deadList[i];
		}

		m_lookupTable.Clear();
		m_storage.Clear();
		m_indexer.Clear();
	}

	Datum* Scope::Find(const String& name)
	{
		LookupTable::Iterator iter = m_lookupTable.Find(name);
		if (iter != m_lookupTable.End())
		{
			return iter->second;
		}
		return nullptr;
	}

	Datum* Scope::Search(const String& name, Scope** foundContainer)
	{
		Datum* dat = Find(name);

		if (dat)
		{
			if (foundContainer)
				*foundContainer = this;
			return dat;
		}
		
		if (m_parent)
		{
			return m_parent->Search(name, foundContainer);
		}
		return nullptr;
	}

	Datum& Scope::Append(const String& name)
	{
		Datum* dat = Find(name);

		if (dat)
			return *dat;

		return AddDatum(name);
	}

	Scope& Scope::AppendScope(const String& name, uint32 datumIndex)
	{
		Datum* dat = Find(name);

		if (dat)
		{
			if (dat->GetType() != Datum::DAT_Table)
			{
				throw Exception(L"Type mismatch");
			}

			if (dat->Get<Scope*>(datumIndex))
			{
				return *(dat->Get<Scope*>(datumIndex));
			}
		}
		else
		{
			dat = &AddDatum(name);
		}
		

		uint32 oldSize = dat->Size();
		Scope* newScope = new Scope();
		newScope->m_parent = this;

		dat->Set(newScope, datumIndex);

		// set clean pointers for Datum's auto increase
		for (uint32 i=oldSize;i<dat->Size()-1;i++)
		{
			dat->Set((Scope*)nullptr, i);
		}

		return *newScope;
	}

	void Scope::Adopt(Scope* child, const String& name, uint32 datumIndex)
	{
		Datum& dat = Append(name);

		if (dat.GetType() != Datum::DAT_Unknown && dat.GetType() != Datum::DAT_Table)
		{
			throw Exception(L"Type mismatch");
		}

		if (dat.GetType() == Datum::DAT_Unknown && datumIndex < dat.Size() && dat.Get<Scope*>(datumIndex))
		{
			throw Exception(L"Datum is already in use");
		}

		if (child->m_parent)
			child->m_parent->Orphan(child);

		child->m_parent = this;

		uint32 oldSize = dat.Size();

		dat.Set(child, datumIndex);

		// set clean pointers for Datum's auto increase
		for (uint32 i=oldSize;i<dat.Size()-1;i++)
		{
			dat.Set((Scope*)nullptr, i);
		}
	}

	bool Scope::FindName(const Scope* child, String& name) const
	{
		assert(child);

		for (StorageList::Iterator iter = m_storage.Begin(); iter != m_storage.End(); ++iter)
		{
			Datum& d = (*iter).second;

			if (d.GetType() == Datum::DAT_Table)
			{
				for (uint32 i=0;i<d.Size(); i++)
				{
					Scope* ch = d.Get<Scope*>(i);

					if (ch == child)
					{
						name = (*iter).first;
						return true;
					}
				}
			}
		}
		return false;
	}

	Datum& Scope::AddDatum(const String& name)
	{
		StorageList::Iterator iter = m_storage.PushBack_Iter(std::make_pair(name, Datum()));
		m_indexer.PushBack(iter);
		m_lookupTable.Insert(LookupTable::PairType(name, &(*iter).second));

		return (*iter).second;
	}
	
	bool Scope::CompareScopeDatum(Datum& lhs, Datum& rhs)
	{
		if (lhs.IsExternal() && rhs.IsExternal())
		{
			return lhs == rhs;
		}

		uint32 actuallCountL = GetActuallDatumScopeCount(lhs);
		uint32 actuallCountR = GetActuallDatumScopeCount(rhs);

		if (actuallCountR == actuallCountL)
		{
			uint32 j = 0;

			// still order dependent in side datum
			for (uint32 i=0;i<lhs.Size();i++)
			{
				Scope* lscope = lhs.Get<Scope*>(i);
				if (lscope)
				{
					for (;j<rhs.Size();j++)
					{
						Scope* rscope = rhs.Get<Scope*>(j);
						if (rscope)
						{
							if (*lscope != *rscope)
								return false;
						}
					}
				}
			}
			return true;
		}
		return false;
	}
	uint32 Scope::GetActuallDatumScopeCount(Datum& d)
	{
		uint32 actuallScopeCount = 0;
		for (uint32 i=0;i<d.Size();i++)
		{
			if (d.Get<Scope*>(i))
			{
				actuallScopeCount++;
			}
		}
		return actuallScopeCount;
	}

	void Scope::FillNames(Vector<String>& nameList)
	{
		for (LookupTable::Iterator iter = m_lookupTable.Begin(); iter != m_lookupTable.End(); ++iter)
		{
			nameList.PushBack(iter->first);
		}
	}

	Datum& Scope::operator [](uint32 index) 
	{
		return (*m_indexer[index]).second; 
	}

	bool Scope::operator==(Scope& rhs) const
	{
		if (&rhs == this)
			return true;

		if (m_indexer.Size() == rhs.m_indexer.Size() && m_lookupTable.Size() == rhs.m_lookupTable.Size() &&
			m_storage.Size() == rhs.m_storage.Size())
		{
			for (StorageList::Iterator iter = m_storage.Begin(); iter != m_storage.End(); ++iter)
			{
				const String& name = (*iter).first;

				Datum* thisDat = &(*iter).second;
				Datum* rhsDat = rhs.Find(name);
				if (!rhsDat)
				{
					return false;
				}

				if (rhsDat->GetType() == Datum::DAT_Table && thisDat->GetType() == Datum::DAT_Table)
				{
					if (!CompareScopeDatum(*thisDat, *rhsDat))
					{
						return false;
					}
				}
				else
				{
					if (*rhsDat != *thisDat)
					{
						return false;
					}
				}
				
			}
			return true;
		}
		return false;
	}

}