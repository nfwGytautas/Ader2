using System;
using System.Runtime.CompilerServices;

namespace Ader2.Core
{
    /// <summary>
    /// Listener is used to specify the point and orientation of
    /// the listening object
    /// </summary>
    public class AudioListener
    {
        // Instance of the scene object
        IntPtr _SceneCInstance;

        /// <summary>
        /// Position of the audio listener
        /// </summary>
        public Vector3 Position 
        {
            get
            {
                Vector3 value;
                __getPosition(_SceneCInstance, out value);
                return value;
            }

            set
            {
                __setPosition(_SceneCInstance, ref value);
            }
        }

        /// <summary>
        /// Volume for the listener, changing this changes all
        /// received volume by this amount
        /// </summary>
        public float Volume
        {
            get
            {
                float value;
                __getVolume(_SceneCInstance, out value);
                return value;
            }

            set
            {
                __setVolume(_SceneCInstance, ref value);
            }
        }

        /// <summary>
        /// Target of the sound
        /// </summary>
        public Vector3 OrientationAt
        {
            get
            {
                Vector3 value;
                __getOrientationAt(_SceneCInstance, out value);
                return value;
            }

            set
            {
                __setOrientationAt(_SceneCInstance, ref value);
            }
        }

        /// <summary>
        /// Similar to up value of the camera
        /// </summary>
        public Vector3 OrientationUp
        {
            get
            {
                Vector3 value;
                __getOrientationUp(_SceneCInstance, out value);
                return value;
            }

            set
            {
                __setOrientationUp(_SceneCInstance, ref value);
            }
        }

        /// <summary>
        /// Create audio listener for the specified scene
        /// </summary>
        /// <param name="Scene">Scene of this audio listener</param>
        public AudioListener(AderScene Scene)
        {
            _SceneCInstance = Scene.GetCInstance();
        }

        // Gets the position of the audio listener
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void __getPosition(IntPtr scene, out Vector3 value);

        // Sets the position of the audio listener
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void __setPosition(IntPtr scene, ref Vector3 value);

        // Gets the volume of the audio listener
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void __getVolume(IntPtr scene, out float value);

        // Sets the volume of the audio listener
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void __setVolume(IntPtr scene, ref float value);

        // Gets the 'at' component of the orientation of the audio listener
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void __getOrientationAt(IntPtr scene, out Vector3 value);

        // Sets the 'at' component of the orientation of the audio listener
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void __setOrientationAt(IntPtr scene, ref Vector3 value);

        // Gets the 'up' component of the orientation of the audio listener
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void __getOrientationUp(IntPtr scene, out Vector3 value);

        // Sets the 'up' component of the orientation of the audio listener
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void __setOrientationUp(IntPtr scene, ref Vector3 value);
    }

    /// <summary>
    /// Audio object used to load, play and manipulate audio files
    /// </summary>
    public class Audio : AderAsset
    {
        /// <summary>
        /// Audio file source
        /// </summary>
        public string Source { get; set; }

        /// <summary>
        /// Pitch of this audio object
        /// </summary>
        public float Pitch
        {
            get
            {
                float value;
                __getPitch(_CInstance, out value);
                return value;
            }

            set
            {
                __setPitch(_CInstance, ref value);
            }
        }

        /// <summary>
        /// Volume of this audio object
        /// </summary>
        public float Volume
        {
            get
            {
                float value;
                __getVolume(_CInstance, out value);
                return value;
            }

            set
            {
                __setVolume(_CInstance, ref value);
            }
        }

        /// <summary>
        /// Position of this audio object
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
        /// Velocity of this audio object
        /// </summary>
        public Vector3 Velocity
        {
            get
            {
                Vector3 value;
                __getVelocity(_CInstance, out value);
                return value;
            }

            set
            {
                __setVelocity(_CInstance, ref value);
            }
        }

        /// <summary>
        /// Velocity of this audio object
        /// </summary>
        public bool Looping
        {
            get
            {
                bool value;
                __getLooping(_CInstance, out value);
                return value;
            }

            set
            {
                __setLooping(_CInstance, ref value);
            }
        }

        // Creates new audio object
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static IntPtr __new(IntPtr manager, string name);

        // Loads the shader with the specified paths
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void __load(IntPtr instance, string source);

        // Play audio
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void __play(IntPtr instance);

        // Pause audio
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void __pause(IntPtr instance);

        // Stop audio
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void __stop(IntPtr instance);

        // Gets the pitch of audio
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void __getPitch(IntPtr scene, out float value);

        // Sets the pitch of audio
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void __setPitch(IntPtr scene, ref float value);

        // Gets the volume of audio
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void __getVolume(IntPtr scene, out float value);

        // Sets the volume of audio
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void __setVolume(IntPtr scene, ref float value);

        // Gets the position of audio
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void __getPosition(IntPtr scene, out Vector3 value);

        // Sets the position of audio
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void __setPosition(IntPtr scene, ref Vector3 value);

        // Gets the velocity of audio
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void __getVelocity(IntPtr scene, out Vector3 value);

        // Sets the velocity of audio
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void __setVelocity(IntPtr scene, ref Vector3 value);

        // Gets the loop value of audio
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void __getLooping(IntPtr scene, out bool value);

        // Sets the loop value of audio
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void __setLooping(IntPtr scene, ref bool value);

        public Audio()
        {
        }

        public Audio(IntPtr instance)
        {
            InstantiateFromPtr(instance);
        }

        /// <summary>
        /// Play audio
        /// </summary>
        public void Play()
        {
            __play(_CInstance);
        }

        /// <summary>
        /// Pause audio
        /// </summary>
        public void Pause()
        {
            __pause(_CInstance);
        }

        /// <summary>
        /// Stop audio
        /// </summary>
        public void Stop()
        {
            __stop(_CInstance);
        }

        /// <summary>
        /// Loads the audio using the Source property
        /// </summary>
        public void Load()
        {
            __load(_CInstance, Source);
        }

        protected internal override void InstantiateFromPtr(IntPtr ptr)
        {
            _CInstance = ptr;
        }

        protected internal override void InstantiateNew(IntPtr manager, string name)
        {
            _CInstance = __new(manager, name);
        }
    }
}
