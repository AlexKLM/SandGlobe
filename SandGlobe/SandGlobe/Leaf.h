#pragma once
#include "Global.h"
#include <Windows.h>
#include <xnamath.h>

class Leaf
{
public:
	Leaf(void);
	~Leaf(void);
	void setup(const XMFLOAT3& _pos,float rotation,float _length,float worldangle);
	void grow(double dt);
	void update(double dt,bool _grow);
	bool is_grow_finish() const;
	bool is_burning() const;
	
	float return_angle() const;
	XMFLOAT3 return_pos() const;
	XMFLOAT4X4 returnMX() const;
private:
	bool burning;
	XMFLOAT4X4 modelMX;
	float current_length;
	bool grow_finished;
	float angle;
	XMFLOAT3 pos;
	float rot;
	float length;
};

