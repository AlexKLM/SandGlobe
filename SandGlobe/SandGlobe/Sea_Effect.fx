// Add the following to the effects file properties: Configuations Proporties->Custom Build Tool->General
// [Debug] Command Line:  fxc /Fc "fx/%(Filename).cod" /Od /Zi /T fx_5_0 /Fo "fx/%(Filename).fxo" "%(FullPath)"
// [Debug] Output: fx/%(Filename).fxo; fx/%(Filename).cod
// [Release] Command Line: fxc  /T fx_5_0 /Fo "fx/%(Filename).fxo" "%(FullPath)"
// [Release] Outputs: fx/%(Filename).fxo

// Global variables
cbuffer cbPerObject: register(b0) 
{
	float4x4 worldViewProj;
	float4x4 worldMx;
	float sea_level;
	float WaveTime;
};


// Shaders
struct VertexIn {
    float3 pos : POSITION; 
    float4 color : COLOR; 
};

struct VertexOut { 
    float4 posH : SV_POSITION; 
	float3 posV : POSITION ;
	float3 sea_level :POSITION1;
    float4 color : COLOR; 
};


VertexOut RenderSceneVS(VertexIn vin) {
    VertexOut vout; 
    vout.posH = mul(float4(vin.pos, 1.0f), worldViewProj);
	vout.posV = mul(float4(vin.pos, 1.0f), worldMx);
	//vout.posV  = mul(vin.pos, worldMx);
    vout.color = vin.color;
	float xmod = mul(vin.pos.x, 0.5) * WaveTime;
	float zmod = mul(vin.pos.z, 0.5) * WaveTime;
	xmod = mul(0.1, sin(xmod));
	zmod = mul(0.1, sin(zmod));
	vin.pos.y += xmod;
	vin.pos.y += zmod;
    return vout;
}

float4 RenderScenePS(VertexOut pin) : SV_TARGET {
	if(pin.posV.y < sea_level)
	{
		float xmod = mul(pin.posV.x, 0.5) * WaveTime;
		float zmod = mul(pin.posV.z, 0.5) * WaveTime;
		xmod = mul(0.1, sin(xmod));
		zmod = mul(0.1, sin(zmod));
		pin.posV.y += xmod;
		pin.posV.y += zmod;
	}
	else
	{
		discard;
	}

	if(pin.posV.y <-0.8f)
	{
		discard;
	}

	return pin.color;
	
}

float4 RenderTransperent(VertexOut pin) : SV_TARGET {
	if(pin.posV.y < sea_level)
	{
		float xmod = mul(pin.posV.x, 0.5) * WaveTime;
		float zmod = mul(pin.posV.z, 0.5) * WaveTime;
		xmod = mul(0.1, sin(xmod));
		zmod = mul(0.1, sin(zmod));
		pin.posV.y += xmod;
		pin.posV.y += zmod;
	}
	else
	{
		discard;
	}

	float4 color = pin.color;
	color.a =  0.5;
	return color;
    //return float4(0,1,0,0);
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
        SetPixelShader( CompileShader( ps_4_0, RenderScenePS() ) );
	}
}


