/*
-----------------------------------------------------------------------------
This source file is part of Apoc3D Engine

Copyright (c) 2009+ Tao Games

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  if not, write to the Free Software Foundation, 
Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/gpl.txt.

-----------------------------------------------------------------------------
*/

#ifndef MESHSIMPLIFIER_H
#define MESHSIMPLIFIER_H

#include "Common.h"
#include "Math/Vector.h"
#include "Math/MathCommon.h"
#include "Collections/FastList.h"
#include "ModelTypes.h"

using namespace Apoc3D::Collections;
using namespace Apoc3D::Math;

namespace Apoc3D
{
	namespace Graphics
	{
		class APAPI MeshSimplifier
		{
		private:
			class Triangle;
			class Vertex
			{
			private:
				MeshSimplifier* m_parent;
			public:
				/** location of point in euclidean space
				*/
				Vector3 m_position;
				/** place of vertex in original list
				*/
				int m_id;
				/** adjacent vertices
				*/
				FastList<Vertex*> m_neighbour;
				/** adjacent triangles
				*/
				FastList<Triangle*> m_face;
				/** cached cost of collapsing edge
				*/
				float m_objdist;
				/** candidate vertex for collapse
				*/
				Vertex* m_collapse;

				Vertex(MeshSimplifier* parent, Vector3 v, int _id)
					: m_position(v), m_id(_id), m_parent(parent)
				{
					m_parent->m_vertices.Add(this);
				}
				~Vertex()
				{
					assert(m_face.getCount()==0);
					while (m_neighbour.getCount())
					{
						m_neighbour[0]->m_neighbour.Remove(this);
						m_neighbour.Remove(m_neighbour[0]);
					}
					m_parent->m_vertices.Remove(this);
				}

				void RemoveIfNonNeighbour(Vertex* n)
				{
					// removes n from neighbor list if n isn't a neighbor.
					if (!m_neighbour.Contains(n))
						return;

					for (int i=0;i<m_face.getCount();i++)
					{
						if (m_face[i]->HasVertex(n))
							return;
					}
					m_neighbour.Remove(n);
				}
			};
			class Triangle
			{
			private:
				MeshSimplifier* m_parent;
			public:
				/** the 3 points that make this triangle
				*/
				Vertex* m_vertex[3]; 
				/** unit vector orthogonal to this face
				*/
				Vector3 m_normal;

				Triangle(MeshSimplifier* parent, Vertex *v0, Vertex *v1, Vertex *v2)
					: m_parent(parent)
				{
					assert(v0!=v1 && v1!=v2 && v2!=v0);
					m_vertex[0]=v0;
					m_vertex[1]=v1;
					m_vertex[2]=v2;
					ComputeNormal();
					m_parent->m_triangles.Add(this);
					for(int i=0;i<3;i++) 
					{
						m_vertex[i]->m_face.Add(this);
						for(int j=0;j<3;j++) if(i!=j) 
						{
							if (!m_vertex[i]->m_neighbour.Contains(m_vertex[j]))
								m_vertex[i]->m_neighbour.Add(m_vertex[j]);
						}
					}
				}
				~Triangle()
				{
					int i;
					m_parent->m_triangles.Remove(this);
					for(i=0;i<3;i++)
					{
						if(m_vertex[i]) 
							m_vertex[i]->m_face.Remove(this);
					}
					for(i=0;i<3;i++)
					{
						int i2 = (i+1)%3;
						if(!m_vertex[i] || !m_vertex[i2]) continue;
						m_vertex[i ]->RemoveIfNonNeighbour(m_vertex[i2]);
						m_vertex[i2]->RemoveIfNonNeighbour(m_vertex[i ]);
					}
				}
				void ComputeNormal()
				{
					Vector3 v0=m_vertex[0]->m_position;
					Vector3 v1=m_vertex[1]->m_position;
					Vector3 v2=m_vertex[2]->m_position;

					v0 = Vector3Utils::Subtract(v1, v0);
					v1 = Vector3Utils::Subtract(v2, v1);
					m_normal = Vector3Utils::Cross(v0, v1);
					Vector3Utils::Normalize(m_normal);
				}
				void ReplaceVertex(Vertex *vold, Vertex *vnew)
				{
					assert(vold && vnew);
					assert(vold==m_vertex[0] || vold==m_vertex[1] || vold==m_vertex[2]);
					assert(vnew!=m_vertex[0] && vnew!=m_vertex[1] && vnew!=m_vertex[2]);

					if(vold==m_vertex[0])
					{
						m_vertex[0]=vnew;
					}
					else if(vold==m_vertex[1])
					{
						m_vertex[1]=vnew;
					}
					else
					{
						assert(vold==m_vertex[2]);
						m_vertex[2]=vnew;
					}

					int i;
					vold->m_face.Remove(this);
					assert(!vnew->m_face.Contains(this));
					vnew->m_face.Add(this);
					for(i=0;i<3;i++)
					{
						vold->RemoveIfNonNeighbour(m_vertex[i]);
						m_vertex[i]->RemoveIfNonNeighbour(vold);
					}
					for (i=0;i<3;i++)
					{
						assert(m_vertex[i]->m_face.Contains(this)==1);
						for (int j=0;j<3;j++)
						{
							if (i!=j)
							{
								if (m_vertex[i]->m_neighbour.Contains(m_vertex[j]))
								{
									m_vertex[i]->m_neighbour.Add(m_vertex[j]);
								}
							}
						}

					}
					ComputeNormal();
				}
				int HasVertex(Vertex *v) const
				{
					return v == m_vertex[0] || v == m_vertex[1] || v == m_vertex[2];
				}
				
			};
			
