using Ader2;
using Ader2.Core;
using System;

namespace Scripts
{
    class StateScript : AderScript
    {
        WindowState wState;

        public override void Init()
        {
            wState = AderEngine.GetStateHook<WindowState>();
        }

        public override void Update()
        {
            wState.Query();

            if (wState.Resized)
            {
                Console.WriteLine(String.Format($"(Width, Height) : ({wState.Width}, {wState.Height})"));
            }
        }
    }

    class LogScript : AderScript
    {
        public LogScript()
        {
        }

        public override void Init()
        {
            Console.WriteLine("Init called");
        }

        public override void Update()
        {
            //Console.WriteLine("Test!");
            //throw new Exception("Test exception");
        }
    }
}
