
float4x4 mvp : register(c0);

float4x4 world : register(c4);
float4x4 mv : register(c8);

struct VSInput
{
    float4 Position : POSITION0;
    float3 Normal : NORMAL;
    //float2 TexCoord : TEXCOORD0;
};
struct VSOutput
{
    float4 Position : POSITION0;
    float4 TexCoord : TEXCOORD0;
    float3 Normal : TEXCOORD1;
};

VSOutput main(VSInput ip)
{
    VSOutput o;
	o.TexCoord.z = ip.Position.y;
	
	
    o.Position = mul(ip.Position, mvp);

	float4 viewSpacePos = mul(ip.Position, mv);
    o.TexCoord.w = viewSpacePos.z;
	
	o.TexCoord.xy = ip.Position.xz * 16.0f / 256.0f;
	
    o.Normal = normalize((float3)mul(float4(ip.Normal,0), world));
    
    return o;
}