			FastList<Triangle*> m_triangles;
			FastList<Vertex*> m_vertices;
		private:
			float ComputeEdgeCollapseCose(Vertex* u, Vertex* v)
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
				float edgelength = Vector3Utils::Distance(u->m_position, v->m_position);
				float curvature = 0;

				// find the "sides" triangles that are on the edge uv
				FastList<Triangle*> sides;
				for (i = 0; i < u->m_face.getCount(); i++)
				{
					if (u->m_face[i]->HasVertex(v))
					{
						sides.Add(u->m_face[i]);
					}
				}

				// use the triangle facing most away from the sides 
				// to determine our curvature term

				for (i = 0; i < u->m_face.getCount(); i++)
				{
					float mincurv = 1; // curve for face i and closer side to it
					for (int j = 0; j < sides.getCount(); j++)
					{
						// use dot product of face normals. '^' defined in vector
						float dotprod = Vector3Utils::Dot(u->m_face[i]->m_normal, sides[j]->m_normal);
						mincurv = minf(mincurv, (1 - dotprod) / 2.0f);
					}
					curvature = maxf(curvature, mincurv);
				}
				// the more coplanar the lower the curvature term   
				return edgelength * curvature; 
			}

			void ComputeEdgeCostAtVertex(Vertex* v)
			{
				// compute the edge collapse cost for all edges that start
				// from vertex v.  Since we are only interested in reducing
				// the object by selecting the min cost edge at each step, we
				// only cache the cost of the least cost edge at this vertex
				// (in member variable collapse) as well as the value of the 
				// cost (in member variable objdist).
				if(v->m_neighbour.getCount()==0) 
				{
					// v doesn't have neighbors so it costs nothing to collapse
					v->m_collapse = 0;
					v->m_objdist = -0.01f;
					return;
				}
				v->m_objdist = MaxFloat;
				v->m_collapse = 0;
				// search all neighboring edges for "least cost" edge
				for (int i=0;i<v->m_neighbour.getCount();i++)
				{
					float dist = ComputeEdgeCollapseCose(v, v->m_neighbour[i]);
					if (dist < v->m_objdist)
					{
						v->m_collapse = v->m_neighbour[i];  // candidate for edge collapse
						v->m_objdist = dist; // cost of the collapse
					}
				}
			}

			void ComputeAllEdgeCollapseCosts()
			{
				// For all the edges, compute the difference it would make
				// to the model if it was collapsed.  The least of these
				// per vertex is cached in each vertex object.
				for (int i = 0; i < m_vertices.getCount(); i++)
				{
					ComputeEdgeCostAtVertex(m_vertices[i]);
				}
			}

