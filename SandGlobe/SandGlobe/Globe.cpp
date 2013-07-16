#include "Globe.h"
#include "Scene_manager.h"

Globe::Globe(void)
{
}

Globe::~Globe(void)
{
}



void Globe::create_globe()
{
	XMFLOAT4 col = WHITE;
	Scene_object::loadSphere(col,5);
}

void Globe::Render(const XMMATRIX& viewMX,const XMMATRIX& projMX, bool multilights)
{
	XMMATRIX tempModelMX = XMLoadFloat4x4(&get_matrix());
	XMMATRIX mx = tempModelMX*viewMX*projMX;
	XMMATRIX wvpMX = XMMatrixTranspose(mx);

	set_wvp(wvpMX);
	ID3D11Buffer* bufferptr = get_buffer();

	Scene_object::set_active(0);
	get_DeviceContext()->UpdateSubresource(bufferptr, 0, 0, &get_objBuff(),0, 0);
	get_DeviceContext()->VSSetConstantBuffers(0,1,&bufferptr);
	ID3D11Buffer* lightbufferptr = get_manager()->return_PL_Buffer();

	Scene_object::enable_blend();

	Scene_object::enable_depthStateLess();

	Scene_object::enable_CCW();

	Scene_object::draw();

	get_manager()->enable_depthEqual();
	if(multilights)
	{
		
		std::vector<pointlight> lights;
		lights = get_manager()->get_light_list();

		for(int i = 0; i < lights.size(); i++)
		{
			//_sh.set_active(get_VI_Buffer(),2);
			Scene_object::set_active(2);
			
			get_DeviceContext()->VSSetConstantBuffers(0,1,&bufferptr);
			get_DeviceContext()->UpdateSubresource(lightbufferptr, 0, 0, &lights[i],0, 0);
			get_DeviceContext()->PSSetConstantBuffers(1,1,&lightbufferptr);
			Scene_object::draw();
		}
	}
	else
	{
		Scene_object::set_active(3);
	
		get_DeviceContext()->VSSetConstantBuffers(0,1,&bufferptr);
		get_DeviceContext()->UpdateSubresource(lightbufferptr, 0, 0, &get_manager()->get_p_light(),0, 0);
		get_DeviceContext()->PSSetConstantBuffers(1,1,&lightbufferptr);
		Scene_object::draw();
	}


	Scene_object::set_active(1);
	get_DeviceContext()->UpdateSubresource(bufferptr, 0, 0, &get_objBuff(),0, 0);
	get_DeviceContext()->VSSetConstantBuffers(0,1,&bufferptr);
	Scene_object::enable_depthStateLess();
	
	Scene_object::enable_CW();

	Scene_object::draw();

}

