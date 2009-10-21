using System;
using System.Collections.Generic;
using System.Text;

namespace Apoc.Graphics.Geometry
{
    /// <summary>
    /// A torus geometry primitive constructed with PrimitiveMesh
    /// </summary>
    public class Torus : Model
    {
        /// <summary>
        /// Creates a torus of the given inner and outer radius around the Y axis centered around 
        /// the origin. The torus is subdivided around the Y axis into slices and around the
        /// torus tubes into stacks.
        /// </summary>
        /// <param name="inner">The inner radius of the torus. This has to be greater than or
        /// equal to 0 and less than outer radius.</param>
        /// <param name="outer">The outer radius of the torus. This has to be greater than 0
        /// and larger than the inner radius</param>
        /// <param name="slices">Specifies the number of subdivisions around the Y axis.
        /// This has to be greater than 4 and greater than 101.</param>
        /// <param name="stacks">Specifies the number of subdivisions around the torus tube. 
        /// This has to be greater than 4 and greater than 101.</param>
        public Torus(float inner, float outer, int slices, int stacks)
            : base(CreateTorus(inner, outer, slices, stacks))
        {
        }

        private static PrimitiveMesh CreateTorus(float inner, float outer, int slices, int stacks)
        {
            if (slices < 5)
                throw new ArgumentException("Cannot draw a torus with slices less than 5");
            if (slices > 100)
                throw new ArgumentException("Cannot draw a torus with slices greater than 100");
            if (stacks < 5)
                throw new ArgumentException("Cannot draw a torus with stacks less than 5");
            if (stacks > 100)
                throw new ArgumentException("Cannot draw a torus with stacks greater than 100");
            if (inner < 0)
                throw new ArgumentException("Inner radius has to be greater than or equal to 0");
            if (outer <= 0)
                throw new ArgumentException("Outer radius has to be greater than 0");
            if (inner >= outer)
                throw new ArgumentException("Inner radius has to be less than outer radius");

            PrimitiveMesh mesh = new PrimitiveMesh();

            List<VertexPositionNormal> vertices = new List<VertexPositionNormal>();

            // Add the vertices
            double thetaIncr = Math.PI * 2 / slices;
            double thaiIncr = Math.PI * 2 / stacks;
            int countA, countB = 0;
            double theta, thai;
            float c = (outer + inner) / 2;
            float a = (outer - inner) / 2;
            double common;
            Vector3 tubeCenter;
            for (countA = 0, theta = 0; countA < slices; theta += thetaIncr, countA++)
            {
                tubeCenter = new Vector3((float)(c * Math.Cos(theta)), 0, (float)(c * Math.Sin(theta)));
                for (countB = 0, thai = 0; countB < stacks; thai += thaiIncr, countB++)
                {
                    VertexPositionNormal vert = new VertexPositionNormal();
                    common = (c + a * Math.Cos(thai));
                    vert.Position = new Vector3((float)(common * Math.Cos(theta)),
                        (float)(a * Math.Sin(thai)), (float)(common * Math.Sin(theta)));
                    vert.Normal = Vector3.Normalize(vert.Position - tubeCenter);
                    vertices.Add(vert);
                }
            }

            mesh.VertexDeclaration = new VertexDeclaration(State.Device,
                VertexPositionNormal.VertexElements);

            mesh.VertexBuffer = new VertexBuffer(State.Device,
                VertexPositionNormal.SizeInBytes * vertices.Count, BufferUsage.None);
            mesh.VertexBuffer.SetData(vertices.ToArray());

            List<short> indices = new List<short>();

            for (int i = 0; i < slices - 1; i++)
            {
                for (int j = 0; j < stacks - 1; j++)
                {
                    indices.Add((short)(i * stacks + j));
                    indices.Add((short)((i + 1) * stacks + j));
                    indices.Add((short)(i * stacks + j + 1));

                    indices.Add((short)(i * stacks + j + 1));
                    indices.Add((short)((i + 1) * stacks + j));
                    indices.Add((short)((i + 1) * stacks + j + 1));
                }

                indices.Add((short)((i + 1) * stacks - 1));
                indices.Add((short)((i + 2) * stacks - 1));
                indices.Add((short)(i * stacks));

                indices.Add((short)(i * stacks));
                indices.Add((short)((i + 2) * stacks - 1));
                indices.Add((short)((i + 1) * stacks));
            }

            for (int j = 0; j < stacks - 1; j++)
            {
                indices.Add((short)((slices - 1) * stacks + j));
                indices.Add((short)j);
                indices.Add((short)((slices - 1) * stacks + j + 1));

                indices.Add((short)((slices - 1) * stacks + j + 1));
                indices.Add((short)j);
                indices.Add((short)(j + 1));
            }
            indices.Add((short)(slices * stacks - 1));
            indices.Add((short)(slices - 1));
            indices.Add((short)((slices - 1) * stacks));

            indices.Add((short)((slices - 1) * stacks));
            indices.Add((short)(slices - 1));
            indices.Add((short)0);

            mesh.IndexBuffer = new IndexBuffer(State.Device, typeof(short), indices.Count,
                BufferUsage.None);
            mesh.IndexBuffer.SetData(indices.ToArray());

            mesh.SizeInBytes = VertexPositionNormal.SizeInBytes;
            mesh.NumberOfVertices = vertices.Count;
            mesh.NumberOfPrimitives = indices.Count / 3;

            return mesh;
        }
    }
}
