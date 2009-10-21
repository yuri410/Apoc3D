#region Using directives

using System;

#endregion

namespace Apoc.MathLib.Random
{
	/// <summary>
	/// Class to generate pseudorandom numbers
	/// </summary>
	public class MersenneTwister : Random
	{
		#region Constants

		// Period parameters
		
		/// <summary>
		/// TODO
		/// </summary>
		private const int _n = 624;

		/// <summary>
		/// TODO
		/// </summary>
		private const int _m = 397;

		/// <summary>
		/// Constant vector a
		/// </summary>
		private const uint _matrixA = 0x9908b0df;

		/// <summary>
		/// Most significant w-r bits
		/// </summary>
		private const uint _upperMask = 0x80000000;

		/// <summary>
		/// Least significant r bits
		/// </summary>
		private const uint _lowerMask = 0x7fffffff;

		// Tempering parameters

		/// <summary>
		/// TODO
		/// </summary>
		private const uint _temperingMaskB = 0x9d2c5680;

		/// <summary>
		/// TODO
		/// </summary>
		private const uint _temperingMaskC = 0xefc60000;

		#endregion
		
		#region Fields

		/// <summary>
		/// The array for the state vector
		/// </summary>
		private uint[] _mt = new uint[_n];

		/// <summary>
		/// TODO
		/// </summary>
		private short _mti;

		/// <summary>
		/// TODO
		/// </summary>
		private static uint[] _mag01 = { 0x0, _matrixA };

		#endregion

		#region Constructors

		/// <summary>
		/// Default constructor
		/// </summary>
		/// <remarks>A default initial seed is used</remarks>
		public MersenneTwister() : this(4357)
		{}

		// 
		/// <summary>
		/// Overloaded constructor initializing the array with a NONZERO seed
		/// </summary>
		/// <param name="seed">The initial seed</param>
		public MersenneTwister(uint seed)
		{
			// setting initial seeds to mt[N] using
			// the generator Line 25 of Table 1 in
			// [KNUTH 1981, The Art of Computer Programming
			// Vol. 2 (2nd Ed.), pp102]
			_mt[0] = seed & 0xffffffffU;
			for (_mti = 1; _mti < _n; ++_mti)
				_mt[_mti] = (69069*_mt[_mti - 1]) & 0xffffffffU;
		}

		#endregion Constructores

		#region Methods

		/// <summary>
		/// TODO
		/// </summary>
		/// <param name="y">TODO</param>
		/// <returns>TODO</returns>
		private static uint temperingShiftU(uint y) { return (y >> 11); }
		
		/// <summary>
		/// TODO
		/// </summary>
		/// <param name="y">TODO</param>
		/// <returns>TODO</returns>
		private static uint temperingShiftS(uint y) { return (y << 7); }
		
		/// <summary>
		/// TODO
		/// </summary>
		/// <param name="y">TODO</param>
		/// <returns>TODO</returns>
		private static uint temperingShiftT(uint y) { return (y << 15); }
		
		/// <summary>
		/// TODO
		/// </summary>
		/// <param name="y">TODO</param>
		/// <returns>TODO</returns>
		private static uint temperingShiftL(uint y) { return (y >> 18); }

		/// <summary>
		/// TODO
		/// </summary>
		/// <returns>TODO</returns>
		protected uint GenerateUnsignedInt()
		{
			uint y;

			// mag01[x] = x * MATRIX_A  for x=0,1
			if (_mti >= _n) // generate N words at one time
			{
				short kk = 0;

				for (; kk < _n - _m; ++kk)
				{
					y = (_mt[kk] & _upperMask) | (_mt[kk + 1] & _lowerMask);
					_mt[kk] = _mt[kk + _m] ^ (y >> 1) ^ _mag01[y & 0x1];
				}

				for(;kk < _n - 1; ++kk)
				{
					y = (_mt[kk] & _upperMask) | (_mt[kk + 1] & _lowerMask);
					_mt[kk] = _mt[kk+(_m - _n)] ^ (y >> 1) ^ _mag01[y & 0x1];
				}

				y = (_mt[_n - 1] & _upperMask) | (_mt[0] & _lowerMask);
				_mt[_n - 1] = _mt[_m - 1] ^ (y >> 1) ^ _mag01[y & 0x1];

				_mti = 0;
			}

			y = _mt[_mti++];
			y ^= temperingShiftU(y);
			y ^= temperingShiftS(y) & _temperingMaskB;
			y ^= temperingShiftT(y) & _temperingMaskC;
			y ^= temperingShiftL(y);

			return y;
		}

