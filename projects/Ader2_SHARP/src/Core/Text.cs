using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace Ader2.Core
{
    /// <summary>
    /// Text slot class denotes a single text area with its own
    /// content and position using a Text object where the slot originates from
    /// </summary>
    public class TextSlot
    {
        private IntPtr _CInstance;

        /// <summary>
        /// Position for this text slot
        /// </summary>
        public Vector2 Position 
        {
            get
            {
                Vector2 value;
                __getPosition(_CInstance, out value);
                return value;
            }

            set
            {
                __setPosition(_CInstance, ref value);
            }
        }

        /// <summary>
        /// Content of this text slot
        /// </summary>
        public string Content 
        { 
            get
            {
                return __getContent(_CInstance);
            }
            set
            {
                __setContent(_CInstance, value);
            }
        }

        /// <summary>
        /// Visibility of this text slot
        /// </summary>
        public bool Visible
        {
            get
            {
                bool value;
                __getVisible(_CInstance, out value);
                return value;
            }

            set
            {
                __setVisible(_CInstance, ref value);
            }
        }


        private TextSlot() { }
        internal TextSlot(IntPtr instance)
        {
            _CInstance = instance;
        }

        // Gets the contents of this object
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static string __getContent(IntPtr instance);

        // Sets the contents of this object
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void __setContent(IntPtr instance, string value);

        // Gets the position of this object
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static string __getPosition(IntPtr instance, out Vector2 value);

        // Sets the position of this object
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void __setPosition(IntPtr instance, ref Vector2 value);

        // Gets the visibility of this object
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static string __getVisible(IntPtr instance, out bool value);

        // Sets the visibility of this object
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void __setVisible(IntPtr instance, ref bool value);
    }


    public class Text : AderAsset
    {
        /// <summary>
        /// Font source of the Text object
        /// </summary>
        public string Source { get; set; }

        /// <summary>
        /// Used to get a specified text slot if it doesn't exist
        /// than a new one will be created
        /// </summary>
        /// <param name="slot">Slot name</param>
        /// <returns>TextSlot instance</returns>
        public TextSlot this[string slot]
        {
            get => GetSlot(slot);
        }

        // Creates new Text
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static IntPtr __new(IntPtr manager, string name);

        // Loads the text with the specified source
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void __load(IntPtr instance, string source);

        // Sets the contents of this object
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void __setShader(IntPtr instance, IntPtr shader);

        // Sets the contents of this object
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static IntPtr __getSlot(IntPtr instance, string slot);

        public Text()
        {
        }

        public Text(IntPtr instance)
        {
            InstantiateFromPtr(instance);
        }

        /// <summary>
        /// Loads the texture using the Source property
        /// </summary>
        public void Load()
        {
            __load(_CInstance, Source);
        }

        /// <summary>
        /// Used to get a specified text slot if it doesn't exist
        /// than a new one will be created
        /// </summary>
        /// <param name="slot">Slot name</param>
        /// <returns>TextSlot instance</returns>
        public TextSlot GetSlot(string slot)
        {
            return new TextSlot(__getSlot(_CInstance, slot));
        }

        /// <summary>
        /// Sets this text shader to the specified one
        /// </summary>
        /// <param name="Shader">Shader to use when rendering this text</param>
        public void SetShader(Shader shader)
        {
            __setShader(_CInstance, shader._CInstance);
        }

        protected internal override void InstantiateNew(IntPtr manager, string name)
        {
            _CInstance = __new(manager, name);
        }

        protected internal override void InstantiateFromPtr(IntPtr ptr)
        {
            _CInstance = ptr;
        }
    }
}
