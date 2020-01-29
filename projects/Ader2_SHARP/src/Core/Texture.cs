using System;
using System.Runtime.CompilerServices;

namespace Ader2.Core
{
    public class Texture : AderAsset
    {
        /// <summary>
        /// Image source of the texture
        /// </summary>
        public string Source { get; set; }

        // Creates new Texture
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static IntPtr __new(IntPtr manager, string name);

        // Loads the shader with the specified paths
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void __load(IntPtr instance, string source);

        public Texture()
        {
        }

        public Texture(IntPtr instance)
        {
            InstantiateFromPtr(instance);
        }

        /// <summary>
        /// Loads the shader using the VertexSource and FragmentSource properties
        /// </summary>
        public void Load()
        {
            __load(_CInstance, Source);
        }

        protected internal override void InstantiateNew(IntPtr manager, string name)
        {
            _CInstance = __new(manager, name);
        }

        protected internal override void InstantiateFromPtr(IntPtr ptr)
        {
            _CInstance = ptr;
        }
    }
}
