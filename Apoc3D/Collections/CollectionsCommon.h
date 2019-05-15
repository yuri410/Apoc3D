#pragma once

#ifndef APOC3D_COLLECTIONSCOMMON_H
#define APOC3D_COLLECTIONSCOMMON_H

/* -----------------------------------------------------------------------
 * This source file is part of Apoc3D Framework
 * 
 * Copyright (c) 2011-2017 Tao Xin
 * 
 * This content of this file is subject to the terms of the Mozilla Public 
 * License v2.0. If a copy of the MPL was not distributed with this file, 
 * you can obtain one at http://mozilla.org/MPL/2.0/.
 * 
 * This program is distributed in the hope that it will be useful, 
 * WITHOUT WARRANTY OF ANY KIND; either express or implied. See the 
 * Mozilla Public License for more details.
 * 
 * ------------------------------------------------------------------------
 */

#include "apoc3d/ApocCommon.h"

using namespace Apoc3D::Core;


namespace Apoc3D
{
	namespace Collections
	{
		template<typename T, typename = void>
		struct EqualityComparer
		{
			/** Determines whether the specified objects are equal. */
			static bool Equals(const T& x, const T& y) { return x == y; }
			
			/** Returns a hash code for the specified object. */
			static int32 GetHashCode(const T& obj);
		};

		template <typename T>
		struct EqualityComparer<T*>
		{
			static bool Equals(T* const& x, T* const& y) { return x == y; }

			static int32 GetHashCode(T* const& obj)
			{
				if (sizeof(T*) == sizeof(int32))
					return static_cast<int32>(reinterpret_cast<uintptr>(obj));
				
				int64 av = static_cast<int64>(reinterpret_cast<uintptr>(obj));
				return (int32)(av ^ (av >> 32));
			}
		};

		template <typename T>
		struct EqualityComparer <T, typename std::enable_if<std::is_enum<T>::value>::type>
		{
			static bool Equals(T const& x, T const& y) { return x == y; }

			static int32 GetHashCode(T const& obj) { return static_cast<int32>(obj); }
		};

		template <>
		struct EqualityComparer<uint64>
		{
			static bool Equals(const uint64& x, const uint64& y) { return x == y; }
			static int32 GetHashCode(const uint64& obj) { return (int32)(obj ^ (obj >> 32)); }
		};

		template <>
		struct EqualityComparer<uint32>
		{
			static bool Equals(const uint32& x, const uint32& y) { return x == y; }
			static int32 GetHashCode(const uint32& obj) { return obj; }
		};

		template <>
		struct EqualityComparer<int32>
		{
			static bool Equals(const int32& x, const int32& y) { return x == y; }
			static int32 GetHashCode(const int32& obj) { return obj; }
		};

		template <>
		struct EqualityComparer<wchar_t>
		{
			static bool Equals(const wchar_t& x, const wchar_t& y) { return x == y; }
			static int32 GetHashCode(const wchar_t& obj) { return obj; }
		};

		template <>
		struct APAPI EqualityComparer<std::string>
		{
			static bool Equals(const std::string& x, const std::string& y);
			static int32 GetHashCode(const std::string& obj);
		};


		template <>
		struct APAPI EqualityComparer<String>
		{
			static bool Equals(const String& x, const String& y);
			static int32 GetHashCode(const String& obj);
		};

		template <>
		struct APAPI EqualityComparer<Apoc3D::Math::Rectangle>
		{
			static bool Equals(const Apoc3D::Math::Rectangle& x, const Apoc3D::Math::Rectangle& y);
			static int32 GetHashCode(const Apoc3D::Math::Rectangle& obj);
		};

		template <>
		struct APAPI EqualityComparer<Apoc3D::Math::Size>
		{
			static bool Equals(const Apoc3D::Math::Size& x, const Apoc3D::Math::Size& y);
			static int32 GetHashCode(const Apoc3D::Math::Size& obj);
		};
		template <>
		struct APAPI EqualityComparer<Apoc3D::Math::Point>
		{
			static bool Equals(const Apoc3D::Math::Point& x, const Apoc3D::Math::Point& y);
			static int32 GetHashCode(const Apoc3D::Math::Point& obj);
		};

