using System;
using System.Collections.Generic;
using System.Text;

namespace Apoc.RenderSystem
{
    public enum StateBlockType
    {
        All = 0,
        VertexState = 1,
        PixelState = 2
    }
    public enum LockMode
    {
        None,
        Discard,
        ReadOnly,
        NoOverwrite
    }

    public enum CompareFunction
    {
        Always = 8,
        Equal = 3,
        Greater = 5,
        GreaterEqual = 7,
        Less = 2,
        LessEqual = 4,
        Never = 1,
        NotEqual = 6
    }

    [Flags()]
    public enum ColorWriteChannels
    {
        All = 15,
        Alpha = 8,
        Blue = 4,
        Green = 2,
        None = 0,
        Red = 1
    }

    [Flags()]
    public enum BufferUsage
    {
        Static = 1,
        Dynamic = 2,
        WriteOnly = 4,
        Discardable = 8
    }


    public enum IndexBufferType    
    {
        Bit16,
        Bit32
    }

    //public enum VertexFormat
    //{
    //    // 摘要:
    //    //     Vertex format contains no texture coordinate sets.
    //    Texture0 = 0,
    //    //
    //    // 摘要:
    //    //     No vertex format defined.
    //    None = 0,
    //    //
    //    // 摘要:
    //    //     Vertex format includes the position of an untransformed vertex.
    //    Position = 2,
    //    //
    //    // 摘要:
    //    //     Vertex format includes the position of a transformed vertex.
    //    PositionRhw = 4,
    //    //
    //    // 摘要:
    //    //     Vertex format contains position and weighting values for multimatrix blending
    //    //     operations.
    //    PositionBlend1 = 6,
    //    //
    //    // 摘要:
    //    //     Vertex format contains position and weighting values for multimatrix blending
    //    //     operations.
    //    PositionBlend2 = 8,
    //    //
    //    // 摘要:
    //    //     The number of bits by which to shift an integer value that identifies the
    //    //     number of texture coordinates for a vertex.
    //    TextureCountShift = 8,
    //    //
    //    // 摘要:
    //    //     Vertex format contains position and weighting values for multimatrix blending
    //    //     operations.
    //    PositionBlend3 = 10,
    //    //
    //    // 摘要:
    //    //     Vertex format contains position and weighting values for multimatrix blending
    //    //     operations.
    //    PositionBlend4 = 12,
    //    //
    //    // 摘要:
    //    //     Vertex format contains position and weighting values for multimatrix blending
    //    //     operations.
    //    PositionBlend5 = 14,
    //    //
    //    // 摘要:
    //    //     Vertex format includes a vertex normal vector.
    //    Normal = 16,
    //    //
    //    // 摘要:
    //    //     Vertex format contains a position and a normal.
    //    PositionNormal = 18,
    //    //
    //    // 摘要:
    //    //     Vertex format contains a point size.
    //    PointSize = 32,
    //    //
    //    // 摘要:
    //    //     Vertex format includes a diffuse color component.
    //    Diffuse = 64,
    //    //
    //    // 摘要:
    //    //     Vertex format includes a specular color component.
    //    Specular = 128,
    //    //
    //    // 摘要:
    //    //     Vertex format contains 1 texture coordinate set.
    //    Texture1 = 256,
    //    //
    //    // 摘要:
    //    //     Vertex format contains 2 texture coordinate sets.
    //    Texture2 = 512,
    //    //
    //    // 摘要:
    //    //     Vertex format contains 3 texture coordinate sets.
    //    Texture3 = 768,
    //    //
    //    // 摘要:
    //    //     Vertex format contains 4 texture coordinate sets.
    //    Texture4 = 1024,
    //    //
    //    // 摘要:
    //    //     Vertex format contains 5 texture coordinate sets.
    //    Texture5 = 1280,
    //    //
    //    // 摘要:
    //    //     Vertex format contains 6 texture coordinate sets.
    //    Texture6 = 1536,
    //    //
    //    // 摘要:
    //    //     Vertex format contains 7 texture coordinate sets.
    //    Texture7 = 1792,
    //    //
    //    // 摘要:
    //    //     Vertex format contains 8 texture coordinate sets.
    //    Texture8 = 2048,
    //    //
    //    // 摘要:
    //    //     Mask for texture flag bits.
    //    TextureCountMask = 3840,
    //    //
    //    // 摘要:
    //    //     The last beta field in the vertex position data will be of type UByte4. The
    //    //     data in the beta fields are used with matrix palette skinning to specify
    //    //     matrix indices.
    //    LastBetaUByte4 = 4096,
    //    //
    //    // 摘要:
    //    //     Vertex format contains transformed and clipped data.
    //    PositionW = 16386,
    //    //
    //    // 摘要:
    //    //     Mask for position bits.
    //    PositionMask = 16398,
    //    //
    //    // 摘要:
    //    //     The last beta field in the vertex position data will be of type Color. The
    //    //     data in the beta fields are used with matrix palette skinning to specify
    //    //     matrix indices.
    //    LastBetaColor = 32768,
    //}

