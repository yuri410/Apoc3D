#region Using Directives

using System;

#endregion

namespace Apoc.MathLib.Random
{
    /// <summary>
    /// A uniformly-distributed, extremely long period (> 2.3 * 10^18)
    /// pseudo-random number generator.
    /// </summary>
    /// <remarks>
    /// Linear congruential generator
    /// I(j+1) = aI(j) + c (mod m)
    /// 
    /// Simple multiplicative congruential generator
    /// I(j+1) = aI(j) (mod m)
    /// 
    ///</remarks>
    public class JUniformDeviate : Random
    {
        #region Fields

        /// <summary>M1 - 1 = 2 x 3 x 7 x 631 x 81031</summary>
        private const int _m1 = 2147483563;
        private const int _a1 = 40014;

        private const int _q1 = 53668;
        private const int _r1 = 12211;

        /// <summary>M2 - 1 = 2 x 19 x 31 x 1019 x 1789</summary>
        private const int _m2 = 2147483399;
        private const int _a2 = 40692;

        private const int _q2 = 52774;
        private const int _r2 = 3791;

        private const double _am = (1.0 / _m1);
        private const int _ntab = 32;
        private const int _ndiv = (1 + _m1 / _ntab);

        // smallest representable number such that 1.0+_doubleEpsilon != 1.0
        private const double _doubleEpsilon = 2.2204460492503131e-016;

        /// <summary>
        /// _largestFloatingValueLT1 should approximate the largest floating value that is less than 1.
        /// </summary>
        private const double _largestFloatingValueLT1 = (1.0 - _doubleEpsilon);

        private int _idum;
        private int _idum2;
        private int _iy;
        private int[] _iv = new int[_ntab];

        private static double[] _cof;

        #endregion

        #region Properties
        /// <summary>
        /// Coefficients for gamma function
        /// </summary>
        protected static double[] Cof
        {
            get { return JUniformDeviate._cof; }
            set { JUniformDeviate._cof = value; }
        }
        #endregion

        #region Constructors

        /// <summary>
        /// Instantiates with a random seed based on date/time.
        /// </summary>
        public JUniformDeviate()
        {
            ReloadShuffleTable(Environment.TickCount + 1);
        }

        /// <summary>
        /// Instantiates with a specific seed for a repeatable sequence.
        /// </summary>
        /// <param name="newSeed">Initial seed for generator</param>
        public JUniformDeviate(int newSeed)
        {
            ReloadShuffleTable(newSeed);
        }

        #endregion

        #region Methods

        /// <summary>
        /// Provides a uniform deviate that lies within the range 0.0 to 1.0 
        /// (exclusive of the endpoint values).
        /// </summary>
        /// <remarks>
        /// This generator produces an extended random sequence by combining two
        /// different sequences with different periods so as to obtain a new sequence
        /// whose period is the least common multiple of the two periods using the
        /// L'Ecuyer method (1988, Communications of the ACM, vol. 31, pp 742-774)
        /// with Bays-Durham shuffle and added safeguards.
        /// 
        /// Code derived from Numerical Recipes in C, Second edition, p 282.
        /// </remarks>
        protected override double Sample()
        {

            // Schrage's algorithm is based on an approximate factorization of m,
            // m = aq + r, i.e., q = [m/a], r = m mod a
            // with square brackets denoting integer part
            //
            // When r is small, specifically r < q, and 0 < z < m - 1,
            // then both a(z mod q) and r[z/q] lie in the range 0, ..., m - 1
            // and that
            // 
            // az mod m = a(z mod q) - r[z/q]       if it is >= 0
            //            a(z mod q) - r[z/q] + m   otherwise

            int k = _idum / _q1;
            _idum = _a1 * (_idum - k * _q1) - k * _r1;   // Compute idum = (A1 * idum) % M1
            if (_idum < 0)                       //  without overflows by Schrage's method
                _idum += _m1;

            k = _idum2 / _q2;
            _idum2 = _a2 * (_idum2 - k * _q2) - k * _r2; // Compute idum2 = (A2 * idum2) % M2 likewise
            if (_idum2 < 0)
                _idum2 += _m2;

            int j = _iy / _ndiv;                  // Will be in the range 0..NTAB-1
            _iy = _iv[j] - _idum2;                 // Here idum is shuffled, idum and idum2 are
            _iv[j] = _idum;                       //  combined to generate output
            if (_iy < 1)
                _iy += _m1 - 1;

            double temp = _am * _iy;

            if (temp > _largestFloatingValueLT1)
                return _largestFloatingValueLT1;     // Because users don't expect endpoint values
            else
                return temp;
        }

        /// <summary>
        /// Reload the shuffle table. We use the shuffle table
        /// to remove low-order serial correlations in the random deviates. A random deviate
        /// derived from the j-th value in the sequence, I(j), is output not on the j-th call
        /// but rather on a randomized later call, j + 32 on average. The shuffling algorithm
        /// is due to Bays and Durham as described in Knuth (1981, Seminumerical Algorithms,
        /// 2nd ed. vol. 2 of The Art of Computer Programming, 3.2-3.3).
        /// </summary>
        protected void ReloadShuffleTable(int newSeed)
        {
            if (0 == newSeed)
                throw new ArgumentOutOfRangeException("newSeed", "New seed value cannot be zero.");

            _idum = newSeed;
            if (_idum <= 0)
            {                        // Initialize
                _idum = -_idum;
                _idum2 = _idum;
            }

            for (int j = _ntab + 7; j >= 0; j--)
            {   // Load the shuffle table (after 8 warm-ups)
                int k = _idum / _q1;                  // k = [z/q]
                _idum = _a1 * (_idum - k * _q1) - k * _r1;   // idum - k*Q1 = z mod q
                if (_idum < 0)
                    _idum += _m1;

                if (j < _ntab)                       // Fill shuffle table entry
                    _iv[j] = _idum;
            }
            _iy = _iv[0];
        }

        /// <summary>
        /// Return the natural log of the gamma function ln[gamma(xx)]
        /// </summary>
        protected double LnGamma(double xx)
        {
            if (Cof == null)
            {
                Cof = new double[6];

                Cof[0] = 76.18009172947146;
                Cof[1] = -86.50532032941677;
                Cof[2] = 24.01409824083091;
                Cof[3] = -1.231739572450155;
                Cof[4] = 0.1208650973866179e-2;
                Cof[5] = -0.5395239384953e-5;
            }

            double x = xx;
            double y = x;

            double tmp = x + 5.5;
            tmp -= (x + 0.5) * Math.Log(tmp);

            double ser = 1.000000000190015;

            for (int j = 0; j <= 5; j++)
                ser += Cof[j] / ++y;
            return -tmp + Math.Log(2.5066282746310005 * ser / x);
        }

        #endregion
    }
}
