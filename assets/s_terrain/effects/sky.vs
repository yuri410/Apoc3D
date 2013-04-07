
float4x4 mvp : register(c0);

struct VSInput
{
    float4 Position : POSITION0;
    float2 TexCoord : TEXCOORD0;
};
struct VSOutput
{
    float4 Position : POSITION0;
    float2 TexCoord : TEXCOORD0;
};

VSOutput main(VSInput ip)
{
    VSOutput o;
	ip.Position.y *= 0.33f;

    o.Position = mul(ip.Position, mvp);
    o.TexCoord = ip.TexCoord;
	
    return o;
}
