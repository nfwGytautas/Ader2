using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Ader2.Core
{
    /// <summary>
    /// Interface of a Ader asset
    /// </summary>
    public abstract class AderAsset
    {
        // Instance of the asset
        protected internal IntPtr _CInstance;

        /// <summary>
        /// Create a new instance of the asset with the specified name
        /// </summary>
        /// <param name="manager">Asset manager C++ instance</param>
        /// <param name="name">Name of the asset</param>
        protected internal abstract void InstantiateNew(IntPtr manager, string name);

        /// <summary>
        /// Create the asset from the specified C++ instance pointer
        /// </summary>
        /// <param name="ptr">C++ instance</param>
        protected internal abstract void InstantiateFromPtr(IntPtr ptr);

        /// <summary>
        /// Internal use only
        /// Returns the C++ instance of the visual
        /// </summary>
        /// <returns>IntPtr to the C++ instance</returns>
        internal IntPtr GetCInstance()
        {
            return _CInstance;
        }
    }
}
