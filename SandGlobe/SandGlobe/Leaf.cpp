#include "Leaf.h"


Leaf::Leaf(void)
{
	burning = NULL;
	current_length = NULL;
	grow_finished = NULL;//all these varibles will be assaigned later in setup
	angle = NULL;
	rot = NULL;
	length = NULL;
}


Leaf::~Leaf(void)
{
}




void Leaf::setup(const XMFLOAT3& _pos,float rotation,float _length,float worldangle)
{
	pos = _pos;
	rot = rotation;
	current_length = 0;
	grow_finished = false;
	float rad = -rotation *XM_PI/180;
	length = _length;
	angle = worldangle;
}

void Leaf::update(double dt,bool _grow)
{
	
	if(_grow)
	{
		grow(dt);
	}
	float rad = -rot *XM_PI/180;
	//float rad = -rotation *XM_PI/180;
	//length = _length;
	/*XMMATRIX modelMXtemp = XMMatrixIdentity();
	XMMATRIX transMx = XMMatrixTranslation(pos.x,pos.y,pos.z);
	XMVECTOR rotaxis = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX rotMx = XMMatrixRotationAxis(rotaxis,rad);
	XMVECTOR rotworld = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	float radworld= angle*XM_PI/180;
	
	if(rot>360)
	{
		XMVECTOR rotaxis2 = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		float rad_flip = 215*XM_PI/180;
		XMMATRIX rotMx2 = XMMatrixRotationAxis(rotaxis2,rad_flip);
		rotMx *=rotMx2;
	}
	else
	{
		XMVECTOR rotaxis2 = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		float rad_flip = -25*XM_PI/180;
		XMMATRIX rotMx2 = XMMatrixRotationAxis(rotaxis2,rad_flip);
		rotMx *=rotMx2;
	}
	
	XMMATRIX rotworldMx = XMMatrixRotationAxis(rotworld,radworld);
	
	modelMXtemp *= rotMx*transMx;



	modelMXtemp.r[0] = XMVectorScale(modelMXtemp.r[0],0);
	modelMXtemp *=rotworldMx;
	XMStoreFloat4x4(&modelMX,modelMXtemp);*/
	XMMATRIX transMx = XMMatrixTranslation(pos.x,pos.y,pos.z);
	//XMMATRIX transMx = XMMatrixTranslation(2,0,0);
	XMVECTOR rotaxis = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX rotMx = XMMatrixRotationAxis(rotaxis,rad);
	XMVECTOR rotworld = XMVectorSet(0,1, 0, 0.0f);
	float radworld= angle*XM_PI/180;
	
	if(rot>360)
	{
		XMVECTOR rotaxis2 = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		float rad_flip = 215*XM_PI/180;
		XMMATRIX rotMx2 = XMMatrixRotationAxis(rotaxis2,rad_flip);
		rotMx *=rotMx2;
	}
	else
	{
		XMVECTOR rotaxis2 = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		float rad_flip = -25*XM_PI/180;
		XMMATRIX rotMx2 = XMMatrixRotationAxis(rotaxis2,rad_flip);
		rotMx *=rotMx2;
	}
	XMMATRIX scaleMx = XMMatrixScaling( 1 + current_length , 1,1 );
	XMMATRIX modelMXtemp2 = XMMatrixIdentity();
	XMMATRIX modelMXtemp = XMMatrixIdentity();
	XMMATRIX rotworldMx = XMMatrixRotationAxis(rotworld,radworld);
	modelMXtemp = scaleMx*rotMx*transMx*rotworldMx;
	//modelMXtemp2 = transMx*rotworldMx;
	//modelMXtemp *= modelMXtemp2;
	//modelMXtemp *= transMx;
	//modelMXtemp *= rotworldMx;
	//modelMXtemp = ;

	//modelMXtemp *=;
	
	



	
	//modelMXtemp *=
	XMStoreFloat4x4(&modelMX,modelMXtemp);
}


XMFLOAT3 Leaf::return_pos() const
{
	return pos;
}

void Leaf::grow(double dt)
{
	if(!grow_finished)
	{
		if(current_length < length)
		{
			current_length += 1.5*dt;
		}
		else
		{
			current_length = length; //incase it goes further than desired length
			grow_finished = true;
		}
	}
}

float Leaf::return_angle() const
{
	return angle;
}
bool Leaf::is_grow_finish() const
{
	return grow_finished;
}
 
XMFLOAT4X4 Leaf::returnMX() const
{
	return modelMX;
}