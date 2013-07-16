#include "shaders.h"


shaders::shaders(void)
{
	_technique = NULL;
	_inputLayout = NULL;
	_particle_inputLayout = NULL;
	_WorldViewProjMx = NULL;
	Shading_tech = NULL;
	_calc_particles = NULL;
	_render_particles = NULL;
	_dxImmedDC = NULL;
}


shaders::~shaders(void)
{
	try
	{
		cleanup();
	}
	catch(...)
	{
	}
}

void shaders::cleanup()
{
	if(_inputLayout!=0)
	{
		_inputLayout->Release();
	}
	if(_particle_inputLayout!=0)
	{
		_particle_inputLayout->Release();
	}
	if(Shading_tech !=0)
	{
		Shading_tech->Release();
	}

}

void shaders::set_active_calc_particles(ID3D11Buffer* input_buffer,int pass)
{
	_dxImmedDC->IASetInputLayout(_particle_inputLayout);
	UINT stride[1] = { sizeof( particle_Vertex ) };
	UINT offset[1] = { 0 };
	
	_dxImmedDC->IASetVertexBuffers( 0, 1, &input_buffer, stride, offset );
    _dxImmedDC->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );

	
	_calc_particles->GetPassByIndex(pass)->Apply(0, _dxImmedDC);

}

void shaders::set_active_rend_particles(ID3D11Buffer* input_buffer,int pass)
{
	
	 UINT stride[1] = { sizeof( particle_Vertex ) };
    UINT offset[1] = { 0 };
	_dxImmedDC->IASetInputLayout(_particle_inputLayout);
    _dxImmedDC->IASetVertexBuffers( 0, 1, &input_buffer, stride, offset );
    _dxImmedDC->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );
	_render_particles->GetPassByIndex(pass)->Apply(0, _dxImmedDC);
}

void shaders::set_active(const vertex_index_buffer& vi_buff,int pass)
{
	_dxImmedDC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	UINT stride = sizeof(Vertex), offset = 0;
	_dxImmedDC->IASetInputLayout(_inputLayout);
	_dxImmedDC->IASetVertexBuffers(0, 1, &vi_buff.vbuf_, &stride, &offset);
	_dxImmedDC->IASetIndexBuffer(vi_buff.ibuf_, DXGI_FORMAT_R32_UINT, 0);
	_technique->GetPassByIndex(pass)->Apply(0, _dxImmedDC);
}

void shaders::set_active(const vertex_index_buffer& vi_buff,ID3D11Buffer* instance_buff,int pass)
{
	UINT stride[2] = {sizeof(Vertex),sizeof(instanceData)};
	UINT offset[2] = {0,0};
	ID3D11Buffer* vertInstBuffers[2] = {vi_buff.vbuf_, instance_buff};
	_dxImmedDC->IASetInputLayout(_inputLayout);
	_dxImmedDC->IASetVertexBuffers(0, 2, vertInstBuffers, stride, offset);
	_dxImmedDC->IASetIndexBuffer(vi_buff.ibuf_, DXGI_FORMAT_R32_UINT, 0);
	_technique->GetPassByIndex(pass)->Apply(0, _dxImmedDC);
}

void shaders::set_active(ID3DX11EffectTechnique* output_effect,const vertex_index_buffer& vi_buff,int pass)
{
	_dxImmedDC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	UINT stride = sizeof(Vertex), offset = 0;
	_dxImmedDC->IASetInputLayout(_inputLayout);
	_dxImmedDC->IASetVertexBuffers(0, 1, &vi_buff.vbuf_, &stride, &offset);
	_dxImmedDC->IASetIndexBuffer(vi_buff.ibuf_, DXGI_FORMAT_R32_UINT, 0);
	output_effect->GetPassByIndex(pass)->Apply(0, _dxImmedDC);
}

void shaders::load_varibles(const XMMATRIX& WVP_MX)
{
	_WorldViewProjMx->SetMatrix((float*)&WVP_MX);
}

