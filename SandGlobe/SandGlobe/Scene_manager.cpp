#include "Scene_manager.h"


Scene_manager::Scene_manager(void)
{
	for(int i = 0; i < 3; i ++)
	{
		blendFactor[i] = 0;
	}
	blendState = NULL;
	m_depthStencilStateLess = NULL;
	m_depthStencilStateEqual = NULL;
	shadow_pass_depthStencil = NULL;
	m_depthDisabledStencilState = NULL;
	season_timer = 0;
	current_season = NULL;
	speed_mod = 1;
	sea_step =0;
	seaview = false;
	multilights = true;
	time_step = 0;
	_WorldViewProjMx = NULL;
	_dxDev = NULL;
	_dxImmedDC = NULL;
	d_states = NULL;
	b_states = NULL;
	suntimestep = NULL;
	default_renderTargetView = NULL;
	default_depthStencilView = NULL;
	Shadow_map = NULL;
	Shadow_map_tex = NULL;
	Shadow_RV = NULL;
	shadow_depth = NULL;
	rtv_shadowmap = NULL;
	shadowBuff = NULL;
	CCW = NULL;
	CW = NULL;
	plightbuffer = NULL;
	slightbuffer = NULL;
	sealevel = 0;
	burn_cooldown = 2;
	burn_timer = 0;
}

Scene_manager::~Scene_manager(void)
{
}

void Scene_manager::initialize(ID3D11Device *_dxD,ID3D11DeviceContext *_dxImDC,ID3D11RenderTargetView* _renderTargetView, ID3D11DepthStencilView *_depthStencilView)
{
	readin_vars(L"config.txt");
	speed_mod = config_data.sim_speed;
	suntimestep = 0;
	time_step = 0;
	_dxDev = _dxD;
	_dxImmedDC = _dxImDC;
	//initialize_effect();
	setup_blendstate();
	setup_depthStencils();

	sandglobe.initialise(this,config_data.globe_pos,_dxImmedDC,_dxDev);
	island.initialise(this,config_data.island_pos,_dxImmedDC,_dxDev);
	//sea.initialise(this,config_data.sea_pos,_dxImmedDC,_dxDev);
	base.initialise(this,config_data.base_pos,_dxImmedDC,_dxDev);
	Tree.initialise(this,config_data.tree_pos,_dxImmedDC,_dxDev);
	sun.initialise(this,config_data.sun_pos,_dxImmedDC,_dxDev);
	sea_surface.initialise(this,config_data.sea_pos,_dxImmedDC,_dxDev);
	bottle.initialise(this,config_data.bottle_pos,_dxImmedDC,_dxDev);

	Particle_m.Initialsie(this,_dxImmedDC,_dxDev);
	Particle_m.Setup_Water(XMFLOAT3(0,0,0),2);
	Particle_m.Setup_Fog(XMFLOAT3(0,-1,0),5,0.5,0.5,20);

	//Test only
	//Particle_m.Add_Fire(XMFLOAT3(1,1,1));


	generate_lights();
	//TEST
	//Particle_m.start_Fog();

	default_renderTargetView = _renderTargetView;
	default_depthStencilView = _depthStencilView;

	a_light.color = WHITE;
	a_light.intensity = 0.3f;

	p_light.plight_ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	p_light.plight_diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	p_light.plight_att = XMFLOAT3(0.0f, 0.1f, 0.0f);
	p_light.plight_range = 10.0f;
	p_light.plight_pos = XMFLOAT3(2,4,0);
	p_light.plight_specular = XMFLOAT4(1,1,1,1.0f);
	p_light.plight_spec_color = XMFLOAT4(0.1f,0.1f,0.1f,1.0f);
	p_light.spec_power = 0.5;
	
	initialize_lightbuffers();


	//generate resources for shadow mapping
	setup_shadow_map_mat();

	//initialise cull modes 
	D3D11_RASTERIZER_DESC ras_desc;
	ZeroMemory(&ras_desc, sizeof(D3D11_RASTERIZER_DESC));
	ras_desc.CullMode = D3D11_CULL_BACK;
	ras_desc.FillMode = D3D11_FILL_SOLID;
	ras_desc.FrontCounterClockwise = true;
	_dxDev->CreateRasterizerState(&ras_desc, &CCW);
	ras_desc.FrontCounterClockwise = false;
	_dxDev->CreateRasterizerState(&ras_desc, &CW);


	leaf_m.initialise(this,XMFLOAT3(config_data.tree_pos),_dxImmedDC,_dxDev);
	
	//initialise leaf gen
	//all of these should be read from config file
	std::string start = "FA";
	std::string rule = "S[-B][+B]FA";
	std::string target = "A";
	leaf_m.setup_generator(rule,start,target);

	//TESTING ONLY
	//leaf_m.start_grow();

	//Seasons
	setup_seasons();

}

void Scene_manager::Stop_fog()
{
	Particle_m.stop_Fog();
}

