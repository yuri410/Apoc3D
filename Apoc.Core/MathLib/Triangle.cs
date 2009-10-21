using System;
using System.Collections.Generic;
using System.Text;

namespace Apoc.MathLib
{
    public struct Triangle
    {
        public Vector3 vA;
        public Vector3 vB;
        public Vector3 vC;
        public Vector3 vN;

        public Triangle(Vector3 a, Vector3 b, Vector3 c)
        {
            vA = a;
            vB = b;
            vC = c;
            MathEx.ComputePlaneNormal(ref a, ref b, ref c, out vN);
        }
        public Triangle(ref Vector3 a, ref Vector3 b, ref Vector3 c)
        {
            vA = a;
            vB = b;
            vC = c;
            MathEx.ComputePlaneNormal(ref a, ref b, ref c, out vN);
        }
        public bool Match(ref Vector3 p)
        {
            Vector3 tP;
            bool tBool;
            Vector3 spanTP;

            Vector3.Subtract(ref p, ref vA, out spanTP);

            Vector3.Add(ref vC, ref vN, out tP);

            tBool = GetSPoint(out tP, ref vB, ref vC, ref tP, ref vA, ref spanTP);

            float a_tP = MathEx.Distance(ref vA, ref tP);
            float b_c = MathEx.Distance(ref vB, ref vC);
            if ((MathEx.Distance(ref  vA, ref p)) <= a_tP &&
                (MathEx.Distance(ref p, ref tP)) <= a_tP &&
                (MathEx.Distance(ref  vB, ref tP)) <= b_c &&
                (MathEx.Distance(ref vC, ref tP)) <= b_c)
                return true;

            return false;

        }

        public bool DirTriCollision(out Vector3 vOut, ref Vector3 vFrom, ref Vector3 vDir)
        {
            Vector3 tVec;
            Vector3 tVec2;

            vDir.Normalize();

            if (GetSPoint(out tVec, ref vA, ref vB, ref vC, ref vFrom, ref vDir))
            {
                Vector3.Subtract(ref tVec, ref vFrom, out tVec2);
                tVec2.Normalize();

                if (MathEx.Vec3Dot(ref tVec2, ref vDir) > 0)
                    if (Match(ref tVec))
                    {
                        vOut = tVec;
                        return true;
                    }
            }
            vOut = new Vector3();
            return false;
        }
        public bool DirTriCollision(out Vector3 vOut, Vector3 vFrom, Vector3 vDir)
        {
            Vector3 tVec;
            Vector3 tVec2;

            vDir.Normalize();

            if (GetSPoint(out tVec, ref vA, ref vB, ref vC, ref vFrom, ref vDir))
            {
                Vector3.Subtract(ref tVec, ref vFrom, out tVec2);
                tVec2.Normalize();

                if (MathEx.Vec3Dot(ref tVec2, ref vDir) > 0)
                    if (Match(ref tVec))
                    {
                        vOut = tVec;
                        return true;
                    }
            }
            vOut = new Vector3();
            return false;
        }
        public bool RayTriCollision(out Vector3 vOut, Vector3 vFrom, Vector3 vTo)
        {
            Vector3 tVec;
            Vector3 tVec2;
            Vector3 vTo2;

            vTo2 = vFrom - vTo;

            if (GetSPoint(out tVec, ref vA, ref vB, ref vC, ref vFrom, ref vTo2))
            {
                tVec2 = tVec - vFrom;
                tVec2.Normalize();

                vTo2 = vTo;
                vTo2.Normalize();

                if (MathEx.Vec3Dot(ref tVec2, ref vTo2) > 0)
                    if (Match(ref tVec))
                    {
                        vOut = tVec;
                        return true;
                    }
            }
            vOut = new Vector3();
            return false;
        }
        public bool RayTriCollision(out Vector3 vOut, ref Vector3 vFrom, ref Vector3 vTo)
        {
            Vector3 tVec;
            Vector3 tVec2;
            Vector3 vTo2;

            vTo2 = vFrom - vTo;

            if (GetSPoint(out tVec, ref vA, ref vB, ref vC, ref vFrom, ref vTo2))
            {
                tVec2 = tVec - vFrom;
                tVec2.Normalize();

                vTo2 = vTo;
                vTo2.Normalize();

                if (MathEx.Vec3Dot(ref tVec2, ref vTo2) > 0)
                    if (Match(ref tVec))
                    {
                        vOut = tVec;
                        return true;
                    }
            }
            vOut = new Vector3();
            return false;
        }

