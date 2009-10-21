using System;
using System.Collections.Generic;
using System.Text;
using SlimDX;

namespace Apoc.Graphics
{
    /// <summary>
    ///  封装一个阶段网格(ProgressiveMesh)算法
    /// </summary>
    public unsafe class MeshSimplifier
    {
        //struct tridata
        //{
        //    public fixed int v[3];

        //}
        class Triangle
        {
            MeshSimplifier ms;
            //Vertex *         vertex[3]; // the 3 points that make this tri
            public Vertex[] vertex;
            public Vector3 normal;    // unit vector othogonal to this face
            bool disposed;

            public bool Disposed
            {
                get { return disposed; }
            }
            public Triangle(MeshSimplifier ms, Vertex v0, Vertex v1, Vertex v2)
            {
                this.ms = ms;
                vertex = new Vertex[3] { v0, v1, v2 };

                ComputeNormal();
                ms.triangles.Add(this);
                for (int i = 0; i < 3; i++)
                {
                    vertex[i].face.Add(this);
                    for (int j = 0; j < 3; j++) if (i != j)
                        {
                            if (!vertex[i].neighbor.Contains(vertex[j]))
                                vertex[i].neighbor.Add(vertex[j]);
                        }
                }
            }
            public void ComputeNormal()
            {
                Vector3 v0 = vertex[0].position;
                Vector3 v1 = vertex[1].position;
                Vector3 v2 = vertex[2].position;
                normal = Vector3.Cross(v1 - v0, v2 - v1);// Vector3(v1 - v0) * (v2 - v1);                                

                if (normal.Length() == 0) return;
                normal.Normalize();
            }
            public void ReplaceVertex(Vertex vold, Vertex vnew)
            {
                //assert(vold && vnew);
                //assert(vold == vertex[0] || vold == vertex[1] || vold == vertex[2]);
                //assert(vnew != vertex[0] && vnew != vertex[1] && vnew != vertex[2]);
                if (vold == vertex[0])
                {
                    vertex[0] = vnew;
                }
                else if (vold == vertex[1])
                {
                    vertex[1] = vnew;
                }
                else
                {
                    //assert(vold == vertex[2]);
                    vertex[2] = vnew;
                }
                int i;
                vold.face.Remove(this);
                //assert(!vnew->face.Contains(this));
                vnew.face.Add(this);
                for (i = 0; i < 3; i++)
                {
                    vold.RemoveIfNonNeighbor(vertex[i]);
                    vertex[i].RemoveIfNonNeighbor(vold);
                }
                for (i = 0; i < 3; i++)
                {
                    //assert(vertex[i]->face.Contains(this) == 1);
                    for (int j = 0; j < 3; j++) if (i != j)
                        {
                            if (!vertex[i].neighbor.Contains(vertex[j]))
                                vertex[i].neighbor.Add(vertex[j]);
                        }
                }
                ComputeNormal();
            }

            public void Dispose()
            {
                if (!disposed)
                {
                    int i;
                    ms.triangles.Remove(this);
                    for (i = 0; i < 3; i++)
                    {
                        if (vertex[i] != null)
                            vertex[i].face.Remove(this);
                    }
                    for (i = 0; i < 3; i++)
                    {
                        int i2 = (i + 1) % 3;
                        if (vertex[i] == null || vertex[i2] == null)
                            continue;
                        vertex[i].RemoveIfNonNeighbor(vertex[i2]);
                        vertex[i2].RemoveIfNonNeighbor(vertex[i]);
                    }
                    disposed = true;
                }
            }
            public bool HasVertex(Vertex v)
            {
                return (v == vertex[0] || v == vertex[1] || v == vertex[2]);
            }
            ~Triangle()
            {
                Dispose();
            }
        }
        class Vertex
        {
            bool disposed;

            public Vector3 position; // location of point in euclidean space
            public int id;       // place of vertex in original list
            public List<Vertex> neighbor = new List<Vertex>(); // adjacent vertices
            public List<Triangle> face = new List<Triangle>();     // adjacent triangles
            public float objdist;  // cached cost of collapsing edge
            public Vertex collapse; // candidate vertex for collapse
            MeshSimplifier ms;


