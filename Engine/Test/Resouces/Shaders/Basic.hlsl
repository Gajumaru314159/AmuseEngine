struct VSInput
{
	float3 position : POSITION;
	float3 color : COLOR;
};

struct VSOutput
{
	float4 position : SV_POSITION;
	[[vk::location(0)]] float3 color : COLOR;
};

VSOutput VS_Main(VSInput input)
{
	VSOutput output;
	output.position = float4(input.position, 1.0);
	output.color = input.color;
	return output;
}

struct HSInput
{
	float3 position : POSITION;
};

struct HSOutput
{
	float3 position : POSITION;
};

struct PatchConstant
{
	float edges[3] : SV_TessFactor;
	float inside : SV_InsideTessFactor;
};

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[patchconstantfunc("PatchConstantFunction")]
[outputcontrolpoints(3)]
HSOutput HS_Main(InputPatch<HSInput, 3> patch, uint id : SV_OutputControlPointID)
{
	HSOutput output;
	output.position = patch[id].position;
	return output;
}

PatchConstant PatchConstantFunction(InputPatch<HSInput, 3> patch)
{
	PatchConstant pc;
	pc.edges[0] = 1.0;
	pc.edges[1] = 1.0;
	pc.edges[2] = 1.0;
	pc.inside = 1.0;
	return pc;
}

struct DSInput
{
	float3 position : POSITION;
};

struct DSOutput
{
	float4 position : SV_POSITION;
};

[domain("tri")]
DSOutput DS_Main(PatchConstant pc, float3 bary : SV_DomainLocation, const OutputPatch<DSInput, 3> patch)
{
	DSOutput output;
	output.position = float4(0.0, 0.0, 0.0, 1.0);
	for (int i = 0; i < 3; ++i)
	{
		output.position += float4(patch[i].position, 1.0) * bary[i];
	}
	return output;
}

struct GSInput
{
	float4 position : SV_POSITION;
	[[vk::location(0)]] float3 color : COLOR;
};

struct GSOutput
{
	float4 position : SV_POSITION;
	[[vk::location(0)]] float3 color : COLOR;
};

[maxvertexcount(3)]
void GS_Main(triangle GSInput input[3], inout TriangleStream<GSOutput> stream)
{
	for (int i = 0; i < 3; ++i)
	{
		GSOutput output;
		output.position = input[i].position;
		output.color = input[i].color;
		stream.Append(output);
	}
}

[[vk::location(0)]] float4 PS_Main(VSOutput input) : SV_TARGET
{
	return float4(input.color, 1.0);
}

[[vk::binding(0)]] RWStructuredBuffer<float4> buffer;

[numthreads(1, 1, 1)]
void CS_Main(uint3 DTid : SV_DispatchThreadID)
{
	buffer[DTid.x] = float4(DTid.x, DTid.y, DTid.z, 1.0);
}
