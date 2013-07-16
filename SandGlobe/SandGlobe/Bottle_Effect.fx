// Add the following to the effects file properties: Configuations Proporties->Custom Build Tool->General
// [Debug] Command Line:  fxc /Fc "fx/%(Filename).cod" /Od /Zi /T fx_5_0 /Fo "fx/%(Filename).fxo" "%(FullPath)"
// [Debug] Output: fx/%(Filename).fxo; fx/%(Filename).cod
// [Release] Command Line: fxc  /T fx_5_0 /Fo "fx/%(Filename).fxo" "%(FullPath)"
// [Release] Outputs: fx/%(Filename).fxo

// Global variables
cbuffer cbPerObject : register(b0)
{
	float4x4 worldViewProj;
	float4x4 worldInverseTranspose;
	float4x4 worldMx;
	float4 clip_level;
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


// Shaders
struct VertexIn {
     float3 pos : POSITION; 
	float3 normal: NORMAL;
    float4 color : COLOR; 
	float2 TexUV : TEXCOORD;
};

struct VertexOut { 
     float4 posH : SV_POSITION; 
	float3 posV : POSITION ;
	float3 normal : NORMAL;
	float2 TexCoords : TEXCOORD;
    float4 color : COLOR; 
	float clip : SV_ClipDistance0;
};

RasterizerState mainState {
	FillMode = Solid;
	CullMode = None;
};

VertexOut RenderSceneVS(VertexIn vin) {
	VertexOut vout; 
    vout.posH = mul(float4(vin.pos, 1.0f), worldViewProj);
	vout.posV = vin.pos;
    vout.color = vin.color;
	//float3 innormal = normalize(vin.normal);
	//float3 normal = mul(innormal, worldInverseTranspose);
	//vout.normal = normal;
	vout.normal = vin.normal;
	vout.clip = dot(mul(vout.posH, worldMx), clip_level);
	//vout.TexCoords = vin.TexUV;
    return vout;
}

float4 RenderScenePS(VertexOut pin) : SV_TARGET {

		return 0.4 * pin.color;
	
}

float4 PSPointLight(VertexOut pin) : SV_TARGET {


	float4 LightColor = plight_ambient;
    float3 Lightvector = (plight_pos - pin.posV);
	float3 pNormal = pin.normal;
    float3 LDir = normalize(Lightvector);
	float4 LightIntensity  = LightColor/pow(length(Lightvector),0.8);
	float4 FinalColor = saturate( LightIntensity*dot(pNormal, LDir) );

    FinalColor.a = 1;
	return FinalColor;
}

float4 PSDirLight(VertexOut pin) : SV_TARGET {


	float4 LightColor = plight_ambient;
  //  float3 Lightvector = (plight_pos - pin.posV);
	float3 pNormal = pin.normal;
    float3 LDir = normalize(-plight_direction);
	float4 LightIntensity  = LightColor/pow(length(plight_direction),0.8);
	float4 FinalColor = saturate(LightIntensity* dot(pNormal, LDir) );

    FinalColor.a = 1;
	return FinalColor;
}

// Techniques
technique10 RenderScene {
    pass P0 {
        SetVertexShader( CompileShader( vs_4_0, RenderSceneVS() ) );
		SetGeometryShader(NULL);
        SetPixelShader( CompileShader( ps_4_0, RenderScenePS() ) );
    
		SetRasterizerState( mainState );  
	}
	 pass P1 {
        SetVertexShader( CompileShader( vs_4_0, RenderSceneVS() ) );
		SetGeometryShader(NULL);
        SetPixelShader( CompileShader( ps_4_0, PSPointLight() ) );
    
		SetRasterizerState( mainState );  
	}
	pass P2{
		SetVertexShader( CompileShader( vs_4_0, RenderSceneVS() ) );
		SetGeometryShader(NULL);
        SetPixelShader( CompileShader( ps_4_0, PSDirLight() ) );
    
		SetRasterizerState( mainState );  
	}
}

