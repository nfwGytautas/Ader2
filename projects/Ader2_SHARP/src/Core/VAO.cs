using System;
using System.Runtime.CompilerServices;

namespace Ader2.Core
{
    public class VAO : AderAsset
    {
        // Creates new VAO
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static IntPtr __new(IntPtr manager, string name);

        // Sets indices
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void __setIndices(IntPtr instance, uint[] indices);

        // Sets vertices
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void __setVertices(IntPtr instance, float[] vertices);

        // Sets texture coordinates
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void __setUV(IntPtr instance, float[] texCoords);

        public VAO()
        {
        }

        public VAO(IntPtr instance)
        {
            InstantiateFromPtr(instance);
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
        /// Sets the UV coordinates of the array
        /// </summary>
        /// <param name="texCoords">New UV coordinates of the array</param>
        public void SetUV(float[] texCoords)
        {
            __setUV(_CInstance, texCoords);
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