    public enum CullMode
    {
        Clockwise = 2,
        CounterClockwise = 3,
        None = 1
    }

    public enum CubeMapFace
    {
        PositiveX,
        NegativeX,
        PositiveY,
        NegativeY,
        PositiveZ,
        NegativeZ
    }

    //public enum DepthFormat
    //{
    //    Unknown = 0,
    //    Depth15Stencil1 = 1,
    //    Depth16 = 2,
    //    Depth16Lockable = 3,
    //    Depth24 = 4,
    //    Depth24Stencil4 = 5,
    //    Depth24Stencil8 = 6,
    //    Depth24Stencil8Single = 7,
    //    Depth32 = 8,
    //    Depth32Lockable = 9,
    //    Depth32Single = 10,
    //}


    public enum FillMode
    {
        Point = 1,
        Solid = 3,
        WireFrame = 2
    }

    public enum FogMode
    {
        None = 0,
        Exponent = 1,
        ExponentSquared = 2,
        Linear = 3
    }

    [Flags]
    public enum FilterOptions
    {
        Box = 5,
        Dither = 524288,
        DitherDiffusion = 1048576,
        Linear = 3,
        Mirror = 458752,
        MirrorU = 65536,
        MirrorV = 131072,
        MirrorW = 262144,
        None = 1,
        Point = 2,
        Srgb = 6291456,
        SrgbIn = 2097152,
        SrgbOut = 4194304,
        Triangle = 4
    }

    public enum MultiSampleType
    {
        None,
        NonMaskable,
        TwoSamples,
        ThreeSamples,
        FourSamples,
        FiveSamples,
        SixSamples,
        SevenSamples,
        EightSamples,
        NineSamples,
        TenSamples,
        ElevenSamples,
        TwelveSamples,
        ThirteenSamples,
        FourteenSamples,
        FifteenSamples,
        SixteenSamples
    }

    [Flags]
    public enum PresentOptions
    {
        DeviceClip = 4,
        DiscardDepthStencil = 2,
        None = 0,
        Video = 16
    }

    [Flags]
    public enum ClearFlags
    {
        DepthBuffer = 2,
        Stencil = 4,
        Target = 1
    }
    public enum BlendFunction
    {
        Add = 1,
        Max = 5,
        Min = 4,
        ReverseSubtract = 3,
        Subtract = 2
    }

    public enum Blend
    {
        BlendFactor = 14,
        BothInverseSourceAlpha = 13,
        BothSourceAlpha = 12,
        DestinationAlpha = 7,
        DestinationColor = 9,
        InverseBlendFactor = 15,
        InverseDestinationAlpha = 8,
        InverseDestinationColor = 10,
        InverseSourceAlpha = 6,
        InverseSourceColor = 4,
        One = 2,
        SourceAlpha = 5,
        SourceAlphaSaturation = 11,
        SourceColor = 3,
        Zero = 1
    }

    [Flags]
    public enum QueryUsages
    {
        Filter = 131072,
        None = 0,
        PostPixelShaderBlending = 524288,
        SrgbRead = 65536,
        SrgbWrite = 262144,
        VertexTexture = 1048576,
        WrapAndMip = 2097152
    }

    public enum StencilOperation
    {
        Decrement = 8,
        DecrementSaturation = 5,
        Increment = 7,
        IncrementSaturation = 4,
        Invert = 6,
        Keep = 1,
        Replace = 3,
        Zero = 2
    }

