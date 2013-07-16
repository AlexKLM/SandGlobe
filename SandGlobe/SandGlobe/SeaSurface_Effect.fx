// Add the following to the effects file properties: Configuations Proporties->Custom Build Tool->General
// [Debug] Command Line:  fxc /Fc "fx/%(Filename).cod" /Od /Zi /T fx_5_0 /Fo "fx/%(Filename).fxo" "%(FullPath)"
// [Debug] Output: fx/%(Filename).fxo; fx/%(Filename).cod
// [Release] Command Line: fxc  /T fx_5_0 /Fo "fx/%(Filename).fxo" "%(FullPath)"
// [Release] Outputs: fx/%(Filename).fxo

// Global variables
cbuffer cbPerObject : register(b0)
{
	float4x4 worldViewProj;
	float4x4 viewMx;
	float4x4 projMx;
	float4x4 reflecMx;
	float4x4 worldMx;
	float Time;
	float WaveTime;
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

struct Reflect_VertexOut{
	float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 posV : POSITION ;
    float4 reflectionPosition : TEXCOORD1;
	float2 TexCoordsNormal :TEXCOORD2;
	float4 color : COLOR;
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


	float angle=(Time%360)*2;
	float xmod = mul(vin.pos.x, 0.5) * WaveTime;
	float zmod = mul(vin.pos.z, 0.5) * WaveTime;

	xmod = mul(0.1, sin(xmod));
	zmod = mul(0.1, sin(zmod));
	vin.pos.y += xmod;
	vin.pos.y += zmod;

    vout.posH = mul(float4(vin.pos, 1.0f), worldViewProj);
	vout.posV = vin.pos;
    vout.color = vin.color;
	vout.normal = vin.normal;
	vout.TexCoords = vin.TexUV;
    return vout;
}

Reflect_VertexOut RelflectVS(VertexIn vin){
	Reflect_VertexOut vout;
	matrix reflectProjectWorld;

	float angle=(Time%360)*2;
	float xmod = mul(vin.pos.x, 0.5) * WaveTime;
	float zmod = mul(vin.pos.z, 0.5) * WaveTime;

	xmod = mul(0.1, sin(xmod));
	zmod = mul(0.1, sin(zmod));

	vin.pos.y += xmod;
	vin.pos.y += zmod;
	//vin.pos.y += zmod;

	float4x4 preViewProjection = mul (viewMx, projMx);
    float4x4 preWorldViewProjection = mul (worldMx, preViewProjection);
    float4x4 preReflectionViewProjection = mul (reflecMx, projMx);
   float4x4 preWorldReflectionViewProjection = mul (worldMx, preReflectionViewProjection);


	vout.position = mul(float4(vin.pos, 1.0f), preWorldViewProjection);
    vout.reflectionPosition = mul(float4(vin.pos, 1.0f), preWorldReflectionViewProjection);


	vout.position = mul(float4(vin.pos, 1.0f), worldMx);
    vout.position = mul(vout.position, viewMx);
    vout.position = mul(vout.position, projMx);
	vout.position = mul(float4(vin.pos, 1.0f), worldViewProj);
	vout.tex = vin.TexUV;
	//vout.tex.x += Time;
	vout.tex.y += Time* 0.02;

	reflectProjectWorld = mul(reflecMx, projMx);
    reflectProjectWorld = mul(worldMx, reflectProjectWorld);
	
	
	//vout.reflectionPosition = mul(float4(vin.pos, 1.0f), reflectProjectWorld);

    vout.posV = vin.pos;
		vout.TexCoordsNormal = vin.TexUV;

	return vout;
}

Texture2D reflectionTexture : register(t0);
Texture2D skyTexture : register(t1);
SamplerState ss: register(s0);

float4 ReflectionPS(Reflect_VertexOut pin) : SV_TARGET
{
	float3 center_of_sphere = (0,0,0);
	float dist = distance(pin.posV,center_of_sphere);
	if(dist >= 4.95f)
	{
		discard;
	}

	float4 textureColor;
    float2 reflectTexCoord;
    float4 reflectionColor;
    float4 color;
	//textureColor = pin.color;
	
	//reflectTexCoord.x = pin.reflectionPosition.x / pin.reflectionPosition.w / 2.0f + 0.5f;
   //reflectTexCoord.y = -pin.reflectionPosition.y / pin.reflectionPosition.w / 2.0f + 0.5f;


	float2 ProjectedTexCoords;
    ProjectedTexCoords.x = pin.reflectionPosition.x/pin.reflectionPosition.w/2.0f + 0.5f;
    ProjectedTexCoords.y = -pin.reflectionPosition.y/pin.reflectionPosition.w/2.0f + 0.5f;

	reflectionColor = reflectionTexture.Sample(ss, ProjectedTexCoords);
	float4 transcolor = float4(1,1,1,1);
	if(transcolor.r ==reflectionColor.r && transcolor.g ==reflectionColor.g && transcolor.b ==reflectionColor.b)
	{
		reflectionColor = skyTexture.Sample(ss,pin.tex);
	}
	//textureColor = skyTexture.Sample(ss,pin.tex);
	
	return 0.4 *reflectionColor;

}
Texture2D IceTexture : register(t2);
float4 ReflectionPSIce(Reflect_VertexOut pin) : SV_TARGET
{
	float3 center_of_sphere = (0,0,0);
	float dist = distance(pin.posV,center_of_sphere);
	if(dist >= 4.95f)
	{
		discard;
	}

	float4 textureColor;
    float2 reflectTexCoord;
    float4 reflectionColor;
    float4 color;
	//textureColor = pin.color;
	
	//reflectTexCoord.x = pin.reflectionPosition.x / pin.reflectionPosition.w / 2.0f + 0.5f;
   //reflectTexCoord.y = -pin.reflectionPosition.y / pin.reflectionPosition.w / 2.0f + 0.5f;


	float2 ProjectedTexCoords;
    ProjectedTexCoords.x = pin.reflectionPosition.x/pin.reflectionPosition.w/2.0f + 0.5f;
    ProjectedTexCoords.y = -pin.reflectionPosition.y/pin.reflectionPosition.w/2.0f + 0.5f;

	reflectionColor = reflectionTexture.Sample(ss, ProjectedTexCoords);
	float4 transcolor = float4(1,1,1,1);
	if(transcolor.r ==reflectionColor.r && transcolor.g ==reflectionColor.g && transcolor.b ==reflectionColor.b)
	{
		reflectionColor = skyTexture.Sample(ss,pin.tex);
	}
	//textureColor = skyTexture.Sample(ss,pin.tex);
	float ice = IceTexture.Sample(ss,pin.TexCoordsNormal);
	color = lerp(ice, reflectionColor, 0.3f);
	//color = 
	//return color;
	return 0.4 *color;

}

float4 RenderScenePS(VertexOut pin) : SV_TARGET {
	float3 center_of_sphere = (0,0,0);
	float dist = distance(pin.posV,center_of_sphere);
	if(dist >= 4.95f)
	{
		discard;
	}
	//float4 color = (0,1,0,1);
	float4 color =  skyTexture.Sample(ss,pin.TexCoords);
    return color;
	
}

float4 PSPointLight(VertexOut pin) : SV_TARGET {


	float3 center_of_sphere = (0,0,0);
	float dist = distance(pin.posV,center_of_sphere);
	if(dist >= 4.95f)
	{
		discard;
	}
	float4 color_plus_ambuient = plight_ambient * 0.2;
	float3 Lightvector = (plight_pos - pin.posV);
	float3 pNormal = pin.normal;
   float3 LDir = normalize(Lightvector);
	float4 LightIntensity  = color_plus_ambuient * dot(normalize(pin.normal),LDir);
	float4 FinalColor = saturate( LightIntensity*dot(pNormal, LDir) );

    FinalColor.a = 1;
	return FinalColor;
}

float4 PSDirLight(VertexOut pin) : SV_TARGET {


	float3 center_of_sphere = (0,0,0);
	float dist = distance(pin.posV,center_of_sphere);
	if(dist >= 4.95f)
	{
		discard;
	}
	float4 LightColor = plight_ambient;
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
        SetVertexShader( CompileShader( vs_4_0, RelflectVS() ) );
		SetGeometryShader(NULL);
        SetPixelShader( CompileShader( ps_4_0, ReflectionPS() ) );
    
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

technique10 RenderSceneIce{
	pass P0 {
        SetVertexShader( CompileShader( vs_4_0, RelflectVS() ) );
		SetGeometryShader(NULL);
        SetPixelShader( CompileShader( ps_4_0, ReflectionPSIce() ) );
    
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


