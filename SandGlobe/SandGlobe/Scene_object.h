#pragma once
#include <Windows.h>
#include <xnamath.h>
#include "Global.h"
#include <d3d11.h>
#include <D3DX11.h>
#include <d3dx11Effect.h>
#include "shaders.h"
#include "Shape_Maker.h"

class Scene_manager;

//scene_object deals with loading shader and model of object, and rendering of the object

class Scene_object
{
struct object_buffer{
	XMFLOAT4X4 worldViewProj;
};

public:
	Scene_object(void);
	virtual void Render(XMMATRIX& viewMX, XMMATRIX& projMX, shaders& _sh);
	//vertex_index_buffer *return_vibuffer();
	virtual void initialise_shader(const char* filename);
	ID3DX11EffectTechnique* initialise_effect(LPCSTR technique_name); //incase object require cutom effect, maybe have a way to store them in parent class instead?
	void rotate(const XMMATRIX& rotationMX);
	void translate(const XMMATRIX& translateMX);
	void loadmodel(const std::wstring& strFileName,const XMFLOAT4& color,bool Bumpmap);
	void loadSphere(const XMFLOAT4& color,float size);
	void loadSphere(const XMFLOAT4& color,float size, const XMFLOAT3& custom_pos); //override with custom sphere pos
	void scale();
	XMFLOAT4X4 get_matrix() const;
	//virtual void update();
	//void bind_effect(ID3DX11EffectTechnique *_tech);

	virtual ~Scene_object(void);
	virtual void initialise(Scene_manager* _manager,XMFLOAT3& _position,ID3D11DeviceContext *_dxDC, ID3D11Device *_dxD);
	XMFLOAT3 get_position() const;

private:
	vertex_index_buffer VIbuffer;
	ID3D11Device *_dxDev;
	ID3D11DeviceContext *_dxImmedDC;
	object_buffer objbuff;
	ID3D11Buffer *Buffer; 
	Scene_manager* manager;
	shaders shader;
	XMFLOAT3 position;
	XMFLOAT4X4 modelMX;
	//Material mat;
	
	
protected:
	void destroy();
	void set_buffer(const vertex_index_buffer& input);
	void set_position(const XMFLOAT3& input);
	void set_ModelMX(const XMFLOAT4X4& input);
	void set_mat(const Material& input);
	void set_wvp(const XMMATRIX& wvp);


	void draw();
	void render(vertex_index_buffer* custom_VI);
	void set_active(int pass);
	void set_active(ID3DX11EffectTechnique* custom_effect,int pass);

	void enable_blend();
	void disable_blend();
	void enable_CW();
	void enable_CCW();

	void enable_depthStateLess();
	void enable_depthEqual();
	void enable_depthShadow();

	Scene_manager* get_manager() const;
	ID3D11Device* get_DXDevice() const;
	ID3D11DeviceContext* get_DeviceContext() const;
	ID3DX11EffectTechnique* get_technique() const;
	const Material& get_material() const;
	const object_buffer& get_objBuff()const;
	ID3D11Buffer* get_buffer() const;
	const shaders& get_shader() const;
	const vertex_index_buffer& get_VI_Buffer()const;
	float get_blendfactoer() const;
};

