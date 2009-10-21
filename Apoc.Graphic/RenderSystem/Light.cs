using System;
using System.Collections.Generic;
using System.Text;
using Apoc.MathLib;
using Apoc.Media;

namespace Apoc.RenderSystem
{
    public enum LightType : int
    {
        /// Point light sources give off light equally in all directions, so require only position not direction
        Point = 1,
        /// Directional lights simulate parallel light beams from a distant source, hence have direction but no position
        Directional = 3,
        /// Spotlights simulate a cone of light from a source so require position and direction, plus extra values for falloff
        Spotlight = 2
    }

    public class Light
    {
        Vector3 position;
        Vector3 direction;
        LightType ltType;
        Color4F ambient;
        Color4F diffuse;
        Color4F specular;

        public float SpotOuter
        {
            get { return spotOuter; }
            set { spotOuter = value; }
        }
        public float SpotInner
        {
            get { return spotInner; }
            set { spotInner = value; }
        }
        public float SpotFalloff
        {
            get { return spotFalloff; }
            set { spotFalloff = value; }
        }
        public float Range
        {
            get { return range; }
            set { range = value; }
        }
        public float AttenuationConst
        {
            get { return attenuationConst; }
            set { attenuationConst = value; }
        }
        public float AttenuationLinear
        {
            get { return attenuationLinear; }
            set { attenuationLinear = value; }
        }
        public float AttenuationQuad
        {
            get { return attenuationQuad; }
            set { attenuationQuad = value; }
        }


        float spotOuter;
        float spotInner;
        float spotFalloff;
        float range;
        float attenuationConst;
        float attenuationLinear;
        float attenuationQuad;
        float power;

        public float Power
        {
            get { return power; }
            set { power = value; }
        }
        public Vector3 Position
        {
            get { return position; }
            set { position = value; }
        }
        public Vector3 Direction
        {
            get { return direction; }
            set { direction = value; }
        }
        public Color4F Ambient
        {
            get { return ambient; }
            set { ambient = value; }
        }
        public Color4F Diffuse
        {
            get { return diffuse; }
            set { diffuse = value; }
        }
        public Color4F Specular
        {
            get { return specular; }
            set { specular = value; }
        }
        public LightType Type
        {
            get { return ltType; }
            set { ltType = value; }
        }
    }
}
