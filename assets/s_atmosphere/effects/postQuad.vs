float2 viewportSize;
float4x4 world;

struct VSInput
{
	float4 Position : POSITION0;
	//float2 TexCoord : TEXCOORD0;
};

struct VSOutput
{
	float4 Position : POSITION0;
	float2 TexCoord : TEXCOORD0;
};

VSOutput main(VSInput ip)
{
	VSOutput o;

	o.TexCoord.xy = ip.Position.xy;
	
	ip.Position = mul(ip.Position, world);

	// input coords are [0,1]
	// dc = (c-0.5/vpSize) * (2,-2) - (1,-1)
	
	float2 b = float2(0.5,0.5) / viewportSize;

	o.Position = ip.Position;
	o.Position.xy = (ip.Position - b) * float2(2,-2) - float2(1,-1);
	
	
	//o.Position.xy -= 0.5;
	//o.Position.xy /= float2(1280, 800);
	//o.Position.xy *= float2(2, -2);
	//o.Position.xy -= float2(1, -1);

	//o.TexCoord = ip.TexCoord;
	return o;
}
