#include "Globe_Base.h"
#include "Scene_manager.h"

Globe_Base::Globe_Base(void)
{
	Base_tex = NULL;
	Base_Sampler = NULL;
}

Globe_Base::~Globe_Base(void)
{
}

void Globe_Base::create_Base()
{

	XMFLOAT4 col = WHITE;
	Scene_object::loadmodel(L"media\\base2.obj",col,false);
	//load texture
	D3DX11CreateShaderResourceViewFromFile( get_DXDevice(), L"media\\wood.png",
		NULL, NULL, &Base_tex, NULL );

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
	hr = get_DXDevice()->CreateSamplerState( &sampDesc, &Base_Sampler );

}

void Globe_Base::Render(const XMMATRIX& viewMX,const XMMATRIX& projMX)
{
	Scene_object::disable_blend();

	Scene_object::enable_depthStateLess();

	ID3D11Buffer* bufferptr = get_buffer();

	XMMATRIX tempModelMX = XMLoadFloat4x4(&get_matrix());
	XMMATRIX mx = tempModelMX*viewMX*projMX;
	XMMATRIX wvpMX = XMMatrixTranspose(mx);
	set_wvp(wvpMX);
	
	Scene_object::set_active(0);
	get_DeviceContext()->UpdateSubresource(bufferptr, 0, 0, &get_objBuff(),0, 0);
	get_DeviceContext()->VSSetConstantBuffers(0,1,&bufferptr);
	
	

	get_DeviceContext()->PSSetSamplers( 0, 1, &Base_Sampler );
	get_DeviceContext()->PSSetShaderResources( 0, 1, &Base_tex );

	Scene_object::draw();
}