void Scene_manager::generate_lights() //temp method
{

	//Spot light isn't working yet due to bugs?

	/*spotlight spot; 
	spot.slight_pos = config_data.light_pos[0];
	spot.slight_dir = XMFLOAT3(0,0,0);
	spot.slight_range = 100.0f;
	spot.slight_cone = 100;
	spot.slight_att = XMFLOAT3(0.4f, 1, 0.4f);
	spot.slight_ambient = XMFLOAT4(0,0, 0, 1.0f);
	spot.slight_diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
*/

	pointlight light;
	light.plight_ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	light.plight_diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	light.plight_att = XMFLOAT3(0.0f, 0.2f, 0.0f);
	light.plight_range = 10.0f;
	light.plight_pos = config_data.light_pos[0];
	light.plight_specular = XMFLOAT4(1,1,1,1.0f);
	light.plight_spec_color = XMFLOAT4(0.1f,0.1f,0.1f,1.0f);
	light.spec_power = 0.5;

	light_list.push_back(light);

	light.plight_pos = config_data.light_pos[1];


	light_list.push_back(light);

	light.plight_pos = config_data.light_pos[2];
	
	light_list.push_back(light);

	light.plight_pos = config_data.light_pos[3];

	light_list.push_back(light);
}

void Scene_manager::initialize_lightbuffers()
{
	HRESULT hr;
	D3D11_BUFFER_DESC constDesc;
	ZeroMemory(&constDesc, sizeof(constDesc));
	constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constDesc.ByteWidth = (sizeof(pointlight));
	constDesc.Usage = D3D11_USAGE_DEFAULT;
	hr = _dxDev->CreateBuffer(&constDesc, 0, &plightbuffer);

	constDesc.ByteWidth = (sizeof(spotlight));
	hr = _dxDev->CreateBuffer(&constDesc, 0, &slightbuffer);
}

void Scene_manager::setup_leaf_trunks()
{
	if(leaf_m.is_grow_ready())
	{
		leaf_m.start_grow();
	}
}

void Scene_manager::setup_seasons()
{

	//Spring
	Spring.burn_leaf = false;
	Spring.fog = false;
	Spring.grow_leaf = true;
	Spring.life = 10;
	Spring.frozen = false;
	Spring.has_task = true;
	Spring.dirL_amb = XMFLOAT4(0.5f,0.5f,0.5f,1.0f);
	Spring.sL_amb = XMFLOAT4(0.2f,0.2f,0.2f,1.0f);

	//Summer
	Summer.burn_leaf = true;
	Summer.fog = false;
	Summer.frozen = false;
	Summer.grow_leaf = false;
	Summer.life = 30;
	Summer.has_task = true;
	Summer.dirL_amb = XMFLOAT4(1.0f,1.0f,1.0f,1.0f);
	Summer.sL_amb = XMFLOAT4(0.5f,0.5f,0.5f,1.0f);

	//Autumn
	Autumn.burn_leaf = false;
	Autumn.fog = true;
	Autumn.frozen = false;
	Autumn.grow_leaf = false;
	Autumn.life = 30;
	Autumn.has_task = false;
	Autumn.dirL_amb = XMFLOAT4(0.5f,0.5f,0.5f,1.0f);
	Autumn.sL_amb =  XMFLOAT4(0.2f,0.2f,0.2f,1.0f);

	//Winter
	Winter.burn_leaf = false;
	Winter.fog = false;
	Winter.frozen = true;
	Winter.grow_leaf = false;
	Winter.life = 20;
	Winter.has_task = false;
	Winter.dirL_amb = XMFLOAT4(0.1f,0.1f,0.1f,1.0f);
	Winter.sL_amb =  XMFLOAT4(0.05f,0.05f,0.05f,1.0f);

	Spring.next_season = &Summer;
	Summer.next_season = &Autumn;
	Autumn.next_season = &Winter;
	Winter.next_season = &Spring;
	
	change_season(&Spring);

}

void Scene_manager::change_season(Season* next_season)
{
	current_season = next_season;
	if(current_season->fog)
	{
		Particle_m.start_Fog();
	}
	else
	{
		Particle_m.stop_Fog();
	}
	if(current_season->grow_leaf)
	{
		leaf_m.start_grow();
	}
	p_light.plight_ambient = current_season->dirL_amb;
	for(int i = 0; i < light_list.size(); i++)
	{
		light_list[i].plight_ambient = current_season->sL_amb;
	}
	season_timer = 0;
}

