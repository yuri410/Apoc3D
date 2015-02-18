#define SAMPLE_COUNT 25

sampler2D tex : register(s0);

float weights[SAMPLE_COUNT];
float2 offsets[SAMPLE_COUNT];

float4 main(float2 uv : TEXCOORD0) : COLOR
{
	float color = 0;

	for (int i = 0; i < SAMPLE_COUNT; i++)
	{
		color += tex2D(tex, uv + offsets[i]).r * weights[i];
	}

	return float4( color, color, color, 1);
}

