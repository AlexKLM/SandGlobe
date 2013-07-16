// Add the following to the effects file properties: Configuations Proporties->Custom Build Tool->General
// [Debug] Command Line:  fxc /Fc "fx/%(Filename).cod" /Od /Zi /T fx_5_0 /Fo "fx/%(Filename).fxo" "%(FullPath)"
// [Debug] Output: fx/%(Filename).fxo; fx/%(Filename).cod
// [Release] Command Line: fxc  /T fx_5_0 /Fo "fx/%(Filename).fxo" "%(FullPath)"
// [Release] Outputs: fx/%(Filename).fxo

// Global variables
cbuffer vars_for_calculate:register(b0) 
{
	float shouldemit; //act as boolean(1 or -1), also as a padding
	float3 emit_pos;
	float4 gravity;
};

cbuffer global_delta:register(b1)
{
	float2 padding2;
	float GlobalTime;
	float DeltaTime;
};
cbuffer vars_for_render:register(b2)
{
	float4x4 worldViewProj;
	float4x4 InvViewMx;
}

cbuffer vars_for_render:register(b3)
{
	float sea_level;
	float3 padding;
}

SamplerState g_samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
};

SamplerState g_samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Wrap;
};


BlendState AdditiveBlending
{
    AlphaToCoverageEnable = FALSE;
    BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
    BlendOp = ADD;
    SrcBlendAlpha = ZERO;
    DestBlendAlpha = ZERO;
    BlendOpAlpha = ADD;
    RenderTargetWriteMask[0] = 0x0F ;
};

DepthStencilState DisableDepth
{
    DepthEnable = FALSE;
    DepthWriteMask = ZERO;
};

cbuffer cbImmutable 
{
	float3 fog_positions[4] = 
	{
        float3( -0.5, 0.5, 0 ),
        float3( 0.5, 0.5, 0 ),
        float3( -0.5, -0.5, 0 ),
        float3( 0.5, -0.5, 0 ),
    };

    float2 g_texcoords[4] = 
    { 
        float2(0,1), 
        float2(1,1),
        float2(0,0),
        float2(1,0),
    };
};


// Shaders
struct VertexIn {
    float3 pos : POSITION; 
    float3 vel : NORMAL;
	float Timer : TIMER;
	//float angle : ANGLE;
	uint Type : TYPE;
};

struct VertexOut { 
    float3 pos : POSITION; 
    float4 color : COLOR; 
	float radius : RADIUS;
	uint Type :TYPE;
};

struct PixelIn{
	float4 pos : SV_Position;
	float3 posV : POSITION ;
	float2 tex : TEXTURE0;
	float4 color : COLOR0;
};

#define Emitter 0 //emit other particles
#define Fog 1 //water spary particles
#define Emit_time  1 //cooldown between emits
#define Fog_life 18 // life of fog particle
#define Emit_amount 1 //number of Flame particle emitted


//VERTEX SHADER
VertexIn VSPassthrough(VertexIn vin) //do nothin, only pass data to GS
{
	return vin;
}

VertexOut Vertex_Render(VertexIn vin)
{
	VertexOut vout;

	vout.pos = vin.pos;
	vout.radius = 2;
	vout.Type = vin.Type;

	if(vin.Type == Emitter)
	{
		vout.color = float4(1,0,0,0);
	}
	else if(vin.Type == Fog)
	{
		vout.color = float4(1,1,1,0.5);
		//vout.color.y = (vin.Timer / Fog_life );
		vout.color.w *= (vin.Timer / Fog_life ) * 1.8;
	}
	return vout;
}

Texture1D ramdom_tex: register(t0);
//UTILITY METHODS
float3 RandomDir(float fOffset) //use random texture to create a random direction
{
    float tCoord = (GlobalTime + fOffset) / 300.0;
    return ramdom_tex.SampleLevel( g_samPoint, tCoord, 0 );
}


