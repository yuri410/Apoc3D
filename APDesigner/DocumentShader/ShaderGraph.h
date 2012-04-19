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

#ifndef SHADERGRAPH_H
#define SHADERGRAPH_H

#include "APDCommon.h"
#include "Collections/FastList.h"
#include "Math/Rectangle.h"
#include "Math/Vector.h"

using namespace Apoc3D::Collections;
using namespace Apoc3D::Core;
using namespace Apoc3D::Math;
using namespace Apoc3D;

namespace APDesigner
{
	/** Class holds the data for the graph, with features
	 *  like layout, display and loading/saving files.
	 *
	 *  ==Method TECH_Quad==
	 *  Visualization:
	 *   The layout, more specifically, graph visualization, is based on the
	 *   force directed algorithm, and optimized by using space decomposition by a quad tree.
	 *   To calculating the N-body repulsion forces with less time,
	 *   the algorithm evaluate the force from distant repulsion forces based on
	 *   the higher-level nodes in the quad tree.
	 *  Updating the quad tree:
	 *   Once a graph node is moved, and when such translation is crossing leaf nodes(in the quad tree), 
	 *   the node will be attached to a new leaf node and tag both node dirty ones, whose parent will
	 *   be updated on properties like the number of graph node the sub tree contains, the center of mass.
	 *  Physics Model:
	 *   Planned: spring and gravitational force.
	 *
	 *  ==TECH_BruteForce==
	 *   The lazy way. Gravitational force is calculated against other nodes one by one
	 *
	 *  ==TECH_Fuzzy==
	 *   Gravitational force is calculated against other neighbor nodes and the whole graph's center of mass
	 *
	 *  Display(not fully implemented):
	 *   As the total number of nodes may be really high, up to 1 million, LOD is a must-have,
	 *   when drawing the overall view of the graph(zoomed out one). OpenGL display lists are used here.
	 *   The graph at certain LOD is divided in to blocks. Each one is a rectangle region containing 
	 *   the node and links as contents.
	 *   The quad tree used in graph visualization is beneficial for rendering as well.
	 *   When rendering, if the visible block's display list is not yet built, the quad tree will be
	 *   used to determine the visible nodes and links inside by waking through it. 
	 *   The further rendering will use the display list
	 *   directly until any node in side is moved.
	 *
	 *  Realtime animation:
	 *   The program can display the intermediate result in the force directed algorithm
	 *   as physics simulation animation.
	 */
	class ShaderGraph
	{
	public:
		/** Enumeration containing the methods supported to solve the layout
		*/
		enum Technique
		{
			TECH_BruteForce,
			/** The default way 
			*/
			TECH_Quad,
			/** The repulsive is estimate as emitted from a point at geometric center with the total mass
			*/
			TECH_Fuzzy

		};
		ShaderGraph();
		~ShaderGraph();

		/** Load from file
		*/
		void Load(const String& filePath);
		/** Save to file
		*/
		void Save(const String& filePath);

		/** Disposes the current open document
		*/
		void Reset();

		void Update(const GameTime* const time);
		void Draw();

		void ActivateLayout() { m_isProcessingLayout = true; }

		void PanView(float dx, float dy);
		void Zoom(float z)
		{
			m_zooming +=z;
			if (m_zooming <0.0f)
				m_zooming = 0.0f;
			else if(m_zooming>8.0f)
				m_zooming = 8.0f;
		}
		void MoveNode(GraphNode* node, float dx, float dy);
		void AddNode(const String& name, float x, float y);
		void DeleteNode(GraphNode* node);
		void SetHightLightingNode(GraphNode* node) { m_highlightingNode = node; }

		/** Find the node intersected with the given point in view port, using the quad tree
		*/
		GraphNode* IntersectNodes(int mx, int my);

		const FastList<GraphNode*>& getNodes() const { return m_nodes; }
		const FastList<GraphNode*>& getVisibleNodes() const { return m_currentVisisbleNodes; }

		bool getIsProcessingLayout() const { return m_isProcessingLayout; }

		float getAdaptiveTimeScale() const { return m_adaptiveTimeScale; }
		float getCurrentEnergy() const { return m_currentKEnergy; }

		float getCurrentZoomLevel() const { return m_zooming; }
		
		Technique getTechnique() const { return m_technique; }
		void setTechnique(Technique tech) { m_technique = tech; }

		void GetViewMatrix(Matrix& mtrx);
		

		Rectangle Viewport;

		/** Converts a world coord to the quad tree's block coord system,
		 *  a pair of integer representing the grid in a plane divided all
		 *  the quad tree's leaf node, i.e. the flatten leaf nodes to a 2D array.
		 */
		static void ConvertCoord(const Vector2& pos, int& qx, int& qy)
		{
			qx = (int)floor((Vector2Utils::GetX(pos)+1024)/16.0f);
			qy = (int)floor((Vector2Utils::GetY(pos)+1024)/16.0f);

			if (qx<0) qx = 0;
			if (qy<0) qy = 0;
			if (qx>127) qx = 127;
			if (qy>127) qy = 127;
		}

		static void SetUpNodeColor(bool isolated, float scale, float ascale = 1);
	private:
		FastList<GraphNode*> m_nodes;
		
		/** A list of visible nodes generated each frame 
		*/
		FastList<GraphNode*> m_currentVisisbleNodes;

		GraphNode* m_highlightingNode;

		QuadTreeNode* m_quadTree;
		/** the flatten leaf nodes to a 2D array.
		*/
		QuadTreeNode* m_leafNodes[128][128];
		/** State representing whether the graph is currently being processed for layout.
		*/
		bool m_isProcessingLayout;

		/** The number of frame between times updating the quad tree
		*/
		int m_quadTreeUpdateInterval;

		/** number of iterations acceptable not to make the application quite laggy
		*/
		int m_adaptiveLayoutIterationPerFrame;
		
		/** The time step multiplier, calculated based on the maximum kinetic energy among all the nodes.
		*/
		float m_adaptiveTimeScale;

		/** The current-time total kinetic energy
		*/
		float m_currentKEnergy;

		Vector2 m_viewPos;
		float m_zooming;

		/** Center of mass of the entire graph, only calculated and used for TECH_Fuzzy
		*/
		Vector2 m_centerOfMass;


		Technique m_technique;

		/** If a GraphNode has crossed 2 quad tree node, detach it
		 * from the old one(if there is), and then attach it GraphNode to the new one.
		 */
		void PutGraphNode(GraphNode* gn);

		void InitialPlacement();

		void DrawEllipse(const Vector2& pos, float a, float b, int segments);
		void DrawBackground();
		void DrawLinks();
		void FindVisisbleNodes();
	};
}

#endif