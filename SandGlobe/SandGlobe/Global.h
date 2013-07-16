#pragma once
#include <vector>
#include <windows.h>
#include <xnamath.h>
#include <d3d11.h>


const XMFLOAT4 BLACK (0.0f, 0.0f, 0.0f, 1.0f);
const XMFLOAT4 WHITE (1.0f, 1.0f, 1.0f, 1.0f);
const XMFLOAT4 RED   (1.0f, 0.0f, 0.0f, 1.0f);
const XMFLOAT4 YELLOW(1.0f,1.0f,0.2f,1.0f);
const XMFLOAT4 GREEN (0.0f, 1.0f, 0.0f, 1.0f);
const XMFLOAT4 BLUE  (0.0f ,0.0f, 1.0f, 1.0f);

struct cal_cBuffer{
	float shouldemit;
	XMFLOAT3 emit_pos;
	XMFLOAT4 gravity;
};

struct Shadow_cBuffer{
	XMFLOAT4X4 lightViewProj;
	XMFLOAT4X4 worldMx;
	XMFLOAT3 lPos;
	float padding;
};

struct timeBuffers{
	XMFLOAT2 padding2;
	float GlobalTime;
	float DeltaTime;
};

struct render_cBuffer{
	XMFLOAT4X4 worldViewProj;
	XMFLOAT4X4 InvViewMx;
};

struct Season{
	Season* next_season;
	bool grow_leaf;
	bool burn_leaf;
	bool fog;
	bool frozen;
	bool has_task;//only advance if task is finished
	XMFLOAT4 dirL_amb; //directional light ambeint
	XMFLOAT4 sL_amb;//Spot light ambeint
	float life;
};

struct particle_Vertex
{
	XMFLOAT3 pos;
	XMFLOAT3 vel;
	float Timer;
	UINT Type;
};

struct Vertex//Vertex data to be passed into the buffer list
{
	XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT4 color;
	XMFLOAT2 tex;
	XMFLOAT3 tangent;
	XMFLOAT3 binormal;
};

struct ambientlight{
	float intensity;
	XMFLOAT4 color;
};

struct directionlight{
	XMFLOAT3 direction;
	XMFLOAT4 color;
};

struct pointlight{
	XMFLOAT3 plight_pos;
	float padding;
	float plight_range;
	XMFLOAT3 plight_att;
	XMFLOAT4 plight_ambient;
	XMFLOAT4 plight_diffuse;
	XMFLOAT4 plight_specular;
	XMFLOAT4 plight_spec_color;
	float spec_power;
	XMFLOAT3 plight_direction;
};

struct spotlight{
	XMFLOAT3 slight_pos;
	float slight_range;
	XMFLOAT3 slight_dir;
	float slight_cone;
	XMFLOAT3 slight_att;
	XMFLOAT4 slight_ambient;
	XMFLOAT4 slight_diffuse;
	float padding;
};

struct Material //for phong lighting
{
	
	float M_ambient;
	float M_diffuse;
	float M_spec;
	float show_tex;
};

struct config_data
{
	XMFLOAT3 tree_pos;
	XMFLOAT3 island_pos;
	XMFLOAT3 globe_pos;
	XMFLOAT3 base_pos;
	XMFLOAT3 sun_pos;
	XMFLOAT3 sea_pos;
	XMFLOAT3 bottle_pos;
	XMFLOAT3 light_pos[4];
	float sim_speed;
	
};

struct depth_states
{
	ID3D11DepthStencilState* m_depthStencilStateLess;	// NEW
	ID3D11DepthStencilState* m_depthStencilStateEqual;	// NEW
};

struct blend_states
{
	ID3D11BlendState *blendState;
};

struct instanceData
{
	XMFLOAT4X4 modelMX;
	float padding;
};

struct BufferList //buffer list to be passed into the buffer
{
	std::vector<Vertex> Vertex_list;
	//std::vector<XMFLOAT3> Normal_list;
	std::vector<DWORD> Indicies_list;
};

struct vertex_index_buffer
{
	ID3D11Buffer* vbuf_;
	ID3D11Buffer* ibuf_;
	int index_amount;
};