void Scene_manager::setup_shadow_map_mat()//create texture desc, depth stencil and rv
{
	HRESULT hr;

	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(D3D11_TEXTURE2D_DESC));
	texDesc.Width = 800;
	texDesc.Height = 600;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	descDSV.Flags = 0;
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = _dxDev->CreateTexture2D( &texDesc, NULL, &Shadow_map );
	hr = _dxDev->CreateDepthStencilView( Shadow_map, &descDSV, &shadow_depth );
	

	
	D3D11_BUFFER_DESC constDesc;
	ZeroMemory(&constDesc, sizeof(constDesc));
	constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constDesc.ByteWidth = (sizeof(Shadow_cBuffer));
	constDesc.Usage = D3D11_USAGE_DEFAULT;
	 hr = _dxDev->CreateBuffer(&constDesc, 0, &shadowBuff);


	DXGI_FORMAT format;
	D3D11_TEXTURE2D_DESC shadow_texformat ;
	ZeroMemory (& shadow_texformat , sizeof ( shadow_texformat ));
	shadow_texformat.Width = 800;
	shadow_texformat.Height = 600;
	shadow_texformat.MipLevels = 1;
	shadow_texformat.ArraySize = 1;
	shadow_texformat.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	shadow_texformat.SampleDesc . Count = 1;
	shadow_texformat.Usage = D3D11_USAGE_DEFAULT;
	shadow_texformat.BindFlags = D3D11_BIND_RENDER_TARGET|D3D11_BIND_SHADER_RESOURCE ;
	shadow_texformat.CPUAccessFlags = 0;
	shadow_texformat.MiscFlags = 0;
	hr = _dxDev->CreateTexture2D(&shadow_texformat,nullptr,&Shadow_map_tex);
	format = shadow_texformat.Format;


	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	hr = _dxDev->CreateShaderResourceView( Shadow_map_tex, &srvDesc, &Shadow_RV);

	D3D11_RENDER_TARGET_VIEW_DESC target_info;
	target_info.Format = format;
	target_info.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	target_info.Texture2D.MipSlice = 0;
	hr = _dxDev->CreateRenderTargetView(Shadow_map_tex, &target_info ,&rtv_shadowmap);

}

void Scene_manager::create_gemoitry()
{
	sandglobe.create_globe();
	island.make_globe();
	sun.create();
	//sea.create_sea();
	base.create_Base();
	Tree.create();
	sea_surface.create();
	bottle.create();
	Shape_Maker sm;

}

void Scene_manager::setup_render_to_texture(int w, int h)
{
	sea_surface.create_render_to_texture(w,h);
}

void Scene_manager::initialize_effect()
{
	//globe_shader.loadfx("fx/Globe_Effect.fxo");
	//globe_shader.loadeffect(_dxDev,_dxImmedDC);

	sandglobe.initialise_shader("fx/Globe_Effect.fxo");
	
	//island_shader.loadfx("fx/Island_Effect.fxo");
	//island_shader.loadeffect(_dxDev,_dxImmedDC);

	island.initialise_shader("fx/Island_Effect.fxo");

	//Sea_shader.loadfx("fx/Sea_Effect.fxo");
	//Sea_shader.loadeffect(_dxDev,_dxImmedDC);

	//sea.initialise_shader("fx/Sea_Effect.fxo");

	//base_shader.loadfx("fx/Base_Effect.fxo");
	//base_shader.loadeffect(_dxDev,_dxImmedDC);

	base.initialise_shader("fx/Base_Effect.fxo");

	//tree_shader.loadfx("fx/Tree_Effect.fxo");
	//tree_shader.loadeffect(_dxDev,_dxImmedDC);

	Tree.initialise_shader("fx/Tree_Effect.fxo");

	//sun_shader.loadfx("fx/Sun_Effect.fxo");
	//sun_shader.loadeffect(_dxDev,_dxImmedDC);

	sun.initialise_shader("fx/Sun_Effect.fxo");

	//surface_shader.loadfx("fx/SeaSurface_Effect.fxo");
	//surface_shader.loadeffect(_dxDev,_dxImmedDC);

	sea_surface.initialise_shader("fx/SeaSurface_Effect.fxo");

	bottle.initialise_shader("fx/Bottle_Effect.fxo");

	//leaf_shaders.loadfx("fx/Leave_Effect.fxo");
	//leaf_shaders.loadeffect(_dxDev,_dxImmedDC);



	//trunk_shaders.loadfx("fx/Trunk_Effect.fxo");
	//trunk_shaders.loadeffect(_dxDev,_dxImmedDC);

	//water_P_shader.loadfx("fx/Water_P_Effect.fxo");
	//water_P_shader.load_particle_effect(_dxDev,_dxImmedDC);

	//fire_P_shader.loadfx("fx/Fire_P_Effect.fxo");
	//fire_P_shader.load_particle_effect(_dxDev,_dxImmedDC);

	//bottle_shader.loadfx("fx/Bottle_Effect.fxo");
	//bottle_shader.loadeffect(_dxDev,_dxImmedDC);

	shadow_gen_shader.loadfx("fx/Shadow_map_gen.fxo");
	shadow_gen_shader.loadeffect(_dxDev,_dxImmedDC);
}

