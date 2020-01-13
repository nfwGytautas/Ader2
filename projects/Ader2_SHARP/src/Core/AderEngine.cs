using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace Ader2.Core
{
    /// <summary>
    /// The main GLUE class for the C# side of the engine
    /// </summary>
    static public class AderEngine
    {
        /// <summary>
        /// Create a state object of specified type return null if it fails or T
        /// is invalid parameter
        /// </summary>
        /// <typeparam name="T">Type of state</typeparam>
        /// <returns>Non queried T state instance</returns>
        public static T GetStateHook<T>() where T : class
        {
            // Check if WindowState
            if (typeof(WindowState).IsAssignableFrom(typeof(T)))
            {
                return new WindowState(_wndState) as T;
            }

            // State was not found
            return null;
        }
        
        // Window state
        private static IntPtr _wndState = IntPtr.Zero;

        // Keyboard state
        private static IntPtr _keyState = IntPtr.Zero;
    }
}