//GEO SHADERS
void GS_Calc_Emitter(VertexIn vin, inout PointStream<VertexIn> OutStream)
{
	//vin.pos = emit_pos;
	if(vin.Timer <= 0 && shouldemit >0)
	{
		VertexIn Fog_p;
		for(int i=0; i < Emit_amount ; i++)
		{
			float3 vel = normalize( RandomDir( vin.Type ) );
			vel.y = clamp(vel.y,0.1,0.2);
			//if(vel.y <0)
			//{
			//vel.y *= -1;
			//}
			vel.x = clamp(vel.x,-0.1,0.1);
			vel.z = clamp(vel.z,-0.1,0.1);
			Fog_p.pos = vin.pos;
			Fog_p.vel = vel;
			Fog_p.Timer = Fog_life;
			//water_drop.angle = vin.angle;
			Fog_p.Type = Fog;
			OutStream.Append(Fog_p);
		}
		vin.Timer = Emit_time;
	}
	else
	{
		vin.Timer -= DeltaTime;
	}
	OutStream.Append(vin);
}

void GS_Calc_Fog(VertexIn vin, inout PointStream<VertexIn> OutStream)
{
    if(vin.Timer > 0)//only calculate particle if still alive 
	{
		vin.pos += vin.vel*DeltaTime;
		//vin.vel += gravity*DeltaTime;
		vin.Timer -= DeltaTime;
		OutStream.Append(vin); 
	}

}


[maxvertexcount(128)]
void GScalculate_Particles_main(point VertexIn vin[1], inout PointStream<VertexIn> OutStream)
{
	if(vin[0].Type == Emitter)
	{
		GS_Calc_Emitter(vin[0],OutStream);
	}
	else if(vin[0].Type == Fog)
	{
		GS_Calc_Fog(vin[0],OutStream);
	}
}



[maxvertexcount(4)]
void GS_Render(point VertexOut vin[1], inout TriangleStream<PixelIn> OutStream)
{
	PixelIn pin;

	for(int i = 0; i < 4; i ++)
	{
		float3 position;
		if(vin[0].Type == Fog)
		{
			position = fog_positions[i]*vin[0].radius;
		}
		position = mul(position,(float3x3)InvViewMx) + vin[0].pos;
		pin.pos = mul(float4(position,1.0), worldViewProj);
		pin.posV = position;
		pin.color = vin[0].color;
		pin.tex = g_texcoords[i];
		OutStream.Append(pin);
	}
}

Texture2D particle_tex: register(t1); 
//Pixel Shader
float4 PS_Render(PixelIn pin) : SV_TARGET
{
	if(pin.posV.y < sea_level)
	{
		discard;
	}
	float3 center_of_sphere = (0,0,0);
	float dist = distance(pin.posV,center_of_sphere);
	if(dist >= 4.95f)
	{
		discard;
	}
	//float4 color = particle_tex.Sample( g_samLinear, pin.tex );
	//color.w = pin.color.w;

	return particle_tex.Sample( g_samLinear, pin.tex ) * pin.color;
	//return color;
	//return pin.color;
	//return pin.color;
}


// Techniques
technique10 RenderScene {
	pass p0
	{
		SetVertexShader(CompileShader(vs_4_0,Vertex_Render()));
		SetGeometryShader(CompileShader(gs_4_0,GS_Render()));
		SetPixelShader(CompileShader(ps_4_0,PS_Render()));
		SetBlendState( AdditiveBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );

        //SetDepthStencilState( DisableDepth, 0 );
	}

}

GeometryShader geo_shade  = ConstructGSWithSO(CompileShader(gs_4_0,GScalculate_Particles_main()),"POSITION.xyz;NORMAL.xyz;TIMER.x;TYPE.x;");
technique10 Calculate_Particles{

	pass p0
	{
		SetVertexShader(CompileShader(vs_4_0,VSPassthrough()));
		SetGeometryShader(geo_shade);
		SetPixelShader(NULL);

		SetDepthStencilState( DisableDepth, 0 );
	}

}

