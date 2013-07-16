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
	float3 cam_pos;
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

cbuffer spotlight : register(b2)
{
	float3 slight_pos;
	float  slight_range;
	float3 slight_dir;
	float slight_cone;
	float3 slight_att;
	float4 slight_ambient;
	float4 slight_diffuse;
}

cbuffer Material :register(b3)
{
	float M_ambient;
	float M_diffuse;
	float M_spec;
	float show_tex;
}

// Shaders
struct VertexIn {
    float3 pos : POSITION; 
	float3 normal: NORMAL;
    float4 color : COLOR; 
	float2 TexUV : TEXCOORD;
	float3 tangent : TANGENT;
    float3 binormal : BINORMAL;

};

struct VertexOut { 
	float4 worldpos :POSITION1;
	float3 posV : POSITION ;
	float4 posH : SV_POSITION; 
	float3 normal : NORMAL;
    float4 color : COLOR; 
	float3 tangent : TANGENT;
	float2 TexCoords : TEXCOORD;
	float3 cam_dir :TEXCOORD1;
	float clip : SV_ClipDistance0;
};

struct VertexOutFlat { 
	float4 worldpos :POSITION1;
	float3 posV : POSITION ;
	float4 posH : SV_POSITION; 
	nointerpolation float3 normal : NORMAL;
    float4 color : COLOR; 
	nointerpolation float2 TexCoords : TEXCOORD;
	float3 cam_dir :TEXCOORD1;
	float clip : SV_ClipDistance0;
};

RasterizerState mainState {
	FillMode = Solid;
	CullMode = None;
};

RasterizerState wireframe
{
	FillMode = WireFrame;
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
	vout.TexCoords = vin.TexUV;
	vout.tangent = mul(vin.tangent, worldMx);
	vout.cam_dir = normalize( cam_pos - vin.pos );
	vout.worldpos = mul(float4(vin.pos, 1.0f), worldMx);
    return vout;
}

VertexOutFlat RenderSceneVSFlat(VertexIn vin) {
    VertexOutFlat vout; 
    vout.posH = mul(float4(vin.pos, 1.0f), worldViewProj);
	vout.posV = vin.pos;
    vout.color = vin.color;
	//float3 innormal = normalize(vin.normal);
	//float3 normal = mul(innormal, worldInverseTranspose);
	//vout.normal = normal;
	vout.normal = vin.normal;
	vout.clip = dot(mul(vout.posH, worldMx), clip_level);
	vout.TexCoords = vin.TexUV;
	vout.cam_dir = normalize( cam_pos - vin.pos );
	vout.worldpos = mul(float4(vin.pos, 1.0f), worldMx);
    return vout;
}


Texture2D MyTex : register(t0);
SamplerState ss: register(s0);

float4 RenderScenePS(VertexOut pin) : SV_TARGET {

	//float4 color = (0,1,0,1);
	if(show_tex > 0)
	{
		return 0.3 * MyTex.Sample( ss, pin.TexCoords );
	}
	else
	{
		return 0.3*pin.color;
	}
    
	//return 0;
}

float4 RenderScenePSFLAT(VertexOutFlat pin) : SV_TARGET {

	//float4 color = (0,1,0,1);
	if(show_tex > 0)
	{
		return 0.3 * MyTex.Sample( ss, pin.TexCoords );
	}
	else
	{
		return 0.3*pin.color;
	}
    
	//return 0;
}

float4 PSPointLight(VertexOut pin) : SV_TARGET {
	float4 diffuse;
	if(show_tex > 0)
	{
		diffuse =  MyTex.Sample( ss, pin.TexCoords );
	}
	else
	{
		diffuse = pin.color;
	}
	
	pin.normal = normalize(pin.normal);
	float4 finalColor = float4(0,0,0,1);
	
	float3 lightToPixelVec = plight_pos - pin.posV;
	float d = length(lightToPixelVec);

	//float3 finalAmbient = diffuse * plight_ambient;

	float3 light_dir = normalize(pin.posV - plight_pos);
		 float3 R = reflect( light_dir, pin.normal);
		 float3 H = normalize( -light_dir + pin.cam_dir );


		 float4 final_a = M_ambient * plight_ambient;
		 float4 final_d = M_diffuse * saturate( dot(pin.normal,-light_dir) );
		 float4 final_s = M_spec * pow( saturate(dot(pin.normal,H)), 1 );
		 finalColor =  final_a + (final_d + final_s) * plight_ambient;

		 return saturate(finalColor * diffuse);
	
	//finalColor =  saturate((finalColor+finalSpec) + finalAmbient);
	//finalColor = finalSpec + finalColor;
	//return float4(finalSpec, diffuse.a);
	//return  float4(finalColor, diffuse.a);
}

