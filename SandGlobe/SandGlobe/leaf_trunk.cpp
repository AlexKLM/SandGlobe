#include "leaf_trunk.h"
#include "Scene_manager.h"

leaf_trunk::leaf_trunk(void)
{
	manager = NULL;
	Buffer = NULL;
	_dxDev = NULL;
	_dxImmedDC = NULL;
	leaf_gen = NULL;
	fullyburning = NULL;
	current_length = NULL;
	grow_finished = NULL;
	leaf_grow_finished = NULL;
	trunk_length = NULL;
	world_angle = NULL;
	leaf_steps = 0;
	num_of_leaves = 0;
	leaf_instance_buffer = NULL;
}


leaf_trunk::~leaf_trunk(void)
{
	try
	{
		destroy();
	}
	catch(...)
	{

	}
}
 

void leaf_trunk::destroy()
{
	if(leaf_instance_buffer != NULL)
	{
			leaf_instance_buffer->Release();
	}
	if(Buffer != NULL)
	{
		Buffer->Release();
	}


}
void leaf_trunk::initialise(Scene_manager* _manager,XMFLOAT3& _position,ID3D11DeviceContext *_dxDC, ID3D11Device *_dxD,const XMFLOAT3& _treetoppos, leaf_generator* _leafgen)
{
	manager = _manager;
	leaf_gen = _leafgen;
	leaf_grow_finished = false;
	grow_finished = false;

	_dxDev = _dxD;
	tree_pos = _treetoppos;
	position = _position;
	_dxImmedDC = _dxDC;
	XMMATRIX tempModelMX = XMMatrixIdentity();
	XMMATRIX translatematrix = XMMatrixTranslation(_position.x,_position.y,_position.z);
	tempModelMX *= translatematrix;
	XMStoreFloat4x4(&modelMX,tempModelMX);

}

void leaf_trunk::initialise_buffers()
{
	D3D11_BUFFER_DESC constDesc;
	ZeroMemory(&constDesc, sizeof(constDesc));
	constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constDesc.ByteWidth = (sizeof(leaf_buff));
	constDesc.Usage = D3D11_USAGE_DEFAULT;
	_dxDev->CreateBuffer(&constDesc, 0, &Buffer);
}

void leaf_trunk::update(double dt,bool _grow)
{
	int i = 0;
	
	if(_grow && !grow_finished)
	{
		grow(dt);
		leaf_grow_finished = false;
	}
	else if(grow_finished)
	{
		leaf_grow_finished = true;
		for(int i = 0; i < leaf_list.size();i++)
		{

			if(!leaf_list[i].is_grow_finish())
			{
				leaf_grow_finished = false;
				leaf_list[i].update(dt,true);
			}

			/*if(!leaf_grow_finished)
			{
			leaf_grow_finished = true;
			if(!leaf_list[i].is_grow_finish())
			{
			leaf_grow_finished = false;
			leaf_list[i].update(dt,false,false);
			}
			}*/
			leaf_instances_data[i].modelMX = leaf_list[i].returnMX();
		}
	}
	XMMATRIX transMx = XMMatrixTranslation(tree_pos.x,tree_pos.y,tree_pos.z);
	XMMATRIX modelMXtemp = XMMatrixIdentity();
	XMVECTOR rotworld = XMVectorSet(0,1, 0, 0.0f);
	float radworld= world_angle*XM_PI/180;
	XMMATRIX rotworldMx = XMMatrixRotationAxis(rotworld,radworld);
	XMMATRIX scaleMx = XMMatrixScaling( 1, 1, current_length );
	modelMXtemp *= scaleMx;
	modelMXtemp *= rotworldMx*transMx;
	XMStoreFloat4x4(&modelMX,modelMXtemp);
}

void leaf_trunk::create_instance_buffer()
{
	D3D11_BUFFER_DESC instBuffDesc;	
	ZeroMemory( &instBuffDesc, sizeof(instBuffDesc) );

	instBuffDesc.Usage = D3D11_USAGE_DEFAULT;
	instBuffDesc.ByteWidth = sizeof( instanceData ) * num_of_leaves;
	instBuffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instBuffDesc.CPUAccessFlags = 0;
	instBuffDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA instData;
	ZeroMemory( &instData, sizeof(instData) );

	instData.pSysMem = &leaf_instances_data[0];
	HRESULT hr;
	hr = _dxDev->CreateBuffer( &instBuffDesc, &instData, &leaf_instance_buffer);
}

