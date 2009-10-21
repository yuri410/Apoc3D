using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Apoc.Sound
{
    public interface ISample
    {
        /// <summary>
        ///  从头开始播放声音
        /// </summary>
        /// <param name="loop">当为true时，循环播放</param>
        void Play(bool loop);

        /// <summary>
        ///  从当前位置播放声音
        /// </summary>
        void PlayFromCurrentPosition();

        /// <summary>
        ///  从当前位置播放声音
        /// </summary>
        /// <param name="loop">当为true时，循环播放</param>
        void PlayFromCurrentPosition(bool loop);

        /// <summary>
        ///  停止播放声音
        /// </summary>
        void Stop();
    }
}
