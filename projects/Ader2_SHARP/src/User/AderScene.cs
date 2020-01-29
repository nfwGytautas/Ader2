using System;
using System.Runtime.CompilerServices;

namespace Ader2
{
    /// <summary>
    /// Attribute used to specify which scene is the starting one.
    /// Other scenes will have to be transitioned to through code.
    /// </summary>
    [AttributeUsage(AttributeTargets.Class, AllowMultiple = false, Inherited = true)]
    public class StartScene : Attribute
    { 
        
    }

    /// <summary>
    /// Scene object is used to provide the ability to design
    /// a single level in a game
    /// </summary>
    public abstract class AderScene
    {
        // Instance of the scene object
        private IntPtr _CInstance;

        // Returns true if asset with given name exists
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static IntPtr __newGameObject(IntPtr scene);

        /// <summary>
        /// This method should load ALL assets used by the scene
        /// </summary>
        public abstract void LoadAssets();

        /// <summary>
        /// Creates a new GameObject with the specified Name
        /// </summary>
        /// <returns>Game object instance</returns>
        public GameObject NewGameObject()
        {
            // Create a new game object and add it to the scene
            GameObject go = new GameObject(__newGameObject(_CInstance));
            return go;
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
