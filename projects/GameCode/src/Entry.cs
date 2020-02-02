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

            //shader.VertexSource = "res/vertex_texture.txt";
            //shader.FragmentSource = "res/fragment_texture.txt";
            
            shader.VertexSource = "res/vertex_texture_instanced.txt";
            shader.FragmentSource = "res/fragment_texture_instanced.txt";

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
            int x = 0;
            int y = 0;
            Random rnd = new Random();
            for(int i = 0; i < 100000; i++)
            {
                GameObject go = this.NewGameObject();
                go.Position = new Vector3(x++ - 40, y - 20, 0);
                go.Visual = vis;

                go.Rotation = new Vector3(0, 0, rnd.Next(360));

                if (x % 80 == 0)
                {
                    x = 0;
                    y++;
                }
            }

            Console.WriteLine("Creating camera!");
            Camera camera = this.NewCamera();
            camera.Rotation = new Vector3(0, -90, 0);
            camera.Position = new Vector3(0, 0, 50);

            this.ActiveCamera = camera;
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
