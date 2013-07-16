// Add the following to the effects file properties: Configuations Proporties->Custom Build Tool->General
// [Debug] Command Line:  fxc /Fc "fx/%(Filename).cod" /Od /Zi /T fx_5_0 /Fo "fx/%(Filename).fxo" "%(FullPath)"
// [Debug] Output: fx/%(Filename).fxo; fx/%(Filename).cod
// [Release] Command Line: fxc  /T fx_5_0 /Fo "fx/%(Filename).fxo" "%(FullPath)"
// [Release] Outputs: fx/%(Filename).fxo

// Global variables
cbuffer cbPerFrame : register(b0)
{
	//float4x4 worldViewProj;
	float4x4 lightViewProj;
	float4x4 worldMx;
	float3 lPos;
	float padding;
}

// Shaders
struct VertexIn {
    float3 pos : POSITION; 
	float3 normal: NORMAL;
	float2 TexUV : TEXCOORD;
	row_major float4x4 mTransform : mTransform;
};

struct ShadowOut{
	float4 pos : SV_POSITION;
	float3 posV : POSITION ;
	float2 Depth: TEXCOORD0;
};

struct ps_shadowOut{
    float Depth : SV_Depth;
};
 

//VERTEX SHADER
ShadowOut ShadowVs(VertexIn Vin)
{
	ShadowOut sout;
	sout.pos = mul( float4(Vin.pos,1.0f), worldMx );
	sout.pos = mul(sout.pos, lightViewProj );
	sout.posV = Vin.pos;
	sout.Depth = sout.pos.zw;
	return sout;

}


ShadowOut ShadowVsIndexed(VertexIn Vin)
{
	ShadowOut sout;
	sout.pos = mul(float4(Vin.pos, 1.0f), Vin.mTransform);
	sout.pos = mul(sout.pos, lightViewProj );
	sout.Depth = sout.pos.zw;
	sout.posV = Vin.pos;
	return sout;
}

ps_shadowOut SVShadowMapPS(ShadowOut sin)
{
    float d = sin.pos.z / sin.pos.w;
 
    ps_shadowOut pOut = (ps_shadowOut)0;
 
    pOut.Depth = d;
 
    return pOut;
}

float4 debugShadowPS(ShadowOut sin) :SV_TARGET
{
	if(sin.posV.y <-0.8f)
	{
		discard;
	}
   float depth = sin.pos.z / sin.pos.w;
   return float4(depth, depth, depth, 1);
}
void ShadowPS(ShadowOut sin)
{

}

RasterizerState Depth
{
	DepthBias = 100000;
    DepthBiasClamp = 0.0f;
	SlopeScaledDepthBias = 1.0f;
};

// Techniques
technique10 RenderScene {
	pass p0
	{
		SetVertexShader(CompileShader(vs_4_0,ShadowVs()));
		//SetPixelShader(NULL);
		SetPixelShader(CompileShader(ps_4_0,debugShadowPS()));
		SetGeometryShader(NULL);
		SetRasterizerState(Depth);
	}
	pass p1//for indexed items
	{
		SetVertexShader(CompileShader(vs_4_0,ShadowVsIndexed()));
		//SetPixelShader(NULL);
		SetPixelShader(CompileShader(ps_4_0,debugShadowPS()));
		SetGeometryShader(NULL);
		SetRasterizerState(Depth);
	}

}

