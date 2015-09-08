#include "TestCommon.h"

namespace UnitTestVC
{		
	TEST_CLASS(ListTest)
	{
		MemoryService mem;
	public:
		

		TEST_METHOD(List_Reverse)
		{
			List<int> subject;

			subject.Add(0);
			subject.Add(1);
			subject.Add(2);
			subject.Add(3);

			subject.Reverse();

			Assert::AreEqual(subject[0], 3);
			Assert::AreEqual(subject[1], 2);
			Assert::AreEqual(subject[2], 1);
			Assert::AreEqual(subject[3], 0);

			subject.Reverse();
			subject.Add(4);
			subject.Reverse();
			Assert::AreEqual(subject[0], 4);
			Assert::AreEqual(subject[1], 3);
			Assert::AreEqual(subject[2], 2);
			Assert::AreEqual(subject[3], 1);
			Assert::AreEqual(subject[4], 0);

		}

		TEST_METHOD(List_Sort)
		{
			List<int> subject(100);
			int counter[1000];
			memset(counter, 0, sizeof(counter));

			for (int i = 0; i < 100; i++)
			{
				int val = Randomizer::NextExclusive(1000);
				subject.Add(val);
				counter[val]++;
			}

			subject.Sort();


			for (int i = 0; i < subject.getCount() - 1; i++)
			{
				if (subject[i + 1] < subject[i])
				{
					Assert::Fail();
				}
			}

			int counter2[1000] = { 0 };
			for (int i = 0; i < subject.getCount(); i++)
			{
				counter2[subject[i]]++;
			}

			Assert::IsTrue(memcmp(counter, counter2, sizeof(counter)) == 0);
		}

		TEST_METHOD(List_Sort2)
		{
			List<int> subject(100);
			int counter[1000];
			memset(counter, 0, sizeof(counter));

			for (int i = 0; i < 100; i++)
			{
				int val = Randomizer::NextExclusive(1000);
				subject.Add(val);
				counter[val]++;
			}

			subject.Sort<OrderComparer>();


			for (int i = 0; i < subject.getCount() - 1; i++)
			{
				if (subject[i + 1] < subject[i])
				{
					Assert::Fail();
				}
			}

			int counter2[1000] = { 0 };
			for (int i = 0; i < subject.getCount(); i++)
			{
				counter2[subject[i]]++;
			}

			Assert::IsTrue(memcmp(counter, counter2, sizeof(counter)) == 0);
		}

		TEST_METHOD(List_Iterator)
		{
			List<int> subject;

			subject.Add(0);
			subject.Add(1);
			subject.Add(2);

			List<int> result;
			for (int e : subject)
			{
				result.Add(e);
			}

			Assert::AreEqual(3, result.getCount());
			if (result.getCount() == 3)
			{
				Assert::AreEqual(0, result[0]);
				Assert::AreEqual(1, result[1]);
				Assert::AreEqual(2, result[2]);
			}

			subject.Clear();

			for (auto e : subject)
			{
				Assert::Fail();
			}
		}

		TEST_METHOD(List_AddMult)
		{
			const int preData[] = { 44, 55, 66, 77 };

			for (int32 i = 0; i < countof(preData); i++)
			{
				List<int> s1;

				for (int32 j = 0; j <= i; j++)
				{
					s1.Add(preData[j]);
				}

				s1.AddArray(mem.intData, mem.intDataCount);

				for (int32 j = 0; j < s1.getCount(); j++)
				{
					if (j <= i)
						Assert::AreEqual(preData[j], s1[j]);
					else
						Assert::AreEqual(mem.intData[j - i - 1], s1[j]);
				}

				Assert::AreEqual(i + mem.intDataCount + 1, s1.getCount());
			}

			List<int> s2;
			s2.AddArray(preData);

			Assert::AreEqual(countof(preData), s2.getCount());
			for (int32 i = 0; i < s2.getCount(); i++)
			{
				Assert::AreEqual(preData[i], s2[i]);
			}
		}

		//////////////////////////////////////////////////////////////////////////

		TEST_METHOD(List_OrderInt)
		{
			_TestOrder<int>(mem.intData, mem.intDataCount);
		}

		TEST_METHOD(List_OrderVoid)
		{
			_TestOrder<void*>(mem.ptrData, mem.ptrDataCount);
		}

		TEST_METHOD(List_OrderString)
		{
			_TestOrder<String>(mem.stringData, mem.strDataCount);
		}

		TEST_METHOD(List_OrderCString)
		{
			_TestOrder<NonMovableString>(mem.cstringData, mem.strDataCount);
		}

		//////////////////////////////////////////////////////////////////////////

		TEST_METHOD(List_ClearInt)
		{
			int data[5] = { 1, 2, 3, 4, 5 };
			_TestClear<int>(data, 5, 100, 101);
		}

		TEST_METHOD(List_ClearVoid)
		{
			void* data[5] = { (void*)1, (void*)2, (void*)3, (void*)4, (void*)5 };
			_TestClear<void*>(data, 5, (void*)100, (void*)101);
		}

		TEST_METHOD(List_ClearString)
		{
			_TestClear<String>(mem.stringData, mem.strDataCount, L"xxxxxxxxxxxxxxxxxx", L"yyyyyyyyyyyyyyyyyy");
		}

		TEST_METHOD(List_ClearCString)
		{
			_TestClear<NonMovableString>(mem.cstringData, mem.strDataCount, L"xxxxxxxxxxxxxxxxxx", L"yyyyyyyyyyyyyyyyyy");
		}

		//////////////////////////////////////////////////////////////////////////

		TEST_METHOD(List_CopyInt)
		{
			int data[5] = { 1, 2, 3, 4, 5 };
			_TestCopy<int>(data, 5, 100, 101);
		}

		TEST_METHOD(List_CopyVoid)
		{
			void* data[5] = { (void*)1, (void*)2, (void*)3, (void*)4, (void*)5 };
			_TestCopy<void*>(data, 5, (void*)100, (void*)101);
		}

		TEST_METHOD(List_CopyString)
		{
			_TestCopy<String>(mem.stringData, mem.strDataCount, L"xxxxxxxxxxxxxxxxxx", L"yyyyyyyyyyyyyyyyyy");
		}

		TEST_METHOD(List_CopyCString)
		{
			_TestClear<NonMovableString>(mem.cstringData, mem.strDataCount, L"xxxxxxxxxxxxxxxxxx", L"yyyyyyyyyyyyyyyyyy");
		}

		//////////////////////////////////////////////////////////////////////////

		TEST_METHOD(List_IteratorInt)
		{
			_TestIterator<int>(mem.intData, mem.intDataCount);
		}

		TEST_METHOD(List_IteratorVoid)
		{
			_TestIterator<void*>(mem.ptrData, mem.ptrDataCount);
		}

		TEST_METHOD(List_IteratorString)
		{
			_TestIterator<String>(mem.stringData, mem.strDataCount);
		}

		TEST_METHOD(List_IteratorCString)
		{
			_TestIterator<NonMovableString>(mem.cstringData, mem.strDataCount);
		}

		//////////////////////////////////////////////////////////////////////////

		TEST_METHOD(List_RemoveInt)
		{
			int data[5] = { 1, 2, 3, 4, 5 };
			_TestRemove<int>(data, 5, 100);
		}

		TEST_METHOD(List_RemoveVoid)
		{
			void* data[5] = { (void*)1, (void*)2, (void*)3, (void*)4, (void*)5 };
			_TestRemove<void*>(data, 5, (void*)100);
		}

		TEST_METHOD(List_RemoveString) { _TestRemove<String>(mem.stringData, mem.strDataCount, L"[missing]"); }
		TEST_METHOD(List_RemoveCString) { _TestRemove<NonMovableString>(mem.cstringData, mem.strDataCount, L"[missing]"); }

		//////////////////////////////////////////////////////////////////////////

		TEST_METHOD(List_Insert)
		{
			const List<String> srcLst =
			{
				L"0000000000000000000",
				L"1111111111111111111",
				L"2222222222222222222",
				L"3333333333333333333",
				L"4444444444444444444",
				L"5555555555555555555"
			};
			List<String> lst = 
			{
				srcLst[1],
				srcLst[2],
				srcLst[3],
				srcLst[4],
				srcLst[5],
			};

			lst.Insert(0, srcLst[0]);
			Assert::AreEqual(6, lst.getCount());
			
			Assert::AreEqual(srcLst[0], lst[0]);
			Assert::AreEqual(srcLst[1], lst[1]);
			Assert::AreEqual(srcLst[2], lst[2]);
			Assert::AreEqual(srcLst[3], lst[3]);
			Assert::AreEqual(srcLst[4], lst[4]);
			Assert::AreEqual(srcLst[5], lst[5]);

			lst.RemoveAt(0);
			Assert::AreEqual(5, lst.getCount());


			String testData[] = { srcLst[2], srcLst[1], srcLst[0] };
			lst.InsertArray(3, testData, countof(testData));

			Assert::AreEqual(8, lst.getCount());

			Assert::AreEqual(srcLst[1], lst[0]);
			Assert::AreEqual(srcLst[2], lst[1]);
			Assert::AreEqual(srcLst[3], lst[2]);
			Assert::AreEqual(srcLst[2], lst[3]);
			Assert::AreEqual(srcLst[1], lst[4]);
			Assert::AreEqual(srcLst[0], lst[5]);
			Assert::AreEqual(srcLst[4], lst[6]);
			Assert::AreEqual(srcLst[5], lst[7]);

			lst.InsertArray(6, testData, countof(testData));

			Assert::AreEqual(11, lst.getCount());

			lst = srcLst;
			lst.InsertArray(4, testData, countof(testData));

			Assert::AreEqual(9, lst.getCount());

			Assert::AreEqual(srcLst[0], lst[0]);
			Assert::AreEqual(srcLst[1], lst[1]);
			Assert::AreEqual(srcLst[2], lst[2]);
			Assert::AreEqual(srcLst[3], lst[3]);
			Assert::AreEqual(srcLst[2], lst[4]);
			Assert::AreEqual(srcLst[1], lst[5]);
			Assert::AreEqual(srcLst[0], lst[6]);
			Assert::AreEqual(srcLst[4], lst[7]);
			Assert::AreEqual(srcLst[5], lst[8]);

		}


		TEST_METHOD(List_RandomAt) { _TestRemoveAt<String>(mem.stringData, mem.strDataCount); }
		TEST_METHOD(List_RandomAtC) { _TestRemoveAt<NonMovableString>(mem.cstringData, mem.strDataCount); }
		TEST_METHOD(List_RandomAtT) { _TestRemoveAt<TrivialString>(mem.tstringData, mem.strDataCount); }

		TEST_METHOD(List_Random) { _TestRandom<String>(mem.stringData, mem.strDataCount); }
		TEST_METHOD(List_RandomC) { _TestRandom<NonMovableString>(mem.cstringData, mem.strDataCount); }
		TEST_METHOD(List_RandomT) { _TestRandom<TrivialString>(mem.tstringData, mem.strDataCount); }

		TEST_METHOD(List_RAII)
		{
			FlaggerStates states;

			{
				List<Flagger<true>> lst;

				lst.Add({ &states });
				lst.RemoveAt(0);
			}
			
			Assert::AreEqual(1, states.ctorCount);
			Assert::AreEqual(1, states.actualDtorCount);
			Assert::AreEqual(1, states.moveCount);
			Assert::AreEqual(0, states.copyCount);

			states.Clear();
			{
				List<Flagger<false>>().Add({ &states });
			}

			Assert::AreEqual(1, states.ctorCount);
			Assert::AreEqual(2, states.actualDtorCount);
			Assert::AreEqual(0, states.moveCount);
			Assert::AreEqual(1, states.copyCount);
		}
	private:

#pragma region GenericTest

		template<typename S>
		void _TestOrder(const S* data, int32 dataCount)
		{
			List<S> subject;
			for (int32 i = 0; i < dataCount; i++)
			{
				Assert::AreEqual(i, subject.getCount());
				subject.Add(data[i]);
				Assert::AreEqual(data[i], subject[i]);
			}

			Assert::AreEqual(dataCount, subject.getCount());

			for (int32 i = dataCount - 1; i >= 0; i--)
			{
				Assert::AreEqual(subject[i], data[i]);
				subject.RemoveAt(subject.getCount() - 1);
			}

			Assert::AreEqual(0, subject.getCount());

		}

		template<typename S>
		void _TestCopy(const S* data, int32 dataCount, const S& a, const S& b)
		{
			List<S> subject;
			for (int32 i = 0; i < dataCount; i++)
			{
				Assert::AreEqual(i, subject.getCount());
				subject.Add(data[i]);
			}

			List<S> testList2(subject);
			List<S> testList3; testList3.Add(a); testList3.Add(b);

			testList3 = subject;

			Assert::AreEqual(dataCount, testList3.getCount());

			for (int32 i = 0; i < dataCount; i++)
			{
				Assert::AreEqual(testList2[i], subject[i]);
				Assert::AreEqual(testList3[i], subject[i]);
				Assert::AreEqual(data[i], subject[i]);

			}

		}

		template<typename S>
		void _TestClear(const S* data, int32 dataCount, const S& a, const S& b)
		{
			List<S> subject;
			for (int32 i = 0; i < dataCount; i++)
			{
				Assert::AreEqual(i, subject.getCount());
				subject.Add(data[i]);
			}

			List<S> testList2(subject);
			List<S> testList3; testList3.Add(a); testList3.Add(b);

			testList3 = subject;


			subject.Clear();
			testList2.Clear();
			testList3.Clear();

			Assert::AreEqual(0, subject.getCount());
			Assert::AreEqual(0, testList2.getCount());
			Assert::AreEqual(0, testList3.getCount());
		}

		template<typename S>
		void _TestIterator(const S* data, int32 dataCount)
		{
			List<S> subject;

			for (int32 i = 0; i < dataCount; i++)
			{
				subject.Add(data[i]);
			}

			S* iter1 = subject.begin();
			S* iter2 = iter1;
			int counter = 0;
			do
			{
				Assert::IsTrue(data[counter] == *iter2);
				Assert::IsTrue(*iter1 == data[counter]);

				Assert::IsTrue(iter1 == iter2);

				S* iter3 = iter2;
				Assert::IsTrue(iter1 == iter3);

				iter1++;
				++iter2;
				counter++;
			} while (iter1 != subject.end());
		}

		template<typename S>
		void _TestRemove(const S* data, int32 dataCount, const S& dataNotFound)
		{
			List<S> subject;

			for (int32 i = 0; i < dataCount; i++)
			{
				subject.Add(data[i]);
			}

			bool result = subject.Remove(data[dataCount - 1]);
			Assert::IsTrue(result);
			Assert::AreEqual(subject.getCount(), dataCount - 1);
			Assert::AreEqual(subject[subject.getCount() - 1], data[dataCount - 2]);
			
			Assert::IsFalse(subject.Remove(dataNotFound));
			Assert::AreEqual(subject.getCount(), dataCount - 1);

			subject.RemoveAt(subject.getCount() - 1);
			Assert::AreEqual(subject.getCount(), dataCount - 2);
			Assert::AreEqual(subject[subject.getCount() - 1], data[dataCount - 3]);
			
			subject.RemoveRange(1, subject.getCount() - 1);
			Assert::AreEqual(subject.getCount(), 1);
			Assert::AreEqual(subject[0], data[0]);

			List<S> subject2;
			for (int32 i = 0; i < dataCount; i++)
			{
				subject2.Add(data[i]);
			}

			subject2.RemoveRange(1, subject2.getCount() - 1);
			Assert::AreEqual(subject2.getCount(), 1);
			Assert::AreEqual(subject2[0], data[0]);

		}

		template <typename S>
		void _TestRandom(const S* data, int32 dataCount)
		{
			List<S> c;
			for (int32 k = 0; k < 1000; k++)
			{
				int32 cnt = Randomizer::NextExclusive(dataCount);
				for (int32 i = 0; i < cnt; i++)
				{
					if (Randomizer::NextBool() && c.getCount() > 0)
					{
						c.RemoveAt(Randomizer::NextExclusive(c.getCount()));
					}
					else
					{
						c.Add(data[i]);
					}
				}

				List<S> lst = c;
				bool same = lst.getCount() == c.getCount();
				for (int32 i = 0; i < lst.getCount(); i++)
				{
					if (lst[i] != c[i])
					{
						same = false;
						break;
					}
				}
				Assert::IsTrue(same);
			}
		}

		template <typename S>
		void _TestRemoveAt(const S* _data, int32 dataCount)
		{
			for (int32 k = 0; k < 10000; k++)
			{
				List<S> data;
				data.AddArray(_data, dataCount);

				List<S> emulatedResult = data;
				List<int32> indices(data.getCount());

				if (k == 0)
				{
					//indices = data;
					for (int32 i = 0; i < data.getCount(); i++)
						indices.Add(i);
					emulatedResult.Clear();
				}
				else
				{
					for (int32 i = data.getCount() - 1; i >= 0; i--)
					{
						if (Randomizer::NextBool())
						{
							indices.Add(i);

							emulatedResult.RemoveAt(i);
						}
					}
					indices.Sort();
				}

				data.RemoveAt(indices);

				bool same = data.getCount() == emulatedResult.getCount();
				for (int32 i = 0; i < data.getCount(); i++)
				{
					if (data[i] != emulatedResult[i])
					{
						same = false;
						break;
					}
				}
				Assert::IsTrue(same);
			}
		}
#pragma endregion
	};

