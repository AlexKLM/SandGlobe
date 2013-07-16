#include "surface.h"
#include "Scene_manager.h"

surface::surface(void)
{
	Wavetime = 0;
	reflection_texture = NULL;
	reflect_SRview = NULL;
	ice_technique = NULL;
	sky_texture = NULL;
	surface_Buffer = NULL;
	ice_texture = NULL;
	seareflect_Sampler = NULL;
	time = 0;
	rtv_render_to_texture = NULL;
	srv_render_to_texture = NULL;
}


surface::~surface(void)
{
	try
	{
		cleanup();
	}
	catch(...)
	{

	}
}

void surface::cleanup()
{
	if(rtv_render_to_texture != 0)
	{
		rtv_render_to_texture->Release();
	}
	if(reflection_texture != 0)
	{
		reflection_texture->Release();
	}
	if(reflect_SRview != 0)
	{
		reflect_SRview->Release();
	}
	if(srv_render_to_texture != 0)
	{
		srv_render_to_texture->Release();
	}
	if(sky_texture != 0)
	{
		sky_texture->Release();
	}
	if(surface_Buffer != 0)
	{
		surface_Buffer->Release();
	}
	if(ice_texture != 0)
	{
		ice_texture->Release();
	}
	if(seareflect_Sampler != 0)
	{
		seareflect_Sampler->Release();
	}
}

Surface_buff surface_b;

void surface::create()
{
	HRESULT hr;
	XMFLOAT4 col = WHITE;
	Scene_object::loadmodel(L"media\\ocean_surface.obj",col,false);


	D3DX11CreateShaderResourceViewFromFile( get_DXDevice(), L"media\\sky.jpg",
		NULL, NULL, &sky_texture, NULL );

	D3DX11CreateShaderResourceViewFromFile( get_DXDevice(), L"media\\ice_tex.jpg",
		NULL, NULL, &ice_texture, NULL );

	D3D11_BUFFER_DESC constDesc2;
	ZeroMemory(&constDesc2, sizeof(constDesc2));
	constDesc2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constDesc2.ByteWidth = (sizeof(Surface_buff));
	constDesc2.Usage = D3D11_USAGE_DEFAULT;
	hr = get_DXDevice()->CreateBuffer(&constDesc2, 0, &surface_Buffer);

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory( &sampDesc, sizeof(sampDesc) );
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR       ;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR       ;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR       ;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//Create the Sample State
	hr = get_DXDevice()->CreateSamplerState( &sampDesc, &seareflect_Sampler );
	
}

void surface::initialise_shader(const char* filename)
{
	Scene_object::initialise_shader(filename);
	ice_technique = Scene_object::initialise_effect("RenderSceneIce");
}

void surface::update(float sealevel,double dt, bool frozen)
{
	XMMATRIX tempModelMX = XMMatrixIdentity();
	XMFLOAT3 pos = get_position();
	XMMATRIX translate = XMMatrixTranslation(pos.x,sealevel,pos.z);
	set_position(pos);
	time += dt;
	if(!frozen)
	{
		Wavetime += dt;
	}
	tempModelMX *= translate;
	
	XMFLOAT4X4 M_MX;
	XMStoreFloat4x4(&M_MX,tempModelMX);
	set_ModelMX(M_MX);
}

