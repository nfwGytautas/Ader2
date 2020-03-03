using System.Runtime.InteropServices;

namespace Ader2.Core
{
    [StructLayout(LayoutKind.Sequential)]
    public struct Vector2
    {
        public float X;
        public float Y;

        /// <summary>
        /// Construct empty Vector2
        /// </summary>
        public Vector2(float aX, float aY)
        {
            X = aX;
            Y = aY;
        }
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct Vector3
    {
        public float X;
        public float Y;
        public float Z;

        /// <summary>
        /// Construct empty Vector3
        /// </summary>
        public Vector3(float aX, float aY, float aZ)
        {
            X = aX;
            Y = aY;
            Z = aZ;
        }
    }
}