void Scene_manager::update(double dt)
{
	dt = dt * speed_mod;
	bool isrising = false;
	suntimestep += dt * 50;
	time_step += dt;
	season_timer += dt;
	
	if(season_timer > current_season->life&&!current_season->has_task)
	{
		change_season(current_season->next_season);
	}

	if(!current_season->frozen)
	{
		sea_step += dt;
		float new_slevel = sin(sea_step) * 0.3f;
		if(sealevel < new_slevel)
		{
			isrising = true;
		}

		sealevel = new_slevel;
		//sea.update(sealevel,dt,true);
	}
	sea_surface.update(sealevel,dt,current_season->frozen);
//	sea.update(sealevel,dt,false);
	if(seaview)
	{
		game_cam.set_sea_level(sealevel+0.5f);
	}
	float rad = suntimestep * XM_PI / 180;
	float x = 0 + sin(rad)*4;
	float y = 0 + cos(rad)*4;
	if(suntimestep > 90)
	{
		suntimestep = -90;
	}

	sun.update(XMFLOAT2(x,y));

	leaf_m.update(dt);
	
	if(current_season->burn_leaf)
	{
		if(burn_timer > burn_cooldown)
		{
			if(!leaf_m.is_fully_burnt())
			{
				std::vector<XMFLOAT3> pos_list;
				leaf_m.return_leaf_positions(pos_list);
				if(!pos_list.empty())
				{
					for(int i = 0; i< pos_list.size();i++)
					{
						Particle_m.Add_Fire(pos_list[i]);
					}
				}
			}
			else
			{
				Particle_m.destroy_Fire();
				leaf_m.destroy_leaf_list();
				change_season(current_season->next_season);
			}
			burn_timer = 0;
		}
		else
		{
			burn_timer +=dt;
		}
	}

	if(current_season->grow_leaf&&leaf_m.is_fully_grown())
	{
		change_season(current_season->next_season);
	}

	x = 0 + sin(rad)*6;
	y = 0 + cos(rad)*6;
	p_light.plight_pos.x = x;
	p_light.plight_pos.y = y;
	XMFLOAT3 target = XMFLOAT3(0,0,0);
	p_light.plight_direction = XMFLOAT3(target.x - p_light.plight_pos.x,target.y - p_light.plight_pos.y,target.z - p_light.plight_pos.z);
	
	/*s_lights[0].slight_pos.x = x;
	s_lights[0].slight_pos.y = y;*/
	//if(y < 0)
	//{
	//	suntimestep = -80;
	//}
	Particle_m.Calculate_Particles(dt,sealevel,isrising);


}

void Scene_manager::render(const XMMATRIX& projMX)
{
	generate_shadow_map();
	XMMATRIX viewMX = game_cam.getCameraMx();
	XMMATRIX sea_reflect_viewMx = game_cam.getReflectMx(sealevel);
	XMFLOAT3 campos = game_cam.getposition();
	cleanup_shadow_render();
	render_reflection_texture(projMX);
	//////
	_dxImmedDC->OMSetRenderTargets(1, &default_renderTargetView, default_depthStencilView);
	_dxImmedDC->ClearRenderTargetView(default_renderTargetView, (float*)&BLACK);
	_dxImmedDC->ClearDepthStencilView(default_depthStencilView, D3D11_CLEAR_DEPTH, 1, 0);

	XMMATRIX lightViewprojMx;
	XMFLOAT3 target_f3 = XMFLOAT3(0,0,0);
	XMFLOAT3 up_f3 = XMFLOAT3(0,-1,0);

	


	XMFLOAT4 normal_n_height = XMFLOAT4(0,-1,0,sealevel);
	XMVECTOR clipplan = XMLoadFloat4(&normal_n_height);
	clipplan *=-1;
	XMMATRIX viewprojMx = viewMX * projMX;
	XMMATRIX inverseviewprojMx = XMMatrixInverse(&clipplan,viewprojMx);
	clipplan = XMVector4Transform(clipplan,viewprojMx);
	XMFLOAT4 clip;
	XMStoreFloat4(&clip,clipplan);
	XMFLOAT4 cliptest = XMFLOAT4(0,-1,0,sealevel+0.5f);	

	XMVECTOR target = XMLoadFloat3(&target_f3);
	//XMVECTOR pos = XMLoadFloat3(&p_light.plight_pos);
	XMVECTOR pos;
	if(multilights)
	{
		XMFLOAT3 lpos = light_list[0].plight_pos;
		lpos.x += 0.1f; //add a little offset to make the shadow a bit more dramatic
		pos = XMLoadFloat3(&lpos);
	}
	else
	{
		pos= XMLoadFloat3(&p_light.plight_pos);
	}
	//XMVECTOR pos = XMLoadFloat3(&light_list[0].plight_pos);
	XMVECTOR up = XMLoadFloat3(&up_f3);
	XMMATRIX proj = XMMatrixPerspectiveFovLH( XM_PIDIV4, (float)800/600, 1, 1000);
	XMMATRIX lightviewMx = XMMatrixLookAtLH(pos, target, up );
	lightViewprojMx = XMMatrixTranspose(XMMatrixMultiply(lightviewMx,proj));

	//island.Render(viewMX,projMX,lightViewprojMx,island_shader,XMFLOAT4(0,1,0,1000));
	base.Render(viewMX,projMX);

	island.Render(viewMX,projMX,lightViewprojMx,XMFLOAT4(0,1,0,1000),multilights);
	
	//sea_surface.Render(viewMX,projMX,sea_reflect_viewMx,surface_shader,false);
	sea_surface.Render(viewMX,projMX,sea_reflect_viewMx,current_season->frozen,multilights);
    leaf_m.Render(viewMX,projMX,multilights);
	//leaf_m.Render(viewMX,projMX);

	
	
	//Tree.Render(viewMX,projMX,tree_shader,XMFLOAT4(0,1,0,1000),campos);
		Particle_m.Render_Fire(viewMX,projMX);
	Tree.Render(viewMX,projMX,XMFLOAT4(0,1,0,1000),campos,multilights);

	
	bottle.Render(viewMX,projMX,XMFLOAT4(0,1,0,1000),multilights);

	sun.Render(viewMX,projMX,XMFLOAT4(0,1,0,1000));

//	sea.Render(viewMX,projMX);

	

	Particle_m.Render_Water(viewMX,projMX);
	
	Particle_m.Render_Fog(viewMX,projMX);

	//sandglobe.Render(viewMX,projMX,globe_shader);
	
	sandglobe.Render(viewMX,projMX,multilights);
	

	


}

