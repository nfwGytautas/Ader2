using Ader2.Core;
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

        /// <summary>
        /// Audio listener of this scene
        /// </summary>
        public AudioListener AudioListener { get; private set; }

        /// <summary>
        /// Current active camera of this scene
        /// </summary>
        public Camera ActiveCamera
        { 
            get
            {
                return new Camera(__getActiveCamera(_CInstance));
            }
            set
            {
                __setActiveCamera(_CInstance, value.GetCInstance());
            }
        }

        // Creates a new game object
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static IntPtr __newGameObject(IntPtr scene);

        // Creates a new camera
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static IntPtr __newCamera(IntPtr scene);

        // Gets the active camera of the scene
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static IntPtr __getActiveCamera(IntPtr scene);

        // Sets the active camera of the scene
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void __setActiveCamera(IntPtr scene, IntPtr camera);

        /// <summary>
        /// Create ader scene object
        /// </summary>
        public AderScene()
        {
            AudioListener = new AudioListener(this);
        }

        /// <summary>
        /// This method should load ALL assets used by the scene
        /// </summary>
        public abstract void LoadAssets();

        /// <summary>
        /// Creates a new GameObject
        /// </summary>
        /// <returns>Game object instance</returns>
        public GameObject NewGameObject()
        {
            // Create a new game object and add it to the scene
            GameObject go = new GameObject(__newGameObject(_CInstance));
            return go;
        }

        /// <summary>
        /// Creates a new Camera
        /// </summary>
        /// <returns>Game object instance</returns>
        public Camera NewCamera()
        {
            // Create a new camera and add it to the scene
            Camera cam = new Camera(__newCamera(_CInstance));
            return cam;
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
