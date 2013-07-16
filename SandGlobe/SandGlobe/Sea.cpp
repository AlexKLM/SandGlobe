#include "Sea.h"
#include "Scene_manager.h"



Sea::Sea(void)
{
	waveTime = 0;
	sea_level = NULL;
	sea_Buffer = NULL;
}

Sea::~Sea(void)
{
}

void Sea::cleanup()
{
	if(sea_Buffer != 0)
	{
		sea_Buffer->Release();
	}
}

Sea_buff sea_b;
void Sea::create_sea()
{
	XMFLOAT4 col = BLUE;
	Scene_object::loadSphere(col,4.95f);
	HRESULT hr;
	
	sea_level = 5;
	D3D11_BUFFER_DESC constDesc;
	ZeroMemory(&constDesc, sizeof(constDesc));
	constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constDesc.ByteWidth = (sizeof(Sea_buff));
	constDesc.Usage = D3D11_USAGE_DEFAULT;
	hr = get_DXDevice()->CreateBuffer(&constDesc, 0, &sea_Buffer);
}

float Sea::return_sealevel() const
{
	return sea_level;
}


void Sea::update(float sealevel, double dt, bool frozen)
{
	sea_level = sealevel;
	if(!frozen)
	{
		waveTime += dt;
	}
}

void Sea::Render(const XMMATRIX& viewMX,const XMMATRIX& projMX)
{	
	XMMATRIX tempModelMX = XMLoadFloat4x4(&get_matrix());
	XMMATRIX mx = tempModelMX*viewMX*projMX;
	sea_b.worldViewProj = XMMatrixTranspose(mx);
	sea_b.worldMx = XMMatrixTranspose(tempModelMX);
	sea_b.sea_level = sea_level;
	sea_b.WaveTime = waveTime;
	//pass 0
	Scene_object::enable_blend();
	Scene_object::enable_depthStateLess();

	Scene_object::set_active(0);

	Scene_object::enable_CCW();

	get_DeviceContext()->UpdateSubresource(sea_Buffer, 0, 0, &sea_b,0, 0);
	get_DeviceContext()->VSSetConstantBuffers(0,1,&sea_Buffer);
	get_DeviceContext()->PSSetConstantBuffers(0,1,&sea_Buffer);
	
	Scene_object::draw();

	//pas1
	Scene_object::set_active(1);
	Scene_object::enable_CW();

	get_DeviceContext()->UpdateSubresource(sea_Buffer, 0, 0, &sea_b,0, 0);
	get_DeviceContext()->VSSetConstantBuffers(0,1,&sea_Buffer);
	get_DeviceContext()->PSSetConstantBuffers(0,1,&sea_Buffer);

	Scene_object::draw();

}