void surface::Render(const XMMATRIX& viewMX,const XMMATRIX& projMX,const XMMATRIX& reflectMX,bool frozen, bool multilight)
{
	XMMATRIX tempModelMX = XMLoadFloat4x4(&get_matrix());
	surface_b.projMx = XMMatrixTranspose(projMX);
	surface_b.reflecMx = XMMatrixTranspose(reflectMX);
	surface_b.worldViewProj = XMMatrixTranspose(tempModelMX*viewMX*projMX);
	surface_b.worldMx = XMMatrixTranspose(tempModelMX);
	surface_b.viewMx = XMMatrixTranspose(viewMX);
	surface_b.Time = time;
	surface_b.WaveTime = Wavetime;

	Scene_object::enable_depthStateLess();

	if(frozen)
	{
		Scene_object::disable_blend();
		Scene_object::set_active(ice_technique,0);

	}
	else
	{
			Scene_object::enable_blend();
		Scene_object::set_active(0);

	}

	get_DeviceContext()->UpdateSubresource(surface_Buffer, 0, 0, &surface_b,0, 0);
	get_DeviceContext()->VSSetConstantBuffers(0,1,&surface_Buffer);
	get_DeviceContext()->PSSetConstantBuffers(0,1,&surface_Buffer);
	get_DeviceContext()->PSSetSamplers( 0, 1, &seareflect_Sampler );
	get_DeviceContext()->PSSetShaderResources( 0, 1, &reflect_SRview );

	get_DeviceContext()->PSSetShaderResources( 1, 1, &sky_texture );
	get_DeviceContext()->PSSetShaderResources(2,1,&ice_texture);
	Scene_object::draw();


	Scene_object::enable_blend();

	Scene_object::enable_depthEqual();
	
	ID3D11Buffer* lightbufferptr = get_manager()->return_PL_Buffer();
	if(multilight)
	{
		std::vector<pointlight> lights;
		lights = get_manager()->get_light_list();
	
		for(int i = 0; i < lights.size(); i++)
		{
			Scene_object::set_active(1);
			
			get_DeviceContext()->UpdateSubresource(lightbufferptr, 0, 0, &lights[i],0, 0);
			get_DeviceContext()->PSSetConstantBuffers(1,1,&lightbufferptr);
			Scene_object::draw();
		}
		lights.clear();
	}
	else
	{
	//render directional

	Scene_object::set_active(2);
	get_DeviceContext()->UpdateSubresource(surface_Buffer, 0, 0, &surface_b,0, 0);
	get_DeviceContext()->VSSetConstantBuffers(0,1,&surface_Buffer);
	get_DeviceContext()->PSSetConstantBuffers(0,1,&surface_Buffer);
	get_DeviceContext()->UpdateSubresource(lightbufferptr, 0, 0, &get_manager()->get_p_light(),0, 0);
	get_DeviceContext()->PSSetConstantBuffers(1,1,&lightbufferptr);

	Scene_object::draw();
	}
	lightbufferptr->Release();
}

void surface::create_render_to_texture(int w, int h)
{
	HRESULT hr;
	//texture2d describtion
	DXGI_FORMAT format;
	D3D11_TEXTURE2D_DESC info ;
	ZeroMemory (& info , sizeof ( info ));
	info.Width = w;
	info.Height = h;
	info.MipLevels = 1;
	info.ArraySize = 1;
	info.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	info.SampleDesc . Count = 1;
	info.Usage = D3D11_USAGE_DEFAULT;
	info.BindFlags = D3D11_BIND_RENDER_TARGET|D3D11_BIND_SHADER_RESOURCE ;
	info.CPUAccessFlags = 0;
	info.MiscFlags = 0;
	hr = get_DXDevice()->CreateTexture2D(&info,nullptr,&reflection_texture);
	format = info.Format;

	// render target view
	D3D11_RENDER_TARGET_VIEW_DESC target_info;
	target_info.Format = format;
	target_info.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	target_info.Texture2D.MipSlice = 0;
	hr = get_DXDevice()->CreateRenderTargetView(reflection_texture, &target_info ,&rtv_render_to_texture);

	D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_info;
	shader_resource_info.Format = format;
	shader_resource_info.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shader_resource_info.Texture2D . MostDetailedMip = 0;
	shader_resource_info.Texture2D . MipLevels = 1;
	hr = get_DXDevice()->CreateShaderResourceView(reflection_texture, &shader_resource_info ,&srv_render_to_texture);

	D3D11_SHADER_RESOURCE_VIEW_DESC reflect_SRVDesc;
	memset( &reflect_SRVDesc, 0, sizeof( reflect_SRVDesc ) );
	reflect_SRVDesc.Format = format;
	reflect_SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	reflect_SRVDesc.Texture2D.MipLevels = 1;

	hr = get_DXDevice()->CreateShaderResourceView(reflection_texture,&reflect_SRVDesc,&reflect_SRview);
}

ID3D11Texture2D* surface::return_ref_texture() const
{
	return reflection_texture;
}

ID3D11ShaderResourceView* surface::return_ref_SRview() const
{
	return reflect_SRview;
}

ID3D11ShaderResourceView* surface::return_rtv_SRview() const
{
	return srv_render_to_texture;
}

ID3D11RenderTargetView* surface::return_rtv() const
{
	return rtv_render_to_texture;
}