	TEST_CLASS(FixedListTest)
	{
		MemoryService mem;
	public:

		TEST_METHOD(FixedList_Iterator)
		{
			FixedList<int, 5> subject;

			subject.Add(0);
			subject.Add(1);
			subject.Add(2);

			FixedList<int, 5> result;
			for (int e : subject)
			{
				result.Add(e);
			}

			Assert::AreEqual(3, result.getCount());
			if (result.getCount() == 3)
			{
				Assert::AreEqual(0, result[0]);
				Assert::AreEqual(1, result[1]);
				Assert::AreEqual(2, result[2]);
			}

			subject.Clear();

			for (auto e : subject)
			{
				Assert::Fail();
			}
		}

		TEST_METHOD(FixedList_AddMult)
		{
			const int preData[] = { 44, 55, 66, 77 };

			for (int32 i = 0; i < countof(preData); i++)
			{
				FixedList<int, 28> s1;

				for (int32 j = 0; j <= i; j++)
				{
					s1.Add(preData[j]);
				}

				s1.AddArray(mem.intData, mem.intDataCount);

				for (int32 j = 0; j < s1.getCount(); j++)
				{
					if (j <= i)
						Assert::AreEqual(preData[j], s1[j]);
					else
						Assert::AreEqual(mem.intData[j - i - 1], s1[j]);
				}

				Assert::AreEqual(i + mem.intDataCount + 1, s1.getCount());
			}

			FixedList<int, 5> s2;
			s2.AddArray(preData);

			Assert::AreEqual(countof(preData), s2.getCount());
			for (int32 i = 0; i < s2.getCount(); i++)
			{
				Assert::AreEqual(preData[i], s2[i]);
			}
		}


		TEST_METHOD(FixedList_Random) { _TestRandom<String>(mem.stringData, mem.strDataCount); }
		TEST_METHOD(FixedList_RandomT) { _TestRandom<TrivialString>(mem.tstringData, mem.strDataCount); }
		TEST_METHOD(FixedList_RandomC) { _TestRandom<NonMovableString>(mem.cstringData, mem.strDataCount); }

		TEST_METHOD(FixedList_RAII)
		{
			FlaggerStates states1;
			FlaggerStates states2;

			{
				FixedList<Flagger<true>, 4> lst;

				lst.Add({ &states1 });

				FixedList<Flagger<true>, 4> lst2 = std::move(lst);

				lst2.Clear();
				lst2.Add({ &states2 });

				lst = lst2;
			}

			Assert::AreEqual(1, states1.ctorCount);
			Assert::AreEqual(1, states1.actualDtorCount);
			Assert::AreEqual(2, states1.moveCount);
			Assert::AreEqual(0, states1.copyCount);

			Assert::AreEqual(1, states2.ctorCount);
			Assert::AreEqual(2, states2.actualDtorCount);
			Assert::AreEqual(1, states2.moveCount);
			Assert::AreEqual(1, states2.copyCount);
		}
	private:
		template <typename S>
		void _TestRandom(const S* data, int32 count)
		{
			FixedList<S, 1000> c;
			for (int32 k = 0; k < 1000; k++)
			{
				int32 cnt = Randomizer::NextExclusive(count);
				for (int32 i = 0; i < cnt; i++)
				{
					if ((Randomizer::NextBool() && c.getCount() > 0) || c.isFull())
					{
						c.RemoveAt(Randomizer::NextExclusive(c.getCount()));
					}
					else
					{
						c.Add(data[i]);
					}
				}

				FixedList<S, 1000> lst = c;
				bool same = lst.getCount() == c.getCount();
				for (int32 i = 0; i < lst.getCount(); i++)
				{
					if (lst[i] != c[i])
					{
						same = false;
						break;
					}
				}
				Assert::IsTrue(same);

				FixedList<S, 1000> lst2 = std::move(lst);
				same = lst2.getCount() == c.getCount();
				for (int32 i = 0; i < lst2.getCount(); i++)
				{
					if (lst2[i] != c[i])
					{
						same = false;
						break;
					}
				}
				Assert::IsTrue(same);

			}
		}
	};

