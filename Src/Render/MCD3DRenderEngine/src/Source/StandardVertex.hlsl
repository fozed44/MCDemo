cbuffer cbPerObject : register(b0) {
	float4x4 gWorldViewProj;
	float time;
}

struct VertexIn {
	float3 Pos: POSITION;
	float4 Color: COLOR;
};

struct VertexOut {
	float4 PosH: SV_POSITION;
	float4 Color: COLOR;
};


VertexOut main(VertexIn vIn) {
	VertexOut vOut;

	//vIn.Pos.x += 0.015f*sin(vIn.Pos.z*3.0f*time);
	//vIn.Pos.y += 0.015f*sin(vIn.Pos.z*3.5f*time);

	vOut.PosH = mul(float4(vIn.Pos, 1.0f), gWorldViewProj);

	vOut.Color = vIn.Color;

	return vOut;
}