void Scene_manager::render_reflection_texture(const XMMATRIX& projMX)
{
	/*_dxImmedDC->OMSetRenderTargets(1, &default_renderTargetView, default_depthStencilView);
	_dxImmedDC->ClearRenderTargetView(default_renderTargetView, (float*)&BLACK);
	_dxImmedDC->ClearDepthStencilView(default_depthStencilView, D3D11_CLEAR_DEPTH, 1, 0);*/

	//XMFLOAT3 normal = XMFLOAT3(0,-1,0);
	ID3D11RenderTargetView* rtv_ptr = sea_surface.return_rtv();
	XMMATRIX sea_reflect_viewMx = game_cam.getReflectMx(sealevel);
	XMFLOAT3 cam_pos = game_cam.getRefpos(sealevel);

	ID3D11ShaderResourceView* nullSRV = 0;
    _dxImmedDC->PSSetShaderResources(0, 1, &nullSRV);
	_dxImmedDC->OMSetRenderTargets(1, &rtv_ptr, default_depthStencilView);
	_dxImmedDC->ClearRenderTargetView(sea_surface.return_rtv(), (float*)&WHITE);
	_dxImmedDC->ClearDepthStencilView(default_depthStencilView, D3D11_CLEAR_DEPTH, 1, 0);

	XMMATRIX lightViewprojMx;
	XMFLOAT3 target_f3 = XMFLOAT3(0,0,0);
	XMFLOAT3 up_f3 = XMFLOAT3(0,-1,0);

	XMVECTOR target = XMLoadFloat3(&target_f3);
	XMVECTOR pos;
	if(multilights)
	{
		XMFLOAT3 lpos = light_list[0].plight_pos;
		lpos.x += 0.1f; //add a little offset to make the shadow a bit more dramatic
		pos = XMLoadFloat3(&lpos);
	}
	else
	{
		pos= XMLoadFloat3(&p_light.plight_pos);
	}
	XMVECTOR up = XMLoadFloat3(&up_f3);
	XMMATRIX lightviewMx = XMMatrixLookAtLH(pos, target, up );
	lightViewprojMx = XMMatrixTranspose(XMMatrixMultiply(lightviewMx,projMX));


	XMFLOAT4 normal_n_height = XMFLOAT4(0,-1,0,sealevel);
	XMVECTOR surface_clip = XMLoadFloat4(&normal_n_height);
	surface_clip *=-1;
	XMMATRIX viewprojMx = sea_reflect_viewMx * projMX;
	XMMATRIX inverseviewprojMx = XMMatrixInverse(&XMMatrixDeterminant(viewprojMx),viewprojMx);
	inverseviewprojMx = XMMatrixTranspose(inverseviewprojMx);
	surface_clip = XMVector4Transform(surface_clip,inverseviewprojMx);
	XMFLOAT4 clip;
	XMStoreFloat4(&clip,surface_clip);
	
	/*normal_n_height = XMFLOAT4(0,-1,0,sealevel);
	surface_clip = XMLoadFloat4(&normal_n_height);
	surface_clip = XMVector4Transform(surface_clip,inverseviewprojMx);
	XMFLOAT4 clip_4_tree;
	XMStoreFloat4(&clip,surface_clip);*/

	island.Render(sea_reflect_viewMx,projMX,lightViewprojMx,clip,multilights);
		Particle_m.Render_Fire(sea_reflect_viewMx,projMX);
	Tree.Render(sea_reflect_viewMx,projMX,clip,cam_pos,multilights);

	bottle.Render(sea_reflect_viewMx,projMX,clip,multilights);

	leaf_m.Render(sea_reflect_viewMx,projMX,multilights);

	/*if(!l_trunk_list.empty())
	{
		for(int i = 0; i < l_trunk_list.size(); i++)
		{
			l_trunk_list[i].Render(trunk_buffer,sea_reflect_viewMx,projMX,trunk_shaders);
			if(l_trunk_list[i].is_trunk_grow_finish())
			{
				l_trunk_list[i].render_leafs(leaf_buffer,sea_reflect_viewMx,projMX,leaf_shaders);
			}
		}
	}*/
	
	//test_particle.Render(sea_reflect_viewMx,projMX,water_P_shader);
	sun.Render(sea_reflect_viewMx,projMX,clip);
	//for(int i = 0; i < waterparticle_list.size(); i++)
	//{
	//	waterparticle_list[i].Render(sea_reflect_viewMx,projMX,water_P_shader);
	//}
}