	TEST_CLASS(HashMapTest)
	{
		MemoryService mem;
	public:

		TEST_METHOD(HashMap_Iterator)
		{
			HashMap<int, int> subject;

			subject.Add(0, 2);
			subject.Add(1, 1);
			subject.Add(2, 0);

			List<int> result;

			for (int e : subject.getKeyAccessor())
			{
				result.Add(e);
			}

			Assert::AreEqual(3, result.getCount());
			if (result.getCount() == 3)
			{
				Assert::AreEqual(0, result[0]);
				Assert::AreEqual(1, result[1]);
				Assert::AreEqual(2, result[2]);
			}

			result.Clear();

			for (int e : subject.getValueAccessor())
			{
				result.Add(e);
			}
			Assert::AreEqual(3, result.getCount());
			if (result.getCount() == 3)
			{
				Assert::AreEqual(2, result[0]);
				Assert::AreEqual(1, result[1]);
				Assert::AreEqual(0, result[2]);
			}

			result.Clear();

			for (auto e : subject)
			{
				result.Add(e.Key);
				result.Add(e.Value);
			}

			Assert::AreEqual(6, result.getCount());
			if (result.getCount() == 6)
			{
				Assert::AreEqual(0, result[0]);
				Assert::AreEqual(2, result[1]);
				Assert::AreEqual(1, result[2]);
				Assert::AreEqual(1, result[3]);
				Assert::AreEqual(2, result[4]);
				Assert::AreEqual(0, result[5]);
			}

			subject.Clear();

			for (auto e : subject)
			{
				Assert::Fail();
			}
		}

		TEST_METHOD(HashMap_Copy) { _TestCopy<String>(); }
		TEST_METHOD(HashMap_CopyT) { _TestCopy<TrivialString>(); }
		TEST_METHOD(HashMap_CopyC) { _TestCopy<NonMovableString>(); }

		TEST_METHOD(HashMap_Count) { _TestCount<String>(); }
		TEST_METHOD(HashMap_CountT) { _TestCount<TrivialString>(); }
		TEST_METHOD(HashMap_CountC) { _TestCount<NonMovableString>(); }

		TEST_METHOD(HashMap_AddRemove)
		{
			const int amount = 1000;
			String srcs[amount];
			bool added[amount] = { 0 };
			for (int32 i = 0; i < amount;i++)
			{
				srcs[i] = StringUtils::IntToString(i) + L" " + mem.stringData[i % mem.strDataCount];
			}

			HashMap<String, int> m2;
			m2.Resize(amount);

			for (int32 k = 0; k < 100;k++)
			{
				for (int32 i = 0; i < amount; i++)
				{
					if ((Randomizer::Next() & 1) == 0)
					{
						if (added[i])
						{
							m2.Remove(srcs[i]);
							added[i] = false;
						}
						else
						{
							m2.Add(srcs[i], i);
							added[i] = true;
						}
					}
				}
			}

			for (auto& e : m2)
			{
				Assert::IsTrue(added[e.Value]);
				Assert::AreEqual(srcs[e.Value], e.Key);
			}

		}

	private:
		template <typename S>
		void _TestCopy()
		{
			HashMap<String, S> m1(10);

			HashMap<String, S> m2 = m1;
			m2.Add(L"1", L"");
			m2.Add(L"sssssssssssssssssssssssssssssssss", L"llllllllllllllllllllllllllllllll");
			m2.Add(L"s", L"dsa");

			HashMap<String, S> m3 = m2;
			m3.Clear();


			HashMap<String, S> m4;

			HashMap<String, S> m5;
			m5 = m4;
			m5.Add(L"1", L"");
			m5.Add(L"sssssssssssssssssssssssssssssssss", L"llllllllllllllllllllllllllllllll");
			m5.Add(L"s", L"dsa");

			m5.Clear();

			HashMap<String, S> m6 = m5;
		}

		template <typename S>
		void _TestCount()
		{
			HashMap<String, S> m2;
			m2.Add(L"1", L"");
			m2.Add(L"sssssssssssssssssssssssssssssssss", L"llllllllllllllllllllllllllllllll");
			m2.Add(L"s", L"dsa");

			Assert::AreEqual(3, m2.getCount());

			m2.Remove(L"1");
			Assert::AreEqual(2, m2.getCount());

			m2.Remove(L"s");
			Assert::AreEqual(1, m2.getCount());

			m2.Add(L"asdsa", L"da");
			Assert::AreEqual(2, m2.getCount());

			m2.Add(L"sdasd", L"f");
			Assert::AreEqual(3, m2.getCount());

			m2.Add(L"sdsdsaf", L"s");
			Assert::AreEqual(4, m2.getCount());

			m2.Remove(L"sssssssssssssssssssssssssssssssss");
			Assert::AreEqual(3, m2.getCount());


			HashMap<String, S> m3 = m2;
			Assert::AreEqual(3, m3.getCount());

			m3.Clear();
			Assert::AreEqual(0, m3.getCount());

		}
	};

