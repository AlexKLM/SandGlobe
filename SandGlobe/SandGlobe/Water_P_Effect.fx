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
    DestBlend = ONE;
    BlendOp = ADD;
    SrcBlendAlpha = ZERO;
    DestBlendAlpha = ZERO;
    BlendOpAlpha = ADD;
    RenderTargetWriteMask[0] = 0x0F;
};

DepthStencilState DisableDepth
{
    DepthEnable = FALSE;
    DepthWriteMask = ZERO;
};

DepthStencilState EnableDepth
{
    DepthEnable = TRUE;
    DepthWriteMask = ZERO;
};

cbuffer cbImmutable 
{
    float3 g_positions[4] =
    {
        float3( -0.005, 0.005, 0 ),
        float3( 0.005, 0.005, 0 ),
        float3( -0.005, -0.005, 0 ),
        float3( 0.005, -0.005, 0 ),
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
};

struct PixelIn{
	float4 pos : SV_Position;
	float2 tex : TEXTURE0;
	float4 color : COLOR0;
};

#define Emitter 0 //emit other particles
#define Water_Drops 1 //water spary particles

#define Emit_time  0.5 //cooldown between emits
#define Water_life 1.5 // life of water particle
#define Emit_amount 10 //number of particle emitted

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

	if(vin.Type == Emitter)
	{
		vout.color = float4(1,0,0,0);
	}
	else if(vin.Type == Water_Drops)
	{
		vout.color = float4(1,1,1,1);
		vout.color *= (vin.Timer / Water_life );
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
	vin.pos = emit_pos;
	if(vin.Timer <= 0 && shouldemit >0)
	{
		VertexIn water_drop;
		for(int i=0; i < Emit_amount ; i++)
		{
			float3 vel = normalize( RandomDir( vin.Type ) );
			if(vel.y < 0)
			{
				vel.y *= -1;
			}
			water_drop.pos = vin.pos;
			water_drop.vel = vel;
			water_drop.Timer = Water_life;
			//water_drop.angle = vin.angle;
			water_drop.Type = Water_Drops;
			OutStream.Append(water_drop);
		}
		vin.Timer = Emit_time;
	}
	else
	{
		vin.Timer -= DeltaTime;
	}
	OutStream.Append(vin);
}

void GS_Calc_Water(VertexIn vin, inout PointStream<VertexIn> OutStream)
{
    if(vin.Timer > 0)//only calculate particle if 
	{
		vin.pos += vin.vel*DeltaTime;
		vin.vel += gravity*DeltaTime;
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
	else if(vin[0].Type == Water_Drops)
	{
		GS_Calc_Water(vin[0],OutStream);
	}
}



[maxvertexcount(4)]
void GS_Render(point VertexOut vin[1], inout TriangleStream<PixelIn> OutStream)
{
	PixelIn pin;

	for(int i = 0; i < 4; i ++)
	{
		float3 position = g_positions[i]*vin[0].radius;
		position = mul(position,(float3x3)InvViewMx) + vin[0].pos;
		pin.pos = mul(float4(position,1.0), worldViewProj);

		pin.color = vin[0].color;
		pin.tex = g_texcoords[i];
		OutStream.Append(pin);
	}
}

Texture2D particle_tex: register(t0); //not used, texture doesn't matter at that size
//Pixel Shader
float4 PS_Render(PixelIn pin) : SV_TARGET
{
	//return particle_tex.Sample( g_samLinear, pin.tex ) * pin.color;
	return pin.color;
}


// Techniques
technique10 RenderScene {
	pass p0
	{
		SetVertexShader(CompileShader(vs_4_0,Vertex_Render()));
		SetGeometryShader(CompileShader(gs_4_0,GS_Render()));
		SetPixelShader(CompileShader(ps_4_0,PS_Render()));
		SetBlendState( AdditiveBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetDepthStencilState( EnableDepth, 0 );
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

