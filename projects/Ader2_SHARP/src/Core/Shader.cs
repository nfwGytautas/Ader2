using System;
using System.Runtime.CompilerServices;

namespace Ader2.Core
{
    public class Shader
    {
        // Instance of the shader object
        private IntPtr _CInstance;

        // Returns the Shader instance of the specified visual
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static IntPtr __get(IntPtr visual);

        // Loads the shader with the specified paths
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void __load(IntPtr instance, string vSource, string fSource);

        /// <summary>
        /// Vertex source of the shader
        /// </summary>
        public string VertexSource { get; set; }

        /// <summary>
        /// Fragment source of the shader
        /// </summary>
        public string FragmentSource { get; set; }

        /// <summary>
        /// Creates a new Shader for the visual
        /// </summary>
        /// <param name="visual"></param>
        public Shader(Visual visual)
        {
            _CInstance = __get(visual.GetCInstance());
        }

        /// <summary>
        /// Loads the shader using the VertexSource and FragmentSource properties
        /// </summary>
        public void Load()
        {
            __load(_CInstance, VertexSource, FragmentSource);
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