float4 PSDirLight(VertexOut pin) : SV_TARGET {

	float4 diffuse;
	if(show_tex > 0)
	{
		diffuse =  MyTex.Sample( ss, pin.TexCoords );
	}
	else
	{
		diffuse = pin.color;
	}
	pin.normal = normalize(pin.normal);
	float4 finalColor = float4(0,0,0,1);
	

	float3 finalAmbient = diffuse * plight_ambient;

	float3 light_dir = normalize(plight_direction);
		 float3 R = reflect( light_dir, pin.normal);
		 float3 H = normalize( -light_dir + pin.cam_dir );


		float4 final_a = M_ambient * plight_ambient;
		float4 final_d = M_diffuse * saturate( dot(pin.normal,-light_dir) );
		float4 final_s = M_spec * pow( saturate(dot(pin.normal,H)), 1 );
		finalColor =  final_a + (final_d + final_s) * plight_ambient;

		 return saturate(finalColor * diffuse);
}



float4 PSDirLightFLAT(VertexOutFlat pin) : SV_TARGET {

	float4 diffuse;
	if(show_tex > 0)
	{
		diffuse =  MyTex.Sample( ss, pin.TexCoords );
	}
	else
	{
		diffuse = pin.color;
	}
	pin.normal = normalize(pin.normal);
	float4 finalColor = float4(0,0,0,1);
	
	//float3 lightToPixelVec = plight_pos - pin.posV;
	//float d = length(lightToPixelVec);

	float3 finalAmbient = diffuse * plight_ambient;

	float3 light_dir = normalize(plight_direction);
	float3 R = reflect( light_dir, pin.normal);
	float3 H = normalize( -light_dir + pin.cam_dir );


		float4 final_a = M_ambient * plight_ambient;
		float4 final_d = M_diffuse * saturate( dot(pin.normal,-light_dir) );
		float4 final_s = M_spec * pow( saturate(dot(pin.normal,H)), 1 );
		finalColor =  final_a + (final_d + final_s) * plight_ambient;

		 return saturate(finalColor * diffuse);
		 //float intensity = dot(normalize(pin.normal), normalize(light_dir));
   // return intensity * diffuse;
	
	//finalColor =  saturate((finalColor+finalSpec) + finalAmbient);
	//finalColor = finalSpec + finalColor;
	//return float4(finalSpec, diffuse.a);
	//return  float4(finalColor, diffuse.a);
}


Texture2D NormMap : register(t1);
float4 PSMultiLBump(VertexOut pin) : SV_TARGET {

	float4 diffuse;
	diffuse =  MyTex.Sample( ss, pin.TexCoords );
	

	float4 normalMap = NormMap.Sample( ss, pin.TexCoords );
	
	normalMap = (2.0f*normalMap) - 1.0f;

	pin.tangent = normalize(pin.tangent - dot(pin.tangent, pin.normal)*pin.normal);

	float3 biTangent = cross(pin.normal, pin.tangent);

	float3x3 texSpace = float3x3(pin.tangent, biTangent, pin.normal);

	pin.normal = normalize(mul(normalMap, texSpace));



	//pin.normal = normalize(pin.normal);
	float4 finalColor = float4(0,0,0,1);
	
	float3 lightToPixelVec = plight_pos - pin.posV;
	float d = length(lightToPixelVec);

	float3 finalAmbient = diffuse * plight_ambient;

	float3 light_dir = normalize(pin.posV - plight_pos);
		 float3 R = reflect( light_dir, pin.normal);
		 float3 H = normalize( -light_dir + pin.cam_dir );


		 float4 final_a = M_ambient * plight_ambient;
		 float4 final_d = M_diffuse * saturate( dot(pin.normal,-light_dir) );
		 float4 final_s = M_spec * pow( saturate(dot(pin.normal,H)), 1 );
		 finalColor =  final_a + (final_d + final_s) * plight_ambient;

		 return saturate(finalColor * diffuse);
	
	//finalColor =  saturate((finalColor+finalSpec) + finalAmbient);
	//finalColor = finalSpec + finalColor;
	//return float4(finalSpec, diffuse.a);
	//return  float4(finalColor, diffuse.a);
}