		template <>
		struct APAPI EqualityComparer<Apoc3D::Math::PointF>
		{
			static bool Equals(const Apoc3D::Math::PointF& x, const Apoc3D::Math::PointF& y);
			static int32 GetHashCode(const Apoc3D::Math::PointF& obj);
		};
		template <>
		struct APAPI EqualityComparer<Apoc3D::Math::Vector2>
		{
			static bool Equals(const Apoc3D::Math::Vector2& x, const Apoc3D::Math::Vector2& y);
			static int32 GetHashCode(const Apoc3D::Math::Vector2& obj);
		};
		template <>
		struct APAPI EqualityComparer<Apoc3D::Math::Vector3>
		{
			static bool Equals(const Apoc3D::Math::Vector3& x, const Apoc3D::Math::Vector3& y);
			static int32 GetHashCode(const Apoc3D::Math::Vector3& obj);
		};
		template <>
		struct APAPI EqualityComparer<Apoc3D::Math::Vector4>
		{
			static bool Equals(const Apoc3D::Math::Vector4& x, const Apoc3D::Math::Vector4& y);
			static int32 GetHashCode(const Apoc3D::Math::Vector4& obj);
		};
		template <>
		struct APAPI EqualityComparer<Apoc3D::Math::Color4>
		{
			static bool Equals(const Apoc3D::Math::Color4& x, const Apoc3D::Math::Color4& y);
			static int32 GetHashCode(const Apoc3D::Math::Color4& obj);
		};
		template <>
		struct APAPI EqualityComparer<std::thread::id>
		{
			static bool Equals(const std::thread::id& x, const std::thread::id& y);
			static int32 GetHashCode(const std::thread::id& obj);

			static const std::hash<std::thread::id> m_threadIDHasher;
		};



		struct APAPI NStringEqualityComparerNoCase
		{
			static bool Equals(const std::string& x, const std::string& y);
			static int32 GetHashCode(const std::string& obj);
		};

		struct APAPI StringEqualityComparerNoCase
		{
			static bool Equals(const String& x, const String& y);
			static int32 GetHashCode(const String& obj);
		};


		

		template <typename T>
		int32 OrderComparer(const T& a, const T& b)
		{
			if (a < b) return -1;
			return (a > b) ? 1 : 0;
		}

		// If T/S is a reference, and a ref to temporary rvalue object is passed in,
		// watch out its life time. This issue exists for both lvalue and rvalue ctors.
		template <typename T, typename S>
		struct KeyPairValue
		{
			T Key;
			S Value;

			KeyPairValue() { }
			KeyPairValue(const T& key, const S& value)
				: Key(key), Value(value) { }

			KeyPairValue(typename std::remove_reference<T>::type&& key, typename std::remove_reference<S>::type&& value)
				: Key(std::move(key)), Value(std::move(value)) { }

			KeyPairValue(KeyPairValue&& other)
				: Key(std::move(other.Key)), Value(std::move(other.Value)) { }

			KeyPairValue& operator =(KeyPairValue&& other)
			{
				if (this != &other)
				{
					Key = std::move(other.Key);
					Value = std::move(other.Value);
				}
				return *this;
			}
		};



		template <typename T, typename P, T& GetItem(const P*, int32, int32), int32 GetCountI(const P*), int32 GetCountJ(const P*, int32)>
		class Iterator2D
		{
		public:
			T& operator*() const
			{
				assert(m_parent);

				return GetItem(m_parent, m_index, m_frame);
			}

			T* operator->() const { return &operator*(); }

			bool operator==(const Iterator2D &other) const
			{
				return other.m_parent == m_parent && other.m_index == m_index && other.m_frame == m_frame;
			}
			bool operator!=(const Iterator2D &other) const { return !(*this == other); }

			Iterator2D& operator++()
			{
				MoveToNext();

				return *this;
			}
			Iterator2D operator++(int)
			{
				Iterator2D result = *this;
				++(*this);
				return result;
			}

			Iterator2D(const P* parent) : m_parent(parent)
			{
				MoveToNext();
			}

			Iterator2D(const P* parent, int32 idx, int32 frm)
				: m_parent(parent), m_index(idx), m_frame(frm) { }

		private:
			void MoveToNext()
			{
				assert(m_parent);

				if (m_index != -1)
				{
					if (m_frame != -1)
					{
						m_frame++;
						if (m_frame >= GetCountJ(m_parent, m_index))
						{
							m_index++;
							m_frame = 0;

							if (m_index >= GetCountI(m_parent))
							{
								m_index = -1;
								m_frame = -1;
							}
						}
					}
					else
					{
						// initial. Locate to the first one
						while (GetCountJ(m_parent, m_index) == 0)
						{
							m_index++;

							if (m_index >= GetCountI(m_parent))
							{
								m_index = -1;
								m_frame = -1;
								break;
							}
						}

						if (m_index != -1)
							m_frame = 0;
					}
				}
			}


