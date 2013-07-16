#pragma once

#include "Global.h"
#include <d3d11.h>
#include <D3DX11.h>
#include <d3dx11Effect.h>
#include "shaders.h"
#include "Leaf.h"
#include "Shape_Maker.h"
#include <time.h>//for random

class leaf_generator;
class Scene_manager;
struct leaf_buff
{
	XMFLOAT4X4 viewMx;
	XMFLOAT4X4 projMx;
};
struct tree_buffer{
	XMFLOAT4X4 worldViewProj;
};
class leaf_trunk
{
public:
	leaf_trunk(void);
	~leaf_trunk(void);
	void destroy();
	void initialise(Scene_manager* _manager,XMFLOAT3& _position,ID3D11DeviceContext *_dxDC, ID3D11Device *_dxD,const XMFLOAT3& _treetoppos, leaf_generator* _leafgen);
	void initialise_buffers();
	void update(double dt,bool _grow);
	void render_leafs(const vertex_index_buffer& leaf_buffer,const XMMATRIX& viewMX,const XMMATRIX& projMX, shaders& _sh);
	void Render(const vertex_index_buffer& trunk_buffer,const XMMATRIX& viewMX,const XMMATRIX& projMX, shaders& _sh);
	void render_Shadow(const vertex_index_buffer& trunk_buffer,const XMMATRIX& lightVPMx,const XMFLOAT3& lightpos, shaders& shadow_shader, ID3D11Buffer* shadowBuff  );
	void render_Shadow_leafs(const vertex_index_buffer& trunk_buffer,const XMMATRIX& lightVPMx,const XMFLOAT3& lightpos, shaders& shadow_shader, ID3D11Buffer* shadowBuff  );
	void render_leafs_lights(const vertex_index_buffer& leaf_buffer,const XMMATRIX& viewMX,const XMMATRIX& projMX, shaders& _sh,std::vector<pointlight>& lights);
	void Render_lights(const vertex_index_buffer& trunk_buffer,const XMMATRIX& viewMX,const XMMATRIX& projMX, shaders& _sh,std::vector<pointlight>& lights);
	void create();
	void p_generate_leafs();
	void grow(double dt);
	bool is_trunk_grow_finish() const;
	bool is_grow_process_finish() const;
	float return_leaf_step() const;
	float return_length() const;
	bool return_next_burn_pos(int cycle, XMFLOAT3& output);
	XMFLOAT3 return_random_leaf_pos();
	const std::vector<Leaf>& return_leaf_list() const;
	const std::vector<instanceData>& return_leaf_instances_data() const;
private:
	std::vector<Leaf> leaf_list;
	std::vector<instanceData> leaf_instances_data;
	tree_buffer objbuff;
	Scene_manager* manager;
	XMFLOAT3 position;
	XMFLOAT4X4 modelMX;
	ID3D11Buffer *Buffer; 
	ID3D11Device *_dxDev;
	ID3D11DeviceContext *_dxImmedDC;
	leaf_generator* leaf_gen;
	bool fullyburning;
	float leaf_steps; //distance between each leaf
	Shadow_cBuffer shadow_data;
	float current_length;
	bool grow_finished;
	bool leaf_grow_finished;
	float trunk_length;
	XMFLOAT3 tree_pos;
	float world_angle; //angle of the leafs
	ID3D11Buffer* leaf_instance_buffer;
	void create_instance_buffer();
	leaf_buff leave_vp_buffer;
	void create_leaf(const XMFLOAT3& _pos,float rotation,float _length,float worldangle);
	int num_of_leaves;
};