float4 PSDirLightBump(VertexOut pin) : SV_TARGET {

	float4 diffuse =  MyTex.Sample( ss, pin.TexCoords );
	

	float4 normalMap = NormMap.Sample( ss, pin.TexCoords );
	
	normalMap = (2.0f*normalMap) - 1.0f;

	pin.tangent = normalize(pin.tangent - dot(pin.tangent, pin.normal)*pin.normal);

	float3 biTangent = cross(pin.normal, pin.tangent);

	float3x3 texSpace = float3x3(pin.tangent, biTangent, pin.normal);

	pin.normal = normalize(mul(normalMap, texSpace));

	float4 finalColor = float4(0,0,0,1);
	
	//float3 lightToPixelVec = plight_pos - pin.posV;
	//float d = length(lightToPixelVec);

	float3 finalAmbient = diffuse * plight_ambient;

	float3 light_dir = normalize(pin.posV - plight_pos);
		 float3 R = reflect( light_dir, pin.normal);
		 float3 H = normalize( -light_dir + pin.cam_dir );


		float4 final_a = M_ambient * plight_ambient;
		float4 final_d = M_diffuse * saturate( dot(pin.normal,-light_dir) );
		float4 final_s = M_spec * pow( saturate(dot(pin.normal,H)), 1 );
		finalColor =  final_a + (final_d + final_s) * plight_ambient;

		 return saturate(finalColor * diffuse);
		 //float intensity = dot(normalize(pin.normal), normalize(light_dir));
   // return intensity * diffuse;
	
	//finalColor =  saturate((finalColor+finalSpec) + finalAmbient);
	//finalColor = finalSpec + finalColor;
	//return float4(finalSpec, diffuse.a);
	//return  float4(finalColor, diffuse.a);
}

float4 PSMultiLFLAT(VertexOutFlat pin) : SV_TARGET {

	float4 diffuse;
	if(show_tex > 0)
	{
		diffuse =  MyTex.Sample( ss, pin.TexCoords );
	}
	else
	{
		diffuse = pin.color;
	}
	pin.normal = normalize(pin.normal);
	float4 finalColor = float4(0,0,0,1);
	
	float3 lightToPixelVec = plight_pos - pin.posV;
	float d = length(lightToPixelVec);

	float3 finalAmbient = diffuse * plight_ambient;

	float3 light_dir = normalize(pin.posV - plight_pos);
		 float3 R = reflect( light_dir, pin.normal);
		 float3 H = normalize( -light_dir + pin.cam_dir );


		 float4 final_a = M_ambient * plight_ambient;
		 float4 final_d = M_diffuse * saturate( dot(pin.normal,-light_dir) );
		 float4 final_s = M_spec * pow( saturate(dot(pin.normal,H)), 1 );
		 finalColor =  final_a + (final_d + final_s) * plight_ambient;

		 return saturate(finalColor * diffuse);
	
	//finalColor =  saturate((finalColor+finalSpec) + finalAmbient);
	//finalColor = finalSpec + finalColor;
	//return float4(finalSpec, diffuse.a);
	//return  float4(finalColor, diffuse.a);
}

float4 PSMultiL(VertexOut pin) : SV_TARGET {

	float4 diffuse;
	if(show_tex > 0)
	{
		diffuse =  MyTex.Sample( ss, pin.TexCoords );
	}
	else
	{
		diffuse = pin.color;
	}
	pin.normal = normalize(pin.normal);
	float4 finalColor = float4(0,0,0,1);
	
	float3 lightToPixelVec = plight_pos - pin.posV;
	float d = length(lightToPixelVec);

	float3 finalAmbient = diffuse * plight_ambient;

	float3 light_dir = normalize(pin.posV - plight_pos);
		 float3 R = reflect( light_dir, pin.normal);
		 float3 H = normalize( -light_dir + pin.cam_dir );


		 float4 final_a = M_ambient * plight_ambient;
		 float4 final_d = M_diffuse * saturate( dot(pin.normal,-light_dir) );
		 float4 final_s = M_spec * pow( saturate(dot(pin.normal,H)), 1 );
		 finalColor =  final_a + (final_d + final_s) * plight_ambient;

		 return saturate(finalColor * diffuse);
	
	//finalColor =  saturate((finalColor+finalSpec) + finalAmbient);
	//finalColor = finalSpec + finalColor;
	//return float4(finalSpec, diffuse.a);
	//return  float4(finalColor, diffuse.a);
}

float4 PSSpotLight(VertexOut pin) : SV_TARGET{ //SPOT LIGHT IS DISABLED DUE TO BUG

	float4 LightColor = (1,1,1,1);


	float3 finalColor = float3(0,0,0);

	float4 diffuse =  pin.color;

	pin.normal = pin.normal;

	float3 lightToPixelVec = pin.posV - slight_pos  ;

	float d = length(lightToPixelVec);
	float4 ambient = float4(0.3f, 0.3f, 0.3f, 0.3f);
	float3 finalAmbient = diffuse*ambient;

	if( d > slight_range )
	{
		return float4(0,1,0,1);
		return float4(finalAmbient, diffuse.a);
	}
	lightToPixelVec /= d; 

	float howMuchLight = dot(lightToPixelVec, pin.normal);

	if( howMuchLight > 0.0f )
	{	
		//Add light to the finalColor of the pixel
		finalColor += diffuse * slight_diffuse;
					
		//Calculate Light's Distance Falloff factor
		finalColor /= (slight_att[0] + (slight_att[1] * d)) + (slight_att[2] * (d*d));		

		//Calculate falloff from center to edge of pointlight cone
		//finalColor *= pow(max(dot(-lightToPixelVec, slight_dir), 0.0f), slight_cone);

	}

	finalColor = saturate(finalColor + finalAmbient);

	return float4(finalColor, diffuse.a);

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
        SetPixelShader( CompileShader( ps_4_0, PSDirLight() ) );
    
		SetRasterizerState( mainState );  
	}
}

