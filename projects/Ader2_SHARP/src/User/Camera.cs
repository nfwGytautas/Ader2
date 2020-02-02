using Ader2.Core;
using System;
using System.Runtime.CompilerServices;

namespace Ader2
{
    public class Camera
    {
        // Instance of the game object
        internal IntPtr _CInstance;

        /// <summary>
        /// Position of the camera
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
        /// Rotation of the camera
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

        // Gets the position of the camera
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void __getPosition(IntPtr camera, out Vector3 value);

        // Sets the position of the camera
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void __setPosition(IntPtr camera, ref Vector3 value);

        // Gets the rotation of the camera
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void __getRotation(IntPtr camera, out Vector3 value);

        // Sets the rotation of the camera
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void __setRotation(IntPtr camera, ref Vector3 value);


        public Camera(IntPtr instance)
        {
            _CInstance = instance;
        }

        /// <summary>
        /// Internal use only
        /// Returns the C++ instance of the camera
        /// </summary>
        /// <returns>IntPtr to the C++ instance</returns>
        internal IntPtr GetCInstance()
        {
            return _CInstance;
        }
    }
}
