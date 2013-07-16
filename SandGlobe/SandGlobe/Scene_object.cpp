#include "Scene_object.h"
#include "Scene_manager.h"

Scene_object::Scene_object(void)
{
	Buffer = NULL;
	manager = NULL;
	//blendfactor[4] = NULL;
	//_technique = NULL;
	_dxDev = NULL;
	_dxImmedDC = NULL;
}

Scene_object::~Scene_object(void)
{
	try
	{
		destroy();
	}
	catch(...)
	{

	}

}

void Scene_object::destroy()
{
	if(Buffer != NULL)
	{
		Buffer->Release();
	}
}

//vertex_index_buffer* Scene_object::return_vibuffer()
//{
//	return buffer;
//}

XMFLOAT3 Scene_object::get_position() const
{
	return position;
}


void Scene_object::Render(XMMATRIX& viewMX, XMMATRIX& projMX, shaders& _sh)
{
	XMMATRIX tempModelMX = XMLoadFloat4x4(&modelMX);
	XMMATRIX mx = tempModelMX*viewMX*projMX;
	XMMATRIX wvpMX = XMMatrixTranspose(mx);
	XMStoreFloat4x4(&objbuff.worldViewProj,wvpMX);
	_sh.set_active(VIbuffer,0);
	get_manager()->disable_blend();
	get_manager()->enable_depthStateLess();
	//_dxImmedDC->OMSetBlendState(0, 0, 0xffffffff);
	//_dxImmedDC->OMSetDepthStencilState(manager->m_depthStencilStateLess, 1);
	_dxImmedDC->UpdateSubresource(Buffer, 0, 0, &objbuff,0, 0);
	_dxImmedDC->VSSetConstantBuffers(0,1,&Buffer);
	_dxImmedDC->DrawIndexed(VIbuffer.index_amount, 0, 0);
}

void Scene_object::rotate(const XMMATRIX& rotationMX)
{
	XMMATRIX tempModelMX = XMMatrixIdentity();
	tempModelMX *= rotationMX;
	XMStoreFloat4x4(&modelMX,tempModelMX);
}

void Scene_object::translate(const XMMATRIX& translateMX)
{
	XMMATRIX tempModelMX = XMMatrixIdentity();
	tempModelMX *= translateMX;
	XMStoreFloat4x4(&modelMX,tempModelMX);
}

void Scene_object::initialise(Scene_manager* _manager,XMFLOAT3& _position, ID3D11DeviceContext *_dxDC, ID3D11Device *_dxD)
{
	manager = _manager;

	_dxDev = _dxD;
	position = _position;
	_dxImmedDC = _dxDC;
	XMMATRIX tempModelMX = XMMatrixIdentity();
	XMMATRIX translatematrix = XMMatrixTranslation(_position.x,_position.y,_position.z);
	tempModelMX *= translatematrix;
	XMStoreFloat4x4(&modelMX,tempModelMX);
	D3D11_BUFFER_DESC constDesc;
	ZeroMemory(&constDesc, sizeof(constDesc));
	constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constDesc.ByteWidth = (sizeof(object_buffer));
	constDesc.Usage = D3D11_USAGE_DEFAULT;
	_dxDev->CreateBuffer(&constDesc, 0, &Buffer);
	//d_states = _depths;
	//b_states = _blends;
}

XMFLOAT4X4 Scene_object::get_matrix() const
{
	return modelMX;
}

void Scene_object::disable_blend()
{
	manager->disable_blend();
}

void Scene_object::enable_blend()
{
	manager->enable_blend();
}

void Scene_object::enable_CCW()
{
	manager->enable_CCW();
}

void Scene_object::enable_CW()
{
	manager->enable_CW();
}

void Scene_object::enable_depthEqual()
{
	manager->enable_depthEqual();
}

void Scene_object::enable_depthShadow()
{
	manager->enable_depthShadow();
}

void Scene_object::enable_depthStateLess()
{
	manager->enable_depthStateLess();
}

void Scene_object::set_buffer(const vertex_index_buffer& input)
{
	VIbuffer = input;
}

void Scene_object::initialise_shader(const char* filename) //can be overloaded by object
{
	shader.loadfx(filename);
	shader.loadeffect(_dxDev,_dxImmedDC);
}

ID3DX11EffectTechnique* Scene_object::initialise_effect(LPCSTR technique_name)
{
	return shader.loadeffect(_dxDev,_dxImmedDC,technique_name);
}

const shaders& Scene_object::get_shader() const
{
	return shader;
}

void Scene_object::set_active(int pass)
{
	shader.set_active(VIbuffer,pass);
}

void Scene_object::set_active(ID3DX11EffectTechnique* custom_effect,int pass)
{
	shader.set_active(custom_effect,VIbuffer,pass);
}

void Scene_object::draw()
{
	//need to figure out how to correctly push custom struct to constant buffer without having the child class to do it!
	_dxImmedDC->DrawIndexed(VIbuffer.index_amount,0,0);
}

void Scene_object::loadSphere(const XMFLOAT4& color,float size)
{
	Shape_Maker sm;
	VIbuffer = sm.make_globe(color,_dxDev,size,position);
}

void Scene_object::loadSphere(const XMFLOAT4& color,float size, const XMFLOAT3& custom_pos)//override sphere with custom position
{
	Shape_Maker sm;
	VIbuffer = sm.make_globe(color,_dxDev,size,custom_pos);
}
void Scene_object::loadmodel(const std::wstring& strFileName,const XMFLOAT4& color,bool Bumpmap)
{
	Shape_Maker sm;
	VIbuffer = sm.loadobj(strFileName,_dxDev,color,Bumpmap);
}

void Scene_object::set_position(const XMFLOAT3& input)
{
	position = input;
}

void Scene_object::set_ModelMX(const XMFLOAT4X4& input)
{
	modelMX = input;
}

void Scene_object::set_wvp(const XMMATRIX& wvp)
{
	XMStoreFloat4x4(&objbuff.worldViewProj,wvp);
}

ID3D11Device* Scene_object::get_DXDevice() const
{
	return _dxDev;
}

ID3D11DeviceContext* Scene_object::get_DeviceContext() const
{
	return _dxImmedDC;
}

const Scene_object::object_buffer& Scene_object::get_objBuff() const
{
	return objbuff;
}

ID3D11Buffer* Scene_object::get_buffer() const
{
	return Buffer;
}

const vertex_index_buffer& Scene_object::get_VI_Buffer() const
{
	return VIbuffer;
}

Scene_manager* Scene_object::get_manager() const
{
	return manager;
}