	TEST_CLASS(HashSetTest)
	{
		MemoryService mem;
	public:

		TEST_METHOD(HashSet_AddRemove)
		{
			const int amount = 1000;
			String srcs[amount];
			bool added[amount] = { 0 };
			for (int32 i = 0; i < amount; i++)
			{
				srcs[i] = StringUtils::IntToString(i) + L" " + mem.stringData[i % mem.strDataCount];
			}

			HashSet<String> m2;
			m2.Resize(amount);

			for (int32 k = 0; k < 100; k++)
			{
				for (int32 i = 0; i < amount; i++)
				{
					if ((Randomizer::Next() & 1) == 0)
					{
						if (added[i])
						{
							m2.Remove(srcs[i]);
							added[i] = false;
						}
						else
						{
							m2.Add(srcs[i]);
							added[i] = true;
						}
					}
				}
			}

			for (const auto& e : m2)
			{
				int32 idx = StringUtils::ParseInt32(e.substr(0, e.find(' ')));
				Assert::IsTrue(added[idx]);
			}
		}


		TEST_METHOD(HashSet_Count)
		{
			HashSet<String> m2;
			m2.Add(L"1");
			m2.Add(L"sssssssssssssssssssssssssssssssss");
			m2.Add(L"s");

			Assert::AreEqual(3, m2.getCount());

			m2.Remove(L"1");
			Assert::AreEqual(2, m2.getCount());

			m2.Remove(L"s");
			Assert::AreEqual(1, m2.getCount());

			m2.Add(L"asdsa");
			Assert::AreEqual(2, m2.getCount());

			m2.Add(L"sdasd");
			Assert::AreEqual(3, m2.getCount());

			m2.Add(L"sdsdsaf");
			Assert::AreEqual(4, m2.getCount());

			m2.Remove(L"sssssssssssssssssssssssssssssssss");
			Assert::AreEqual(3, m2.getCount());


			HashSet<String> m3 = m2;
			Assert::AreEqual(3, m3.getCount());

			m3.Clear();
			Assert::AreEqual(0, m3.getCount());

		}

	};

