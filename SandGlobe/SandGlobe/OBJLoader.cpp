#include "OBJLoader.h"


OBJLoader::OBJLoader(void)
{
}


OBJLoader::~OBJLoader(void)
{
	try
	{
		destroy();
	}
	catch(...)
	{

	}
}

void OBJLoader::destroy()
{
	vi_list.Indicies_list.clear();
	vi_list.Vertex_list.clear();
	Ver_cache.clear();
}

vertex_index_buffer OBJLoader::LoadGeometryFromOBJ( const std::wstring& strFileName , ID3D11Device* dx_dev,const XMFLOAT4& color,bool BumpMap )
{
	ifstream fin;
	std::wifstream filein(strFileName);
	//fin.open(strFileName);
	char txtin;
	//if(fin.fail() == true)
	//{
	//	return false;
	//} //if can't open return false
	 WCHAR strCommand[256] = {0};
	std::vector<XMFLOAT3> vertices;
	std::vector<XMFLOAT2> uvs;
	std::vector<XMFLOAT3> norm;

	while(filein)
	{
		filein >> strCommand;
		if( 0 == wcscmp( strCommand, L"#" ) )
        {
            // Comment
        }
		else if(0 == wcscmp( strCommand, L"v"))  // v -0.395825 0.436485 9.94025e-009
		{
				XMFLOAT3 v;
				filein >> v.x >> v.y >> v.z;
				vertices.push_back(v);
		}
		else if(0 == wcscmp( strCommand, L"vt" )) //vt 0.411765 0.170213 - uv corrdinates
		{
				XMFLOAT2 vt;
				filein >> vt.x >>vt.y;
				uvs.push_back(vt);
		}
		else if( 0 == wcscmp( strCommand, L"vn" ))//vn 0.00444893 -0.966585 0.25631 - normals
		{
				XMFLOAT3 n;
				filein >>n.x>>n.y>>n.z;
				norm.push_back(n);
		}	
		else if( 0 == wcscmp( strCommand, L"f" ) )
		{
			UINT iPosition, iTexCoord, iNormal;
			Vertex vertex;
			for( UINT iFace = 0; iFace < 3; iFace++ )
			{
				ZeroMemory( &vertex, sizeof( Vertex ) );

				// OBJ format uses 1-based arrays
				filein >> iPosition;
				vertex.position = vertices[ iPosition - 1 ];

				if( '/' == filein.peek() )
				{
					filein.ignore();

					if( '/' != filein.peek() )
					{
						// Optional texture coordinate
						filein >> iTexCoord;
						vertex.tex = uvs[ iTexCoord - 1 ];
					}

					if( '/' == filein.peek() )
					{
						filein.ignore();

						// Optional vertex normal
						filein >> iNormal;
						vertex.normal = norm[ iNormal - 1 ];
					}
				}

				// If a duplicate vertex doesn't exist, add this vertex to the Vertices
				// list. Store the index in the Indices array. The Vertices and Indices
				// lists will eventually become the Vertex Buffer and Index Buffer for
				// the mesh.
				vertex.color = color;
				DWORD index = addvertice( iPosition, &vertex );
				/* if ( index == (DWORD)-1 )
				return E_OUTOFMEMORY;*/

				vi_list.Indicies_list.push_back( index );
			}
		}
	}
	if(BumpMap)
	{
		processBumpMap(vi_list.Vertex_list);
	}

	vertex_index_buffer vibuffer;
	D3D11_BUFFER_DESC vbd;
    vbd.ByteWidth =  sizeof(Vertex) * vi_list.Vertex_list.size();
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    vbd.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &vi_list.Vertex_list[0];
    vinitData.SysMemPitch = 0;
    vinitData.SysMemSlicePitch = 0;
	dx_dev->CreateBuffer(&vbd, &vinitData, &vibuffer.vbuf_);
	
	vibuffer.index_amount = vi_list.Indicies_list.size();
    D3D11_BUFFER_DESC ibd;
    ibd.ByteWidth = sizeof(DWORD) * vi_list.Indicies_list.size();
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    ibd.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &vi_list.Indicies_list[0];
    iinitData.SysMemPitch = 0;
    iinitData.SysMemSlicePitch = 0;
    dx_dev->CreateBuffer(&ibd, &iinitData, &vibuffer.ibuf_);
	return vibuffer;
}

