#include "tree.h"
#include "Scene_manager.h"

tree::tree(void)
{
	Norm_map = NULL;
	current = NULL;
	tree_tex = NULL;
	tree_Sampler = NULL;
	m_cBuffer = NULL;
	tree_spot_sh = NULL;
	cbBuffer = NULL;
}


tree::~tree(void)
{
}

void tree::create()
{
	Shape_Maker sm;
	XMFLOAT4 col = GREEN;

	Scene_object::loadmodel(L"media\\tree.obj",col,true);

	D3DX11CreateShaderResourceViewFromFile( get_DXDevice(), L"media\\wood.png",
		NULL, NULL, &tree_tex, NULL );
	D3DX11CreateShaderResourceViewFromFile( get_DXDevice(), L"media\\plam_normal.png",
		NULL, NULL, &Norm_map, NULL );

	//Setup material
	tree_m.M_ambient = 0.2f;
	tree_m.M_diffuse = 0.5f;
	tree_m.M_spec = 0.7;


	//setup sampler state
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory( &sampDesc, sizeof(sampDesc) );
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    HRESULT hr;
	//Create the Sample State
	hr = get_DXDevice()->CreateSamplerState( &sampDesc, &tree_Sampler );

	
	D3D11_BUFFER_DESC constDesc;
	ZeroMemory(&constDesc, sizeof(constDesc));
	constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constDesc.ByteWidth = (sizeof(tree_buff));
	constDesc.Usage = D3D11_USAGE_DEFAULT;
	hr = get_DXDevice()->CreateBuffer(&constDesc, 0, &cbBuffer);

	constDesc.ByteWidth = (sizeof(Material));
	hr = get_DXDevice()->CreateBuffer(&constDesc,0,&m_cBuffer);
}

tree_buff buff;

void tree::Render(const XMMATRIX& viewMX,const XMMATRIX& projMX,const XMFLOAT4& cliplevel,const XMFLOAT3& _cam_pos, bool multilight)
{
	XMMATRIX tempModelMX = XMLoadFloat4x4(&get_matrix());
	XMMATRIX mx = tempModelMX*viewMX*projMX;
	buff.worldViewProj = XMMatrixTranspose(mx);
	buff.cam_pos = _cam_pos;
	buff.worldMx = XMMatrixTranspose(tempModelMX);
	buff.clip_level = cliplevel;
	buff.worldInverseTranspose = (XMMatrixInverse(&XMMatrixDeterminant(tempModelMX), tempModelMX));
	tree_m.show_tex = current->show_tex;
	//pass0
	Scene_object::set_active(current->singlelight,0);

	Scene_object::disable_blend();


	Scene_object::enable_depthStateLess();
	
	get_DeviceContext()->UpdateSubresource(cbBuffer, 0, 0, &buff,0, 0);
	get_DeviceContext()->VSSetConstantBuffers(0,1,&cbBuffer);
	get_DeviceContext()->UpdateSubresource(m_cBuffer, 0, 0, &tree_m,0, 0);
	get_DeviceContext()->PSSetConstantBuffers(3,1,&m_cBuffer);


	get_DeviceContext()->PSSetSamplers( 0, 1, &tree_Sampler );
	get_DeviceContext()->PSSetShaderResources( 0, 1, &tree_tex );
	get_DeviceContext()->PSSetShaderResources( 1, 1, &Norm_map );

	Scene_object::draw();

	Scene_object::enable_blend();
	Scene_object::enable_depthEqual();

	ID3D11Buffer* lightbufferptr = get_manager()->return_PL_Buffer();
	if(multilight)
	{
		tree_m.M_spec = 0.2f;
		std::vector<pointlight> lights;
		lights = get_manager()->get_light_list();
		for(int i = 0; i < lights.size(); i++)
		{
			Scene_object::set_active(current->multilight,1);

			get_DeviceContext()->UpdateSubresource(cbBuffer, 0, 0, &buff,0, 0);
			get_DeviceContext()->VSSetConstantBuffers(0,1,&cbBuffer);

			get_DeviceContext()->UpdateSubresource(m_cBuffer, 0, 0, &tree_m,0, 0);
			get_DeviceContext()->PSSetSamplers( 0, 1, &tree_Sampler );
			get_DeviceContext()->PSSetShaderResources( 0, 1, &tree_tex );
			get_DeviceContext()->PSSetShaderResources( 1, 1, &Norm_map );
			get_DeviceContext()->PSSetConstantBuffers(3,1,&m_cBuffer);
			get_DeviceContext()->UpdateSubresource(lightbufferptr, 0, 0, &lights[i],0, 0);
			get_DeviceContext()->PSSetConstantBuffers(1,1,&lightbufferptr);
	
			Scene_object::draw();
		}
		lights.clear();
	}
	else
	{
	//pass1

	Scene_object::set_active(current->singlelight,1);
	get_DeviceContext()->UpdateSubresource(cbBuffer, 0, 0, &buff,0, 0);
	get_DeviceContext()->VSSetConstantBuffers(0,1,&cbBuffer);

	get_DeviceContext()->PSSetSamplers( 0, 1, &tree_Sampler );
	get_DeviceContext()->PSSetShaderResources( 0, 1, &tree_tex );
	get_DeviceContext()->PSSetShaderResources( 1, 1, &Norm_map );
	get_DeviceContext()->PSSetConstantBuffers(3,1,&m_cBuffer);
	get_DeviceContext()->UpdateSubresource(lightbufferptr, 0, 0, &get_manager()->get_p_light(),0, 0);
	get_DeviceContext()->PSSetConstantBuffers(1,1,&lightbufferptr);

	Scene_object::draw();
	}

	lightbufferptr->Release();

}

