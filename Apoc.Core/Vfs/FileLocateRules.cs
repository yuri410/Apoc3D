using System;
using System.Collections.Generic;
using System.Text;

namespace Apoc.Vfs
{
    public static class FileLocateRules
    {
        public static FileLocateRule Default
        {
            get;
            private set;
        }
        public static FileLocateRule Model
        {
            get;
            private set;
        }
        public static FileLocateRule Music
        {
            get;
            private set;
        }


        static FileLocateRules()
        {
            LocateCheckPoint[] pts = new LocateCheckPoint[1];
            pts[0] = new LocateCheckPoint();
            pts[0].AddPath(string.Empty);

            Default = new FileLocateRule(pts);


            pts = new LocateCheckPoint[1];
            pts[0] = new LocateCheckPoint();
            pts[0].AddPath(Paths.Models);

            Model = new FileLocateRule(pts);


            pts = new LocateCheckPoint[1];
            pts[0] = new LocateCheckPoint();
            pts[0].AddPath(Paths.Music);

            Music = new FileLocateRule(pts);
        }
    }
}