			void Collapse(Vertex* u, Vertex* v)
			{
				// Collapse the edge uv by moving vertex u onto v
				// Actually remove triangles on uv, then update triangles that
				// have u to have v, and then remove u.
				if (!v)
				{
					// u is a vertex all by itself so just delete it
					delete u;
					return;
				}

				int i;
				FastList<Vertex*> tmp;
				// make tmp a list of all the neighbors of u
				for (i = 0; i < u->m_neighbour.getCount(); i++)
				{
					tmp.Add(u->m_neighbour[i]);
				}
				// delete triangles on edge uv:
				for (i = u->m_face.getCount() - 1; i >= 0; i--)
				{
					if (u->m_face[i]->HasVertex(v))
					{
						delete (u->m_face[i]);
						u->m_face[i] = 0;
					}
				}
				// update remaining triangles to have v instead of u
				for (i = u->m_face.getCount() - 1; i >= 0; i--)
				{
					u->m_face[i]->ReplaceVertex(u, v);
				}
				delete u;
				// recompute the edge collapse costs for neighboring vertices
				for (i = 0; i < tmp.getCount(); i++)
				{
					ComputeEdgeCostAtVertex(tmp[i]);
				}
			}

			void AddVertex(const FastList<Vector3>& vert)
			{
				for (int i=0;i<vert.getCount();i++)
				{
					new Vertex(this, vert[i], i);
				}
			}

			void AddFaces(const FastList<MeshFace>& tri)
			{
				for (int i = 0; i < tri.getCount(); i++)
				{
					MeshFace td = tri[i];
					new Triangle(this,
						m_vertices[td.IndexA],
						m_vertices[td.IndexB],
						m_vertices[td.IndexC]);
				}
			}

			Vertex* MinimumCostEdge()
			{
				// Find the edge that when collapsed will affect model the least.
				// This function actually returns a Vertex, the second vertex
				// of the edge (collapse candidate) is stored in the vertex sounds.
				// Serious optimization opportunity here: this function currently
				// does a sequential search through an unsorted list :-(
				// Our algorithm could be O(n*lg(n)) instead of O(n*n)
				Vertex* mn = m_vertices[0];
				for (int i = 0; i < m_vertices.getCount(); i++)
				{
					if (m_vertices[i]->m_objdist < mn->m_objdist)
					{
						mn = m_vertices[i];
					}
				}
				return mn;
			}

		public:
			
			void ProgressiveMesh(const FastList<Vector3>& vert, const FastList<MeshFace>& tri,
				 FastList<int>& map, FastList<int>& permutation)
			 {
				 m_vertices.ResizeDiscard(vert.getCount());
				 m_triangles.ResizeDiscard(tri.getCount());

				 AddVertex(vert);  // put input sounds into our sounds structures
				 AddFaces(tri);
				 ComputeAllEdgeCollapseCosts(); // cache all edge collapse costs
				 //permutation.Capacity = vertices.Count;  // allocate space

				 permutation.ResizeDiscard(m_vertices.getCount());
				 map.ResizeDiscard(m_vertices.getCount());
				 //map.Capacity = vertices.Count;          // allocate space
				 // reduce the object down to nothing:
				 while (m_vertices.getCount() > 0)
				 {
					 // get the next vertex to collapse
					 Vertex* mn = MinimumCostEdge();
					 // keep track of this vertex, i.e. the collapse ordering
					 permutation[mn->m_id] = m_vertices.getCount() - 1;
					 // keep track of vertex to which we collapse to
					 map[m_vertices.getCount() - 1] = (mn->m_collapse) ? mn->m_collapse->m_id : -1;
					 // Collapse this edge
					 Collapse(mn, mn->m_collapse);
				 }
				 // reorder the map list based on the collapse ordering
				 for (int i = 0; i < map.getCount(); i++)
				 {
					 map[i] = (map[i] == -1) ? 0 : permutation[map[i]];
				 }
				 // The caller of this function should reorder their vertices
				 // according to the returned "permutation".
			 }
		};
	}
}

#endif