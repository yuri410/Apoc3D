#include "Datum.h"

#include "apoc3d/Math/Matrix.h"
#include "StringUtils.h"

namespace os
{
	Datum::Datum()
		: m_isExternal(false), m_type(DAT_Unknown), m_size(0), m_capacity(0)
	{
		m_array.universal = nullptr;
	}

	Datum::Datum(const Datum& other)
		: m_isExternal(false), m_type(DAT_Unknown), m_size(0), m_capacity(0)
	{
		m_array.universal = nullptr;
		this->operator=(other);
	}

	Datum::~Datum()
	{
		if (!m_isExternal && m_type != DAT_Unknown)
		{
			SetSize(0);
		}
	}

#define COPYFROM(enumType, type, newSize) case enumType: \
	Resize<type>(newSize); \
	CopyArray(reinterpret_cast<type*>(rhs.m_array.universal), reinterpret_cast<type*>(m_array.universal), m_size); \
	break;

#define RESIZETYPE(enumType, type, newSize) case enumType: \
	Resize<type>(newSize); \
	break;

#define COMPARETYPE(enumType, type) case enumType: \
	return CompareArray(reinterpret_cast<type*>(rhs.m_array.universal), reinterpret_cast<type*>(m_array.universal), m_size); \

	Datum& Datum::operator=(const Datum& rhs)
	{
		if (&rhs == this)
			return *this;

		if (!m_isExternal && m_type != DAT_Unknown)
		{
			SetSize(0);
		}
		m_array.universal = nullptr;
		m_size = rhs.m_size;
		m_type = rhs.m_type;
		m_isExternal = rhs.m_isExternal;

		if (m_isExternal)
		{
			m_array.universal = rhs.m_array.universal;
			m_capacity = rhs.m_capacity;
		}
		else
		{
			switch (m_type)
			{
				COPYFROM(DAT_Int32, int32, rhs.m_capacity);
				COPYFROM(DAT_Float, float, rhs.m_capacity);
				COPYFROM(DAT_Matrix, Matrix, rhs.m_capacity);
				COPYFROM(DAT_Pointer, RTTI*, rhs.m_capacity);
				COPYFROM(DAT_String, String, rhs.m_capacity);
				COPYFROM(DAT_Table, Scope*, rhs.m_capacity);
				COPYFROM(DAT_Vector4, Vector4, rhs.m_capacity);
			}
		}
		return *this;
	}

	

	void Datum::Clear()
	{
		m_size = 0;
	}

	void Datum::SetType(Type t)
	{
		if (m_type != DAT_Unknown)
		{
			throw Exception(L"Can not change the type of a Datum if it already have type");
		}
		m_type = t;
	}

	void Datum::SetFromString(const String& text, uint32 index)
	{
		switch (m_type)
		{
		case DAT_Vector4:
			{
				Vector<String> params = StringUtils::Split(text, L" ,");
				assert(params.Size() == 4);
				float vec[4] = { 
					StringUtils::ParseSingle(params[0]), 
					StringUtils::ParseSingle(params[1]), 
					StringUtils::ParseSingle(params[2]),
					StringUtils::ParseSingle(params[3])
				};
				
				Set(Apoc3D::Math::Vector4Utils::LDVectorPtr(vec), index);
				break;
			}
		case DAT_Matrix:
			{
				const int32 elementCount = 16;
				Vector<String> params = StringUtils::Split(text, L" ,", elementCount);
				
				assert(params.Size() == elementCount);
				
				float elem[elementCount];
				for (int32 i=0;i<elementCount;i++)
				{
					elem[i] = StringUtils::ParseSingle(params[i]);
				}

				Set(Matrix(elem), index);
				break;
			}
		case DAT_Float:
			Set(StringUtils::ParseSingle(text), index);
			break;
		case DAT_Int32:
			Set(StringUtils::ParseInt32(text), index);
			break;
		case DAT_String:
			{
				String stringContent = text;
				if (stringContent.size() > 2 && 
					StringUtils::StartsWidth(stringContent, L"\"") && 
					StringUtils::EndsWidth(stringContent, L"\""))
				{
					stringContent = stringContent.substr(1, stringContent.size()-2);
				}
				Set(stringContent, index);
				break;
			}
		default:
			throw Exception(L"Not supported");
		}
	}
	String Datum::ToString(uint32 index) const
	{
		switch (m_type)
		{
		case DAT_Vector4:
			{
				const Vector4& v = Get<Vector4>(index);

				String result;
				result.append(StringUtils::ToString(v4x(v)));
				result.append(StringUtils::ToString(v4y(v)));
				result.append(StringUtils::ToString(v4z(v)));
				result.append(StringUtils::ToString(v4w(v)));

				return result;
			}
		case DAT_Matrix:
			{
				const Matrix& m = Get<Matrix>(index);

				const int32 elementCount = sizeof(m.Elements) / sizeof(*m.Elements);
				String result;
				for (int32 i=0;i<elementCount;i++)
				{
					result.append(StringUtils::ToString(m.Elements[i]));

					if ( i != elementCount-1 )
					{
						result.append(L", ");
					}
				}
				return result;
			}
		case DAT_Float:
			return StringUtils::ToString(Get<float>(index));
		case DAT_Int32:
			return StringUtils::ToString(Get<int32>(index));
		case DAT_String:
			{
				String text = L"\"" + Get<String>(index) + L"\"";
				return text;
			}
		default:
			throw Exception(L"Not supported");
		}
	}

