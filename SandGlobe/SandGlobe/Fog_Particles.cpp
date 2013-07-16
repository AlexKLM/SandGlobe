#include "Fog_Particles.h"


Fog_Particles::Fog_Particles(void)
{
	current_time = 0;
	p_pvBuffer = NULL;
	sea_level = 0;
}

Fog_Particles::~Fog_Particles(void)
{
}


void Fog_Particles::initialize(Scene_manager* _manager,ID3D11DeviceContext *_dxDC, ID3D11Device *_dxD,const XMFLOAT3& _position,float final_fog_life)
{
	current_time = final_fog_life;

	Particle::initialize(_manager,_dxDC,_dxD,_position,final_fog_life);

}


void Fog_Particles::create_buffers()
{
	Particle::create_buffers();
	D3D11_BUFFER_DESC buffDesc;
	D3D11_SUBRESOURCE_DATA vbInitData;
	ZeroMemory(&buffDesc, sizeof(buffDesc));
	buffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	buffDesc.Usage = D3D11_USAGE_DEFAULT;
	buffDesc.CPUAccessFlags = 0;

	buffDesc.ByteWidth = (sizeof(PVdata));	
	Particle::get_DXDevice()->CreateBuffer(&buffDesc, 0, &p_pvBuffer);
}

void Fog_Particles::Calculate_Particles(double dt, shaders& _sh,bool _shouldemit, float _sea_level)
{
	sea_level = _sea_level;
	if(Particle::is_mark_to_kill())
	{
		if(current_time > 0)
		{
			current_time -=dt;
		}
		else
		{
			Particle::set_dieready(true);
			current_time = Particle::get_life_timer();
			return;
		}
	}
	cal_cBuffer temp_cdata = Particle::get_c_data();
	timeBuffers temp_tBuffer = Particle::get_tBuffer();

	if(_shouldemit)
	{
		temp_cdata.shouldemit = 1;
	}
	else
	{
		temp_cdata.shouldemit = -1;
	}
	temp_cdata.gravity = XMFLOAT4(0,-1,0,0);
	temp_tBuffer.padding2 = XMFLOAT2(0,0);
	temp_tBuffer.GlobalTime =  float( ( rand() % 10000 ) - 5000 );
	temp_tBuffer.DeltaTime = dt;

	if(!Particle::is_first_cycle())
	{
		_sh.set_active_calc_particles(Particle::return_render_buffer(),0);
	}
	else
	{
		_sh.set_active_calc_particles(Particle::return_start_buffer(),0);
	}
	ID3D11Buffer* c_cBufferptr = Particle::return_c_cBuffer();
	ID3D11Buffer* dt_gtBufferptr = Particle::return_dt_gtBuffer();
	ID3D11Buffer* Stream_target_bufferptr = Particle::return_S_T_buffer();
	ID3D11ShaderResourceView* RandomTexptr = Particle::return_randomTextureRV();

	Particle::get_DeviceContext()->UpdateSubresource(c_cBufferptr, 0, 0, &temp_cdata,0, 0);
	Particle::get_DeviceContext()->GSSetConstantBuffers(0,1,&c_cBufferptr);
	Particle::get_DeviceContext()->UpdateSubresource(dt_gtBufferptr, 0, 0, &temp_tBuffer,0, 0);
	Particle::get_DeviceContext()->GSSetConstantBuffers(1,1,&dt_gtBufferptr);
	Particle::get_DeviceContext()->GSSetShaderResources( 0, 1, &RandomTexptr );
	UINT offset[1] = { 0 };
	ID3D11Buffer* pBuffers[1];
	pBuffers[0] = Stream_target_bufferptr;

	Particle::get_DeviceContext()->SOSetTargets( 1, pBuffers, offset );
	
	if(!Particle::is_first_cycle())
	{
		Particle::get_DeviceContext()->DrawAuto();
	}
	else
	{
		Particle::get_DeviceContext()->Draw(1,0);
	}
	Particle::swap_buffers();

	Particle::set_first_cycle(false);
}

void Fog_Particles::Render(const XMMATRIX& viewMX,const XMMATRIX& projMX, shaders& _sh)
{
	p_pv_data.sea_level = sea_level;
	XMMATRIX WVP_MX;
	WVP_MX = viewMX*projMX;
	
	render_cBuffer temp_rc_data = Particle::get_r_data();
	ID3D11Buffer* render_buffer_ptr = Particle::return_render_buffer();
	ID3D11Buffer* r_cbuff_ptr = Particle::return_r_cBuffer();
	ID3D11ShaderResourceView* P_tex = Particle::return_particle_tex();



	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(viewMX),viewMX);
	XMStoreFloat4x4(&temp_rc_data.worldViewProj,XMMatrixTranspose(WVP_MX));
	XMStoreFloat4x4(&temp_rc_data.InvViewMx,XMMatrixTranspose(invView));
	_sh.set_active_rend_particles(render_buffer_ptr,0);
	Particle::get_DeviceContext()->UpdateSubresource(p_pvBuffer, 0, 0, &p_pv_data,0, 0);
	Particle::get_DeviceContext()->PSSetShaderResources( 1, 1, &P_tex );

	Particle::get_DeviceContext()->UpdateSubresource(r_cbuff_ptr, 0, 0, &temp_rc_data,0, 0);
	Particle::get_DeviceContext()->GSSetConstantBuffers(2,1,&r_cbuff_ptr);
	Particle::get_DeviceContext()->PSSetConstantBuffers(3,1,&p_pvBuffer);
	Particle::get_DeviceContext()->DrawAuto();
}

void Fog_Particles::start_Fog()
{
	Particle::set_dieready(false);
	Particle::set_mark_to_kill(false);

}

void Fog_Particles::stop_Fog()
{
	Particle::set_mark_to_kill(true);

}