﻿using Ader2.Core;
using System;
using System.Runtime.CompilerServices;

namespace Ader2
{
    /// <summary>
    /// Visual is a class that is used to specify a single look of an object
    /// </summary>
    public class Visual : AderAsset
    {
        /// <summary>
        /// Vertex array of the visual
        /// </summary>
        public VAO VAO
        {
            get
            {
                return new VAO(__getVAO(_CInstance));
            }
            set
            {
                __setVAO(_CInstance, value.GetCInstance());
            }
        }

        /// <summary>
        /// Shader of the visual
        /// </summary>
        public Shader Shader
        {
            get
            {
                return new Shader(__getShader(_CInstance));
            }
            set
            {
                __setShader(_CInstance, value.GetCInstance());
            }
        }

        /// <summary>
        /// Sets the visual texture slot to the specified texture
        /// </summary>
        /// <param name="Slot">Texture slot</param>
        /// <param name="Texture">The texture to assign</param>
        public void SetTexture(int Slot, Texture Texture)
        {
            __setTexture(_CInstance, Slot, Texture.GetCInstance());
        }

        /// <summary>
        /// Get the Texture of the visual at the specified slot
        /// </summary>
        /// <param name="Slot">Texture slot</param>
        /// <returns>Texture instance</returns>
        public Texture GetTexture(int Slot)
        {
            return new Texture(__getTexture(_CInstance, Slot));
        }

        // Creates new Visual
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static IntPtr __new(IntPtr manager, string name);

        // Sets visual VAO
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void __setVAO(IntPtr visual, IntPtr vao);

        // Sets visual Shader
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void __setShader(IntPtr visual, IntPtr shader);

        // Sets visual Texture
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void __setTexture(IntPtr visual, int slot, IntPtr texture);

        // Returns visual VAO
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static IntPtr __getVAO(IntPtr visual);

        // Returns visual Shader
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static IntPtr __getShader(IntPtr visual);

        // Returns visual Textures
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static IntPtr __getTexture(IntPtr visual, int slot);

        public Visual()
        {
        }

        public Visual(IntPtr instance)
        {
            InstantiateFromPtr(instance);
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
