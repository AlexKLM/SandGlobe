// Add the following to the effects file properties: Configuations Proporties->Custom Build Tool->General
// [Debug] Command Line:  fxc /Fc "fx/%(Filename).cod" /Od /Zi /T fx_5_0 /Fo "fx/%(Filename).fxo" "%(FullPath)"
// [Debug] Output: fx/%(Filename).fxo; fx/%(Filename).cod
// [Release] Command Line: fxc  /T fx_5_0 /Fo "fx/%(Filename).fxo" "%(FullPath)"
// [Release] Outputs: fx/%(Filename).fxo

// Global variables
cbuffer cbPerObject {
	float4x4 worldViewProj;
	float4x4 worldMx;
	float4 clip_level;
};

// Shaders
struct VertexIn {
    float3 pos : POSITION; 
    float4 color : COLOR; 
};

struct VertexOut { 
    float4 posH : SV_POSITION; 
	float3 posV : POSITION ;
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
	vout.clip = dot(mul(vout.posH, worldMx), clip_level);
    return vout;
}

float4 RenderScenePS(VertexOut pin) : SV_TARGET {

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

