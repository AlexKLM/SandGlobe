#pragma once
#include "scene_object.h"
#include "Shape_Maker.h"
#include "shaders.h"


struct tree_state{
	ID3DX11EffectTechnique* singlelight;
	ID3DX11EffectTechnique* multilight;
	bool show_tex;
	tree_state* next;
};

struct tree_buff {
	XMMATRIX worldViewProj;
	XMMATRIX worldInverseTranspose;
	XMMATRIX worldMx;
	XMFLOAT4 clip_level;
	XMFLOAT3 cam_pos;
};
class tree :
	public Scene_object
{
public:
	tree(void);
	~tree(void);
	void Render(const XMMATRIX& viewMX,const XMMATRIX& projMX,const XMFLOAT4& cliplevel,const XMFLOAT3& _cam_pos, bool multi_light);
	void generate_shadow(const XMMATRIX& lightVPMx,const XMFLOAT3& lightpos, shaders& shadow_shader, ID3D11Buffer* shadowBuff);
	void initialise_shader(const char* filename);	
	void create();
	void change_state();
private:
	void create_tree_states();
	//render modes
	tree_state wireframe;
	tree_state Flat;
	tree_state Smooth;
	tree_state Smooth_tex;
	tree_state Bump;
	tree_state* current;

	Shadow_cBuffer shadow_data;
	Material tree_m;
	ID3D11ShaderResourceView* tree_tex;
	ID3D11ShaderResourceView* Norm_map;
	ID3D11SamplerState *tree_Sampler;
	ID3D11Buffer *m_cBuffer;


	ID3DX11EffectTechnique* tree_spot_sh;

	ID3D11Buffer *cbBuffer; 
};

