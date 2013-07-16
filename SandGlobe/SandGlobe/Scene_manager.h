#pragma once
#include <time.h>//for random
#include <fstream>
#include "dxapp.h"
#include "Global.h"
//Scene_objects
#include "Globe.h"
#include "Island.h"
//#include "leaf_trunk.h"
#include "Sea.h"
#include "Globe_Base.h"
#include "tree.h"
//#include "leaf_generator.h"
#include "Sun.h"
#include "surface.h"
//#include "Water_Particles.h"
//#include "Fire_Particles.h"
#include "Bottle.h"
#include "Leaf_manager.h"
#include "Particle_Manager.h"

#include "Camera.h"
//shaders
#include "shaders.h"

class Scene_manager
{
public:
	Scene_manager(void);
	~Scene_manager(void);
	void initialize(ID3D11Device *_dxD,ID3D11DeviceContext *_dxImDC,ID3D11RenderTargetView* _renderTargetView, ID3D11DepthStencilView *_depthStencilView);
	void update(double dt);
	void render_reflection_texture(const XMMATRIX& projMX);
	void render(const XMMATRIX& projMX);
	void setup_leaf_trunks();
	void destroy_leaf_list();
	void getReflection_texture();
	void create_gemoitry();
	//float get_sealevel() const;
	void setup_2d_depth();
	void setup_seasons();
	void setup_render_to_texture(int w, int h);
	void setup_shadow_map_mat();
	void toggle_multi(); //toggle multi_light on and off
	//shadow map
	void generate_shadow_map();
	void Stop_fog();
	const pointlight& get_p_light() const;


	void readin_vars(const std::wstring& FileName);

	void movecam(const XMFLOAT3& axis);
	void rotatecam(const XMFLOAT2& input);
	void movecam_z(float _movement);
	void tree_view();
	void leaf_view();
	void bottle_view();
	void sea_view();
	void globe_view();

	void modify_speedmod(float input);
	void reset_speedmod();

	const std::vector<spotlight>& get_s_lights() const;
	const std::vector<pointlight>& get_light_list() const;
	ID3D11Buffer* return_PL_Buffer() const;
	ID3D11Buffer* return_SL_Buffer() const;
	ID3D11Buffer* return_Shadow_Buffer() const;
	ID3D11ShaderResourceView* return_shadow_RV() const;

	void change_tree_state();
	void enable_CW();
	void enable_CCW();
	//depth_states& return_dstates();
	//blend_states& return_bstates();

	
	void disable_blend();
	void enable_blend();
	void enable_depthStateLess();
	void enable_depthEqual();
	void enable_depthShadow();

	
	/*ID3D11ShaderResourceView * srv_render_to_texture;*/

	//ID3D11Buffer* leaf_instance_buffer; //for leafs


	
	
	
	void initialize_effect();
	

private:
	void setup_blendstate();
	void setup_depthStencils();

	ID3D11RenderTargetView *default_renderTargetView;
	ID3D11DepthStencilView *default_depthStencilView;


	//blendstates
	float blendFactor[4];								// NEW
	ID3D11BlendState *blendState;						// NEW
	ID3D11DepthStencilState* m_depthStencilStateLess;	
	ID3D11DepthStencilState* m_depthStencilStateEqual;	
	ID3D11DepthStencilState* shadow_pass_depthStencil;
	ID3D11DepthStencilState* m_depthDisabledStencilState;

	//for shadow map
	ID3D11Texture2D* Shadow_map;
	ID3D11Texture2D* Shadow_map_tex;
	ID3D11ShaderResourceView* Shadow_RV;
	ID3D11DepthStencilView* shadow_depth;
	ID3D11RenderTargetView * rtv_shadowmap;
	ID3D11Buffer* shadowBuff;

	ID3D11Buffer *plightbuffer;
	ID3D11Buffer *slightbuffer;

	ID3D11RasterizerState *CCW;
	ID3D11RasterizerState *CW;

	std::vector<spotlight> s_lights;


	float speed_mod;
	float time_step;
	float sea_step;
	void cleanup_shadow_render();
	void initialize_lightbuffers();
	void change_season(Season* next_season);
	void generate_lights();
	pointlight p_light;
	bool seaview;
	bool multilights;

	config_data config_data;

	ambientlight a_light;

	std::vector<pointlight> light_list;
	
	ID3DX11EffectMatrixVariable *_WorldViewProjMx;
	ID3D11Device *_dxDev;
	ID3D11DeviceContext *_dxImmedDC;
	//vertex_index_buffer trunk_buffer;
	//vertex_index_buffer leaf_buffer;

	Camera game_cam;

	depth_states *d_states;
	blend_states *b_states;
	float sealevel;
	//bool ready_to_grow; //ready to grow leafs
	float suntimestep;
	float season_timer;
	float burn_cooldown; //cooldown between adding fires
	float burn_timer;
	//bool burning;

	//objects
	Globe sandglobe;
	Island island;
	//Sea sea;
	//std::vector<leaf_trunk> l_trunk_list;
	Globe_Base base;
	tree Tree;
	Sun sun;
	surface sea_surface;
	Bottle bottle;

	//Seasons
	Season Spring;
	Season Summer;
	Season Autumn;
	Season Winter;
	Season* current_season;

	//particles
	//std::vector<Water_Particles> waterparticle_list;
	//Fire_Particles test_fire;

	//tools
	Leaf_manager leaf_m;
	Particle_Manager Particle_m;

	//shaders
	/*shaders trunk_shaders;
	shaders leaf_shaders;
	shaders globe_shader;
	shaders island_shader;
	shaders Sea_shader;
	shaders base_shader;
	shaders tree_shader;
	shaders sun_shader;
	shaders surface_shader;
	shaders water_P_shader;
	shaders fire_P_shader;
	shaders bottle_shader;*/


	shaders shadow_gen_shader;
};

