#include "Leaf_manager.h"
#include "Scene_manager.h"


Leaf_manager::Leaf_manager(void)
{
	manager = NULL;
	_dxDev = NULL;
	_dxImmedDC = NULL;
	cycle = 0;
	is_growing = false;
	fully_grown = false;
	ready_to_grow = true;
	burning = false;
	destroied = true;
	fully_burnt = false;
}


Leaf_manager::~Leaf_manager(void)
{
	try
	{
		cleanup();
	}
	catch(...)
	{

	}
}

void Leaf_manager::cleanup()
{
	l_trunk_list.clear();
}

void Leaf_manager::initialise(Scene_manager* _manager,const XMFLOAT3& _pos,ID3D11DeviceContext *_dxDC, ID3D11Device *_dxD)
{
	manager = _manager;
	position = _pos;
	position.y += 2.9f;
	_dxDev = _dxD;
	_dxImmedDC = _dxDC;

	leaf_shade.loadfx("fx/Leave_Effect.fxo");
	leaf_shade.loadeffect(_dxDev,_dxImmedDC);

	leaf_trunk_shade.loadfx("fx/Trunk_Effect.fxo");
	leaf_trunk_shade.loadeffect(_dxDev,_dxImmedDC);

	Shape_Maker sm;
	trunk_buffer = sm.loadobj(L"media\\trunk.obj", _dxDev, XMFLOAT4(0,0.5f,0,1),false);
	leaf_buffer = sm.loadobj(L"media\\leaf.obj", _dxDev, XMFLOAT4(0,0.3f,0,1),false);

}

void Leaf_manager::update(double dt)
{
	if(!is_growing || fully_grown)
	{
		return;
	}
	fully_grown = true;
	if(!l_trunk_list.empty())
	{
		for(int i = 0; i < l_trunk_list.size(); i++)
		{
			if(!l_trunk_list[i].is_grow_process_finish())
			{
				fully_grown = false;
				l_trunk_list[i].update(dt,true);
			}
			else
			{
				l_trunk_list[i].update(dt,false);
			}
		}
	}
}

void Leaf_manager::destroy_leaf_list()
{
	l_trunk_list.clear();
	cycle = 0;
	ready_to_grow = true;
	destroied = true;
	burning = false;
	fully_burnt = false;
}

void Leaf_manager::setup_generator(const std::string& _rule,const std::string& _start,const std::string& _target)
{
	leaf_gen.setup(_start,_rule,_target);
}

void Leaf_manager::generate_leaf() //
{
	srand ( time(NULL) );
	int num_of_leafs = rand()%9+4;
	
	for(int i = 0; i < num_of_leafs;i++)
	{
		leaf_trunk l;
		l.initialise(manager,XMFLOAT3(0,0,0),_dxImmedDC,_dxDev,position,&leaf_gen);
		l_trunk_list.push_back(l);
	}
	for(int i = 0; i < l_trunk_list.size(); i++)
	{
		l_trunk_list[i].initialise_buffers();
		l_trunk_list[i].p_generate_leafs();
	}
}

void Leaf_manager::Render(const XMMATRIX& viewMX,const XMMATRIX& projMX, bool multilights)
{
	if(!l_trunk_list.empty())
	{
		if(multilights)
		{
			std::vector<pointlight> lights;
			lights = manager->get_light_list();
			if(!l_trunk_list.empty())
			{
				for(int i = 0; i < l_trunk_list.size(); i++)
				{
					l_trunk_list[i].Render_lights(trunk_buffer,viewMX,projMX,leaf_trunk_shade,lights);
					if(l_trunk_list[i].is_trunk_grow_finish())
					{
						l_trunk_list[i].render_leafs_lights(leaf_buffer,viewMX,projMX,leaf_shade,lights);
					}
				}
			}
		}
		else
		{
			for(int i = 0; i < l_trunk_list.size(); i++)
			{
				l_trunk_list[i].Render(trunk_buffer,viewMX,projMX,leaf_trunk_shade);
				if(l_trunk_list[i].is_trunk_grow_finish())
				{
					l_trunk_list[i].render_leafs(leaf_buffer,viewMX,projMX,leaf_shade);
				}
			}
		}
	}
}


void Leaf_manager::Render_shadow(const XMMATRIX& lightVPMx,const XMFLOAT3& lightpos, shaders& shadow_shader, ID3D11Buffer* shadowBuff )
{
	if(!l_trunk_list.empty())
	{
		for(int i = 0; i < l_trunk_list.size(); i++)
		{
			l_trunk_list[i].render_Shadow(trunk_buffer,lightVPMx,lightpos,shadow_shader,shadowBuff);
			if(l_trunk_list[i].is_trunk_grow_finish())
			{
				l_trunk_list[i].render_Shadow_leafs(leaf_buffer,lightVPMx,lightpos,shadow_shader,shadowBuff);
			}
		}
	}
}

void Leaf_manager::return_leaf_positions(std::vector<XMFLOAT3>& output)
{
	float step;
	float length;
	//float result;
	XMFLOAT3 result = XMFLOAT3(0,0,0);
	for(int i = 0; i < l_trunk_list.size(); i++)
	{
		if(l_trunk_list[i].return_next_burn_pos(cycle,result))
		{
			output.push_back(result);
		}
	}
	cycle +=6; //leaf stores in pairs with same position
	if(output.empty())
	{
		fully_burnt = true;
	}
}

XMFLOAT3 Leaf_manager::return_leaf_pos_rand()
{
	int i = rand() %l_trunk_list.size()-1;
	return l_trunk_list[i].return_random_leaf_pos();
}

void Leaf_manager::start_grow()
{
	ready_to_grow = false;
	fully_grown = false;
	is_growing = true;
	burning = false;
	fully_burnt = false;
	cycle = 0;
	generate_leaf();
}

void Leaf_manager::start_burn()
{
	burning = true;
}

bool Leaf_manager::is_destoried() const
{
	return destroied;
}

bool Leaf_manager::is_fully_grown() const
{
	return fully_grown;
}

bool Leaf_manager::is_grow_ready() const
{
	return ready_to_grow;
}

bool Leaf_manager::is_fully_burnt() const
{
	return fully_burnt;
}

bool Leaf_manager::have_leafs() const
{
	if(l_trunk_list.empty())
	{
		return false;
	}
	else
	{
		return true;
	}
}