void tree::generate_shadow(const XMMATRIX& lightVPMx,const XMFLOAT3& lightpos, shaders& shadow_shader , ID3D11Buffer* shadowBuff)
{
	XMMATRIX tempModelMX = XMLoadFloat4x4(&get_matrix());
	XMStoreFloat4x4(&shadow_data.worldMx,XMMatrixTranspose(tempModelMX));
	shadow_data.lPos = lightpos;
	XMStoreFloat4x4(&shadow_data.lightViewProj,lightVPMx);
	shadow_shader.set_active(get_VI_Buffer(),0);
	get_DeviceContext()->UpdateSubresource(shadowBuff, 0, 0, &shadow_data,0, 0);
	get_DeviceContext()->VSSetConstantBuffers(0,1,&shadowBuff);
	get_DeviceContext()->DrawIndexed(get_VI_Buffer().index_amount, 0, 0);
}

void tree::initialise_shader(const char* filename)
{

	Scene_object::initialise_shader(filename);

	create_tree_states();
}

void tree::create_tree_states()
{
	wireframe.singlelight = Scene_object::initialise_effect("RenderSceneWireFrame");
	wireframe.multilight = Scene_object::initialise_effect("RenderSceneMultiWireFrame");
	wireframe.show_tex = false;

	Flat.singlelight = Scene_object::initialise_effect("RenderSceneFLAT");
	Flat.multilight  = Scene_object::initialise_effect("RenderSceneMultiFLAT");
	Flat.show_tex = false;

	Smooth.singlelight =  Scene_object::initialise_effect("RenderScene");
	Smooth.multilight =  Scene_object::initialise_effect("RenderSceneMulti");
	Smooth.show_tex = false;

	Smooth_tex.singlelight = Scene_object::initialise_effect("RenderScene");
	Smooth_tex.multilight =  Scene_object::initialise_effect("RenderSceneMulti");
	Smooth_tex.show_tex = true;

	Bump.singlelight = Scene_object::initialise_effect("RenderSceneBump");
	Bump.multilight = Scene_object::initialise_effect("RenderSceneMultiBump");
	Bump.show_tex  = true;

	wireframe.next = &Flat;
	Flat.next = &Smooth;
	Smooth.next = &Smooth_tex;
	Smooth_tex.next = &Bump;
	Bump.next = &wireframe;

	current = &Smooth_tex;
}

void tree::change_state()
{
	current = current->next;
}