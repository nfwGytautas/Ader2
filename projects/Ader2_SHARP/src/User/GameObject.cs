using System;
using System.Runtime.CompilerServices;

namespace Ader2
{
    public class GameObject
    {
        // Instance of the game object
        internal IntPtr _CInstance;

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
