using System;
using System.Collections.Generic;
using System.Text;

namespace Apoc.Graphics.Animation
{
    public class MaterialAnimation : AnimationData
    {

        public MaterialAnimation(int frameCount, float frameLength)
        {
            this.FrameCount = frameCount;
            this.FrameLength = frameLength;
        }
    }
    public class MaterialAnimationInstance : AnimationInstance
    {
        MaterialAnimation data;

        public MaterialAnimationInstance(MaterialAnimation data)
            : base(data)
        {
            this.data = data;
        }

        public MaterialAnimation Data 
        {
            get { return data; }
        }
    }
}
