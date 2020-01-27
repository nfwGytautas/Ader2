using Ader2;
using Ader2.Core;
using System;

namespace Scripts
{
    [StartScene]
    class TestScene : AderScene
    {
        public override void LoadAssets()
        {
            Console.WriteLine("Loading test scene!");

            Console.WriteLine("Creating visual!");
            Visual vis = NewVisual();

            Console.WriteLine("Loading shader!");
            vis.Shader.VertexSource = "res/vertex_1.txt";
            vis.Shader.FragmentSource = "res/fragment_1.txt";
            vis.Shader.Load();

            Console.WriteLine("Shader loaded!");
            float[] vertices = {
                0.5f,  0.5f, 0.0f,  // top right
		        0.5f, -0.5f, 0.0f,  // bottom right
		       -0.5f, -0.5f, 0.0f,  // bottom left
		       -0.5f,  0.5f, 0.0f   // top left 
            };

            uint[] indices = { 
                0, 1, 3,   // first triangle
			    1, 2, 3    // second triangle
            };

            Console.WriteLine("Setting vertices!");
            vis.VAO.SetVertices(vertices);

            Console.WriteLine("Setting indices!");
            vis.VAO.SetIndices(indices);
        }
    }

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
        int i = 0;

        public LogScript()
        {
        }

        public override void Init()
        {
            Console.WriteLine("Init called");
        }

        public override void Update()
        {
            //throw new Exception("Test exception");
        }
    }
}
