// Add the following to the effects file properties: Configuations Proporties->Custom Build Tool->General
// [Debug] Command Line:  fxc /Fc "fx/%(Filename).cod" /Od /Zi /T fx_5_0 /Fo "fx/%(Filename).fxo" "%(FullPath)"
// [Debug] Output: fx/%(Filename).fxo; fx/%(Filename).cod
// [Release] Command Line: fxc  /T fx_5_0 /Fo "fx/%(Filename).fxo" "%(FullPath)"
// [Release] Outputs: fx/%(Filename).fxo


// Global variables
cbuffer cbPerObject {
	float4x4 worldViewProj;
};

// Shaders
struct VertexIn {
    float3 pos : POSITION; 
	float3 norm : NORMAL;
    float4 color : COLOR; 
};

struct VertexOut { 
    float4 posH : SV_POSITION; 
	float3 posV : POSITION ;
	float3 normal : NORMAL;
    float4 color : COLOR; 
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
    vout.posH = mul(float4(vin.pos, 1.0f), worldViewProj);
	vout.posV = vin.pos;
	vout.normal = vin.norm;
    vout.color = vin.color;
    return vout;
}

float4 RenderScenePS(VertexOut pin) : SV_TARGET {

	if(pin.posV.y <-0.8f)
	{
		discard;
	}
	float4 color = pin.color;
	color.a =  0.05;
	return color;
	
}

float4 RenderTransperent(VertexOut pin) : SV_TARGET {
	if(pin.posV.y <-0.8f)
	{
		discard;
	}
	float4 color = pin.color;
	color.a =  0.05;
	return color;
    //return float4(0,1,0,0);
}


float4 RenderPointL(VertexOut pin) : SV_TARGET{
	if(pin.posV.y <-0.8f)
	{
		discard;
	}
	float4 LightColor = plight_ambient;
    float3 Lightvector = (plight_pos - pin.posV);
	float3 pNormal = pin.normal;
    float3 LDir = normalize(Lightvector);
	float4 LightIntensity  = pin.color *LightColor/pow(length(Lightvector),0.8);
	float4 FinalColor = saturate( LightIntensity*dot(pNormal, LDir) );

    FinalColor.a = 1;
	return FinalColor;
}

float4 RenderDirL(VertexOut pin) : SV_TARGET{
	if(pin.posV.y <-0.8f)
	{
		discard;
	}
	float4 LightColor = plight_ambient;
	float3 pNormal = pin.normal;
    float3 LDir = normalize(-plight_direction);
	float4 LightIntensity  = pin.color *LightColor/pow(length(plight_direction),0.8);
	float4 FinalColor = saturate( LightIntensity*dot(pNormal, LDir) );

    FinalColor.a = 1;
	return FinalColor;
}

// Techniques
technique10 RenderScene {
    pass P0 {
	    SetVertexShader( CompileShader( vs_4_0, RenderSceneVS() ) );
		SetGeometryShader(NULL);
        SetPixelShader( CompileShader( ps_4_0, RenderScenePS() ) );
	}
	pass P1{
		SetVertexShader( CompileShader( vs_4_0, RenderSceneVS() ) );
		SetGeometryShader(NULL);
        SetPixelShader( CompileShader( ps_4_0, RenderTransperent() ) );
	}
	pass P2{
		SetVertexShader( CompileShader( vs_4_0, RenderSceneVS() ) );
		SetGeometryShader(NULL);
        SetPixelShader( CompileShader( ps_4_0, RenderPointL() ) );
	}
	pass P3{
		SetVertexShader( CompileShader( vs_4_0, RenderSceneVS() ) );
		SetGeometryShader(NULL);
        SetPixelShader( CompileShader( ps_4_0, RenderDirL() ) );
	}
}


