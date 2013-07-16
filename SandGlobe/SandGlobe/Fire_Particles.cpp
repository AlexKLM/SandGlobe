#include "Fire_Particles.h"


Fire_Particles::Fire_Particles(void)
{

}


Fire_Particles::~Fire_Particles(void)
{
}


//void Fire_Particles::initialize(Scene_manager* _manager,ID3D11DeviceContext *_dxDC, ID3D11Device *_dxD, const XMFLOAT3& _position,float final_flame_life)
//{
//	life_timer = final_flame_life;
//	position = _position;
//	manager = _manager;
//	first_cycle = true;
//	_dxDev = _dxD;
//	_dxImmedDC = _dxDC;
//	create_buffers();
//	generate_randoms();
//	D3DX11CreateShaderResourceViewFromFile( _dxDev, L"media\\particle_tex.png",
//		NULL, NULL, &Particle_tex, NULL );
//}
//
//bool Fire_Particles::is_die_ready() const
//{
//	return die_ready;
//}
//
//void Fire_Particles::set_delete()
//{
//	mark_to_kill = true;
//}
//
//void Fire_Particles::setgravity(const XMFLOAT4& grav)
//{
//	c_data.gravity = grav;
//}
//
//void Fire_Particles::Render(const XMMATRIX& viewMX,const XMMATRIX& projMX, shaders& _sh)
//{
//	XMMATRIX WVP_MX;
//	WVP_MX = viewMX*projMX;
//	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(viewMX),viewMX);
//	XMStoreFloat4x4(&r_data.worldViewProj,XMMatrixTranspose(WVP_MX));
//	XMStoreFloat4x4(&r_data.InvViewMx,XMMatrixTranspose(invView));
//	_sh.set_active_rend_particles(render_buffer,0);
//	_dxImmedDC->PSSetShaderResources( 1, 1, &Particle_tex );
//	_dxImmedDC->UpdateSubresource(r_cBuffer, 0, 0, &r_data,0, 0);
//	_dxImmedDC->GSSetConstantBuffers(2,1,&r_cBuffer);
//	_dxImmedDC->DrawAuto();
//}
//
//void Fire_Particles::Calculate_Particles(double dt, shaders& _sh,bool _shouldemit)
//{
//	if(mark_to_kill)
//	{
//		if(life_timer > 0)
//		{
//			life_timer -=dt;
//		}
//		else
//		{
//			die_ready = true;
//			return;
//		}
//	}
//	if(_shouldemit)
//	{
//		c_data.shouldemit = 1;
//	}
//	else
//	{
//		c_data.shouldemit = -1;
//	}
//	c_data.gravity = XMFLOAT4(0,-1,0,0);
//	tBuffer.padding2 = XMFLOAT2(0,0);
//	tBuffer.GlobalTime =  float( ( rand() % 10000 ) - 5000 );
//	tBuffer.DeltaTime = dt;
//
//	if(!first_cycle)
//	{
//		_sh.set_active_calc_particles(render_buffer,0);
//	}
//	else
//	{
//		_sh.set_active_calc_particles(start_buffer,0);
//	}
//	_dxImmedDC->UpdateSubresource(c_cBuffer, 0, 0, &c_data,0, 0);
//	_dxImmedDC->GSSetConstantBuffers(0,1,&c_cBuffer);
//	_dxImmedDC->UpdateSubresource(dt_gtBuffer, 0, 0, &tBuffer,0, 0);
//	_dxImmedDC->GSSetConstantBuffers(1,1,&dt_gtBuffer);
//	_dxImmedDC->GSSetShaderResources( 0, 1, &RandomTexRV );
//	UINT offset[1] = { 0 };
//	ID3D11Buffer* pBuffers[1];
//	pBuffers[0] = Stream_target_buffer;
//
//	_dxImmedDC->SOSetTargets( 1, pBuffers, offset );
//	
//	if(!first_cycle)
//	{
//		_dxImmedDC->DrawAuto();
//	}
//	else
//	{
//		_dxImmedDC->Draw(1,0);
//	}
//	pBuffers[0] = NULL;
//    _dxImmedDC->SOSetTargets( 1, pBuffers, offset );
//	ID3D11Buffer* tempB = render_buffer;
//	render_buffer = Stream_target_buffer;
//	Stream_target_buffer = tempB;
//	first_cycle = false;
//}
//
//void Fire_Particles::create_buffers()
//{
//	particle_Vertex seed =
//		{
//			position,
//			XMFLOAT3(0,0.5f,0),
//			//float(i),
//			float(0),
//			UINT(0),//is Emitter type
//		};
//	D3D11_BUFFER_DESC buffDesc;
//	buffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//	buffDesc.ByteWidth = sizeof(particle_Vertex);
//	buffDesc.Usage = D3D11_USAGE_DEFAULT;
//	buffDesc.CPUAccessFlags = 0;
//	buffDesc.MiscFlags = 0;
//
//	D3D11_SUBRESOURCE_DATA vbInitData;
//	ZeroMemory(&vbInitData,sizeof(D3D11_SUBRESOURCE_DATA));
//	vbInitData.pSysMem = &seed;
//	vbInitData.SysMemPitch = 0;
//	vbInitData.SysMemSlicePitch = 0;
//	//vbInitData.SysMemPitch = sizeof(particle_Vertex);
//	_dxDev->CreateBuffer(&buffDesc,&vbInitData,&start_buffer);//seed buffer for first cycle
//
//	//create the other 2 buffers
//	buffDesc.ByteWidth = MAX_PARTICLES * sizeof(particle_Vertex);
//	buffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;
//	_dxDev->CreateBuffer(&buffDesc,NULL,&render_buffer);
//	_dxDev->CreateBuffer(&buffDesc,NULL,&Stream_target_buffer);
//	HRESULT hr;
//
//	ZeroMemory(&buffDesc, sizeof(buffDesc));
//	buffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//	buffDesc.ByteWidth = (sizeof(cal_cBuffer));
//	//buffDesc.ByteWidth = 64;
//	buffDesc.Usage = D3D11_USAGE_DEFAULT;
//	hr = _dxDev->CreateBuffer(&buffDesc, 0, &c_cBuffer);
//
//	buffDesc.ByteWidth = (sizeof(timeBuffers));
//	hr = _dxDev->CreateBuffer(&buffDesc, 0, &dt_gtBuffer);
//
//	buffDesc.ByteWidth = (sizeof(render_cBuffer));
//	hr = _dxDev->CreateBuffer(&buffDesc, 0, &r_cBuffer);
//}
//
//void Fire_Particles::generate_randoms()
//{
//	int iNumRandValues = 1024;
//    srand( (unsigned)time(NULL));
//
//    D3D11_SUBRESOURCE_DATA InitData;
//    InitData.pSysMem = new float[iNumRandValues * 4];
//    InitData.SysMemPitch = iNumRandValues * 4 * sizeof( float );
//    InitData.SysMemSlicePitch = iNumRandValues * 4 * sizeof( float );
//    for( int i = 0; i < iNumRandValues * 4; i++ )
//    {
//		float f = float( ( rand() % 10000 ) - 5000 );
//        ( ( float* )InitData.pSysMem )[i] = f;
//    }
//
//    D3D11_TEXTURE1D_DESC dstex;
//    dstex.Width = iNumRandValues;
//    dstex.MipLevels = 1;
//    dstex.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
//    dstex.Usage = D3D11_USAGE_DEFAULT;
//    dstex.BindFlags = D3D11_BIND_SHADER_RESOURCE;
//    dstex.CPUAccessFlags = 0;
//    dstex.MiscFlags = 0;
//    dstex.ArraySize = 1;
//    _dxDev->CreateTexture1D( &dstex, &InitData, &RandomTexture ) ;
//
//    D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
//    ZeroMemory( &SRVDesc, sizeof( SRVDesc ) );
//    SRVDesc.Format = dstex.Format;
//    SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
//    SRVDesc.Texture2D.MipLevels = dstex.MipLevels;
//    _dxDev->CreateShaderResourceView( RandomTexture, &SRVDesc, &RandomTexRV ) ;
//}
