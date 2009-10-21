using System;
using System.Collections.Generic;
using System.Drawing;
using System.Text;
using SlimDX;
using VirtualBicycle;
using VirtualBicycle.MathLib;

namespace VirtualBicycle.Ide.Designers.WorldBuilder
{
    public class ViewCamera : Camera
    {
        bool isPerspective;
        float height = 60;
        float orthoZoom;

        protected float moveRate = 2.5f;
        protected float turnRate = MathEx.PIf / 45f;

        /// <summary>
        /// 
        /// </summary>
        /// <param name="fovy">The fovy angle in degrees. 使用角度制.</param>
        /// <param name="aspect"></param>
        public ViewCamera(float fovy, float aspect)
            : base(aspect)
        {
            OrthoZoom = 65;
            isPerspective = fovy < 175 && fovy > 5;
            FieldOfView = fovy;

            ResetView();

            Update(0);
        }

        public override void ResetView()
        {
            isPerspective = FieldOfView < 175 && FieldOfView > 5;
            if (isPerspective)
            {
                Orientation = Quaternion.RotationAxis(new Vector3(0, 1, 0), -MathEx.PIf / 4f);
                Orientation *= Quaternion.RotationAxis(new Vector3(1, 0, 0), MathEx.PIf / 4f);
            }
            else
            {
                Orientation = Quaternion.RotationAxis(new Vector3(0, 1, 0), -MathEx.PIf / 4f);
                Orientation *= Quaternion.RotationAxis(new Vector3(1, 0, 0), MathEx.PIf / 6f);
            }
        }

        public override void GetSubareaProjection(ref RectangleF rect, out Matrix mat)
        {
            if (isPerspective)
            {
                //mat = Matrix.Translation(rect.X * NearPlaneWidth, rect.Y * NearPlaneHeight, 0);

                //mat *= Matrix.PerspectiveRH(rect.Width * NearPlaneWidth, rect.Height * NearPlaneHeight, NearPlane, FarPlane);
                //Matrix.PerspectiveOffCenterRH(-NearPlaneWidth * 0.5f, NearPlaneWidth * 0.5f, NearPlaneHeight * 0.5f, -NearPlaneHeight * 0.5f, NearPlane, FarPlane, out mat);
                //MathEx.GetProjectionMatrixFrustum 
                //mat = MathEx.GetProjectionMatrixFrustum(NearPlaneWidth * rect.Left, NearPlaneWidth * rect.Right, NearPlaneHeight * rect.Top, NearPlaneHeight * rect.Bottom, NearPlane, FarPlane);

                //Matrix.PerspectiveOffCenterRH(NearPlaneWidth * -0.5f, NearPlaneWidth * 0.5f, NearPlaneHeight * 0.5f, NearPlaneHeight * -0.5f, NearPlane, FarPlane, out mat);
                Matrix.PerspectiveOffCenterRH(rect.Left * NearPlaneWidth, rect.Right * NearPlaneWidth, rect.Top * NearPlaneHeight, rect.Bottom * NearPlaneHeight, NearPlane, FarPlane, out mat);
            }
            else
            {
                //mat = MathEx.GetOrthoProjectionMatrix(rect.Left * NearPlaneWidth, rect.Right * NearPlaneWidth, rect.Top * NearPlaneHeight, rect.Bottom * NearPlaneHeight, NearPlane, FarPlane);
                Matrix.OrthoOffCenterRH(rect.Left * NearPlaneWidth, rect.Right * NearPlaneWidth, rect.Top * NearPlaneHeight, rect.Bottom * NearPlaneHeight, NearPlane, FarPlane, out mat);
            }
        }

        public float HeightOffset
        {
            get;
            set;
        }

        public override void UpdateProjection()
        {
            isPerspective = FieldOfView < 175 && FieldOfView > 5;




            if (isPerspective)
            {
                //fFrustum.proj = Matrix.PerspectiveFovRH(MathEx.Angle2Radian(FieldOfView), AspectRatio, NearPlane, FarPlane);
                base.UpdateProjection();
            }
            else
            {
                NearPlaneWidth = AspectRatio * OrthoZoom;
                NearPlaneHeight = OrthoZoom;
                ProjectionMatrix = Matrix.OrthoRH(NearPlaneWidth, NearPlaneHeight, NearPlane, FarPlane);

                isProjDirty = false;
            }
        }

        public override void Update(float dt)
        {
            base.Update(dt);

            position.Y = HeightOffset + height;
            if (isProjDirty)
            {
                UpdateProjection();
            }

            ViewMatrix = MathEx.QuaternionToMatrix(orientation);

            ViewMatrix = Matrix.Translation(-Position) * ViewMatrix;
            //Frustum.Matrix = ViewMatrix * ProjectionMatrix;  //ref mViewMatrix, ref mProjMatrix);

            front = MathEx.QuaternionRotate(orientation, new Vector3(0, 0, -1));
            top = MathEx.QuaternionRotate(orientation, new Vector3(0, 1, 0));
            right = MathEx.QuaternionRotate(orientation, new Vector3(1, 0, 0));

            Frustum.View = ViewMatrix;
            Frustum.Projection = ProjectionMatrix;
            Frustum.Update();//ref mViewMatrix, ref mProjMatrix);
        }

        public float OrthoZoom
        {
            get { return orthoZoom; }
            set
            {
                orthoZoom = value;
                isProjDirty = true;
            }
        }

        public float Height
        {
            get
            {
                return height;
            }
            set
            {
                if (value >= 30 && value < 105)
                {
                    orientation *= Quaternion.RotationAxis(new Vector3(1, 0, 0), MathEx.Degree2Radian(value - height));

                    height = value;
                }
            }
        }
        public bool IsPerspective
        {
            get { return isPerspective; }
        }

        public void MoveForward()
        {
            Vector3 f = front;
            f.Y = 0;
            f.Normalize();
            position += f * moveRate;
        }
        public void MoveBack()
        {
            Vector3 f = front;
            f.Y = 0;
            f.Normalize();
            position -= f * moveRate;
        }

        public void Turn(float amount)
        {
            Quaternion iq = orientation;
            iq.Conjugate();

            Vector3 top = MathEx.QuaternionRotate(iq, new Vector3(0, 1, 0));

            orientation *= Quaternion.RotationAxis(top, -turnRate * amount);
        }
        public void TurnLeft()
        {
            Quaternion iq = orientation;
            iq.Conjugate();

            Vector3 top = MathEx.QuaternionRotate(iq, new Vector3(0, 1, 0));

            orientation *= Quaternion.RotationAxis(top, -turnRate);
            //qOri *= Quaternion.FromAngleAxis(dTurnSpeed, new Vector(0, -1, 0));
            orientation.Normalize();
        }
        public void TurnRight()
        {
            Quaternion iq = orientation;
            iq.Conjugate();

            Vector3 top = MathEx.QuaternionRotate(iq, new Vector3(0, 1, 0));// (~qOri).Rotate(new Vector(0, 1, 0));

            orientation *= Quaternion.RotationAxis(top, turnRate);
            orientation.Normalize();
        }
        public void MoveLeft()
        {
            position -= right * moveRate;
        }
        public void MoveRight()
        {
            position += right * moveRate;
        }

        public void Move(float x, float y)
        {
            position -= right * moveRate * x;
            Vector3 f = front;
            f.Y = 0;
            f.Normalize();
            position += f * moveRate * y;
        }

    }

}
