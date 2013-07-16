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
	float3 normal: NORMAL;
    float4 color : COLOR; 
	float2 TexUV : TEXCOORD;
};

struct VertexOut { 
	float3 posV : POSITION ;
	float4 posH : SV_POSITION; 
	float3 normal : NORMAL;
    float4 color : COLOR; 
	float2 TexCoords : TEXCOORD;
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
	vout.normal = vin.normal;
	vout.TexCoords = vin.TexUV;
    return vout;
}

Texture2D MyTex;
SamplerState ss: register(s0);

float4 RenderScenePS(VertexOut pin) : SV_TARGET {

	//float4 color = (0,1,0,1);
    return  MyTex.Sample( ss, pin.TexCoords );
	
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
        SetPixelShader( CompileShader( ps_4_0, RenderScenePS() ) );
    
		SetRasterizerState( mainState );  
	}
}
technique10 Test {
    pass P0 {
        SetVertexShader( CompileShader( vs_4_0, RenderSceneVS() ) );
		SetGeometryShader(NULL);
        SetPixelShader( CompileShader( ps_4_0, RenderScenePS2() ) );
    
		SetRasterizerState( mainState );  
	}
}