technique10 RenderSceneFLAT {
    pass P0 {
        SetVertexShader( CompileShader( vs_4_0, RenderSceneVSFlat() ) );
		SetGeometryShader(NULL);
        SetPixelShader( CompileShader( ps_4_0, RenderScenePSFLAT() ) );
    
		SetRasterizerState( mainState );  
	}
	 pass P1 {
        SetVertexShader( CompileShader( vs_4_0, RenderSceneVSFlat() ) );
		SetGeometryShader(NULL);
        SetPixelShader( CompileShader( ps_4_0, PSDirLightFLAT() ) );
    
		SetRasterizerState( mainState );  
	}
}

technique10 RenderSceneWireFrame{
    pass P0 {
        SetVertexShader( CompileShader( vs_4_0, RenderSceneVS() ) );
		SetGeometryShader(NULL);
        SetPixelShader( CompileShader( ps_4_0, RenderScenePS() ) );
    
		SetRasterizerState( wireframe );  
	}
	 pass P1 {
        SetVertexShader( CompileShader( vs_4_0, RenderSceneVS() ) );
		SetGeometryShader(NULL);
        SetPixelShader( CompileShader( ps_4_0, PSDirLight() ) );
    
		SetRasterizerState( wireframe );  
	}
}

technique10 RenderSceneMultiWireFrame{
	pass P0 {
        SetVertexShader( CompileShader( vs_4_0, RenderSceneVS() ) );
		SetGeometryShader(NULL);
        SetPixelShader( CompileShader( ps_4_0, RenderScenePS() ) );
    
		SetRasterizerState( wireframe );  
	}
	 pass P1 {
        SetVertexShader( CompileShader( vs_4_0, RenderSceneVS() ) );
		SetGeometryShader(NULL);
        SetPixelShader( CompileShader( ps_4_0, PSDirLight() ) );
    
		SetRasterizerState( wireframe );  
	}
}

technique10 RenderSceneMulti{
   pass P0 {
        SetVertexShader( CompileShader( vs_4_0, RenderSceneVS() ) );
		SetGeometryShader(NULL);
        SetPixelShader( CompileShader( ps_4_0, RenderScenePS() ) );
    
		SetRasterizerState( mainState );  
	}
    pass P1 {
        SetVertexShader( CompileShader( vs_4_0, RenderSceneVS() ) );
		SetGeometryShader(NULL);
        SetPixelShader( CompileShader( ps_4_0, PSMultiL() ) );
    
		SetRasterizerState( mainState );  
	}
}

technique10 RenderSceneMultiFLAT{
   pass P0 {
        SetVertexShader( CompileShader( vs_4_0, RenderSceneVSFlat() ) );
		SetGeometryShader(NULL);
        SetPixelShader( CompileShader( ps_4_0, RenderScenePSFLAT() ) );
    
		SetRasterizerState( mainState );  
	}
    pass P1 {
        SetVertexShader( CompileShader( vs_4_0, RenderSceneVSFlat() ) );
		SetGeometryShader(NULL);
        SetPixelShader( CompileShader( ps_4_0, PSMultiLFLAT() ) );
    
		SetRasterizerState( mainState );  
	}
}

technique10 RenderSceneBump{
    pass P0 {
        SetVertexShader( CompileShader( vs_4_0, RenderSceneVS() ) );
		SetGeometryShader(NULL);
        SetPixelShader( CompileShader( ps_4_0, RenderScenePS() ) );
    
		SetRasterizerState( mainState );  
	}
	 pass P1 {
        SetVertexShader( CompileShader( vs_4_0, RenderSceneVS() ) );
		SetGeometryShader(NULL);
        SetPixelShader( CompileShader( ps_4_0, PSDirLightBump() ) );
    
		SetRasterizerState( mainState );  
	}
}

technique10 RenderSceneMultiBump{
    pass P0 {
        SetVertexShader( CompileShader( vs_4_0, RenderSceneVS() ) );
		SetGeometryShader(NULL);
        SetPixelShader( CompileShader( ps_4_0, RenderScenePS() ) );
    
		SetRasterizerState( mainState );  
	}
	 pass P1 {
        SetVertexShader( CompileShader( vs_4_0, RenderSceneVS() ) );
		SetGeometryShader(NULL);
        SetPixelShader( CompileShader( ps_4_0, PSMultiLBump() ) );
    
		SetRasterizerState( mainState );  
	}
}

