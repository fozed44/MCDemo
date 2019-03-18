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

	vIn.Pos.x += 0.00615f*sin(vIn.Pos.y*1.0f*time);
	vIn.Pos.y += 0.0062f*sin(vIn.Pos.z*1.5f*time);
	vIn.Pos.z += 0.00615f*sin(vIn.Pos.x*1.25f*time);

	vOut.PosH = mul(float4(vIn.Pos, 1.0f), gWorldViewProj);

	vOut.Color = vIn.Color;

	return vOut;
}