	TEST_CLASS(BitArrayTest)
	{
	public:
		TEST_METHOD(BitArray_Setting)
		{
			const int amount = 1000;
			bool bit[amount] = { 0 };
			BitArray<amount> bitArray;
			for (int32 i = 0; i < amount; i++)
			{
				bit[i] = Randomizer::NextBool();
				bitArray.Set(i, bit[i]);
			}

			for (int32 i = 0; i < amount; i++)
			{
				Assert::AreEqual(bit[i], !!bitArray[i]);
			}

			for (int32 i = 0; i < amount;i++)
			{
				bit[i] = (i & 1) != 0;
				bitArray.Set(i, bit[i]);
			}

			for (int32 i = 0; i < amount; i++)
			{
				Assert::AreEqual(bit[i], !!bitArray[i]);
			}

		}

	};

	TEST_CLASS(ConditionalAccessorTest)
	{
	public:
		TEST_METHOD(ConditionalAccessorTest_Iterate)
		{
			List<int32> lst = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
			List<int32> result;

			for (int32 v : ConditionalAccessor<int32>(lst.begin(), lst.end()))
			{
				result.Add(v);
			}

			Assert::AreEqual(lst.getCount(), result.getCount());
			for (int32 i = 0; i < lst.getCount(); i++)
			{
				Assert::AreEqual(lst[i], result[i]);
			}
		}

		static bool TestCond(int32& r) { return r > 1 && r < 7; }

		TEST_METHOD(ConditionalAccessorTest_TestCond)
		{
			List<int32> lst = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
			List<int32> result;

			for (int32 v : ConditionalAccessor<int32, int32, TestCond>(lst.begin(), lst.end()))
			{
				result.Add(v);
			}

			Assert::AreEqual(5, result.getCount());
			for (int32 v : result)
			{
				Assert::IsTrue(TestCond(v));
			}
		}
	};

