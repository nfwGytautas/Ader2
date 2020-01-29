using System;
using System.Runtime.CompilerServices;

namespace Ader2.Core
{
    /// <summary>
    /// Asset manager is used to provide the interface for creating graphics
    /// objects such as VAO, Texture, Shader, etc. and also the ability to 
    /// create and store Visuals
    /// </summary>
    public static class AderAssets
    {
        // Instance of the asset manager object
        private static IntPtr _CInstance;

        // Returns the C++ instance of the asset with specified name
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static IntPtr __get(IntPtr manager, string name);

        // Returns true if asset with given name exists
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static bool __has(IntPtr manager, string name);

        /// <summary>
        /// Creates a new instance of the asset and returns it
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="Name"></param>
        /// <returns>Instantiated asset</returns>
        public static T New<T>(string Name) where T : AderAsset, new()
        {
            // Create the asset
            T asset = new T();
            
            // Instantiate the asset
            asset.InstantiateNew(_CInstance, Name);

            // Return the asset
            return asset as T;
        }

        /// <summary>
        /// Get the asset with the specified name
        /// </summary>
        /// <typeparam name="T">Type of the asset</typeparam>
        /// <param name="Name">Name of the asset</param>
        /// <returns>Null if the asset doesn't exist, asset otherwise</returns>
        public static T Get<T>(string Name) where T : AderAsset, new()
        {
            // Create the asset
            T asset = new T();

            // Get instance
            IntPtr cInstance = __get(_CInstance, Name);

            // Return null if asset doesn't exist
            if (cInstance != IntPtr.Zero)
            {
                return null;
            }

            // Instantiate from C++ pointer
            asset.InstantiateFromPtr(cInstance);

            // Return the asset
            return asset;
        }

        /// <summary>
        /// Check if the specified asset exists
        /// </summary>
        /// <param name="Name">Name of the asset</param>
        /// <returns>True if the asset exists, false otherwise</returns>
        public static bool Has(string Name)
        {
            return __has(_CInstance, Name);
        }

        /// <summary>
        /// Internal use only
        /// Returns the C++ instance of the visual
        /// </summary>
        /// <returns>IntPtr to the C++ instance</returns>
        internal static IntPtr GetCInstance()
        {
            return _CInstance;
        }
    }
}