    public enum SurfaceFormat
    {
        Alpha8 = 15,
        Bgr233 = 16,
        Bgr24 = 17,
        Bgr32 = 2,
        Bgr444 = 13,
        Bgr555 = 11,
        Bgr565 = 9,
        Bgra1010102 = 3,
        Bgra2338 = 14,
        Bgra4444 = 12,
        Bgra5551 = 10,
        Color = 1,
        Depth15Stencil1 = 56,
        Depth16 = 54,
        Depth24 = 51,
        Depth24Stencil4 = 50,
        Depth24Stencil8 = 48,
        Depth24Stencil8Single = 49,
        Depth32 = 52,
        Dxt1 = 28,
        Dxt2 = 29,
        Dxt3 = 30,
        Dxt4 = 31,
        Dxt5 = 32,
        HalfSingle = 25,
        HalfVector2 = 26,
        HalfVector4 = 27,
        Luminance16 = 34,
        Luminance8 = 33,
        LuminanceAlpha16 = 36,
        LuminanceAlpha8 = 35,
        Multi2Bgra32 = 47,
        NormalizedAlpha1010102 = 41,
        NormalizedByte2 = 18,
        NormalizedByte2Computed = 42,
        NormalizedByte4 = 19,
        NormalizedLuminance16 = 39,
        NormalizedLuminance32 = 40,
        NormalizedShort2 = 20,
        NormalizedShort4 = 21,
        Palette8 = 37,
        PaletteAlpha16 = 38,
        Rg32 = 7,
        Rgb32 = 5,
        Rgba1010102 = 6,
        Rgba32 = 4,
        Rgba64 = 8,
        Single = 22,
        Unknown = -1,
        Vector2 = 23,
        Vector4 = 24,
        VideoGrGb = 45,
        VideoRgBg = 46,
        VideoUyVy = 44,
        VideoYuYv = 43
    }

    public enum SwapEffect
    {
        Default,
        Discard,
        Flip,
        Copy
    }



    /// <summary>
    /// Texture stage states define multi-blender texture operations. 
    /// Some sampler states set up vertex processing, and some set up pixel processing. 
    /// Texture stage states can be saved and restored using stateblocks. 
    /// </summary>
    public enum TextureStage
    {
        /// <summary>
        /// Settings for the third alpha operand for triadic operations. 
        /// Use values from TextureArgument to set this state. 
        /// The default value is TextureArgument.Current. 
        /// </summary>
        AlphaArg0 = 27,
        /// <summary>
        /// Texture-stage state is the first alpha argument for the stage. 
        /// Use values from TextureArgument to set this state. 
        /// The default value is TextureArgument.Texture. 
        /// </summary>
        AlphaArg1 = 5,
        /// <summary>
        /// Texture-stage state is the second alpha argument for the stage. Use values from TextureArgument to set this state. The default value is TextureArgument.Current.
        /// </summary>
        AlphaArg2 = 6,
        /// <summary>
        /// 
        /// </summary>
        AlphaOperation = 4,
        /// <summary>
        /// 
        /// </summary>
        BumpEnvironmentLOffset = 23,

        /// <summary>
        /// 
        /// </summary>
        BumpEnvironmentLScale = 22,
        /// <summary>
        /// 
        /// </summary>
        BumpEnvironmentMat00 = 7,
        /// <summary>
        /// 
        /// </summary>
        BumpEnvironmentMat01 = 8,
        /// <summary>
        /// 
        /// </summary>
        BumpEnvironmentMat10 = 9,
        /// <summary>
        /// 
        /// </summary>
        BumpEnvironmentMat11 = 10,
        /// <summary>
        /// 
        /// </summary>
        ColorArg0 = 26,
        /// <summary>
        /// 
        /// </summary>
        ColorArg1 = 2,
        /// <summary>
        /// 
        /// </summary>
        ColorArg2 = 3,
        /// <summary>
        /// 
        /// </summary>
        ColorOperation = 1,
        /// <summary>
        /// 
        /// </summary>
        Constant = 32,
        /// <summary>
        /// 
        /// </summary>
        ResultArg = 28,
        /// <summary>
        /// 
        /// </summary>
        TexCoordIndex = 11,
        /// <summary>
        /// 
        /// </summary>
        TextureTransformFlags = 24
    }

    public enum TextureAddressMode
    {
        Border = 4,
        Clamp = 3,
        Mirror = 2,
        MirrorOnce = 5,
        Wrap = 1
    }

    public enum TextureFilter
    {
        Anisotropic = 3,
        GaussianQuad = 7,
        Linear = 2,
        None = 0,
        Point = 1,
        PyramidalQuad = 6
    }