	void Datum::SetSize(uint32 newSize)
	{
		if (m_isExternal || m_type == DAT_Unknown)
		{
			throw Exception(L"Invalid Operation");
		}
		if (newSize > m_size || newSize == 0)
		{
			switch (m_type)
			{
				RESIZETYPE(DAT_Int32, int32, newSize);
				RESIZETYPE(DAT_Float, float, newSize);
				RESIZETYPE(DAT_Matrix, Matrix, newSize);
				RESIZETYPE(DAT_Pointer, RTTI*, newSize);
				RESIZETYPE(DAT_String, String, newSize);
				RESIZETYPE(DAT_Table, Scope*, newSize);
				RESIZETYPE(DAT_Vector4, Vector4, newSize);
			}
			m_size = newSize;
		}
	}

#define SET_VALUE(dat, enumType, type, index) case enumType: \
	Set(dat.Get<type>(index), index); \
	break;

	void Datum::Set(const Datum& val, uint32 index)
	{
		switch (val.GetType())
		{
		SET_VALUE(val, DAT_Int32, int32, index);
		SET_VALUE(val, DAT_Float, float, index);
		SET_VALUE(val, DAT_Vector4, Vector4, index);
		SET_VALUE(val, DAT_Matrix, Matrix, index);
		SET_VALUE(val, DAT_Table, Scope*, index);
		SET_VALUE(val, DAT_String, String, index);
		SET_VALUE(val, DAT_Pointer, RTTI*, index);
		default:
			throw Exception(L"Not supported");
		}
	}

	void Datum::CheckSetStorage()
	{
		if (!m_isExternal && m_array.universal)
		{
			throw Exception(L"Invalid operation");
		}
	}


	void Datum::SetterEnsureType(Type t)
	{
		if (m_type == DAT_Unknown)
		{
			m_type = t;
		}
		CheckType(t);
	}
	void Datum::CheckType(Type t) const
	{
		if (m_type != t)
		{
			throw Exception(L"Type incompatible");
		}
	}
	void Datum::CheckGetterIndexBounds(uint32 idx) const
	{
		if (idx>=m_size)
		{
			throw Exception(L"Index out of range.");
		}
	}

	template <typename T>
	void Datum::Resize(uint32 size)
	{
		assert(!m_isExternal);
		
		if (size)
		{
			//assert(size>m_capacity);

			T* newArr = new T[size];

			if (m_array.universal)
			{
				T* oldArr = reinterpret_cast<T*>(m_array.universal);

				CopyArray<T>(oldArr, newArr, m_size);

				delete[] oldArr;
			}
			m_array.universal = newArr;
		}
		else if (m_array.universal)
		{
			delete[] reinterpret_cast<T*>(m_array.universal);

			m_array.universal = nullptr;
		}
		m_capacity = size;
	}


	
	bool Datum::operator==(const Datum& rhs) const
	{
		if (m_isExternal && rhs.m_isExternal)
		{
			if (m_size == rhs.m_size && m_type == rhs.m_type)
			{
				return m_array.universal == rhs.m_array.universal;
			}
		}

		if (m_size == rhs.m_size && m_type == rhs.m_type)
		{
			switch (m_type)
			{
				COMPARETYPE(DAT_Int32, int32);
				COMPARETYPE(DAT_Float, float);
				COMPARETYPE(DAT_Matrix, Matrix);
				COMPARETYPE(DAT_Pointer, RTTI*);
				COMPARETYPE(DAT_String, String);
				COMPARETYPE(DAT_Table, Scope*);
			case DAT_Vector4:
				{
					for (uint32 i=0;i<m_size;i++)
					{
						if (v4x(m_array.v[i]) != v4x(rhs.m_array.v[i]) ||
							v4y(m_array.v[i]) != v4y(rhs.m_array.v[i]) ||
							v4z(m_array.v[i]) != v4z(rhs.m_array.v[i]) ||
							v4w(m_array.v[i]) != v4w(rhs.m_array.v[i]))
						{
							return false;
						}
					}
					return true;
				}
			}
			return false;
		}
		return false;
	}


	bool Datum::operator==(const Vector4& rhs) const
	{
		return m_size == 1 && m_type == DAT_Vector4 && 
			v4x(m_array.v[0]) == v4x(rhs) &&
			v4y(m_array.v[0]) == v4y(rhs) &&
			v4z(m_array.v[0]) == v4z(rhs) &&
			v4w(m_array.v[0]) == v4w(rhs);
	}

	template <typename T>
	void Datum::CopyArray(const T* src, T* dst, int32 count)
	{
		for (int32 i=0;i<count;i++)
		{
			dst[i] = src[i];
		}
	}

	template <typename T>
	bool Datum::CompareArray(const T* a, const T* b, int32 count)
	{
		for (int32 i=0;i<count;i++)
		{
			if (a[i] != b[i])
				return false;
		}
		return true;
	}
}