            public bool Disposed
            {
                get { return disposed; }
            }
            public Vertex(MeshSimplifier ms, Vector3 v, int _id)
            {
                this.ms = ms;
                position = v;
                id = _id;
                ms.vertices.Add(this);
            }
            public void Dispose()
            {
                if (!disposed)
                {
                    while (neighbor.Count > 0)
                    {
                        neighbor[0].neighbor.Remove(this);
                        neighbor.Remove(neighbor[0]);
                    }
                    ms.vertices.Remove(this);
                    disposed = true;
                }
            }
            public void RemoveIfNonNeighbor(Vertex n)
            {
                // removes n from neighbor list if n isn't a neighbor.
                if (!neighbor.Contains(n)) return;
                for (int i = 0; i < face.Count; i++)
                {
                    if (face[i].HasVertex(n)) return;
                }
                neighbor.Remove(n);
            }
            ~Vertex()
            {
                Dispose();
            }
        }


        List<Vertex> vertices = new List<Vertex>();
        List<Triangle> triangles = new List<Triangle>();


        float ComputeEdgeCollapseCost(Vertex u, Vertex v)
        {
            // if we collapse edge uv by moving u to v then how 
            // much different will the model change, i.e. how much "error".
            // Texture, vertex normal, and border vertex code was removed
            // to keep this demo as simple as possible.
            // The method of determining cost was designed in order 
            // to exploit small and coplanar regions for
            // effective polygon reduction.
            // Is is possible to add some checks here to see if "folds"
            // would be generated.  i.e. normal of a remaining face gets
            // flipped.  I never seemed to run into this problem and
            // therefore never added code to detect this case.
            int i;
            float edgelength = Vector3.Distance(v.position, u.position);
            float curvature = 0;

            // find the "sides" triangles that are on the edge uv
            List<Triangle> sides = new List<Triangle>();
            for (i = 0; i < u.face.Count; i++)
            {
                if (u.face[i].HasVertex(v))
                {
                    sides.Add(u.face[i]);
                }
            }
            // use the triangle facing most away from the sides 
            // to determine our curvature term
            for (i = 0; i < u.face.Count; i++)
            {
                float mincurv = 1; // curve for face i and closer side to it
                for (int j = 0; j < sides.Count; j++)
                {
                    // use dot product of face normals. '^' defined in vector
                    float dotprod = Vector3.Dot(u.face[i].normal, sides[j].normal);
                    mincurv = Math.Min(mincurv, (1 - dotprod) / 2.0f);
                }
                curvature = Math.Max(curvature, mincurv);
            }
            // the more coplanar the lower the curvature term   
            return edgelength * curvature;
        }

        void ComputeEdgeCostAtVertex(Vertex v)
        {
            // compute the edge collapse cost for all edges that start
            // from vertex v.  Since we are only interested in reducing
            // the object by selecting the min cost edge at each step, we
            // only cache the cost of the least cost edge at this vertex
            // (in member variable collapse) as well as the value of the 
            // cost (in member variable objdist).
            if (v.neighbor.Count == 0)
            {
                // v doesn't have neighbors so it costs nothing to collapse
                v.collapse = null;
                v.objdist = -0.01f;
                return;
            }
            v.objdist = float.MaxValue;
            v.collapse = null;
            // search all neighboring edges for "least cost" edge
            for (int i = 0; i < v.neighbor.Count; i++)
            {
                float dist;
                dist = ComputeEdgeCollapseCost(v, v.neighbor[i]);
                if (dist < v.objdist)
                {
                    v.collapse = v.neighbor[i];  // candidate for edge collapse
                    v.objdist = dist;             // cost of the collapse
                }
            }
        }
        void ComputeAllEdgeCollapseCosts()
        {
            // For all the edges, compute the difference it would make
            // to the model if it was collapsed.  The least of these
            // per vertex is cached in each vertex object.
            for (int i = 0; i < vertices.Count; i++)
            {
                ComputeEdgeCostAtVertex(vertices[i]);
            }
        }

