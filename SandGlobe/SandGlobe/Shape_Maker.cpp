#include "Shape_Maker.h"


Shape_Maker::Shape_Maker(void)
{
}


Shape_Maker::~Shape_Maker(void)
{
}

vertex_index_buffer Shape_Maker::loadobj(const std::wstring& strFileName, ID3D11Device* dx_dev,const XMFLOAT4& color,bool Bumpmap) const
{
	OBJLoader loader;
	XMFLOAT4 col = color;
	return loader.LoadGeometryFromOBJ(strFileName,dx_dev,col,Bumpmap);
}

vertex_index_buffer Shape_Maker::make_globe(const XMFLOAT4& color, ID3D11Device* dx_dev, float size,const XMFLOAT3& position)
{
	int nRings = 50;
	int nSegments = 50;
	BufferList blist; 
	vertex_index_buffer result;

	DWORD dwVertices = ( nRings + 1 ) * ( nSegments + 1 ) ;
	DWORD dwIndices = 2 * nRings * ( nSegments + 1 ) ;

	FLOAT fDeltaRingAngle = (   XM_PI / nRings );
	FLOAT fDeltaSegAngle = ( 2* XM_PI / nSegments );

	WORD wVerticeIndex = 0 ; 

	for( int ring = 0; ring < nRings +1 ; ring++ )
	{
		FLOAT r0 =  sinf ( ring * fDeltaRingAngle );
		FLOAT y0 = size*  cosf ( ring * fDeltaRingAngle );

		for( int seg = 0; seg < nSegments + 1 ; seg++ )
		{
			FLOAT x0 = size* r0 * sinf( seg * fDeltaSegAngle );
			FLOAT z0 = size* r0 * cosf( seg * fDeltaSegAngle );
			Vertex v;
			XMVECTOR vpos = XMVectorSet(x0+position.x,y0+position.y,z0+position.z,0);
			XMVECTOR nor = XMVector3Normalize(vpos);
			XMStoreFloat3(&v.position,vpos);
			XMStoreFloat3(&v.normal,nor);
			v.color = color;
			//v.tex(((FLOAT) seg / (FLOAT) nSegments),((FLOAT) ring / (FLOAT) nRings));
			v.tex = XMFLOAT2(((FLOAT) seg / (FLOAT) nSegments),((FLOAT) ring / (FLOAT) nRings));
			blist.Vertex_list.push_back(v);
			//if ( ring != nRings ) 
			//{
			/*result.Indicies_list.push_back(wVerticeIndex) ; 
			result.Indicies_list.push_back(wVerticeIndex + ( WORD ) ( nSegments + 1 ));
			result.Indicies_list.push_back(wVerticeIndex + ( WORD ) ( nSegments + 2 ));*/

			blist.Indicies_list.push_back(wVerticeIndex + nSegments + 1);
			blist.Indicies_list.push_back(wVerticeIndex);               
			blist.Indicies_list.push_back(wVerticeIndex + nSegments);
			blist.Indicies_list.push_back(wVerticeIndex + nSegments + 1);
			blist.Indicies_list.push_back(wVerticeIndex + 1);
			blist.Indicies_list.push_back(wVerticeIndex);
			/*result.Indicies_list.push_back(wVerticeIndex+ 1) ; 
			result.Indicies_list.push_back(wVerticeIndex+ 1 + ( WORD ) ( nSegments + 1 ));
			result.Indicies_list.push_back(wVerticeIndex+ 1 + ( WORD ) ( nSegments + 2 ));*/
			wVerticeIndex ++ ; 
			//} ; 
		}
	}

	D3D11_BUFFER_DESC vbd;
	vbd.ByteWidth = sizeof(Vertex) * blist.Vertex_list.size();
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &blist.Vertex_list[0];
	vinitData.SysMemPitch = 0;
	vinitData.SysMemSlicePitch = 0;

	dx_dev->CreateBuffer(&vbd, &vinitData, &result.vbuf_);

	result.index_amount = blist.Indicies_list.size();
	D3D11_BUFFER_DESC ibd;
	ibd.ByteWidth = sizeof(DWORD) * blist.Indicies_list.size();
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem =  &blist.Indicies_list[0];
	iinitData.SysMemPitch = 0;
	iinitData.SysMemSlicePitch = 0;

	dx_dev->CreateBuffer(&ibd, &iinitData, &result.ibuf_);

	return result;
}