bool shaders::loadfx(const char* filename)
{
	std::ifstream fin(filename, std::ios::binary);
	if (!fin)
		return false;
	fin.seekg(0, std::ios_base::end);
	int size = (int)fin.tellg();

	compiledShader.resize(size);
	fin.seekg(0, std::ios_base::beg);
	fin.read(&compiledShader[0], size);
	if (!fin)
		return false;
	return true;
}

bool shaders::loadeffect(ID3D11Device *_dxDev,ID3D11DeviceContext *_dxImDC)
{
	_dxImmedDC = _dxImDC;
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0,  DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(Vertex, color), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(Vertex,normal),D3D11_INPUT_PER_VERTEX_DATA,0},
		{ "TEXCOORD", 0,DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(Vertex,tex),D3D11_INPUT_PER_VERTEX_DATA,0},
		{ "mTransform", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "mTransform", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "mTransform", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "mTransform", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	};
	D3DX11CreateEffectFromMemory(&compiledShader[0], compiledShader.size(), 0, _dxDev, &Shading_tech);
	_WorldViewProjMx = Shading_tech->GetVariableByName("worldViewProj")->AsMatrix();
	if (!_WorldViewProjMx)
		return false;

	_technique = Shading_tech->GetTechniqueByName("RenderScene");
	if (!_technique)
		return false;

	D3DX11_PASS_DESC passDesc;
	_technique->GetPassByIndex(0)->GetDesc(&passDesc);
	_dxDev->CreateInputLayout(layout, 8, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &_inputLayout);
	//setup_blendstate(_dxDev);
	//setup_depthStencils(_dxDev);
	return true;
}

ID3DX11EffectTechnique* shaders::loadeffect(ID3D11Device *_dxDev, ID3D11DeviceContext *_dxImDC, LPCSTR technique_name)
{
	ID3DX11EffectTechnique* effect;
	_dxImmedDC = _dxImDC;
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0,  DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(Vertex, color), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(Vertex,normal),D3D11_INPUT_PER_VERTEX_DATA,0},
		{ "TEXCOORD", 0,DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(Vertex,tex),D3D11_INPUT_PER_VERTEX_DATA,0},
		{ "TANGENT",0,DXGI_FORMAT_R32G32B32_FLOAT,0,offsetof(Vertex,tangent),D3D11_INPUT_PER_VERTEX_DATA,0},
		{ "BINORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,offsetof(Vertex,binormal),D3D11_INPUT_PER_VERTEX_DATA,0},

		{ "mTransform", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "mTransform", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "mTransform", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "mTransform", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	};
	D3DX11CreateEffectFromMemory(&compiledShader[0], compiledShader.size(), 0, _dxDev, &Shading_tech);

	effect = Shading_tech->GetTechniqueByName(technique_name);

	D3DX11_PASS_DESC passDesc;
	effect->GetPassByIndex(0)->GetDesc(&passDesc);
	_dxDev->CreateInputLayout(layout, 10, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &_inputLayout);
	/*setup_blendstate(_dxDev);
	setup_depthStencils(_dxDev);*/
	return effect;
}

bool shaders::load_particle_effect(ID3D11Device *_dxDev,ID3D11DeviceContext *_dxImDC)
{
	_dxImmedDC = _dxImDC;
	const D3D11_INPUT_ELEMENT_DESC layout2[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TIMER", 0, DXGI_FORMAT_R32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TYPE", 0, DXGI_FORMAT_R32_UINT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
	D3DX11CreateEffectFromMemory(&compiledShader[0], compiledShader.size(), 0, _dxDev, &Shading_tech);
	_calc_particles = Shading_tech->GetTechniqueByName("Calculate_Particles");
	_render_particles = Shading_tech->GetTechniqueByName("RenderScene");

	D3DX11_PASS_DESC passDesc;
	_calc_particles->GetPassByIndex(0)->GetDesc(&passDesc);
	_dxDev->CreateInputLayout(layout2, 4, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &_particle_inputLayout);
	//setup_blendstate(_dxDev);
	//setup_depthStencils(_dxDev);
	return true;
}

