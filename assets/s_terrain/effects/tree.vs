#include "instancing.fxh"

float4x4 view : register(c0);
float4x4 proj : register(c4);
float time : register(c8);

struct VSInput
{
    float4 Position : POSITION0;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD0;
	float InstID : TEXCOORD15;
};
struct VSOutput
{
    float4 Position : POSITION0;
    float3 TexCoord : TEXCOORD0;
    float3 Normal : TEXCOORD1;
	float3 WPos : TEXCOORD2;
};

VSOutput main(VSInput ip)
{
    VSOutput o;

	int id = (int)ip.InstID;

	float4x4 mv = mul(InstanceWorld[id], view);
	float4x4 mvp = mul(mv, proj);
	
	o.WPos = mul(ip.Position, InstanceWorld[id]).xyz;

	float wind = ip.Position.y / 10.0f;
	
	ip.Position.x += wind * sin(time + InstanceWorld[id]._41);

	ip.Position.xyz *= 1.5;
	o.Position = mul(ip.Position, mvp);

    o.TexCoord.xy = ip.TexCoord;
    o.Normal = normalize((float3)mul(float4(ip.Normal,0), InstanceWorld[id]));


	float4 viewSpacePos = mul(ip.Position, mv);
    o.TexCoord.z = viewSpacePos.z;
	
    return o;
}
