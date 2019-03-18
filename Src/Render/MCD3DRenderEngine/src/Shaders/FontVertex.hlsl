struct VertexIn
{
	float4 pos : POSITION;
	float4 tex : TEXCOORD;
	float4 color : COLOR;
};

struct VertexOut
{
	float4 pos   : SV_POSITION;
	float4 color : COLOR;
	float2 tex   : TEXCOORD;
};

VertexOut main(VertexIn vIn, uint vId : SV_VertexID)
{
	VertexOut vOut;

	// vId 0 = 0000, uv = (0, 0)
	// vId 1 = 0001, uv = (1, 0)
	// vId 2 = 0010, uv = (0, 1)
	// vId 3 = 0011, uv = (1, 1)
	float2 uv = float2(vId & 1, (vId >> 1) & 1);

	// Set the position for the vertex based on which vertex it is (uv)
	vOut.pos = float4(vIn.pos.x + (vIn.pos.z * uv.x), vIn.pos.y - (vIn.pos.w * uv.y), 0, 1);
	vOut.color = vIn.color;

	// set the texture coordinate based on which vertex it is (uv)
	vOut.tex = float2(vIn.tex.x + (vIn.tex.z * uv.x), vIn.tex.y + (vIn.tex.w * uv.y));

	return vOut;
}