#define SAMPLE_COUNT 13

sampler2D tex : register(s0);

float weights[SAMPLE_COUNT];
float2 offsets[SAMPLE_COUNT];

float4 main(float2 uv : TEXCOORD0) : COLOR
{
	float4 color = 0;

	for (int i = 0; i < SAMPLE_COUNT; i++)
	{
		color += tex2D(tex, uv + offsets[i]) * weights[i];
	}

	return color;
}

