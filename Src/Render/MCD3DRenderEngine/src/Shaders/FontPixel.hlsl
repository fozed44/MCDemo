Texture2D    t1 : register(t0);
SamplerState s1 : register(s0);

struct VertexIn {
	float4 pos   : SV_POSITION;
	float4 color : COLOR;
	float2 tex   : TEXCOORD;
};

float4 main(VertexIn vIn) : SV_TARGET 
{
	return float4(vIn.color.rgb, vIn.color.a * t1.Sample(s1, vIn.tex).a);
}