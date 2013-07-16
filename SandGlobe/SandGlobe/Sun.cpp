#include "Sun.h"
#include "Scene_manager.h"

Sun::Sun(void)
{
}


Sun::~Sun(void)
{
}

void Sun::initialise(Scene_manager* _manager,XMFLOAT3& _position,ID3D11DeviceContext *_dxDC, ID3D11Device *_dxD)
{
	Scene_object::initialise(_manager,_position,_dxDC,_dxD);
	ID3D11Buffer* bufferptr = get_buffer();
	D3D11_BUFFER_DESC constDesc;
	ZeroMemory(&constDesc, sizeof(constDesc));
	constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constDesc.ByteWidth = (sizeof(sun_buff));
	constDesc.Usage = D3D11_USAGE_DEFAULT;
	get_DXDevice()->CreateBuffer(&constDesc, 0, &bufferptr);
}


void Sun::create()
{
	XMFLOAT4 col = YELLOW;
	Scene_object::loadSphere(col,0.5f,XMFLOAT3(0,0,0));

}

void Sun::update(XMFLOAT2 &sun_pos)
{
	XMMATRIX tempModelMX = XMMatrixIdentity();
	XMMATRIX transMx = XMMatrixTranslation(sun_pos.x,sun_pos.y,0);
	tempModelMX *= transMx;
	XMFLOAT4X4 M_MX;
	XMStoreFloat4x4(&M_MX,tempModelMX);
	set_ModelMX(M_MX);
}

void Sun::Render(const XMMATRIX& viewMX,const XMMATRIX& projMX,const XMFLOAT4& cliplevel)
{
	XMMATRIX tempModelMX = XMLoadFloat4x4(&get_matrix());
	XMMATRIX mx = tempModelMX*viewMX*projMX;
	buff.worldViewProj = XMMatrixTranspose(mx);
	buff.worldMx = XMMatrixTranspose(mx);
	buff.clip_level = cliplevel;
	Scene_object::set_active(0);
	ID3D11Buffer* bufferptr = get_buffer();

	Scene_object::disable_blend();
	Scene_object::enable_depthStateLess();

	get_DeviceContext()->UpdateSubresource(bufferptr, 0, 0, &buff,0, 0);
	get_DeviceContext()->VSSetConstantBuffers(0,1,&bufferptr);

	Scene_object::draw();
	bufferptr->Release();
}
