using System;
using System.Collections.Generic;
using System.Text;

namespace Apoc.Graphics.Geometry
{
    /// <summary>
    /// A sphere geometry primitive constructed with PrimitiveMesh
    /// </summary>
    public class Sphere : Model
    {
        /// <summary>
        /// Creates a sphere of the given radius centered around the origin. The sphere 
        /// is subdivided around the Y axis into slices and along the Y axis into stacks 
        /// (similar to lines of longitude and latitude). 
        /// </summary>
        /// <param name="radius">Specifies the radius of the sphere.</param>
        /// <param name="slices">Specifies the number of subdivisions around the Y axis 
        /// (similar to lines of longitude). This has to be greater than 4 and less than 101.</param>
        /// <param name="stacks">Specifies the number of subdivisions along the Y axis 
        /// (similar to lines of latitude). This has to be greater than 4 and less than 101.</param>
        public Sphere(float radius, int slices, int stacks)
            : base(CreateSphere(radius, slices, stacks))
        {
        }

        private static PrimitiveMesh CreateSphere(float radius, int slices, int stacks)
        {
            if (slices < 5)
                throw new ArgumentException("Cannot draw a sphere with slices less than 5");
            if (slices > 100)
                throw new ArgumentException("Cannot draw a sphere with slices greater than 100");
            if (stacks < 5)
                throw new ArgumentException("Cannot draw a sphere with stacks less than 5");
            if (stacks > 100)
                throw new ArgumentException("Cannot draw a sphere with stacks greater than 100");
            if (radius <= 0)
                throw new ArgumentException("Radius has to be greater than 0");

            PrimitiveMesh mesh = new PrimitiveMesh();

            List<VertexPositionNormal> vertices = new List<VertexPositionNormal>();

            double thai = 0, theta = 0;
            double thaiIncr = Math.PI / stacks;
            double thetaIncr = Math.PI * 2 / slices;
            int countB, countA;
            // Add sphere vertices
            for (countA = 0, thai = thaiIncr; thai < Math.PI - thaiIncr/2; thai += thaiIncr, countA++)
            {
                for (countB = 0, theta = 0; countB < slices; theta += thetaIncr, countB++)
                {
                    VertexPositionNormal vert = new VertexPositionNormal();
                    vert.Position = new Vector3((float)(radius * Math.Sin(thai) * Math.Cos(theta)),
                        (float)(radius * Math.Cos(thai)), (float)(radius * Math.Sin(thai) * Math.Sin(theta)));
                    vert.Normal = Vector3.Normalize(vert.Position);
                    vertices.Add(vert);
                }
            }

            // Add north pole vertex
            vertices.Add(new VertexPositionNormal(new Vector3(0, radius, 0), new Vector3(0, 1, 0)));
            // Add south pole vertex
            vertices.Add(new VertexPositionNormal(new Vector3(0, -radius, 0), new Vector3(0, -1, 0)));

            mesh.VertexDeclaration = new VertexDeclaration(State.Device,
                VertexPositionNormal.VertexElements);

            mesh.VertexBuffer = new VertexBuffer(State.Device,
                VertexPositionNormal.SizeInBytes * vertices.Count, BufferUsage.None);
            mesh.VertexBuffer.SetData(vertices.ToArray());

            List<short> indices = new List<short>();

            // Create the north and south pole area mesh
            for (int i = 0, j = (countA - 1) * slices; i < slices - 1; i++, j++)
            {
                indices.Add((short)(vertices.Count - 2));
                indices.Add((short)i);
                indices.Add((short)(i + 1));

                indices.Add((short)(vertices.Count - 1));
                indices.Add((short)(j + 1));
                indices.Add((short)j);
            }
            indices.Add((short)(vertices.Count - 2));
            indices.Add((short)(slices - 1));
            indices.Add(0);

            indices.Add((short)(vertices.Count - 1));
            indices.Add((short)((countA - 1) * slices));
            indices.Add((short)(vertices.Count - 3));

            // Create side of the sphere
            for (int i = 0; i < countA - 1; i++)
            {
                for (int j = 0; j < slices - 1; j++)
                {
                    indices.Add((short)(i * slices + j));
                    indices.Add((short)((i + 1) * slices + j));
                    indices.Add((short)((i + 1) * slices + j + 1));

                    indices.Add((short)(i * slices + j));
                    indices.Add((short)((i + 1) * slices + j + 1));
                    indices.Add((short)(i * slices + j + 1));
                }

                indices.Add((short)((i + 1) * slices - 1));
                indices.Add((short)((i + 2) * slices - 1));
                indices.Add((short)((i + 1) * slices));

                indices.Add((short)((i + 1) * slices - 1));
                indices.Add((short)((i + 1) * slices));
                indices.Add((short)(i * slices));
            }

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
