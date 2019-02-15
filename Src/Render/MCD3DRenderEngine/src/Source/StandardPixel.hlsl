struct VertexIn {
	float4 PosH: SV_POSITION;
	float4 Color: COLOR;
};

float4 main(VertexIn vIn) : SV_TARGET {
	return vIn.Color;
}