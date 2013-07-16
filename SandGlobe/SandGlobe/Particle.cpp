#include "Particle.h"

static const int MAX_PARTICLES =  30000;

Particle::Particle(void)
{
	RandomTexture = NULL;
	life_timer = NULL;
	first_cycle = true;
	RandomTexRV = NULL;
	Particle_tex = NULL;
	dt_gtBuffer = NULL;
	c_cBuffer = NULL;
	r_cBuffer = NULL;
	_dxDev = NULL;
	_dxImmedDC = NULL;
	manager = NULL;
	Stream_target_buffer = NULL;
	render_buffer = NULL;
	start_buffer = NULL;
	die_ready = false;
	mark_to_kill = false;
}

Particle::~Particle(void)
{

}

void Particle::destroy()
{
	if(Stream_target_buffer != 0)
	{
		Stream_target_buffer->Release();
	}

	if(render_buffer != 0)
	{
		render_buffer->Release();
	}

	if(start_buffer != 0)
	{
		start_buffer->Release();
	}

	if(RandomTexture != 0)
	{
		RandomTexture->Release();
	}

	if(RandomTexRV != 0)
	{
		RandomTexRV->Release();
	}

	if(Particle_tex != 0)
	{
		Particle_tex->Release();
	}

	if(dt_gtBuffer != 0)
	{
		dt_gtBuffer->Release();
	}

	if(c_cBuffer != 0)
	{
		c_cBuffer->Release();
	}

	if(r_cBuffer !=0)
	{
		r_cBuffer->Release();
	}
}

void Particle::initialize(Scene_manager* _manager,ID3D11DeviceContext *_dxDC, ID3D11Device *_dxD, const XMFLOAT3& _position,float final_life_time)
{
	life_timer = final_life_time;
	position = _position;
	manager = _manager;
	first_cycle = true;
	_dxDev = _dxD;
	_dxImmedDC = _dxDC;
	//create_buffers();
	//generate_randoms();
}

void Particle::create_buffers()
{
		particle_Vertex seed =
		{
			position,
			XMFLOAT3(0,0.5f,0),
			//float(i),
			float(0),
			UINT(0),//is Emitter type
		};
	D3D11_BUFFER_DESC buffDesc;
	buffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffDesc.ByteWidth = sizeof(particle_Vertex);
	buffDesc.Usage = D3D11_USAGE_DEFAULT;
	buffDesc.CPUAccessFlags = 0;
	buffDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vbInitData;
	ZeroMemory(&vbInitData,sizeof(D3D11_SUBRESOURCE_DATA));
	vbInitData.pSysMem = &seed;
	vbInitData.SysMemPitch = 0;
	vbInitData.SysMemSlicePitch = 0;
	//vbInitData.SysMemPitch = sizeof(particle_Vertex);
	_dxDev->CreateBuffer(&buffDesc,&vbInitData,&start_buffer);//seed buffer for first cycle

	//create the other 2 buffers
	buffDesc.ByteWidth = MAX_PARTICLES * sizeof(particle_Vertex);
	buffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;
	_dxDev->CreateBuffer(&buffDesc,NULL,&render_buffer);
	_dxDev->CreateBuffer(&buffDesc,NULL,&Stream_target_buffer);
	HRESULT hr;

	ZeroMemory(&buffDesc, sizeof(buffDesc));
	buffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffDesc.ByteWidth = (sizeof(cal_cBuffer));
	//buffDesc.ByteWidth = 64;
	buffDesc.Usage = D3D11_USAGE_DEFAULT;
	hr = _dxDev->CreateBuffer(&buffDesc, 0, &c_cBuffer);

	buffDesc.ByteWidth = (sizeof(timeBuffers));
	hr = _dxDev->CreateBuffer(&buffDesc, 0, &dt_gtBuffer);

	buffDesc.ByteWidth = (sizeof(render_cBuffer));
	hr = _dxDev->CreateBuffer(&buffDesc, 0, &r_cBuffer);
}

