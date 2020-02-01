using Ader2.Core;
using System;
using System.Runtime.CompilerServices;

namespace Ader2
{
    public class GameObject
    {
        // Instance of the game object
        internal IntPtr _CInstance;

        /// <summary>
        /// Position of the game object
        /// </summary>
        public Vector3 Position
        {
            get
            {
                Vector3 value;
                __getPosition(_CInstance, out value);
                return value;
            }

            set
            {
                __setPosition(_CInstance, ref value);
            }
        }

        /// <summary>
        /// Rotation of the game object
        /// </summary>
        public Vector3 Rotation
        {
            get
            {
                Vector3 value;
                __getRotation(_CInstance, out value);
                return value;
            }

            set
            {
                __setRotation(_CInstance, ref value);
            }
        }

        /// <summary>
        /// Scale of the game object
        /// </summary>
        public Vector3 Scale
        {
            get
            {
                Vector3 value;
                __getScale(_CInstance, out value);
                return value;
            }

            set
            {
                __setScale(_CInstance, ref value);
            }
        }

        /// <summary>
        /// Visual of the object
        /// </summary>
        public Visual Visual
        {
            get
            {
                return new Visual(__getVisual(_CInstance));
            }
            set
            {
                __setVisual(_CInstance, value.GetCInstance());
            }
        }

        // Returns visual of the game object
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static IntPtr __getVisual(IntPtr gObject);

        // Sets the visual of the game object
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void __setVisual(IntPtr gObject, IntPtr visual);

        // Gets the position of the game object
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void __getPosition(IntPtr gObject, out Vector3 value);

        // Sets the position of the game object
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void __setPosition(IntPtr gObject, ref Vector3 value);

        // Gets the rotation of the game object
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void __getRotation(IntPtr gObject, out Vector3 value);

        // Sets the rotation of the game object
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void __setRotation(IntPtr gObject, ref Vector3 value);

        // Gets the scale of the game object
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void __getScale(IntPtr gObject, out Vector3 value);

        // Sets the scale of the game object
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void __setScale(IntPtr gObject, ref Vector3 value);


        public GameObject(IntPtr instance)
        {
            _CInstance = instance;
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