void Scene_manager::generate_shadow_map()
{
	ID3D11ShaderResourceView* nullSRV = 0;
	_dxImmedDC->PSSetShaderResources(0, 1, &nullSRV);
	_dxImmedDC->OMSetDepthStencilState(m_depthStencilStateLess,1);
	_dxImmedDC->OMSetRenderTargets(1, &rtv_shadowmap, shadow_depth);
	_dxImmedDC->ClearRenderTargetView(rtv_shadowmap, (float*)&BLACK);
	_dxImmedDC->ClearDepthStencilView(shadow_depth, D3D11_CLEAR_DEPTH, 1, 0);
	

	//_dxImmedDC->OMSetRenderTargets(0, 0, shadow_depth);
	//_dxImmedDC->ClearRenderTargetView(0, (float*)&BLACK);
	//_dxImmedDC->ClearDepthStencilView( shadow_depth, D3D11_CLEAR_DEPTH, 1.0f, 0 );

	/*ID3D11ShaderResourceView* nullSRV = 0;
	_dxImmedDC->OMSetDepthStencilState(shadow_pass_depthStencil,1);
	_dxImmedDC->PSSetShaderResources(0, 1, &nullSRV);
	_dxImmedDC->ClearDepthStencilView( shadow_depth, D3D11_CLEAR_DEPTH, 1.0, 0 );
	_dxImmedDC->ClearDepthStencilView(default_depthStencilView, D3D11_CLEAR_DEPTH, 1, 0);*/

	//ID3D11RenderTargetView* nullView = 0;
	//_dxImmedDC->OMSetRenderTargets( 1, &default_renderTargetView, shadow_depth );
	//_dxImmedDC->PSSetShaderResources(0, 1, &Shadow_RV);
	XMMATRIX lightViewprojMx;
	XMFLOAT3 target_f3 = XMFLOAT3(0,0,0);
	XMFLOAT3 up_f3 = XMFLOAT3(0,-1,0);
	//XMFLOAT3 test_pos = XMFLOAT3(3,5,3);
	XMVECTOR target = XMLoadFloat3(&target_f3);

	XMVECTOR pos;
	XMFLOAT3 lpos;
	if(multilights)
	{
		lpos = light_list[0].plight_pos;
		lpos.x += 0.1f; //add a little offset to make the shadow a bit more dramatic
		pos = XMLoadFloat3(&lpos);
	}
	else
	{
		lpos = p_light.plight_pos;
		pos= XMLoadFloat3(&p_light.plight_pos);
	}

	//XMVECTOR pos = XMLoadFloat3(&test_pos);
	XMVECTOR up = XMLoadFloat3(&up_f3);
	XMMATRIX lightviewMx = XMMatrixLookAtLH(pos, target, up );

	XMMATRIX proj = XMMatrixPerspectiveFovLH( XM_PIDIV4, (float)800/600, 1, 1000);
	lightViewprojMx = XMMatrixTranspose(XMMatrixMultiply(lightviewMx,proj));
	island.generate_shadow(lightViewprojMx,lpos,shadow_gen_shader,shadowBuff);
	Tree.generate_shadow(lightViewprojMx,lpos,shadow_gen_shader,shadowBuff);
	leaf_m.Render_shadow(lightViewprojMx,lpos,shadow_gen_shader,shadowBuff);

	//ID3D11DepthStencilView* nullSV = 0;
	//_dxImmedDC->OMSetRenderTargets(0,0,nullSV);


	/*if(!l_trunk_list.empty())
	{
		for(int i = 0; i < l_trunk_list.size(); i++)
		{
			l_trunk_list[i].render_Shadow(trunk_buffer,lightViewprojMx,p_light.plight_pos,shadow_gen_shader);
			if(l_trunk_list[i].is_trunk_grow_finish())
			{
				l_trunk_list[i].render_Shadow_leafs(leaf_buffer,lightViewprojMx,p_light.plight_pos,shadow_gen_shader);
			}
		}
	}*/
	//shadow_gen_shader.set_resource_shadow_texture("
}

const std::vector<pointlight>& Scene_manager::get_light_list() const
{
	return light_list;
}

