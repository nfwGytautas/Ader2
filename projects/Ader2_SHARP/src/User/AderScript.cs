using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Ader2
{
    /// <summary>
    /// Script object that has an update method that is invoked
    /// every frame of the engine
    /// </summary>
    abstract public class AderScript
    {
        /// <summary>
        /// Init method is called when the engine is loading the script
        /// All script variables should be initialized/set during this method
        /// </summary>
        public abstract void Init();

        /// <summary>
        /// Update method is called every time by the AderEngine
        /// </summary>
        public abstract void Update();
    }
}