	TEST_CLASS(MaterialSetTest)
	{
	public:
		TEST_METHOD(MaterialSetTest_Iterate)
		{
			MeshMaterialSet<int> test;

			test.Add(1);
			test.AddFrame(2, 0);
			test.AddFrame(3, 0);

			test.Add(4);
			test.AddFrame(5, 1);
			test.AddFrame(6, 1);
			test.AddFrame(7, 1);
			test.AddFrame(8, 1);

			test.Add(9);

			test.Add(10);
			test.AddFrame(9, 3);

			List<int32> lst = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 9 };
			List<int32> result;

			for (int32 v : test)
			{
				result.Add(v);
			}

			Assert::AreEqual(lst.getCount(), result.getCount());
			for (int32 i = 0; i < lst.getCount(); i++)
			{
				Assert::AreEqual(lst[i], result[i]);
			}
		}

	};

	TEST_CLASS(QueueTest)
	{
		MemoryService mem;

	public:
		TEST_METHOD(Queue_General) { _TestGeneral<String>(); }
		TEST_METHOD(Queue_GeneralT) { _TestGeneral<TrivialString>(); }
		TEST_METHOD(Queue_GeneralC) { _TestGeneral<NonMovableString>(); }

		TEST_METHOD(Queue_Remove) { _TestRemove<String>(); }
		TEST_METHOD(Queue_RemoveT) { _TestRemove<TrivialString>(); }
		TEST_METHOD(Queue_RemoveC) { _TestRemove<NonMovableString>(); }

		TEST_METHOD(Queue_DeleteAndClear)
		{
			Queue<String*> vals;
			for (int32 i = 0; i < mem.strDataCount; i++)
				vals.Enqueue(new String(mem.stringData[i]));
			vals.DeleteAndClear();
		}

		TEST_METHOD(Queue_RAII)
		{
			FlaggerStates states;

			{
				Queue<Flagger<true>> lst;

				lst.Enqueue({ &states });
				lst.DequeueOnly();
			}
			Assert::AreEqual(1, states.ctorCount);
			Assert::AreEqual(1, states.actualDtorCount);
			Assert::AreEqual(1, states.moveCount);
			Assert::AreEqual(0, states.copyCount);

			states.Clear();
			{
				Queue<Flagger<false>>().Enqueue({ &states });
			}

			Assert::AreEqual(1, states.ctorCount);
			Assert::AreEqual(2, states.actualDtorCount);
			Assert::AreEqual(0, states.moveCount);
			Assert::AreEqual(1, states.copyCount);
		}
	private:
		template <typename S>
		void _TestGeneral()
		{
			for (int32 i = 0; i < 5000; i++)
			{
				List<S> source;
				Queue<S> queue;
				int32 count = Randomizer::NextExclusive(32);
				for (int32 j = 0; j < count; j++)
				{
					auto v = mem.randomString<S>();
					queue.Enqueue(v);
					source.Add(v);
				}

				int32 k = 0;
				for (auto v : queue)
				{
					assert(source[k++] == v);
				}

				Queue<S> moveTgt = std::move(queue);
				assert(queue.getCount() == 0);

				k = 0;
				for (auto v : moveTgt)
				{
					assert(source[k++] == v);
				}
			}
		}

		template <typename S>
		void _TestRemove()
		{
			for (int32 w : {10, 43333 })
			{
				Randomizer::setSeed(w, true);

				Queue<S> testQueue;
				List<S> refList;

				for (int32 k = 0; k < 100; k++)
				{
					int32 cnt = Randomizer::NextExclusive(64);
					for (int32 i = 0; i < cnt; i++)
					{
						if (refList.getCount()>0 &&
							Randomizer::NextBool())
						{
							testQueue.Dequeue();
							refList.RemoveAt(0);
						}
						else
						{
							auto v = mem.randomString<S>();
							testQueue.Enqueue(v);
							refList.Add(v);
						}
					}

					cnt = Randomizer::NextExclusive(4) + 4;
					for (int32 i = 0; i < cnt; i++)
					{
						auto v = mem.randomString<S>();

						testQueue.Enqueue(v);
						refList.Add(v);
					}

					Assert::IsTrue(testQueue.getCount() == refList.getCount());

					int32 ridx = Randomizer::NextExclusive(refList.getCount());

					testQueue.RemoveAt(ridx);
					refList.RemoveAt(ridx);

					Assert::IsTrue(testQueue.getCount() == refList.getCount());

					for (int32 i = 0; i < refList.getCount(); i++)
					{
						Assert::IsTrue(testQueue[i] == refList[i]);
					}
				}
			}
		}
	};

	TEST_CLASS(FixedQueueTest)
	{
		MemoryService mem;

	public:
		TEST_METHOD(FixedQueue_General) { _TestGeneral<String>(); }
		TEST_METHOD(FixedQueue_GeneralT) { _TestGeneral<TrivialString>(); }
		TEST_METHOD(FixedQueue_GeneralC) { _TestGeneral<NonMovableString>(); }

		TEST_METHOD(FixedQueue_Remove) { _TestRemove<String>(); }
		TEST_METHOD(FixedQueue_RemoveT) { _TestRemove<TrivialString>(); }
		TEST_METHOD(FixedQueue_RemoveC) { _TestRemove<NonMovableString>(); }

		TEST_METHOD(FixedQueue_RAII)
		{
			FlaggerStates states1;
			FlaggerStates states2;

			{
				FixedQueue<Flagger<true>, 4> lst;

				lst.Enqueue({ &states1 });

				FixedQueue<Flagger<true>, 4> lst2 = std::move(lst);

				lst2.Clear();
				lst2.Enqueue({ &states2 });

				lst = lst2;
			}

			Assert::AreEqual(1, states1.ctorCount);
			Assert::AreEqual(1, states1.actualDtorCount);
			Assert::AreEqual(2, states1.moveCount);
			Assert::AreEqual(0, states1.copyCount);

			Assert::AreEqual(1, states2.ctorCount);
			Assert::AreEqual(2, states2.actualDtorCount);
			Assert::AreEqual(1, states2.moveCount);
			Assert::AreEqual(1, states2.copyCount);
		}
	private:
		template <typename S>
		void _TestGeneral()
		{
			for (int32 i = 0; i < 5000; i++)
			{
				List<S> source;
				FixedQueue<S, 32> queue;
				int32 count = Randomizer::NextExclusive(32);
				for (int32 j = 0; j < count; j++)
				{
					auto v = mem.randomString<S>();
					queue.Enqueue(v);
					source.Add(v);
				}

				int32 k = 0;
				for (auto v : queue)
				{
					Assert::AreEqual(source[k++], v);
				}

				FixedQueue<S, 32> moveTgt = std::move(queue);
				Assert::AreEqual(0, queue.getCount());

				k = 0;
				for (auto v : moveTgt)
				{
					Assert::AreEqual(source[k++], v);
				}
			}
		}

		template <typename S>
		void _TestRemove()
		{
			for (int32 w : {10, 43333 })
			{
				Randomizer::setSeed(w, true);

				FixedQueue<S, 3600> testQueue;
				List<S> refList;

				for (int32 k = 0; k < 50; k++)
				{
					int32 cnt = Randomizer::NextExclusive(64);
					for (int32 i = 0; i < cnt; i++)
					{
						if (refList.getCount()>0 &&
							Randomizer::NextBool())
						{
							testQueue.Dequeue();
							refList.RemoveAt(0);
						}
						else
						{
							auto v = mem.randomString<S>();
							testQueue.Enqueue(v);
							refList.Add(v);
						}
					}

					cnt = Randomizer::NextExclusive(4) + 4;
					for (int32 i = 0; i < cnt; i++)
					{
						auto v = mem.randomString<S>();

						testQueue.Enqueue(v);
						refList.Add(v);
					}

					Assert::IsTrue(testQueue.getCount() == refList.getCount());

					int32 ridx = Randomizer::NextExclusive(refList.getCount());

					testQueue.RemoveAt(ridx);
					refList.RemoveAt(ridx);

					Assert::IsTrue(testQueue.getCount() == refList.getCount());

					for (int32 i = 0; i < refList.getCount(); i++)
					{
						Assert::IsTrue(testQueue[i] == refList[i]);
					}
				}
			}
		}
	};
}