void Scene_manager::cleanup_shadow_render()
{
	_dxImmedDC->OMSetDepthStencilState(m_depthStencilStateEqual,1);
	ID3D11RenderTargetView* pNullView = 0;
	_dxImmedDC->OMSetRenderTargets( 0, &pNullView, 0 );
	_dxImmedDC->OMSetRenderTargets(1, &default_renderTargetView, default_depthStencilView);
	_dxImmedDC->ClearRenderTargetView(default_renderTargetView, (float*)&BLACK);
	_dxImmedDC->ClearDepthStencilView(shadow_depth, D3D11_CLEAR_DEPTH, 1, 0);
	_dxImmedDC->ClearDepthStencilView(default_depthStencilView, D3D11_CLEAR_DEPTH, 1, 0);
}

void Scene_manager::enable_CCW()
{
	_dxImmedDC->RSSetState(CCW);
}

void Scene_manager::enable_CW()
{
	_dxImmedDC->RSSetState(CW);
}
//Camera opreations
void Scene_manager::movecam(const XMFLOAT3& axis)
{
	game_cam.move(axis);
}

void Scene_manager::rotatecam(const XMFLOAT2& input)
{
	game_cam.rotate(input);
}

void Scene_manager::movecam_z(float _movement)
{
	game_cam.move_z(_movement);
}

void Scene_manager::tree_view()
{
	XMFLOAT3 position = Tree.get_position();
	XMFLOAT3 direction = position;
	direction.y +=2;
	position.z -= 1;
	position.y += 1;
	position.x -= 1;
	game_cam.change_camera(position,direction);
	seaview = false;
}

void Scene_manager::bottle_view()
{
	XMFLOAT3 position = bottle.get_position();
	XMFLOAT3 direction = position;
	position.z -= 1.0f;
	position.y += 0.8f;
	position.x -= 0.2f;
	game_cam.change_camera(position,direction);
	seaview = false;
}

void Scene_manager::leaf_view()
{
	if(leaf_m.have_leafs())
	{
		XMFLOAT3 position = leaf_m.return_leaf_pos_rand();
		XMFLOAT3 direction = position;
		position.z += 0.2f;
		//position.y += 0.1f;
		position.x += 0.2f;
		game_cam.change_camera(position,direction);
		if(seaview)
		{
			seaview = false;
		}
	}
}

void Scene_manager::sea_view()
{
	seaview = true;
	XMFLOAT3 position = sea_surface.get_position();
	XMFLOAT3 direction = island.get_position();
	direction.y += 2;
	position.x -= 5;
	position.y += 0.5f;
	//position.z += 2;
	position.y = sealevel;
	game_cam.change_camera(position,direction);
}

void Scene_manager::globe_view()
{
	game_cam.globe_view();
	seaview = false;
}

void Scene_manager::toggle_multi()
{
	if(multilights)
	{
		multilights = false;
	}
	else
	{
		multilights = true;
	}
}

void Scene_manager::modify_speedmod(float input)
{
	speed_mod += input;
	if(speed_mod <0)
	{
		speed_mod = 0.1f;
	}
}

void Scene_manager::reset_speedmod()
{
	speed_mod = 1;
}

void Scene_manager::change_tree_state()
{
	Tree.change_state();
}

//Read in config file

