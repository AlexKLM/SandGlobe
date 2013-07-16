#include "Bottle.h"
#include "Scene_manager.h"

Bottle::Bottle(void)
{
	cbBuffer = NULL;
}

Bottle::~Bottle(void)
{
	try
	{
		cleanup();
	}
	catch(...)
	{

	}
}

void Bottle::cleanup()
{
	if(cbBuffer != NULL)
	{
		cbBuffer->Release();
	}
}

void Bottle::create()
{
	Shape_Maker sm;
	//set_buffer(sm.loadobj(L"media\\bottle.obj", get_DXDevice(), XMFLOAT4(0,0.7,0,1),false));
	Scene_object::loadmodel(L"media\\bottle.obj",XMFLOAT4(0,0.7,0,1),false);
	D3D11_BUFFER_DESC constDesc;
	ZeroMemory(&constDesc, sizeof(constDesc));
	constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constDesc.ByteWidth = (sizeof(bottle_buff));
	constDesc.Usage = D3D11_USAGE_DEFAULT;
	get_DXDevice()->CreateBuffer(&constDesc, 0, &cbBuffer);
}

void Bottle::Render(const XMMATRIX& viewMX,const XMMATRIX& projMX, const XMFLOAT4& cliplevel,bool multi_light)
{
	//Scene_manager* managerptr = Sceget_manager();
	//pass 1
	XMMATRIX tempModelMX = XMLoadFloat4x4(&get_matrix());
	XMMATRIX mx = tempModelMX*viewMX*projMX;
	buff.worldViewProj = XMMatrixTranspose(mx);
	buff.worldMx = XMMatrixTranspose(tempModelMX);
	buff.clip_level = cliplevel;
	buff.worldInverseTranspose = (XMMatrixInverse(&XMMatrixDeterminant(tempModelMX), tempModelMX));
	Scene_object::set_active(0);

	Scene_object::disable_blend();
	Scene_object::enable_depthStateLess();

	get_DeviceContext()->UpdateSubresource(cbBuffer, 0, 0, &buff,0, 0);
	get_DeviceContext()->VSSetConstantBuffers(0,1,&cbBuffer);

	Scene_object::draw();

	ID3D11Buffer* lightbufferptr = get_manager()->return_PL_Buffer();
	if(multi_light)
	{
		std::vector<pointlight> lights = get_manager()->get_light_list();
		for(int i = 0; i < lights.size(); i++)
		{
			//pass 2

			get_manager()->enable_blend();
			get_manager()->enable_depthEqual();

			Scene_object::set_active(1);
			get_DeviceContext()->UpdateSubresource(cbBuffer, 0, 0, &buff,0, 0);
			get_DeviceContext()->VSSetConstantBuffers(0,1,&cbBuffer);
			get_DeviceContext()->UpdateSubresource(lightbufferptr, 0, 0, &lights[i],0, 0);
			get_DeviceContext()->PSSetConstantBuffers(1,1,&lightbufferptr);

			Scene_object::draw();
		}
	}
	else
	{
	//pass 2

	Scene_object::enable_blend();
	Scene_object::enable_depthEqual();

	Scene_object::set_active(2);
	get_DeviceContext()->UpdateSubresource(cbBuffer, 0, 0, &buff,0, 0);
	get_DeviceContext()->VSSetConstantBuffers(0,1,&cbBuffer);
	get_DeviceContext()->UpdateSubresource(lightbufferptr, 0, 0, &get_manager()->get_p_light(),0, 0);
	get_DeviceContext()->PSSetConstantBuffers(1,1,&lightbufferptr);
	get_DeviceContext()->DrawIndexed(get_VI_Buffer().index_amount, 0, 0);
	Scene_object::draw();
	}
}