void OBJLoader::processBumpMap(std::vector<Vertex>& Vertex_list)
{
	int faceCount, i, index;
	Vertex vertex1, vertex2, vertex3;
	XMFLOAT3 tangent, binormal, normal;

	faceCount = Vertex_list.size() / 3;

	index = 0;

	for(i=0; i<faceCount; i++)
	{
		// Get the three vertices for this face from the model.
		vertex1.position.x = Vertex_list[index].position.x;
		vertex1.position.y = Vertex_list[index].position.y;
		vertex1.position.z = Vertex_list[index].position.z;
		vertex1.tex.x = Vertex_list[index].tex.x;
		vertex1.tex.y = Vertex_list[index].tex.y;
		vertex1.normal.x = Vertex_list[index].normal.x;
		vertex1.normal.y = Vertex_list[index].normal.y;
		vertex1.normal.z = Vertex_list[index].normal.z;
		index++;

		vertex2.position.x = Vertex_list[index].position.x;
		vertex2.position.y = Vertex_list[index].position.y;
		vertex2.position.z = Vertex_list[index].position.z;
		vertex2.tex.x = Vertex_list[index].tex.x;
		vertex2.tex.y = Vertex_list[index].tex.y;
		vertex2.normal.x = Vertex_list[index].normal.x;
		vertex2.normal.y = Vertex_list[index].normal.y;
		vertex2.normal.z = Vertex_list[index].normal.z;
		index++;

		vertex3.position.x = Vertex_list[index].position.x;
		vertex3.position.y = Vertex_list[index].position.y;
		vertex3.position.z = Vertex_list[index].position.z;
		vertex3.tex.x = Vertex_list[index].tex.x;
		vertex3.tex.y = Vertex_list[index].tex.y;
		vertex3.normal.x = Vertex_list[index].normal.x;
		vertex3.normal.y = Vertex_list[index].normal.y;
		vertex3.normal.z = Vertex_list[index].normal.z;
		index++;


		CalculateTangentBinormal(vertex1, vertex2, vertex3, tangent,binormal);
		//CalculateNormal(tangent, binormal, normal);

	
		//Vertex_list[index-1].normal.x = normal.x;
		//Vertex_list[index-1].normal.y = normal.y;
		//Vertex_list[index-1].normal.z = normal.z;
		Vertex_list[index-1].tangent.x = tangent.x;
		Vertex_list[index-1].tangent.y = tangent.y;
		Vertex_list[index-1].tangent.z = tangent.z;
		Vertex_list[index-1].binormal.x = binormal.x;
		Vertex_list[index-1].binormal.y = binormal.y;
		Vertex_list[index-1].binormal.z = binormal.z;

		//Vertex_list[index-2].normal.x = normal.x;
		//Vertex_list[index-2].normal.y = normal.y;
		//Vertex_list[index-2].normal.z = normal.z;
		Vertex_list[index-2].tangent.x = tangent.x;
		Vertex_list[index-2].tangent.y = tangent.y;
		Vertex_list[index-2].tangent.z = tangent.z;
		//Vertex_list[index-2].binormal.x = binormal.x;
		//Vertex_list[index-2].binormal.y = binormal.y;
		//Vertex_list[index-2].binormal.z = binormal.z;

		/*Vertex_list[index-3].normal.x = normal.x;
		Vertex_list[index-3].normal.y = normal.y;
		Vertex_list[index-3].normal.z = normal.z;*/
		Vertex_list[index-3].tangent.x = tangent.x;
		Vertex_list[index-3].tangent.y = tangent.y;
		Vertex_list[index-3].tangent.z = tangent.z;
	/*	Vertex_list[index-3].binormal.x = binormal.x;
		Vertex_list[index-3].binormal.y = binormal.y;
		Vertex_list[index-3].binormal.z = binormal.z;
		*/
	}
}

