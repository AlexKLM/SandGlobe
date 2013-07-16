// Add the following to the effects file properties: Configuations Proporties->Custom Build Tool->General
// [Debug] Command Line:  fxc /Fc "fx/%(Filename).cod" /Od /Zi /T fx_5_0 /Fo "fx/%(Filename).fxo" "%(FullPath)"
// [Debug] Output: fx/%(Filename).fxo; fx/%(Filename).cod
// [Release] Command Line: fxc  /T fx_5_0 /Fo "fx/%(Filename).fxo" "%(FullPath)"
// [Release] Outputs: fx/%(Filename).fxo

// Global variables
cbuffer cbPerObject {
	float4x4 viewMx;
	float4x4 projMx;
	float4 clip_level;
};

// Shaders
struct VertexIn
{
    float3 pos : POSITION;
    float3 norm : NORMAL;
	float4 color : COLOR; 
    row_major float4x4 mTransform : mTransform;
};

struct VertexOut { 
    float4 posH : SV_POSITION; 
    float4 color : COLOR; 
	float3 posV : POSITION ;
	float3 normal : NORMAL;
	float clip : SV_ClipDistance0;
};

RasterizerState mainState {
	FillMode = Solid;
	CullMode = None;
};

cbuffer pointLight  : register(b1) 
{
	float3 plight_pos;
	float  plight_range;
	float3 plight_att;
	float4 plight_ambient;
	float4 plight_diffuse;
	float4 plight_specular;
	float4 plight_spec_color;
	float spec_power;
	float3 plight_direction;
};

VertexOut RenderSceneVS(VertexIn vin) {
    VertexOut vout; 
    vout.posH = mul(float4(vin.pos, 1.0f), vin.mTransform);
    vout.posH = mul( vout.posH, viewMx);
    vout.posH = mul( vout.posH, projMx);
	vout.posV = vin.pos;
	vout.normal = vin.norm;
    vout.color = vin.color;
	//vout.clip = dot(mul(vout.posH, worldMx), clip_level);
    return vout;
}

float4 RenderScenePointL(VertexOut pin) : SV_TARGET
{
	float4 LightColor = plight_ambient;
    float3 Lightvector = (plight_pos - pin.posV);
	float3 pNormal = pin.normal;
    float3 LDir = normalize(Lightvector);
	float4 LightIntensity  = pin.color *LightColor/pow(length(Lightvector),0.8);
	float4 FinalColor = saturate( LightIntensity*dot(pNormal, LDir) );

    FinalColor.a = 1;
	return FinalColor;
}

float4 RenderSceneDirL(VertexOut pin) : SV_TARGET
{
	float4 LightColor = plight_ambient;
    //float3 Lightvector = (plight_pos - pin.posV);
	float3 pNormal = pin.normal;
    float3 LDir = normalize(-plight_direction);
	float4 LightIntensity  = pin.color *LightColor/pow(length(plight_direction),0.8);
	float4 FinalColor = saturate( LightIntensity*dot(pNormal, LDir) );

    FinalColor.a = 1;
	return FinalColor;
}

float4 RenderScenePS(VertexOut pin) : SV_TARGET
 {
		return pin.color;
}

float4 RenderScenePS2(VertexOut pin) : SV_TARGET {
	return pin.color;
    //return float4(0,1,0,0);
}

// Techniques
technique10 RenderScene {
    pass P0 {
        SetVertexShader( CompileShader( vs_4_0, RenderSceneVS() ) );
		SetGeometryShader(NULL);
        SetPixelShader( CompileShader( ps_4_0, RenderScenePointL() ) );
    
		SetRasterizerState( mainState );  
	}
	pass P1{
		SetVertexShader( CompileShader( vs_4_0, RenderSceneVS() ) );
		SetGeometryShader(NULL);
        SetPixelShader( CompileShader( ps_4_0, RenderSceneDirL() ) );
    
		SetRasterizerState( mainState );  
	}
}