			const P* m_parent;

			int32 m_index = 0;
			int32 m_frame = -1;
		};


		namespace Utils
		{
			int32 APAPI GetHashTableSize(int32 min);

			template <typename T>
			struct _ToString
			{
				static String Invoke(const T& item) { return L""; }
			};

			template<> struct APAPI _ToString<std::string> { static String Invoke(const std::string& str); };
			template<> struct APAPI _ToString<String> { static String Invoke(const String& str); };
			template<> struct APAPI _ToString<const char*> { static String Invoke(const char* const& str); };
			template<> struct APAPI _ToString<const wchar_t*> { static String Invoke(const wchar_t* const& str); };

			template<> struct APAPI _ToString<int16> { static String Invoke(const int16& v); };
			template<> struct APAPI _ToString<int32> { static String Invoke(const int32& v); };
			template<> struct APAPI _ToString<int64> { static String Invoke(const int64& v); };

			template<> struct APAPI _ToString<uint16> { static String Invoke(const uint16& v); };
			template<> struct APAPI _ToString<uint32> { static String Invoke(const uint32& v); };
			template<> struct APAPI _ToString<uint64> { static String Invoke(const uint64& v); };


			template <typename T>
			String ToString(const T& item) { return _ToString<T>::Invoke(item); }


			template <typename A>
			struct HashSetEntry final
			{
				int32 hashCode = -1;
				int32 next;

				char dataStor[sizeof(A)];

				HashSetEntry() { }

				HashSetEntry(const HashSetEntry& o)
					: hashCode(o.hashCode), next(o.next)
				{
					if (hashCode != -1)
						new (dataStor)A(o.getData());
				}
				HashSetEntry(HashSetEntry&& o)
					: hashCode(o.hashCode), next(o.next)
				{
					if (hashCode != -1)
						new (dataStor)A(std::move(o.getData()));
				}

				~HashSetEntry() { Clear(); } 

				HashSetEntry& operator=(HashSetEntry&& o)
				{
					if (this != &o)
					{
						this->~HashSetEntry();
						new (this)HashSetEntry(std::move(o));
					}
					return *this;
				}

				HashSetEntry& operator=(const HashSetEntry& o)
				{
					if (this != &o)
					{
						this->~HashSetEntry();
						new (this)HashSetEntry(o);
					}
					return *this;
				}


				void Clear()
				{
					if (hashCode != -1)
					{
						getData().~A();

						hashCode = -1;
					}
				}

				template <typename AA, typename Acceptable = typename std::enable_if<IsSimilar<A, AA>::value>::type>
				void Set(int32 hash, AA&& d)
				{
					assert(hash != -1);

					if (hashCode != -1)
						getData() = std::forward<AA>(d);
					else
						new (dataStor)A(std::forward<AA>(d));
					hashCode = hash;
				}

				A& getData() const { assert(hashCode != -1); return *(A*)dataStor; }
			};

			template <typename A, typename B>
			struct HashMapEntry final
			{
				int32 hashCode = -1;
				int32 next;

				char keyStor[sizeof(A)];
				char valueStor[sizeof(B)];

				HashMapEntry() { }
				HashMapEntry(const HashMapEntry& o)
					: hashCode(o.hashCode), next(o.next)
				{
					if (hashCode != -1)
					{
						new (keyStor)A(o.getData());
						new (valueStor)B(o.getValue());
					}
				}
				HashMapEntry(HashMapEntry&& o)
					: hashCode(o.hashCode), next(o.next)
				{
					if (hashCode != -1)
					{
						new (keyStor)A(std::move(o.getData()));
						new (valueStor)B(std::move(o.getValue()));
					}
				}

				~HashMapEntry() { Clear(); }

				HashMapEntry& operator=(HashMapEntry&& o)
				{
					if (this != &o)
					{
						this->~HashMapEntry();
						new (this)HashMapEntry(std::move(o));
					}
					return *this;
				}

				HashMapEntry& operator=(const HashMapEntry& o)
				{
					if (this != &o)
					{
						this->~HashMapEntry();
						new (this)HashMapEntry(o);
					}
					return *this;
				}


