using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace Apoc
{
    public static class EngineTimer
    {
        class TimeAdjuster
        {
            public TimeAdjuster()
            {
                timeBeginPeriod(tCaps.min);
            }
            ~TimeAdjuster()
            {
                timeEndPeriod(tCaps.min);
            }
        }

        struct TimeCaps
        {
            public int min, max;
        }

        static TimeAdjuster tAdj;
        static TimeCaps tCaps;

        static int loopPassed;
        static uint startTime;
        static long curTime;

        static EngineTimer()
        {
            timeGetDevCaps(out tCaps, 8);
            tAdj = new TimeAdjuster();

            startTime = GetTime();
            Update();
        }

        #region 属性

        public static int MinDelay
        {
            get { return tCaps.min; }
        }
        public static int MaxDelay
        {
            get { return tCaps.max; }
        }

        public static double TimeSecond
        {
            get { return Time / 1000.0; }
        }

        public static long Time
        {
            get
            {
                return curTime - startTime;
            }
        }

        public static TimeSpan TimeSpan
        {
            get { return TimeSpan.FromMilliseconds(Time); }
        }

        #endregion

        #region PInvoke

        [DllImport("winmm.dll")]
        static extern int timeGetDevCaps(out TimeCaps cap, int cbdc);
        [DllImport("winmm.dll")]
        static extern int timeBeginPeriod(int per);
        [DllImport("winmm.dll")]
        static extern int timeEndPeriod(int per);

        [DllImport("winmm.dll", EntryPoint = "timeGetTime")]
        public static extern uint GetTime();

        #endregion

        #region 方法

        /// <summary>
        /// 得到两次更新的时间差
        /// </summary>
        /// <returns>返回时间的单位为ms</returns>
        public static long GetInterval()
        {
            long old = curTime;
            Update();
            return curTime - old;
        }

        public static void Update()
        {
            long t = GetTime() + uint.MaxValue * loopPassed;
            if (t < curTime)
            {
                loopPassed++;
                curTime = t + uint.MaxValue;
            }
            else
                curTime = t;
        }

        #endregion


    }
}