void Particle::generate_randoms()
{
	int iNumRandValues = 1024;
    srand( (unsigned)time(NULL));

    D3D11_SUBRESOURCE_DATA InitData;
    InitData.pSysMem = new float[iNumRandValues * 4];
    InitData.SysMemPitch = iNumRandValues * 4 * sizeof( float );
    InitData.SysMemSlicePitch = iNumRandValues * 4 * sizeof( float );
    for( int i = 0; i < iNumRandValues * 4; i++ )
    {
		float f = float( ( rand() % 10000 ) - 5000 );
        ( ( float* )InitData.pSysMem )[i] = f;
    }

    D3D11_TEXTURE1D_DESC dstex;
    dstex.Width = iNumRandValues;
    dstex.MipLevels = 1;
    dstex.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    dstex.Usage = D3D11_USAGE_DEFAULT;
    dstex.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    dstex.CPUAccessFlags = 0;
    dstex.MiscFlags = 0;
    dstex.ArraySize = 1;
    _dxDev->CreateTexture1D( &dstex, &InitData, &RandomTexture ) ;

    D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
    ZeroMemory( &SRVDesc, sizeof( SRVDesc ) );
    SRVDesc.Format = dstex.Format;
    SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
    SRVDesc.Texture2D.MipLevels = dstex.MipLevels;
    _dxDev->CreateShaderResourceView( RandomTexture, &SRVDesc, &RandomTexRV ) ;
}

void Particle::Calculate_Particles(double dt, shaders& _sh,bool _shouldemit)
{
	if(mark_to_kill)
	{
		if(life_timer > 0)
		{
			life_timer -=dt;
		}
		else
		{
			die_ready = true;
			return;
		}
	}
	if(_shouldemit)
	{
		c_data.shouldemit = 1;
	}
	else
	{
		c_data.shouldemit = -1;
	}
	//c_data.gravity = XMFLOAT4(0,-1,0,0);
	tBuffer.padding2 = XMFLOAT2(0,0);
	tBuffer.GlobalTime =  float( ( rand() % 10000 ) - 5000 );
	tBuffer.DeltaTime = dt;

	if(!first_cycle)
	{
		_sh.set_active_calc_particles(render_buffer,0);
	}
	else
	{
		_sh.set_active_calc_particles(start_buffer,0);
	}
	_dxImmedDC->UpdateSubresource(c_cBuffer, 0, 0, &c_data,0, 0);
	_dxImmedDC->GSSetConstantBuffers(0,1,&c_cBuffer);
	_dxImmedDC->UpdateSubresource(dt_gtBuffer, 0, 0, &tBuffer,0, 0);
	_dxImmedDC->GSSetConstantBuffers(1,1,&dt_gtBuffer);
	_dxImmedDC->GSSetShaderResources( 0, 1, &RandomTexRV );
	UINT offset[1] = { 0 };
	ID3D11Buffer* pBuffers[1];
	pBuffers[0] = Stream_target_buffer;

	_dxImmedDC->SOSetTargets( 1, pBuffers, offset );
	
	if(!first_cycle)
	{
		_dxImmedDC->DrawAuto();
	}
	else
	{
		_dxImmedDC->Draw(1,0);
	}
	pBuffers[0] = NULL;
    _dxImmedDC->SOSetTargets( 1, pBuffers, offset );
	ID3D11Buffer* tempB = render_buffer;
	render_buffer = Stream_target_buffer;
	Stream_target_buffer = tempB;
	first_cycle = false;
}

void Particle::Calculate_Particles(double dt, shaders& _sh, const cal_cBuffer& input)
{
	if(mark_to_kill)
	{
		if(life_timer > 0)
		{
			life_timer -=dt;
		}
		else
		{
			die_ready = true;
			return;
		}
	}
	tBuffer.padding2 = XMFLOAT2(0,0);
	tBuffer.GlobalTime =  float( ( rand() % 10000 ) - 5000 );
	tBuffer.DeltaTime = dt;

	if(!first_cycle)
	{
		_sh.set_active_calc_particles(render_buffer,0);
	}
	else
	{
		_sh.set_active_calc_particles(start_buffer,0);
	}
	_dxImmedDC->UpdateSubresource(c_cBuffer, 0, 0, &input,0, 0);
	_dxImmedDC->GSSetConstantBuffers(0,1,&c_cBuffer);
	_dxImmedDC->UpdateSubresource(dt_gtBuffer, 0, 0, &tBuffer,0, 0);
	_dxImmedDC->GSSetConstantBuffers(1,1,&dt_gtBuffer);
	_dxImmedDC->GSSetShaderResources( 0, 1, &RandomTexRV );
	UINT offset[1] = { 0 };
	ID3D11Buffer* pBuffers[1];
	pBuffers[0] = Stream_target_buffer;

	_dxImmedDC->SOSetTargets( 1, pBuffers, offset );
	
	if(!first_cycle)
	{
		_dxImmedDC->DrawAuto();
	}
	else
	{
		_dxImmedDC->Draw(1,0);
	}
	pBuffers[0] = NULL;
    _dxImmedDC->SOSetTargets( 1, pBuffers, offset );
	ID3D11Buffer* tempB = render_buffer;
	render_buffer = Stream_target_buffer;
	Stream_target_buffer = tempB;
	first_cycle = false;
}

