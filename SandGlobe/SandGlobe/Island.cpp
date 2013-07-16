#include "Island.h"
#include "Scene_manager.h"

Island::Island(void)
{
	cbBuffer = NULL;
}


Island::~Island(void)
{
	try
	{
		cleanup();
	}
	catch(...)
	{
	};
}

void Island::cleanup()
{
	if(cbBuffer !=0)
	{
		cbBuffer->Release();
	}
}

void Island::make_globe()
{
	XMFLOAT3 pos = get_position();
	pos.y -= 2;
	XMFLOAT4 col = YELLOW;
	Scene_object::set_position(pos);
	Scene_object::loadSphere(col,3);
	D3D11_BUFFER_DESC constDesc;
	ZeroMemory(&constDesc, sizeof(constDesc));
	constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constDesc.ByteWidth = (sizeof(island_buff));
	constDesc.Usage = D3D11_USAGE_DEFAULT;
	get_DXDevice()->CreateBuffer(&constDesc, 0, &cbBuffer);

}
island_buff buff;

void Island::Render(const XMMATRIX& viewMX,const XMMATRIX& projMX,const XMMATRIX& lvpMx, const XMFLOAT4& cliplevel,bool multilight)
{
	//pass 0
	XMMATRIX tempModelMX = XMLoadFloat4x4(&get_matrix());
	XMMATRIX mx = viewMX*projMX;
	buff.worldViewProj = XMMatrixTranspose(mx);
	buff.worldMx = XMMatrixTranspose(tempModelMX);
	buff.lightViewProj = lvpMx;
	buff.ViewMx = XMMatrixTranspose(viewMX);

	buff.worldInverseTranspose = (XMMatrixInverse(&XMMatrixDeterminant(tempModelMX), tempModelMX));
	buff.clip_level = cliplevel;
	ID3D11Buffer* lightbufferptr = get_manager()->return_PL_Buffer();
	ID3D11ShaderResourceView* Shadow_RV = get_manager()->return_shadow_RV();


	Scene_object::disable_blend();
	Scene_object::enable_depthStateLess();
	
	Scene_object::set_active(0);
	get_DeviceContext()->UpdateSubresource(cbBuffer, 0, 0, &buff,0, 0);
	get_DeviceContext()->VSSetConstantBuffers(0,1,&cbBuffer);
	get_DeviceContext()->PSSetShaderResources( 0, 1, &Shadow_RV );
	Scene_object::draw();

	enable_blend();
	enable_depthEqual();

	if(multilight)
	{
		std::vector<pointlight> lights = get_manager()->get_light_list();


		Scene_object::set_active(1);
		get_DeviceContext()->UpdateSubresource(cbBuffer, 0, 0, &buff,0, 0);
		get_DeviceContext()->VSSetConstantBuffers(0,1,&cbBuffer);
		get_DeviceContext()->PSSetShaderResources( 0, 1, &Shadow_RV );
		get_DeviceContext()->UpdateSubresource(lightbufferptr, 0, 0, &lights[0],0, 0);
		get_DeviceContext()->PSSetConstantBuffers(1,1,&lightbufferptr);
		Scene_object::draw();


		//multi lights
		for(int i = 1; i <lights.size();i++)
		{
			Scene_object::set_active(2);
			get_DeviceContext()->UpdateSubresource(cbBuffer, 0, 0, &buff,0, 0);
			get_DeviceContext()->VSSetConstantBuffers(0,1,&cbBuffer);
			get_DeviceContext()->PSSetShaderResources( 0, 1, &Shadow_RV );
			get_DeviceContext()->UpdateSubresource(lightbufferptr, 0, 0, &lights[i],0, 0);
			get_DeviceContext()->PSSetConstantBuffers(1,1,&lightbufferptr);
			Scene_object::draw();
		}
	}
	else
	{
	Scene_object::set_active(0);
	get_DeviceContext()->UpdateSubresource(cbBuffer, 0, 0, &buff,0, 0);
	get_DeviceContext()->VSSetConstantBuffers(0,1,&cbBuffer);

	Scene_object::draw();

	//pass1
	Scene_object::set_active(3);
	get_DeviceContext()->UpdateSubresource(cbBuffer, 0, 0, &buff,0, 0);
	get_DeviceContext()->VSSetConstantBuffers(0,1,&cbBuffer);
	get_DeviceContext()->PSSetShaderResources( 0, 1, &Shadow_RV );
	get_DeviceContext()->UpdateSubresource(lightbufferptr, 0, 0, &get_manager()->get_p_light(),0, 0);
	get_DeviceContext()->PSSetConstantBuffers(1,1,&lightbufferptr);
	Scene_object::draw();
	}
}

void Island::generate_shadow(const XMMATRIX& lightVPMx,const XMFLOAT3& lightpos, shaders& shadow_shader, ID3D11Buffer* shadowBuff )
{
	XMMATRIX tempModelMX = XMLoadFloat4x4(&get_matrix());
	XMStoreFloat4x4(&shadow_data.worldMx,XMMatrixTranspose(tempModelMX));
	shadow_data.lPos = lightpos;
	XMStoreFloat4x4(&shadow_data.lightViewProj,lightVPMx);
	shadow_shader.set_active(get_VI_Buffer(),0);

	get_DeviceContext()->UpdateSubresource(shadowBuff, 0, 0, &shadow_data,0, 0);
	get_DeviceContext()->VSSetConstantBuffers(0,1,&shadowBuff);

	Scene_object::draw();
}