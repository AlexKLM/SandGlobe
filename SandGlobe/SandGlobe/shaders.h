#pragma once
#include <cassert>
#include "Global.h"
#include <iostream> 
#include <fstream>
#include <d3d11.h>
#include <d3dx11Effect.h>



class shaders
{

public:
	shaders(void);
	~shaders(void);
	
	bool loadfx(const char* filename);
	bool loadeffect(ID3D11Device *_dxDev, ID3D11DeviceContext *_dxImDC);
	ID3DX11EffectTechnique* loadeffect(ID3D11Device *_dxDev, ID3D11DeviceContext *_dxImDC, LPCSTR technique_name);
	//void set_resource_shadow_texture(string resource_name, ID3D11ShaderResourceView* resource);
	bool load_particle_effect(ID3D11Device *_dxDev,ID3D11DeviceContext *_dxImDC);//for particle effect shaders
	void set_active(const vertex_index_buffer& vi_buff,int pass); //get the shaders to take in buffers and get it ready for render
	void set_active(const vertex_index_buffer& vi_buff,ID3D11Buffer* instance_buff,int pass);
	void set_active(ID3DX11EffectTechnique* output_effect,const vertex_index_buffer& vi_buff,int pass);
	void set_active_calc_particles(ID3D11Buffer* input_buffer,int pass);
	void set_active_rend_particles(ID3D11Buffer* input_buffer,int pass);

	void load_varibles(const XMMATRIX& WVP_MX); //this base function only loads WVP mx
private:
	void cleanup();

	ID3D11InputLayout *_inputLayout;
	ID3D11InputLayout *_particle_inputLayout;
	ID3DX11EffectMatrixVariable *_WorldViewProjMx; //most shader will require this 
	ID3DX11Effect *Shading_tech;
	ID3DX11EffectTechnique *_technique;
	ID3DX11EffectTechnique *_calc_particles; //calculate particles
	ID3DX11EffectTechnique *_render_particles; //render particles
	ID3D11DeviceContext *_dxImmedDC;
	std::vector<char> compiledShader;

	//blendstates
	//float blendFactor[4];								// NEW
	//ID3D11BlendState *blendState;						// NEW
	//ID3D11DepthStencilState* m_depthStencilStateLess;	
	//ID3D11DepthStencilState* m_depthStencilStateEqual;	
	//ID3D11DepthStencilState* shadow_pass_depthStencil;
	//ID3D11DepthStencilState* m_depthDisabledStencilState;
	//

};