void OBJLoader::CalculateTangentBinormal(const Vertex& vertex1,const Vertex& vertex2,const Vertex& vertex3,XMFLOAT3& tangent, XMFLOAT3& binormal) const
{
	float Cvertex1[3], Cvertex2[3];
	float texx[2], texy[2];
	float den;
	float length;


	// Calculate the two vectors for this face.
	Cvertex1[0] = vertex2.position.x - vertex1.position.x;
	Cvertex1[1] = vertex2.position.y - vertex1.position.y;
	Cvertex1[2] = vertex2.position.z - vertex1.position.z;

	Cvertex2[0] = vertex3.position.x - vertex1.position.x;
	Cvertex2[1] = vertex3.position.y - vertex1.position.y;
	Cvertex2[2] = vertex3.position.z - vertex1.position.z;

	// Get tex cord
	texx[0] = vertex2.tex.x - vertex1.tex.x;
	texy[0] = vertex2.tex.y - vertex1.tex.y;

	texx[1] = vertex3.tex.x - vertex1.tex.x;
	texy[1] = vertex3.tex.y - vertex1.tex.y;

	// Calculate the denominator 
	den = 1.0f / (texx[0] * texy[1] - texx[1] * texy[0]);

	// Calculate tangent
	tangent.x = (texy[1] * Cvertex1[0] - texy[0] * Cvertex2[0]) * den;
	tangent.y = (texy[1] * Cvertex1[1] - texy[0] * Cvertex2[1]) * den;
	tangent.z = (texy[1] * Cvertex1[2] - texy[0] * Cvertex2[2]) * den;
	

	//CURRENTLY NOT USED
	binormal.x = (texx[0] * Cvertex2[0] - texx[1] * Cvertex1[0]) * den;
	binormal.y = (texx[0] * Cvertex2[1] - texx[1] * Cvertex1[1]) * den;
	binormal.z = (texx[0] * Cvertex2[2] - texx[1] * Cvertex1[2]) * den;

	length = sqrt((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));
	tangent.x = tangent.x / length;
	tangent.y = tangent.y / length;
	tangent.z = tangent.z / length;


	return;

}

//void OBJLoader::CalculateNormal(const XMFLOAT3& tangent,const XMFLOAT3& binormal, XMFLOAT3& normal)
//{
//	
//	//CURRENTLY NOT USED
//	float length;
//
//
//	// Calculate the cross product of the tangent and binormal
//	normal.x = (tangent.y * binormal.z) - (tangent.z * binormal.y);
//	normal.y = (tangent.z * binormal.x) - (tangent.x * binormal.z);
//	normal.z = (tangent.x * binormal.y) - (tangent.y * binormal.x);
//
//	length = sqrt((normal.x * normal.x) + (normal.y * normal.y) + (normal.z * normal.z));
//
//	// Normalize
//	normal.x = normal.x / length;
//	normal.y = normal.y / length;
//	normal.z = normal.z / length;
//
//	return;
//}


DWORD OBJLoader::addvertice(UINT hash, Vertex* pVertex)
{
	bool bFoundInList = false;
	DWORD index = 0;
	if( ( UINT )Ver_cache.size() > hash )
	{
		CacheEntry* pEntry = Ver_cache[hash];
		while( pEntry != NULL )
		{
			Vertex  v = vi_list.Vertex_list[pEntry->index];
			Vertex* pCacheVertex = (Vertex*)&v;
			if( 0 == memcmp( pVertex, pCacheVertex, sizeof( Vertex ) ) )
			{
				bFoundInList = true;
				index = pEntry->index;
				break;
			}
			pEntry = pEntry->pNext;
		}
	}

	if( !bFoundInList )
	{
		index = (UINT)vi_list.Vertex_list.size();
		vi_list.Vertex_list.push_back(*pVertex);
		CacheEntry* pNewEntry = new CacheEntry;
		if( pNewEntry == NULL )
			return (DWORD)-1;

		pNewEntry->index = index;
		pNewEntry->pNext = NULL;
		while( ( UINT )Ver_cache.size() <= hash )
		{
			Ver_cache.push_back(NULL);
		}

		CacheEntry* pCurEntry = Ver_cache[hash];
		if( pCurEntry == NULL )
		{
			// This is the head element
			Ver_cache.push_back(pNewEntry);
		}
		else
		{
			// Find the tail
			while( pCurEntry->pNext != NULL )
			{
				pCurEntry = pCurEntry->pNext;
			}

			pCurEntry->pNext = pNewEntry;
		}
	}
	return index;
}
