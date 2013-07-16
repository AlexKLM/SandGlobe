#pragma once
#include "Global.h"
#include "shaders.h"
#include <time.h>
#include <xnamath.h>
#include <d3d11.h>
#include <D3DX11.h>
#include <d3dx11Effect.h>
class Scene_manager;

class Particle
{
public:
	Particle(void);
	virtual ~Particle(void);
	void destroy();
	virtual void initialize(Scene_manager* _manager,ID3D11DeviceContext *_dxDC, ID3D11Device *_dxD, const XMFLOAT3& _position,float final_life_time);
	void set_particle_texture(LPCWSTR tex_name);
	virtual void Calculate_Particles(double dt, shaders& _sh,bool _shouldemit);
	void Calculate_Particles(double dt, shaders& _sh, const cal_cBuffer& input);//calculate base on custom caculation data
	virtual void Render(const XMMATRIX& viewMX,const XMMATRIX& projMX, shaders& _sh);
	void set_delete();
	bool is_die_ready() const;
	void set_gravity(const XMFLOAT4& grav);
	virtual void create_buffers();
	void generate_randoms();
protected:
	
	void start_particle();
	void mark_particle_to_kill();
	
	void set_emit();
	void set_pos(const XMFLOAT3& input);
	void set_c_data(const cal_cBuffer& input);
	void set_timeBuffer(const timeBuffers& input);
	void set_r_data(const render_cBuffer& input);
	bool is_mark_to_kill() const;
	bool is_first_cycle() const;

	const XMFLOAT3& get_pos() const;
	const float get_life_timer() const;

	void set_dieready(bool input);
	void set_mark_to_kill(bool input);
	void set_first_cycle(bool input);

	const timeBuffers& get_tBuffer() const;
	const cal_cBuffer& get_c_data() const;
	const render_cBuffer& get_r_data() const;
	
	void swap_buffers(); //after calculation, for child class that require their own calculation method

	
	ID3D11Device* get_DXDevice() const;
	ID3D11DeviceContext* get_DeviceContext() const;
	ID3DX11EffectTechnique* get_technique() const;

	ID3D11Buffer* return_dt_gtBuffer() const;
	ID3D11Buffer* return_c_cBuffer() const;
	ID3D11Buffer* return_r_cBuffer() const;

	ID3D11Buffer* return_S_T_buffer() const;
	ID3D11Buffer* return_render_buffer() const;
	ID3D11Buffer* return_start_buffer() const;

	ID3D11ShaderResourceView* return_randomTextureRV() const;
	ID3D11ShaderResourceView* return_particle_tex() const;

private:
	ID3D11Buffer* Stream_target_buffer; //calculated particle data are sent here
	ID3D11Buffer* render_buffer;
	ID3D11Buffer* start_buffer;//used in first cycle

	timeBuffers tBuffer;
	cal_cBuffer c_data;
	render_cBuffer r_data;
	//particle_Vertex p_vertex;

	ID3D11Texture1D* RandomTexture;
	ID3D11ShaderResourceView* RandomTexRV;
	ID3D11ShaderResourceView* Particle_tex;

	ID3D11Buffer* dt_gtBuffer;
	ID3D11Buffer* c_cBuffer;
	ID3D11Buffer* r_cBuffer;

	ID3D11Device *_dxDev;
	ID3D11DeviceContext *_dxImmedDC;

	Scene_manager* manager;

	bool first_cycle;
	XMFLOAT3 position;
	bool die_ready;//ready to be deleted
	float life_timer;//activate once set to be deleted;
	bool mark_to_kill;//activate the life timer

};

