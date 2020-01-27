using Ader2.Core;
using System;
using System.Runtime.CompilerServices;

namespace Ader2
{
    /// <summary>
    /// Visual is a class that is used to specify a single look of an object
    /// </summary>
    public class Visual
    {
        // Instance of the visual object
        private IntPtr _CInstance;

        /// <summary>
        /// Vertex array of the visual
        /// </summary>
        public VAO VAO { get; set; }

        /// <summary>
        /// Shader of the visual
        /// </summary>
        public Shader Shader { get; set; }

        // Creates new instance of the visual for the specified scene
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static IntPtr __new(IntPtr scene);

        /// <summary>
        /// Create a new visual
        /// </summary>
        public Visual(AderScene scene)
        {
            _CInstance = __new(scene.GetCInstance());

            VAO = new VAO(this);
            Shader = new Shader(this);
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
