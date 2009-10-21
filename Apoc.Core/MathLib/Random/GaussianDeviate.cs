#region Using Directives

using System;

#endregion

namespace Apoc.MathLib.Random
{
    /// <summary>
    /// This class generates random numbers using a gaussian distribution with
    /// zero mean and unit variance.
    /// </summary>
    public class GaussianDeviate : JUniformDeviate
    {
        #region Fields

        private bool _bGaussianSet;
        private double _dCachedGaussian;

        #endregion

        #region Constructors

        /// <summary>
        /// Create a Gaussian-distributed generator with zero mean and unit variance.
        /// Start the psuedo-random sequence using a random seed.
        /// </summary>
        public GaussianDeviate()
        {
            _bGaussianSet = false;
        }

        /// <summary>
        /// Create a Gaussian-distributed generator with zero mean and unit variance.
        /// Start the psuedo-random sequence using the specified seed.
        /// </summary>
        /// <param name="newSeed">The initial seed for the generator.</param>
        public GaussianDeviate(int newSeed)
            : base(newSeed)
        {
            _bGaussianSet = false;
        }

        #endregion

        #region Methods

        /// <summary>
        /// Provides a Gaussian deviate with zero mean and unit variance.
        /// </summary>
        protected override double Sample()
        {
            double fac, rsq, v1, v2;

            if (!_bGaussianSet)
            {      // There is no extra deviate handy so
                do
                {
                    // Select two uniform deviates in the unit square
                    v1 = 2.0 * base.Sample() - 1.0;
                    v2 = 2.0 * base.Sample() - 1.0;
                    rsq = v1 * v1 + v2 * v2;          // See if they are in the unit circle
                } while (rsq >= 1.0 || rsq == 0.0); //  and if not, try again

                fac = Math.Sqrt(-2.0 * Math.Log(rsq) / rsq);

                // Now make the Box-Muller transformation to get two gaussian (normal) deviates
                // Return one and cache the other for the next call
                _dCachedGaussian = v2 * fac;
                _bGaussianSet = true;
                return v1 * fac;
            }
            _bGaussianSet = false;
            return _dCachedGaussian;
        }

        #endregion
    }
}
