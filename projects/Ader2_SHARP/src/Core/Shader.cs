﻿using System;
using System.Runtime.CompilerServices;

namespace Ader2.Core
{
    public class Shader : AderAsset
    {
        /// <summary>
        /// Vertex source of the shader
        /// </summary>
        public string VertexSource { get; set; }

        /// <summary>
        /// Fragment source of the shader
        /// </summary>
        public string FragmentSource { get; set; }

        // Creates new shader
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static IntPtr __new(IntPtr manager, string name);

        // Loads the shader with the specified paths
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void __load(IntPtr instance, string vSource, string fSource);

        public Shader()
        {
        }

        public Shader(IntPtr instance)
        {
            InstantiateFromPtr(instance);
        }

        /// <summary>
        /// Loads the shader using the VertexSource and FragmentSource properties
        /// </summary>
        public void Load()
        {
            __load(_CInstance, VertexSource, FragmentSource);
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
