#pragma once
#include <fstream>

#include "Global.h"
#include <vector>
using namespace std;

struct CacheEntry
{
    UINT index;
    CacheEntry* pNext;
};

class OBJLoader
{
public:
	OBJLoader(void);
	~OBJLoader(void);
	void destroy();
	vertex_index_buffer LoadGeometryFromOBJ( const std::wstring& strFileName , ID3D11Device* dx_dev,const XMFLOAT4& color,bool BumpMap  );
	DWORD addvertice(UINT hash, Vertex* pVertex);
private:
	void processBumpMap(std::vector<Vertex>& Vertex_list);
	void CalculateTangentBinormal(const Vertex& vertex1,const Vertex& vertex2,const Vertex& vertex3,XMFLOAT3& tangent, XMFLOAT3&binormal) const;
	//void CalculateNormal(const XMFLOAT3& tangent,const XMFLOAT3& binormal, XMFLOAT3& normal);
	std::vector<CacheEntry*> Ver_cache;

	BufferList vi_list;
};

