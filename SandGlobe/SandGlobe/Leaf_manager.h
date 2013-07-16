#pragma once
#include <Windows.h>
#include <xnamath.h>
#include "Global.h"
#include <d3d11.h>
#include <D3DX11.h>
#include <d3dx11Effect.h>
#include "shaders.h"
#include "leaf_generator.h"
#include "leaf_trunk.h"
#include "Shape_Maker.h"
class Scene_manager;


//leaf manager manges leaf and leaf trunk rendering and growth, also destroy(burn) them 
class Leaf_manager
{
public:
	Leaf_manager(void);
	~Leaf_manager(void);
	void initialise(Scene_manager* _manager,const XMFLOAT3& _pos,ID3D11DeviceContext *_dxDC, ID3D11Device *_dxD);
	void setup_generator(const std::string& _rule,const std::string& _start,const std::string& _target);
	void update(double dt);
	void Render(const XMMATRIX& viewMX,const XMMATRIX& projMX,bool multilights);
	void Render_shadow(const XMMATRIX& lightVPMx,const XMFLOAT3& lightpos, shaders& shadow_shader, ID3D11Buffer* shadowBuff);
	void start_burn();
	void start_grow();
	void return_leaf_positions(std::vector<XMFLOAT3>& output);
	XMFLOAT3 return_leaf_pos_rand();
	bool have_leafs() const;
	bool is_destoried() const;
	bool is_grow_ready() const;
	bool is_fully_grown() const;
	bool is_fully_burnt() const;
	void destroy_leaf_list();
private:
	void cleanup();
	std::string rule;
	std::string start;
	std::string target;
	void generate_leaf();
	
	void grow();
	bool is_growing;
	bool fully_grown;
	bool ready_to_grow;
	bool burning;
	bool fully_burnt;
	bool destroied;
	float cycle;
	XMFLOAT3 position;
	leaf_generator leaf_gen;
	Scene_manager* manager;
	ID3D11Device *_dxDev;
	ID3D11DeviceContext *_dxImmedDC;
	//objects
	std::vector<leaf_trunk> l_trunk_list;
	vertex_index_buffer trunk_buffer;
	vertex_index_buffer leaf_buffer;

	//shaders
	shaders leaf_shade;
	shaders leaf_trunk_shade;

};

