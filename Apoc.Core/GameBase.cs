using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;

namespace Apoc.Core
{
    /// <summary>
    ///  
    /// </summary>
    public abstract class GameBase
    {
        /// <summary>
        /// Occurs when the game is disposed.
        /// </summary>
        public event EventHandler Disposed;

        /// <summary>
        /// Occurs when the game is activated.
        /// </summary>
        public event EventHandler Activated;

        /// <summary>
        /// Occurs when the game is deactivated.
        /// </summary>
        public event EventHandler Deactivated;

        /// <summary>
        /// Occurs when the game is exiting.
        /// </summary>
        public event EventHandler Exiting;

        /// <summary>
        /// Occurs when a drawing frame is about to start.
        /// </summary>
        public event CancelEventHandler FrameStart;

        /// <summary>
        /// Occurs when a drawing frame ends.
        /// </summary>
        public event EventHandler FrameEnd;


        public abstract void Exit();

        public abstract void Run();

        public void Render() { }

        public void Update(float dt)
        {
        }
    }
}