void Particle::Render(const XMMATRIX& viewMX,const XMMATRIX& projMX, shaders& _sh)
{
	XMMATRIX WVP_MX;
	WVP_MX = viewMX*projMX;
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(viewMX),viewMX);
	XMStoreFloat4x4(&r_data.worldViewProj,XMMatrixTranspose(WVP_MX));
	XMStoreFloat4x4(&r_data.InvViewMx,XMMatrixTranspose(invView));
	_sh.set_active_rend_particles(render_buffer,0);
	_dxImmedDC->PSSetShaderResources( 1, 1, &Particle_tex );
	_dxImmedDC->UpdateSubresource(r_cBuffer, 0, 0, &r_data,0, 0);
	_dxImmedDC->GSSetConstantBuffers(2,1,&r_cBuffer);
	_dxImmedDC->DrawAuto();
}

void Particle::swap_buffers()
{
	UINT offset[1] = { 0 };
	ID3D11Buffer* pBuffers[1];
	pBuffers[0] = NULL;
    _dxImmedDC->SOSetTargets( 1, pBuffers, offset );
	ID3D11Buffer* tempB = render_buffer;
	render_buffer = Stream_target_buffer;
	Stream_target_buffer = tempB;
	first_cycle = false;
}

void Particle::set_particle_texture(LPCWSTR tex_name)
{
	D3DX11CreateShaderResourceViewFromFile( _dxDev, tex_name,
		NULL, NULL, &Particle_tex, NULL );
}

bool Particle::is_die_ready() const
{
	return die_ready;
}

void Particle::set_delete()
{
	mark_to_kill = true;
}

void Particle::set_gravity(const XMFLOAT4& grav)
{
	c_data.gravity = grav;
}

void Particle::set_pos(const XMFLOAT3& input)
{
	position = input;
}

void Particle::set_c_data(const cal_cBuffer& input)
{
	c_data = input;
}

const XMFLOAT3& Particle::get_pos() const
{
	return position;
}

const cal_cBuffer& Particle::get_c_data() const
{
	return c_data;
}

const float Particle::get_life_timer() const
{
	return life_timer;
}

bool Particle::is_first_cycle() const
{
	return first_cycle;
}

bool Particle::is_mark_to_kill() const
{
	return mark_to_kill;
}

void Particle::set_dieready(bool input)
{
	die_ready = input;
}

void Particle::set_mark_to_kill(bool input)
{
	mark_to_kill = input;
}

void Particle::set_first_cycle(bool input)
{
	first_cycle = input;
}

const timeBuffers& Particle::get_tBuffer() const
{
	return tBuffer;
}

const render_cBuffer& Particle::get_r_data() const
{
	return r_data;
}

ID3D11Buffer* Particle::return_dt_gtBuffer() const
{
	return dt_gtBuffer;
}

ID3D11Buffer* Particle::return_c_cBuffer() const
{
	return c_cBuffer;
}

ID3D11Buffer* Particle::return_r_cBuffer() const
{
	return r_cBuffer;
}

ID3D11Buffer* Particle::return_S_T_buffer() const
{
	return Stream_target_buffer;
}

ID3D11Buffer* Particle::return_render_buffer() const
{
	return render_buffer;
}

ID3D11Buffer* Particle::return_start_buffer() const
{
	return start_buffer;
}

ID3D11ShaderResourceView* Particle::return_randomTextureRV() const
{
	return RandomTexRV;
}

ID3D11ShaderResourceView* Particle::return_particle_tex() const
{
	return Particle_tex;
}

ID3D11Device* Particle::get_DXDevice() const
{
	return _dxDev;
}

ID3D11DeviceContext* Particle::get_DeviceContext() const
{
	return _dxImmedDC;
}