void leaf_trunk::render_leafs(const vertex_index_buffer& leaf_buffer,const XMMATRIX& viewMX,const XMMATRIX& projMX, shaders& _sh)
{
	ID3D11Buffer* lightbufferptr = manager->return_PL_Buffer();
	//XMMATRIX mx = modelMX*viewMX*projMX;
	XMStoreFloat4x4(&leave_vp_buffer.viewMx,XMMatrixTranspose(viewMX));
	XMStoreFloat4x4(&leave_vp_buffer.projMx,XMMatrixTranspose(projMX));
	_dxImmedDC->UpdateSubresource(leaf_instance_buffer, 0, 0, &leaf_instances_data[0],0, 0);
	_sh.set_active(leaf_buffer,leaf_instance_buffer,1);
	manager->disable_blend();
	manager->enable_depthStateLess();
	//_dxImmedDC->OMSetBlendState(0, 0, 0xffffffff);
	_dxImmedDC->UpdateSubresource(lightbufferptr, 0, 0, &manager->get_p_light(),0, 0);
	_dxImmedDC->PSSetConstantBuffers(1,1,&lightbufferptr);
	//_dxImmedDC->OMSetDepthStencilState(manager->m_depthStencilStateLess, 1);
	_dxImmedDC->UpdateSubresource(Buffer, 0, 0, &leave_vp_buffer,0, 0);
	_dxImmedDC->VSSetConstantBuffers(0,1,&Buffer);

	//_dxImmedDC->DrawIndexed(leaf_buffer.index_amount, 0, 0);
	_dxImmedDC->DrawIndexedInstanced(leaf_buffer.index_amount,num_of_leaves,0,0,0);
}

void leaf_trunk::p_generate_leafs()
{
	std::string result;
	//srand ( time(NULL) );
	result = "";

	//leaf_gen->generate_leaf(rand() %20 +5,result);
	leaf_gen->generate(rand() %20 +5,result);
	int leaf_num = 0;
	for (int i = 0; i < result.size(); i++)
	{
		char chr = result[i];
		if(chr == 'B')
		{
			leaf_num++;
		}
	}
	world_angle = rand() %360;
	float start_length = rand() % 1 + 0.3f;
	float rotate_step = 45;
	float length_step = 0.1;
	float angle = 0;
	float stored_angle = 0;
	float leaf_length = 10;
	leaf_steps = leaf_length/leaf_num;
	
	trunk_length = start_length;
	float stored_length = angle;//for bracketed L-system
	for (int i = 0; i < result.size(); i++)
	{
		char chr = result[i];
		switch(chr)
		{
			case 'F':
				trunk_length+=length_step;
				break;
			case 'S':
				leaf_length-=leaf_steps;
				break;
			case '[':
				stored_angle = angle;
				stored_length = trunk_length;
				break;
			case ']':
				angle = stored_angle;
				trunk_length = stored_length;
				break;
			case '-':
				angle-=rotate_step;
				if(angle <0)
				{
					angle = 360-angle;
				}
				break;
			case '+':
				angle+=rotate_step;
				break;
			case 'B':
				create_leaf(XMFLOAT3(tree_pos.x,tree_pos.y,tree_pos.z + trunk_length),angle,leaf_length,world_angle);
				break;
		}
	}
	//generate leafs here
	/*create_leaf(XMFLOAT3(0,0,0),0,1);
	create_leaf(XMFLOAT3(1,1,1),0,1);
	create_leaf(XMFLOAT3(2,2,2),0,1);
	create_leaf(XMFLOAT3(3,3,3),0,1);*/
	current_length = 0;
	XMMATRIX transMx = XMMatrixTranslation(tree_pos.x,tree_pos.y,tree_pos.z);
	XMMATRIX scaleMx = XMMatrixScaling( 1, 1, current_length );
	create_instance_buffer();
	XMMATRIX tempModelMX = XMMatrixIdentity();
	XMVECTOR rotworld = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	float radworld= world_angle*XM_PI/180;
	XMMATRIX rotworldMx = XMMatrixRotationAxis(rotworld,radworld);
	tempModelMX *= scaleMx;
	tempModelMX *= rotworldMx*transMx;

	XMStoreFloat4x4(&modelMX,tempModelMX);
}

