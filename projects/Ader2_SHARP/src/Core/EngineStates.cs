using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Ader2.Core
{
    /// <summary>
    /// This class contains the state of the Window
    /// It can be queried from the engine
    /// </summary>
    public class WindowState
    {
        /// <summary>
        /// Width of the main window
        /// </summary>
        public int Width
        {
            get
            {
                return _internal.width;
            }
        }

        /// <summary>
        /// Height of the main window
        /// </summary>
        public int Height
        {
            get
            {
                return _internal.height;
            }
        }

        /// <summary>
        /// Flag used to check if the window is closed or not
        /// </summary>
        public bool Closed
        {
            get
            {
                return _internal.closed;
            }
        }

        /// <summary>
        /// Flag used to check if the window was resized or not
        /// </summary>
        public bool Resized 
        { 
            get 
            {
                return _internal.resized;
            } 
        }

        [StructLayout(LayoutKind.Explicit, Size = 12, Pack = 0)]
        struct Internal
        {
            [FieldOffset(0)]
            public int width;

            [FieldOffset(4)]
            public int height;

            [FieldOffset(8)]
            public bool closed;

            [FieldOffset(9)]
            public bool resized;
        }

        private IntPtr _instance;
        private Internal _internal;

        /// <summary>
        /// Create WindowState from the C++ struct pointer
        /// </summary>
        /// <param name="instance">C++ struct pointer</param>
        public WindowState(System.IntPtr instance)
        {
            _instance = instance;
        }

        /// <summary>
        /// Queries the engine for the current information of the window
        /// </summary>
        public void Query()
        {
            _internal = (Internal)Marshal.PtrToStructure(_instance, typeof(Internal));
        }
    }
}