    public enum TextureType
    {
        Texture1D = 0,
        Texture2D,
        Texture3D,
        CubeTexture
    }
    //public enum SamplerState
    //{
    //    AddressU = 1,
    //    AddressV = 2,
    //    AddressW = 3,
    //    BorderColor = 4,
    //    MagFilter = 5,
    //    MinFilter = 6,
    //    MipFilter = 7,
    //    MipMapLodBias = 8,
    //    MaxMipLevel = 9,        
    //    MaxAnisotropy = 10,        
    //    SrgbTexture = 11 ,
    //    ElementIndex = 12,
    //    DisplacementMapOffset = 13
    //}


    /// <summary>
    /// Defines the possible shading modes.
    /// </summary>
    public enum ShadeMode
    {
        /// <summary>
        /// Flat shading.
        /// </summary>
        Flat = 1,
        /// <summary>
        ///   Gouraud shading.
        /// </summary>
        Gouraud = 2,
    }

    //[Flags]
    //public enum TextureUsage
    //{
    //    AutoGenerateMipMap = 1024,
    //    Linear = 1073741824,
    //    None = 0,
    //    Tiled = -2147483648
    //}
    [Flags]
    public enum TextureArgument
    {
        AlphaReplicate = 32,
        Complement = 16,
        Constant = 6,
        Current = 1,
        Diffuse = 0,
        SelectMask = 15,
        Specular = 4,
        Temp = 5,
        Texture = 2,
        TFactor = 3
    }
    public enum TextureOperation
    {
        Add = 7,
        AddSigned = 8,
        AddSigned2X = 9,
        AddSmooth = 11,
        BlendCurrentAlpha = 16,
        BlendDiffuseAlpha = 12,
        BlendFactorAlpha = 14,
        BlendTextureAlpha = 13,
        BlendTextureAlphaPM = 15,
        BumpEnvironmentMap = 22,
        BumpEnvironmentMapLuminance = 23,
        Disable = 1,
        DotProduct3 = 24,
        Lerp = 26,
        Modulate = 4,
        Modulate2X = 5,
        Modulate4X = 6,
        ModulateAlphaAddColor = 18,
        ModulateColorAddAlpha = 19,
        ModulateInvAlphaAddColor = 20,
        ModulateInvColorAddAlpha = 21,
        MultiplyAdd = 25,
        Premodulate = 17,
        SelectArg1 = 2,
        SelectArg2 = 3,
        Subtract = 10
    }
    public enum TextureTransform
    {
        Count1 = 1,
        Count2 = 2,
        Count3 = 3,
        Count4 = 4,
        Disable = 0,
        Projected = 256
    }


    [Flags]
    public enum TextureWrapCoordinates
    {
        None = 0,
        One = 2,
        Three = 8,
        Two = 4,
        Zero = 1
    }

    public enum PrimitiveType
    {
        /// <summary>
        ///		Render the vertices as individual points.
        /// </summary>
        PointList = 1,
        /// <summary>
        ///		Render the vertices as a series of individual lines.
        /// </summary>
        LineList,
        /// <summary>
        ///		Render the vertices as a continuous line.
        /// </summary>
        LineStrip,
        /// <summary>
        ///		Render the vertices as a series of individual triangles.
        /// </summary>
        TriangleList,
        /// <summary>
        ///		Render the vertices as a continous set of triangles in a zigzag type fashion.
        /// </summary>
        TriangleStrip,
        /// <summary>
        ///		Render the vertices as a set of trinagles in a fan like formation.
        /// </summary>
        TriangleFan
    }

    public enum VertexElementFormat : byte
    {
        Byte4 = 5,
        Color = 4,
        HalfVector2 = 15,
        HalfVector4 = 16,
        Normalized101010 = 14,
        NormalizedShort2 = 9,
        NormalizedShort4 = 10,
        Rg32 = 11,
        Rgba32 = 8,
        Rgba64 = 12,
        Short2 = 6,
        Short4 = 7,
        Single = 0,
        UInt101010 = 13,
        Unused = 17,
        Vector2 = 1,
        Vector3 = 2,
        Vector4 = 3
    }

    public enum VertexElementMethod : byte
    {
        Default = 0,
        LookUp = 5,
        LookUpPresampled = 6,
        UV = 4
    }
 
