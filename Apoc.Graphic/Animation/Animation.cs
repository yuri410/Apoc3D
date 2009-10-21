using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Text;

namespace Apoc.Graphics.Animation
{
    [Flags()]
    public enum ModelAnimationFlags
    {
        EntityTransform = 0,
        Skin = 1 << 1,
        Morph = 1 << 2
    }
    [TypeConverter(typeof(ExpandableObjectConverter))]
    public abstract class AnimationData
    {
        int frameCount;

        public int FrameCount
        {
            get { return frameCount; }
            protected set { frameCount = value; }
        }

        public float FrameLength
        {
            get;
            protected set;
        }
    }
    [TypeConverter(typeof(ExpandableObjectConverter))]
    public abstract class AnimationInstance : IUpdatable
    {
        AnimationData data;


        int currentFrame;

        float currentTime;
        float totalTime;

        protected AnimationInstance(AnimationData data)
        {
            this.data = data;

            totalTime = data.FrameCount * data.FrameLength;
        }
        protected float CurrentTime
        {
            get { return currentTime; }            
        }
        protected float TotalTime
        {
            get { return totalTime; }
            set { totalTime = value; }
        }


        public int CurrentFrame
        {
            get { return currentFrame; }
            protected set { currentFrame = value; }
        }

        #region IUpdatable 成员

        public virtual void Update(float dt)
        {
            if (data.FrameLength > float.Epsilon)
            {
                currentTime += dt;

                if (currentTime >= totalTime)
                {
                    currentTime = 0;
                }

                currentFrame = (int)Math.Truncate(currentTime / data.FrameLength);
            }
        }

        #endregion
    }
}
