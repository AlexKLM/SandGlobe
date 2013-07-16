#pragma once
#include "Global.h"
#include <xnamath.h>
#include "OBJLoader.h"

class Shape_Maker
{
public:
	vertex_index_buffer make_globe(const XMFLOAT4& color, ID3D11Device* dx_dev, float size,const XMFLOAT3& position);
	vertex_index_buffer loadobj(const std::wstring& strFileName, ID3D11Device* dx_dev,const XMFLOAT4& color,bool Bumpmap) const;
	Shape_Maker(void);
	~Shape_Maker(void);
};