    public enum VertexElementUsage : byte
    {
        Binormal = 7,
        BlendIndices = 2,
        BlendWeight = 1,
        Color = 10,
        Depth = 12,
        Fog = 11,
        Normal = 3,
        PointSize = 4,
        Position = 0,
        PositionTransformed = 9,
        Sample = 13,
        Tangent = 6,
        TessellateFactor = 8,
        TextureCoordinate = 5
    }

    /// <summary>
    ///		Specifies how a texture is to be used in the engine.
    /// </summary>
    [Flags]
    public enum TextureUsage
    {
        Static = BufferUsage.Static,
        Dynamic = BufferUsage.Dynamic,
        WriteOnly = BufferUsage.WriteOnly,
        StaticWriteOnly = BufferUsage.Static | BufferUsage.WriteOnly,
        DynamicWriteOnly = BufferUsage.Dynamic | BufferUsage.WriteOnly,
        Discardable = BufferUsage.Discardable,
        /// <summary>
        ///    Mipmaps will be automatically generated for this texture
        ///	 </summary>
        AutoMipMap = 0x100,
        /// <summary>
        ///    This texture will be a render target, ie. used as a target for render to texture
        ///    setting this flag will ignore all other texture usages except AutoMipMap
        ///	 </summary>
        RenderTarget = 0x200,
        /// <summary>
        ///    Default to automatic mipmap generation static textures
        ///	</summary>
        Default = AutoMipMap | StaticWriteOnly
    }

    public enum TransformState
    {
        View = 2,
        Projection = 3,

        Texture0 = 16,
        Texture1 = 17,
        Texture2 = 18,
        Texture3 = 19,
        Texture4 = 20,
        Texture5 = 21,
        Texture6 = 22,
        Texture7 = 23,
        
        World = 256,
        World1 = 257,
        World2 = 258,
        World3 = 259
    }


    ///// <summary>
    /////     Vertex element semantics, used to identify the meaning of vertex buffer contents.
    ///// </summary>
    //public enum VertexElementSemantic
    //{
    //    /// <summary>
    //    ///     Position, 3 reals per vertex.
    //    /// </summary>
    //    Position = 1,
    //    /// <summary>
    //    ///     Blending weights.
    //    /// </summary>
    //    BlendWeights = 2,
    //    /// <summary>
    //    ///     Blending indices.
    //    /// </summary>
    //    BlendIndices = 3,
    //    /// <summary>
    //    ///     Normal, 3 reals per vertex.
    //    /// </summary>
    //    Normal = 4,
    //    /// <summary>
    //    ///     Diffuse colors.
    //    /// </summary>
    //    Diffuse = 5,
    //    /// <summary>
    //    ///     Specular colors.
    //    /// </summary>
    //    Specular = 6,
    //    /// <summary>
    //    ///     Texture coordinates.
    //    /// </summary>
    //    TexCoords = 7,
    //    /// <summary>
    //    ///     Binormal (Y axis if normal is Z).
    //    /// </summary>
    //    Binormal = 8,
    //    /// <summary>
    //    ///     Tangent (X axis if normal is Z).
    //    /// </summary>
    //    Tangent = 9
    //}

    ///// <summary>
    /////     Vertex element type, used to identify the base types of the vertex contents.
    ///// </summary>
    //public enum VertexElementType
    //{
    //    Float1,
    //    Float2,
    //    Float3,
    //    Float4,
    //    Color,
    //    Short1,
    //    Short2,
    //    Short3,
    //    Short4,
    //    UByte4,
    //    /// D3D style compact colour
    //    Color_ARGB = 10,
    //    /// GL style compact colour
    //    Color_ABGR = 11

    //}

    public enum PresentInterval
    {
        /// <summary>
        ///  The device will present immediately without waiting for the refresh.
        /// </summary>
        Immediate = -2147483648,
        /// <summary>
        ///  The device will wait for the vertical retrace period.
        /// </summary>
        Default = 0,        
        /// <summary>
        ///  The device will wait for the vertical retrace period.
        /// </summary>
        One = 1,
        /// <summary>
        ///  Present operations will not be affected more than twice every screen refresh.
        /// </summary>
        Two = 2,
        /// <summary>
        ///  Present operations will not be affected more than three times every screen refresh
        /// </summary>
        Three = 4,
        /// <summary>
        ///  Present operations will not be affected more than four times every screen refresh.
        /// </summary>
        Four = 8,
    }
}