		/// <summary>
		/// TODO
		/// </summary>
		/// <returns>TODO</returns>
		public virtual uint NextUnsignedInt()
		{
			return GenerateUnsignedInt();
		}

		/// <summary>
		/// TODO
		/// </summary>
		/// <param name="maxValue">TODO</param>
		/// <returns>TODO</returns>
		public virtual uint NextUnsignedInt(uint maxValue)
		{
			return (uint)(GenerateUnsignedInt() / ((double)uint.MaxValue / maxValue));
		}

		/// <summary>
		/// TODO
		/// </summary>
		/// <param name="minValue">TODO</param>
		/// <param name="maxValue">TODO</param>
		/// <returns>TODO</returns>
		public virtual uint NextUnsignedInt(uint minValue, uint maxValue)
		{
			if (minValue >= maxValue)
				throw new ArgumentOutOfRangeException("Min value can´t be bigger than max value");

			return (uint)(GenerateUnsignedInt() / ((double)uint.MaxValue / (maxValue - minValue)) + minValue);
		}

		/// <summary>
		/// TODO
		/// </summary>
		/// <param name="maxValue">TODO</param>
		/// <returns>TODO</returns>
		public virtual float NextFloat(float maxValue)
		{
			if (maxValue < 0.0f)
				throw new ArgumentOutOfRangeException("Max value must be equal or bigger than 0.0f");

			return (float)(NextDouble() * maxValue);
		}

		/// <summary>
		/// TODO
		/// </summary>
		/// <param name="minValue">TODO</param>
		/// <param name="maxValue">TODO</param>
		/// <returns></returns>
		public float NextFloat(float minValue, float maxValue)
		{
			if (maxValue < minValue)
			{
				throw new ArgumentOutOfRangeException("Min value can´t be bigger than max value");
			}
			
			else if (maxValue == minValue)
			{
				return minValue;
			}
			
			else
			{
				return NextFloat(maxValue - minValue) + minValue;
			}
		}

		/// <summary>
		/// TODO
		/// </summary>
		/// <param name="maxValue">TODO</param>
		/// <returns>TODO</returns>
		public virtual double NextDouble(double maxValue)
		{
			if (maxValue < 0.0f)
				throw new ArgumentOutOfRangeException("Max value must be equal or bigger than 0.0f");


			return (NextDouble() * maxValue);
		}

		/// <summary>
		/// TODO
		/// </summary>
		/// <param name="minValue">TODO</param>
		/// <param name="maxValue">TODO</param>
		/// <returns>TODO</returns>
		public double NextDouble(double minValue, double maxValue)
		{
			if (maxValue < minValue)
			{
				throw new ArgumentOutOfRangeException("Min value can´t be bigger than max value");
			}
			
			else if (maxValue == minValue)
			{
				return minValue;
			}
			
			else
			{
				return NextDouble(maxValue - minValue) + minValue;
			}
		}

		/// <summary>
		/// TODO
		/// </summary>
		/// <returns>TODO</returns>
		public override int Next()
		{
			return Next(int.MaxValue);
		}

		/// <summary>
		/// TODO
		/// </summary>
		/// <param name="maxValue">TODO</param>
		/// <returns>TODO</returns>
		public override int Next(int maxValue)
		{
			if (maxValue <= 1)
			{
				if (maxValue < 0)
				{
					throw new ArgumentOutOfRangeException("Max value must be equal or bigger than 0");
				}

				return 0;
			}

			return (int)(NextDouble() * maxValue);
		}

		/// <summary>
		/// TODO
		/// </summary>
		/// <param name="minValue">TODO</param>
		/// <param name="maxValue">TODO</param>
		/// <returns>TODO</returns>
		public new int Next(int minValue, int maxValue)
		{
			if (maxValue < minValue)
			{
				throw new ArgumentOutOfRangeException("Min value can´t be bigger than max value");
			}
			
			else if(maxValue == minValue)
			{
				return minValue;
			}
			
			else
			{
				return Next(maxValue - minValue) + minValue;
			}
		}

		/// <summary>
		/// TODO
		/// </summary>
		/// <param name="buffer">TODO</param>
		public override void NextBytes(byte[] buffer)
		{
			int bufLen = buffer.Length;

			if (buffer == null)
				throw new ArgumentNullException();

			for (int idx = 0; idx < bufLen; ++idx)
				buffer[idx] = (byte) Next(256);
		}

		/// <summary>
		/// TODO
		/// </summary>
		/// <returns>TODO</returns>
		public override double NextDouble()
		{
			return (double) GenerateUnsignedInt() / ((ulong)uint.MaxValue + 1);
		}

		#endregion
	}
}
