using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Ader2_SHARP
{
    class Direct
    {
        [StructLayout(LayoutKind.Explicit, Size = 8, Pack = 0)]
        struct Internal
        {
            [FieldOffset(0)]
            public int a;

            [FieldOffset(4)]
            public int b;
        }

        private IntPtr _instance;
        private Internal _internal;

        public Direct(System.IntPtr ptr)
        {
            Console.WriteLine("Creating direct interface");
            _instance = ptr;
            Console.WriteLine(ptr.ToString());
            Console.WriteLine(_instance.ToString());
            Console.WriteLine("Marshaling");
            try
            {
                _internal = (Internal)Marshal.PtrToStructure(_instance, typeof(Internal));
                Console.WriteLine(_internal.a);
                Console.WriteLine(_internal.b);
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }
            Console.WriteLine("Done");
        }

        public int A
        {
            get
            {
                return _internal.a;
            }

            set
            {
                _internal.a = value;
                setPtr();
            }
        }

        public int B
        {
            get
            {
                return _internal.b;
            }

            set
            {
                _internal.b = value;
                setPtr();
            }
        }

        public void testMethod(int aVal, int bVal)
        {
            Console.WriteLine($"Parameters: a: {aVal}  b:{bVal}");
            Console.WriteLine("Test method A");
            A = aVal;
            Console.WriteLine("Test method B");
            B = bVal;
        }

        public void setPtr()
        {
            Marshal.StructureToPtr(_internal, _instance, true);
        }
    }

    class Hello
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static string GetMessage();

        string msg = "Test";

        public Hello()
        {
            Console.WriteLine("CTOR Default");
        }

        public Hello(string _msg)
        {
            Console.WriteLine("CTOR Message");
            msg = _msg;
        }

        static void OUT()
        {
            Console.WriteLine(GetMessage());
        }

        void OUT_INSTANCE()
        {
            Console.WriteLine(msg);
        }
    }
}