void leaf_trunk::Render(const vertex_index_buffer& trunk_buffer,const XMMATRIX& viewMX,const XMMATRIX& projMX, shaders& _sh)
{
	XMMATRIX tempModelMX = XMLoadFloat4x4(&modelMX);
	XMMATRIX mx = tempModelMX*viewMX*projMX;
	XMMATRIX wvpMX = XMMatrixTranspose(mx);
	objbuff.worldViewProj = XMFLOAT4X4();
	XMStoreFloat4x4(&objbuff.worldViewProj,wvpMX);
	_sh.set_active(trunk_buffer,1);
	ID3D11Buffer* lightbufferptr = manager->return_PL_Buffer();
	_dxImmedDC->UpdateSubresource(lightbufferptr, 0, 0, &manager->get_p_light(),0, 0);
	_dxImmedDC->PSSetConstantBuffers(1,1,&lightbufferptr);
	manager->disable_blend();
	manager->enable_depthStateLess();

	_dxImmedDC->UpdateSubresource(Buffer, 0, 0, &objbuff,0, 0);
	_dxImmedDC->VSSetConstantBuffers(0,1,&Buffer);

	_dxImmedDC->DrawIndexed(trunk_buffer.index_amount, 0, 0);
	lightbufferptr->Release();
}

void leaf_trunk::render_Shadow(const vertex_index_buffer& trunk_buffer,const XMMATRIX& lightVPMx,const XMFLOAT3& lightpos, shaders& shadow_shader, ID3D11Buffer* shadowBuff )
{

	XMMATRIX tempModelMX = XMLoadFloat4x4(&modelMX);
		XMStoreFloat4x4(&shadow_data.worldMx,XMMatrixTranspose(tempModelMX));
	shadow_data.lPos = lightpos;
	XMStoreFloat4x4(&shadow_data.lightViewProj,lightVPMx);
	shadow_shader.set_active(trunk_buffer,0);
	_dxImmedDC->UpdateSubresource(shadowBuff, 0, 0, &shadow_data,0, 0);
	_dxImmedDC->VSSetConstantBuffers(0,1,&shadowBuff);
	_dxImmedDC->DrawIndexed(trunk_buffer.index_amount, 0, 0);
}

void leaf_trunk::render_Shadow_leafs(const vertex_index_buffer& leaf_buffer,const XMMATRIX& lightVPMx,const XMFLOAT3& lightpos, shaders& shadow_shader, ID3D11Buffer* shadowBuff )
{
	
	XMMATRIX tempModelMX = XMLoadFloat4x4(&modelMX);
		XMStoreFloat4x4(&shadow_data.worldMx,XMMatrixTranspose(tempModelMX));
	shadow_data.lPos = lightpos;
	XMStoreFloat4x4(&shadow_data.lightViewProj,lightVPMx);
	_dxImmedDC->UpdateSubresource(leaf_instance_buffer, 0, 0, &leaf_instances_data[0],0, 0);
	shadow_shader.set_active(leaf_buffer,leaf_instance_buffer,1);
	_dxImmedDC->UpdateSubresource(shadowBuff, 0, 0, &shadow_data,0, 0);
	_dxImmedDC->VSSetConstantBuffers(0,1,&shadowBuff);
	_dxImmedDC->DrawIndexedInstanced(leaf_buffer.index_amount,num_of_leaves,0,0,0);
}

void leaf_trunk::create_leaf(const XMFLOAT3& _pos,float rotation,float _length,float worldangle)
{
	Leaf leaf;
	leaf.setup(_pos,rotation,_length,worldangle);
	leaf_list.push_back(leaf);
	instanceData inst_d;
	inst_d.modelMX = leaf.returnMX();
	leaf_instances_data.push_back(inst_d);
	num_of_leaves++;
}