void Scene_manager::readin_vars(const std::wstring& FileName)
{
	//setup default values, hard coded values incase the file readin failed(eg file not exist)
	config_data.bottle_pos = XMFLOAT3(-0.3,0.8,-0.3);
	config_data.globe_pos = XMFLOAT3(0,0,0);
	config_data.island_pos = XMFLOAT3(0,0,0);
	config_data.base_pos = XMFLOAT3(0,-0.8f,0);
	config_data.sea_pos = XMFLOAT3(0,0,0);
	config_data.sun_pos = XMFLOAT3(4,0,0);
	config_data.tree_pos = XMFLOAT3(0,0,0);
	config_data.light_pos[0] = XMFLOAT3(0,6,0);
	config_data.light_pos[1] = XMFLOAT3(5,5,0);
	config_data.light_pos[2] = XMFLOAT3(-5,5,-5);
	config_data.light_pos[3] = XMFLOAT3(5,5,5);
	config_data.sim_speed = 1;
	ifstream fin;
	std::wifstream filein(FileName);

	if(filein)
	{
		WCHAR strCommand[256] = {0};
		while(filein)
		{
			filein >> strCommand;
			if( 0 == wcscmp( strCommand, L"//" ) )
			{

			}
			else if( 0 == wcscmp( strCommand, L"GlobePos" ) )
			{
				filein >> config_data.globe_pos.x >> config_data.globe_pos.y >> config_data.globe_pos.z;
			}
			else if( 0 == wcscmp( strCommand, L"BottlePos" ) )
			{
				filein >> config_data.bottle_pos.x >> config_data.bottle_pos.y >> config_data.bottle_pos.z;
			}
			else if( 0 == wcscmp( strCommand, L"IslandPos" ) )
			{
				filein >> config_data.island_pos.x >> config_data.island_pos.y >> config_data.island_pos.z;
			}
			else if( 0 == wcscmp( strCommand, L"BasePos" ) )
			{
				filein >> config_data.base_pos.x >> config_data.base_pos.y >> config_data.base_pos.z;
			}
			else if( 0 == wcscmp( strCommand, L"SeaPos" ) )
			{
				filein >> config_data.sea_pos.x >> config_data.sea_pos.y >> config_data.sea_pos.z;
			}
			else if( 0 == wcscmp( strCommand, L"TreePos" ) )
			{
				filein >> config_data.tree_pos.x >> config_data.tree_pos.y >> config_data.tree_pos.z;
			}
			else if( 0 == wcscmp( strCommand, L"SunPos" ) )
			{
				filein >> config_data.sun_pos.x >> config_data.sun_pos.y >> config_data.sun_pos.z;
			}
			else if( 0 == wcscmp( strCommand, L"Light1Pos" ) )
			{
				filein >> config_data.light_pos[0].x >> config_data.light_pos[0].y >> config_data.light_pos[0].z;
			}
			else if( 0 == wcscmp( strCommand, L"Light2Pos" ) )
			{
				filein >> config_data.light_pos[1].x >> config_data.light_pos[1].y >> config_data.light_pos[1].z;
			}
			else if( 0 == wcscmp( strCommand, L"Light3Pos" ) )
			{
				filein >> config_data.light_pos[2].x >> config_data.light_pos[2].y >> config_data.light_pos[2].z;
			}
			else if( 0 == wcscmp( strCommand, L"Light3Pos" ) )
			{
				filein >> config_data.light_pos[3].x >> config_data.light_pos[3].y >> config_data.light_pos[3].z;
			}
			else if( 0 == wcscmp( strCommand, L"SimSpeed" ) )
			{
				filein >> config_data.sim_speed;
			}
		}
	}
	
}

//depth and blend states

void Scene_manager::setup_blendstate()
{
	D3D11_BLEND_DESC blendStateDesc;
	// Clear the blend state description.
	ZeroMemory(&blendStateDesc, sizeof(D3D11_BLEND_DESC));
	blendStateDesc.RenderTarget[0].BlendEnable = TRUE;
	blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	// Setup the blend factor.
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;

	_dxDev->CreateBlendState(&blendStateDesc, &blendState);
}

void Scene_manager::setup_depthStencils()
{
	D3D11_DEPTH_STENCIL_DESC depthstencil_desc;
	depthstencil_desc.DepthEnable = TRUE;
	depthstencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthstencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
	depthstencil_desc.StencilEnable = TRUE;
	depthstencil_desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depthstencil_desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	depthstencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthstencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INVERT;
	depthstencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthstencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthstencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthstencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_INVERT;
	depthstencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthstencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	_dxDev->CreateDepthStencilState(&depthstencil_desc, &m_depthStencilStateLess);


	depthstencil_desc.DepthFunc = D3D11_COMPARISON_EQUAL;
	 _dxDev->CreateDepthStencilState(&depthstencil_desc, &m_depthStencilStateEqual);

	depthstencil_desc.StencilEnable = true;
	depthstencil_desc.StencilReadMask = 0XFF;
	depthstencil_desc.StencilWriteMask = 0XFF;

	depthstencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthstencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthstencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthstencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depthstencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthstencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthstencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthstencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	_dxDev->CreateDepthStencilState(&depthstencil_desc, &shadow_pass_depthStencil);
}

void Scene_manager::enable_depthEqual()
{
	_dxImmedDC->OMSetDepthStencilState(m_depthStencilStateEqual, 1);
}

void Scene_manager::enable_depthShadow()
{
	_dxImmedDC->OMSetDepthStencilState(shadow_pass_depthStencil,1);
}

void Scene_manager::enable_depthStateLess()
{
	_dxImmedDC->OMSetDepthStencilState(m_depthStencilStateLess,1);
}

void Scene_manager::enable_blend()
{
	_dxImmedDC->OMSetBlendState(blendState,blendFactor, 0xffffffff);
}

void Scene_manager::disable_blend()
{
	_dxImmedDC->OMSetBlendState(0, 0, 0xffffffff);
}

const pointlight& Scene_manager::get_p_light() const
{
	return p_light;
}

const std::vector<spotlight>& Scene_manager::get_s_lights() const
{
	return s_lights;
}

ID3D11Buffer* Scene_manager::return_PL_Buffer() const
{
	return plightbuffer;
}

ID3D11Buffer* Scene_manager::return_SL_Buffer() const
{
	return slightbuffer;
}

ID3D11ShaderResourceView* Scene_manager::return_shadow_RV() const
{
	return Shadow_RV;
}

ID3D11Buffer* Scene_manager::return_Shadow_Buffer() const
{
	return shadowBuff;
}