				void Clear()
				{
					if (hashCode != -1)
					{
						getData().~A();
						getValue().~B();

						hashCode = -1;
					}
				}

				template <typename AA, typename BB,
					typename AcceptableKey = typename std::enable_if<IsSimilar<A, AA>::value>::type,
					typename AcceptableValue = typename std::enable_if<IsSimilar<B, BB>::value>::type>
				void Set(int32 hash, AA&& key, BB&& val)
				{
					assert(hash != -1);
					if (hashCode != -1)
					{
						getData() = std::forward<AA>(key);
						getValue() = std::forward<BB>(val);
					}
					else
					{
						new (keyStor)A(std::forward<AA>(key));
						new (valueStor)B(std::forward<BB>(val));
					}
					hashCode = hash;
				}


				template <typename BB, typename AcceptableValue = typename std::enable_if<IsSimilar<B, BB>::value>::type>
				void SetValue(BB&& d)
				{
					if (hashCode != -1)
						getValue() = std::forward<BB>(d);
					else
						new (valueStor)B(std::forward<BB>(d));
				}


				A& getData() const { assert(hashCode != -1); return *(A*)keyStor; }
				B& getValue() const { assert(hashCode != -1); return *(B*)valueStor; }

			};

			//////////////////////////////////////////////////////////////////////////

			struct CopyToNew_Memcpy
			{
				template <typename T>
				static void Invoke(T* dest, const T* src, int32 count)
				{
					memcpy(dest, src, count * sizeof(T));
				}
			};
			
			struct CopyToNew_Assign
			{
				template <typename T>
				static void Invoke(T* dest, const T* src, int32 count)
				{
					for (int32 i = 0; i < count; i++)
						dest[i] = src[i];
				}
			};
			
			struct CopyToNew_Construct
			{
				template <typename T>
				static void Invoke(T* dest, const T* src, int32 count)
				{
					for (int32 i = 0; i < count; i++)
						new (&dest[i])T(src[i]);
				}
			};


			template <typename T, bool NoRAII = false>
			void CopyToNew(T* dest, const T* src, int32 count)
			{
				constexpr bool isTrivial = std::is_trivially_copyable<T>::value;
				std::conditional<isTrivial, CopyToNew_Memcpy, std::conditional<NoRAII, CopyToNew_Assign, CopyToNew_Construct>::type>::type::Invoke(dest, src, count);
			}


			struct MoveToNew_Memcpy
			{
				template <typename T>
				static void Invoke(T* dest, T* src, int32 count)
				{
					memcpy(dest, src, count * sizeof(T));
				}
			};

			struct MoveToNew_Assign
			{
				template <typename T>
				static void Invoke(T* dest, T* src, int32 count)
				{
					for (int32 i = 0; i < count; i++)
						dest[i] = std::move(src[i]);
				}
			};

			struct MoveToNew_Construct
			{
				template <typename T>
				static void Invoke(T* dest, T* src, int32 count)
				{
					for (int32 i = 0; i < count; i++)
						new (&dest[i])T(std::move(src[i]));
				}
			};

			template <typename T, bool NoRAII = false>
			void MoveToNew(T* dest, T* src, int32 count)
			{
				constexpr bool isTrivial = std::is_trivially_copyable<T>::value;
				std::conditional<isTrivial, MoveToNew_Memcpy, std::conditional<NoRAII, MoveToNew_Assign, MoveToNew_Construct>::type>::type::Invoke(dest, src, count);
			}

			template <typename T, bool NoRAII = false>
			void DoPutNew(const T& val, int32 idx, T* elm)
			{
				assert(idx >= 0);

				if (std::is_trivially_copyable<T>::value || NoRAII)
				{
					elm[idx] = val;
				}
				else
				{
					new (&elm[idx])T(val);
				}
			}

			template <typename T, bool NoRAII = false>
			void DoPutNew(T&& val, int32 idx, T* elm)
			{
				assert(idx >= 0);

				if (std::is_trivially_copyable<T>::value || NoRAII)
				{
					elm[idx] = std::move(val);
				}
				else
				{
					new (&elm[idx])T(std::move(val));
				}
			}

			template <typename T, bool NoRAII = false>
			void DoDestory(int32 start, int32 count, T* elm)
			{
				assert(start >= 0);
				if (!std::is_trivially_copyable<T>::value && !NoRAII)
				{
					for (int32 i = start; i < start + count; i++)
						elm[i].~T();
				}
			}

		};

	}
}

#endif