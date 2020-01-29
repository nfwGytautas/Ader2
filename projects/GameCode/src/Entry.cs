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
            Visual vis = AderAssets.New<Visual>("test_visual");

            Console.WriteLine("Loading shader!");
            Shader shader = AderAssets.New<Shader>("test_shader");

            //shader.VertexSource = "res/vertex_1.txt";
            //shader.FragmentSource = "res/fragment_1.txt";

            shader.VertexSource = "res/vertex_texture.txt";
            shader.FragmentSource = "res/fragment_texture.txt";

            shader.Load();

            vis.Shader = shader;

            Console.WriteLine("Shader loaded!");
            VAO vao = AderAssets.New<VAO>("test_VAO");

            float[] vertices = {
                0.5f,  0.5f, 0.0f,  // top right
		        0.5f, -0.5f, 0.0f,  // bottom right
		       -0.5f, -0.5f, 0.0f,  // bottom left
		       -0.5f,  0.5f, 0.0f   // top left 
            };

            float[] texCoords = {
                1.0f, 1.0f,  // top right
                1.0f, 0.0f,  // bottom right corner
                0.0f, 0.0f,  // bottom left corner  
                0.0f, 1.0f   // top left corner
            };

            uint[] indices = { 
                0, 1, 3,   // first triangle
			    1, 2, 3    // second triangle
            };

            Console.WriteLine("Setting vertices!");
            vao.SetVertices(vertices);

            Console.WriteLine("Setting indices!");
            vao.SetIndices(indices);

            Console.WriteLine("Setting texture coordinates!");
            vao.SetUV(texCoords);

            vis.VAO = vao;

            Console.WriteLine("Creating texture!");
            Texture tex = AderAssets.New<Texture>("test_texture");

            tex.Source = "res/container.jpg";
            tex.Load();

            vis.SetTexture(0, tex);

            Console.WriteLine("Creating game object!");
            GameObject go = NewGameObject();
            go.Visual = vis;
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
