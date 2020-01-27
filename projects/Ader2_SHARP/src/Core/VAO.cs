using System;
using System.Runtime.CompilerServices;

namespace Ader2.Core
{
    public class VAO
    {
        // Instance of the VAO object
        private IntPtr _CInstance;

        // Returns the VAO instance of the specified visual
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static IntPtr __get(IntPtr visual);

        // Sets indices
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void __setIndices(IntPtr instance, uint[] indices);

        // Sets vertices
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void __setVertices(IntPtr instance, float[] vertices);

        /// <summary>
        /// Creates a new VAO for the visual
        /// </summary>
        /// <param name="visual"></param>
        public VAO(Visual visual)
        {
            _CInstance = __get(visual.GetCInstance());
        }

        /// <summary>
        /// Sets the indices of the array
        /// </summary>
        /// <param name="indices">New indices of the array</param>
        public void SetIndices(uint[] indices)
        {
            __setIndices(_CInstance, indices);
        }

        /// <summary>
        /// Sets the vertices of the array
        /// </summary>
        /// <param name="vertices">New vertices of the array</param>
        public void SetVertices(float[] vertices)
        {
            __setVertices(_CInstance, vertices);
        }

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