        void Collapse(Vertex u, Vertex v)
        {
            // Collapse the edge uv by moving vertex u onto v
            // Actually remove tris on uv, then update tris that
            // have u to have v, and then remove u.
            if (v == null || v.Disposed)
            {
                // u is a vertex all by itself so just delete it
                u.Dispose();
                return;
            }
            int i;
            List<Vertex> tmp = new List<Vertex>();
            // make tmp a list of all the neighbors of u
            for (i = 0; i < u.neighbor.Count; i++)
            {
                tmp.Add(u.neighbor[i]);
            }
            // delete triangles on edge uv:
            for (i = u.face.Count - 1; i >= 0; i--)
            {
                if (u.face[i].HasVertex(v))
                {
                    u.face[i].Dispose();
                }
            }
            // update remaining triangles to have v instead of u
            for (i = u.face.Count - 1; i >= 0; i--)
            {
                u.face[i].ReplaceVertex(u, v);
            }
            u.Dispose();
            // recompute the edge collapse costs for neighboring vertices
            for (i = 0; i < tmp.Count; i++)
            {
                ComputeEdgeCostAtVertex(tmp[i]);
            }
        }

        void AddVertex(Vector3[] vert)
        {
            for (int i = 0; i < vert.Length; i++)
            {
                new Vertex(this, vert[i], i);
            }
        }
        void AddFaces(MeshFace[] tri)
        {
            for (int i = 0; i < tri.Length; i++)
            {
                MeshFace td = tri[i];
                new Triangle(this,
                                 vertices[td.IndexA],
                                 vertices[td.IndexB],
                                 vertices[td.IndexC]);
            }
        }

        Vertex MinimumCostEdge()
        {
            // Find the edge that when collapsed will affect model the least.
            // This funtion actually returns a Vertex, the second vertex
            // of the edge (collapse candidate) is stored in the vertex sounds.
            // Serious optimization opportunity here: this function currently
            // does a sequential search through an unsorted list :-(
            // Our algorithm could be O(n*lg(n)) instead of O(n*n)
            Vertex mn = vertices[0];
            for (int i = 0; i < vertices.Count; i++)
            {
                if (vertices[i].objdist < mn.objdist)
                {
                    mn = vertices[i];
                }
            }
            return mn;
        }

        public void ProgressiveMesh(Vector3[] vert, MeshFace[] tri,
                                    out int[] map, out int[] permutation)
        {
            vertices.Capacity = vert.Length;
            triangles.Capacity = tri.Length;

            AddVertex(vert);  // put input sounds into our sounds structures
            AddFaces(tri);
            ComputeAllEdgeCollapseCosts(); // cache all edge collapse costs
            //permutation.Capacity = vertices.Count;  // allocate space

            permutation = new int[vertices.Count];
            map = new int[vertices.Count];
            //map.Capacity = vertices.Count;          // allocate space
            // reduce the object down to nothing:
            while (vertices.Count > 0)
            {
                // get the next vertex to collapse
                Vertex mn = MinimumCostEdge();
                // keep track of this vertex, i.e. the collapse ordering
                permutation[mn.id] = vertices.Count - 1;
                // keep track of vertex to which we collapse to
                map[vertices.Count - 1] = (mn.collapse != null && !mn.Disposed) ? mn.collapse.id : -1;
                // Collapse this edge
                Collapse(mn, mn.collapse);
            }
            // reorder the map list based on the collapse ordering
            for (int i = 0; i < map.Length; i++)
            {
                map[i] = (map[i] == -1) ? 0 : permutation[map[i]];
            }
            // The caller of this function should reorder their vertices
            // according to the returned "permutation".
        }


        public int Map(int[] map, int a, int mx)
        {
            if (mx <= 0)
                return 0;
            while (a >= mx)
            {
                a = map[a];
            }
            return a;
        }         
    }
}