        private static bool GetSPoint(out Vector3 vOut, ref Vector3 vP0, ref Vector3 vP1, ref Vector3 vP2, ref Vector3 vPv, ref Vector3 vUv)
        {
            Vector3 ENorm;
            Vector3 vTmp;
            float Eb = 0;
            float Temp = 0;
            float t = 0;
            bool Result;// = false;

            MathEx.ComputePlaneNormal(ref vP0, ref vP1, ref vP2, out ENorm);
            Eb = MathEx.Vec3Dot(ref ENorm, ref vP0) - MathEx.Vec3Dot(ref ENorm, ref vPv);

            Temp = MathEx.Vec3Dot(ref ENorm, ref vUv);

            if (Temp != 0)
            {
                t = Eb / Temp;
                Result = true;
            }
            else
                Result = false;


            Vector3.Multiply(ref vUv, t, out vTmp);// vUv* t;
            Vector3.Add(ref vPv, ref vTmp, out  vOut);

            return Result;
        }
        const float Epsilon = 0.2f;

        public static bool TriangleIntersect(ref Triangle a, ref Triangle b, out List<DirectDetectData> data)
        {
            data = new List<DirectDetectData>();
            //点-面测试
            //棱-棱测试

            //除去延长线相交，即必有刺穿=>带入方程多符号（相互带入）
            //对于a的每条边，和b求交
            //对于b的每条边，和a求交

            //a两边和一个三角b相交，发现b
            //a、b互交
            //Plane tap = a.GetEquation;
            //Plane tbp = b.GetEquation;
            Vector3 aab = a.vB - a.vA;
            Vector3 abc = a.vC - a.vB;
            Vector3 aca = a.vA - a.vC;

            Vector3 bab = b.vB - b.vA;
            Vector3 bbc = b.vC - b.vB;
            Vector3 bca = b.vA - b.vC;

            Vector3 aabn; Vector3.Cross(ref aab, ref a.vN, out aabn);// (aab % a.vN);
            Vector3 abcn; Vector3.Cross(ref abc, ref a.vN, out abcn);//(abc % a.vN);
            Vector3 acan; Vector3.Cross(ref aca, ref a.vN, out acan);//(aca % a.vN);

            Vector3 babn; Vector3.Cross(ref bab, ref b.vN, out babn);// = (bab % b.vN);
            Vector3 bbcn; Vector3.Cross(ref bbc, ref b.vN, out bbcn);// = (bbc % b.vN);
            Vector3 bcan; Vector3.Cross(ref aab, ref b.vN, out bcan);//= (bca % b.vN);

            Vector3 baaa = a.vA - b.vA;
            Vector3 baab = a.vB - b.vA;
            Vector3 baac = a.vC - b.vA;

            Vector3 aabb = b.vB - a.vA;
            Vector3 aabc = b.vC - a.vA;

            bool aa_bab = MathEx.Vec3Dot(ref baaa, ref  babn) <= 0;//ab
            bool aa_bbc = MathEx.Vec3Dot(ref aabb, ref bbcn) >= 0;//bc
            bool aa_bca = MathEx.Vec3Dot(ref aabc, ref bcan) >= 0;//ca
            bool ba_aab = MathEx.Vec3Dot(ref baaa, ref aabn) >= 0;//ab
            bool ba_abc = MathEx.Vec3Dot(ref baab, ref abcn) >= 0;//bc
            bool ba_aca = MathEx.Vec3Dot(ref baac, ref acan) >= 0;//ca

            bool ab_bab = MathEx.Vec3Dot(ref baab, ref  babn) <= 0;//ab
            bool ab_bbc = Vector3.Dot(a.vB - b.vB, bbcn) <= 0;//bc
            bool ab_bca = Vector3.Dot(a.vB - b.vC, bcan) <= 0;//ca
            bool bb_aab = MathEx.Vec3Dot(ref aabb, ref aabn) <= 0;//ab
            bool bb_abc = Vector3.Dot(b.vB - a.vB, abcn) <= 0;//bc
            bool bb_aca = Vector3.Dot(b.vB - a.vC, acan) <= 0;//ca

            bool ac_bab = MathEx.Vec3Dot(ref baac, ref  babn) <= 0;//ab
            bool ac_bbc = Vector3.Dot(a.vC - b.vB, bbcn) <= 0;//bc
            bool ac_bca = Vector3.Dot(a.vC - b.vC, bcan) <= 0;//ca
            bool bc_aab = MathEx.Vec3Dot(ref aabc, ref  aabn) <= 0;//ab
            bool bc_abc = Vector3.Dot(b.vC - a.vB, abcn) <= 0;//bc
            bool bc_aca = Vector3.Dot(b.vC - a.vC, acan) <= 0;//ca
            //bool aa_bab = (a.vA - b.vA) * babn <= 0;//ab
            //bool aa_bbc = (a.vA - b.vB) * bbcn <= 0;//bc
            //bool aa_bca = (a.vA - b.vC) * bcan <= 0;//ca
            //bool ba_aab = (b.vA - a.vA) * aabn <= 0;//ab
            //bool ba_abc = (b.vA - a.vB) * abcn <= 0;//bc
            //bool ba_aca = (b.vA - a.vC) * acan <= 0;//ca

            //bool ab_bab = (a.vB - b.vA) * babn <= 0;//ab
            //bool ab_bbc = (a.vB - b.vB) * bbcn <= 0;//bc
            //bool ab_bca = (a.vB - b.vC) * bcan <= 0;//ca
            //bool bb_aab = (b.vB - a.vA) * aabn <= 0;//ab
            //bool bb_abc = (b.vB - a.vB) * abcn <= 0;//bc
            //bool bb_aca = (b.vB - a.vC) * acan <= 0;//ca

            //bool ac_bab = (a.vC - b.vA) * babn <= 0;//ab
            //bool ac_bbc = (a.vC - b.vB) * bbcn <= 0;//bc
            //bool ac_bca = (a.vC - b.vC) * bcan <= 0;//ca
            //bool bc_aab = (b.vC - a.vA) * aabn <= 0;//ab
            //bool bc_abc = (b.vC - a.vB) * abcn <= 0;//bc
            //bool bc_aca = (b.vC - a.vC) * acan <= 0;//ca



            if (aa_bab == aa_bbc && aa_bbc == aa_bca)
            {
                float dist = MathEx.Vec3Dot(ref baaa, ref b.vN);
                if (dist < 0) dist = -dist;
                if (dist < Epsilon)
                    data.Add(new DirectDetectData(a.vA, b.vN, -dist));
            }
            if (ab_bab == ab_bbc && ab_bbc == ab_bca)
            {
                float dist = MathEx.Vec3Dot(ref baab, ref b.vN);
                if (dist < 0) dist = -dist;
                if (dist < Epsilon)
                    data.Add(new DirectDetectData(a.vB, b.vN, -dist));
            }
            if (ac_bab == ac_bbc && ac_bbc == ac_bca)
            {
                float dist = MathEx.Vec3Dot(ref baac, ref b.vN);
                if (dist < 0) dist = -dist;
                if (dist < Epsilon)
                    data.Add(new DirectDetectData(a.vC, b.vN, -dist));
            }
            if (ba_aab == ba_abc && ba_abc == ba_aca)
            {
                float dist = MathEx.Vec3Dot(ref baaa, ref a.vN);
                if (dist < 0) dist = -dist;
                if (dist < Epsilon)
                    data.Add(new DirectDetectData(b.vA, a.vN, -dist));
            }
            if (bb_aab == bb_abc && bb_abc == bb_aca)
            {
                float dist = MathEx.Vec3Dot(ref aabb, ref  a.vN);
                if (dist < 0) dist = -dist;
                if (dist < Epsilon)
                    data.Add(new DirectDetectData(b.vB, a.vN, -dist));
            }
            if (bc_aab == bc_abc && bc_abc == bc_aca)
            {
                float dist = MathEx.Vec3Dot(ref aabc, ref  a.vN);
                if (dist < 0) dist = -dist;
                if (dist < Epsilon)
                    data.Add(new DirectDetectData(b.vC, a.vN, -dist));
            }

            //ab ab //ab bc //ab ca
            if (aa_bab != ab_bab)
            {
                DirectDetectData sd;

                Vector3.Cross(ref aab, ref bab, out sd.Normal);
                sd.Normal.Normalize();

                //sd.vNormal = (aab % bab).UnitVector;
                float dist = MathEx.Vec3Dot(ref  baaa, ref  sd.Normal);
                if (dist < 0) dist = -dist;
                if (dist < Epsilon)
                {
                    Vector3 pn;// = (sd.vNormal % aab).UnitVector;
                    Vector3.Cross(ref sd.Normal, ref aab, out pn);
                    pn.Normalize();

                    //b和此平面求交
                    float pdist = MathEx.Vec3Dot(ref baaa, ref pn);
                    if (pdist < 0) pdist = -pdist;
                    Vector3 ln = aab; ln.Normalize();  //aab.UnitVector;
                    float cosine = MathEx.Vec3Dot(ref ln, ref pn);
                    if (cosine < 0) cosine = -cosine;
                    sd.Position = b.vA + ln * (pdist / cosine);

                    sd.Depth = -dist;
                    data.Add(sd);
                }
            }
            if (aa_bbc != ab_bbc)
            {
                DirectDetectData sd;

                Vector3.Cross(ref aab, ref bab, out sd.Normal);
                sd.Normal.Normalize();

                //sd.vNormal = (aab % bbc).UnitVector;
                float dist = MathEx.Vec3Dot(ref aabb, ref  sd.Normal);
                if (dist < 0) dist = -dist;
                if (dist < Epsilon)
                {
                    Vector3 pn;//= (sd.vNormal % aab).UnitVector;
                    Vector3.Cross(ref sd.Normal, ref aab, out pn);
                    pn.Normalize();

                    //b和此平面求交
                    float pdist = MathEx.Vec3Dot(ref aabb, ref pn);
                    if (pdist < 0) pdist = -pdist;
                    Vector3 ln = aab; aab.Normalize();// aab.UnitVector;
                    float cosine = MathEx.Vec3Dot(ref ln, ref pn);
                    if (cosine < 0) cosine = -cosine;
                    sd.Position = b.vB + ln * (pdist / cosine);

                    sd.Depth = -dist;
                    data.Add(sd);
                }
            }
            //if (aa_bca != ab_bca) { }

            ////bc ab//bc bc//bc ca
            //if (ac_bab != ab_bab) { }
            //if (ac_bbc != ab_bbc) { }
            //if (ac_bca != ab_bca) { }

            ////ca ab//ca bc//ca ca
            //if (aa_bab != ac_bab) { }
            //if (aa_bbc != ac_bbc) { }
            //if (aa_bca != ac_bca) { }


            return data.Count > 0;
        }

    }
}