void leaf_trunk::render_leafs_lights(const vertex_index_buffer& leaf_buffer,const XMMATRIX& viewMX,const XMMATRIX& projMX, shaders& _sh,std::vector<pointlight>& lights)
{
	XMStoreFloat4x4(&leave_vp_buffer.viewMx,XMMatrixTranspose(viewMX));
	XMStoreFloat4x4(&leave_vp_buffer.projMx,XMMatrixTranspose(projMX));
	_dxImmedDC->UpdateSubresource(leaf_instance_buffer, 0, 0, &leaf_instances_data[0],0, 0);
	_sh.set_active(leaf_buffer,leaf_instance_buffer,0);

	ID3D11Buffer* lightbufferptr = manager->return_PL_Buffer();
	manager->disable_blend();
	_dxImmedDC->UpdateSubresource(lightbufferptr, 0, 0, &lights[0],0, 0);
	_dxImmedDC->PSSetConstantBuffers(1,1,&lightbufferptr);
	manager->enable_depthStateLess();
	_dxImmedDC->UpdateSubresource(Buffer, 0, 0, &leave_vp_buffer,0, 0);
	_dxImmedDC->VSSetConstantBuffers(0,1,&Buffer);

	_dxImmedDC->DrawIndexedInstanced(leaf_buffer.index_amount,num_of_leaves,0,0,0);


	manager->enable_blend();
	manager->enable_depthStateLess();
	
	for(int i = 1; i < lights.size(); i++)
	{
		_sh.set_active(leaf_buffer,leaf_instance_buffer,0);

		_dxImmedDC->UpdateSubresource(lightbufferptr, 0, 0, &lights[i],0, 0);
		_dxImmedDC->PSSetConstantBuffers(1,1,&lightbufferptr);

		_dxImmedDC->UpdateSubresource(Buffer, 0, 0, &leave_vp_buffer,0, 0);
		_dxImmedDC->VSSetConstantBuffers(0,1,&Buffer);
		_dxImmedDC->DrawIndexedInstanced(leaf_buffer.index_amount,num_of_leaves,0,0,0);
	}

}

void leaf_trunk::Render_lights(const vertex_index_buffer& trunk_buffer,const XMMATRIX& viewMX,const XMMATRIX& projMX, shaders& _sh,std::vector<pointlight>& lights)
{

	ID3D11Buffer* lightbufferptr = manager->return_PL_Buffer();

	XMMATRIX tempModelMX = XMLoadFloat4x4(&modelMX);
	XMMATRIX mx = tempModelMX*viewMX*projMX;
	XMMATRIX wvpMX = XMMatrixTranspose(mx);
	objbuff.worldViewProj = XMFLOAT4X4();
	XMStoreFloat4x4(&objbuff.worldViewProj,wvpMX);
	_sh.set_active(trunk_buffer,0);
	_dxImmedDC->UpdateSubresource(lightbufferptr, 0, 0, &lights[0],0, 0);
	_dxImmedDC->PSSetConstantBuffers(1,1,&lightbufferptr);
	manager->disable_blend();
	manager->enable_depthStateLess();

	_dxImmedDC->UpdateSubresource(Buffer, 0, 0, &objbuff,0, 0);
	_dxImmedDC->VSSetConstantBuffers(0,1,&Buffer);

	_dxImmedDC->DrawIndexed(trunk_buffer.index_amount, 0, 0);


	
	manager->disable_blend();
	manager->enable_depthStateLess();
	for(int i = 1; i < lights.size(); i++)
	{
		_sh.set_active(trunk_buffer,0);
		_dxImmedDC->UpdateSubresource(lightbufferptr, 0, 0, &lights[i],0, 0);
		_dxImmedDC->PSSetConstantBuffers(1,1,&lightbufferptr);

		_dxImmedDC->UpdateSubresource(Buffer, 0, 0, &objbuff,0, 0);
		_dxImmedDC->VSSetConstantBuffers(0,1,&Buffer);

		_dxImmedDC->DrawIndexed(trunk_buffer.index_amount, 0, 0);
	}

}

void leaf_trunk::grow(double dt)
{
	if(!grow_finished)
	{
		if(current_length < trunk_length)
		{
			current_length += dt;
		}
		else
		{
			current_length = trunk_length; //incase it goes further than desired length
			grow_finished = true;
		}
	}

}

bool leaf_trunk::is_trunk_grow_finish() const
{
	return grow_finished;
}

bool leaf_trunk::is_grow_process_finish() const
{
	return leaf_grow_finished;
}

float leaf_trunk::return_length() const
{
	return trunk_length;
}

float leaf_trunk::return_leaf_step() const
{
	return leaf_steps;
}

bool leaf_trunk::return_next_burn_pos(int cycle, XMFLOAT3& output)
{
	if(cycle > leaf_list.size()-1)
	{
		return false;
	}
	output = leaf_list[cycle].return_pos();
	float angle = leaf_list[cycle].return_angle();
	output.x = output.z * sin(angle * XM_PI / 180);
	//output.y = output.y;
	output.z = output.z * cos(angle * XM_PI / 180);

	return true;

}

XMFLOAT3 leaf_trunk::return_random_leaf_pos()
{
	int i = rand() %leaf_list.size()-1;
	return leaf_list[i].return_pos();
}

const std::vector<Leaf>& leaf_trunk::return_leaf_list() const
{
	return leaf_list;
}

const std::vector<instanceData>& leaf_trunk::return_leaf_instances_data() const
{
	return leaf_instances_data;
}