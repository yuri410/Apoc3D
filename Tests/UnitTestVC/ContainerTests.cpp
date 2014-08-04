
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTestVC
{		
	TEST_CLASS(ListTest)
	{
	public:
		
		TEST_METHOD(ListReverse)
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

		TEST_METHOD(ListSort)
		{
			List<int> subject(100);
			int counter[1000];
			memset(counter, 0, sizeof(counter));

			for (int i=0;i<100;i++)
			{
				int val = Randomizer::NextExclusive(1000);
				subject.Add(val);
				counter[val]++;
			}

			subject.Sort();


			for (int i=0;i<subject.getCount()-1;i++)
			{
				if (subject[i+1] < subject[i])
				{
					Assert::Fail();
				}
			}

			int counter2[1000];
			memset(counter2, 0, sizeof(counter2));
			for (int i=0;i<subject.getCount();i++)
			{
				counter2[subject[i]]++;
			}

			Assert::IsTrue(memcmp(counter, counter2, sizeof(counter)) == 0);
		}


		TEST_METHOD(ListSort2)
		{
			List<int> subject(100);
			int counter[1000];
			memset(counter, 0, sizeof(counter));

			for (int i=0;i<100;i++)
			{
				int val = Randomizer::NextExclusive(1000);
				subject.Add(val);
				counter[val]++;
			}

			subject.Sort(OrderComparer);


			for (int i=0;i<subject.getCount()-1;i++)
			{
				if (subject[i+1] < subject[i])
				{
					Assert::Fail();
				}
			}

			int counter2[1000];
			memset(counter2, 0, sizeof(counter2));
			for (int i=0;i<subject.getCount();i++)
			{
				counter2[subject[i]]++;
			}

			Assert::IsTrue(memcmp(counter, counter2, sizeof(counter))==0);
		}

		TEST_METHOD(HashMapIterator)
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

		}
	};
}