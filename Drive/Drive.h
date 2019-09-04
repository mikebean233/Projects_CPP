////
//////
//////// CAMERA CLASS  ////////////////////////////////////////////////////////////////////
class CAMERA 
{
public:
D3DXMATRIX              matView;
D3DXMATRIX              matProj;
D3DXVECTOR3             CamPos;
D3DXVECTOR3             TargetPos;

};
void ResMat();


////
//////
//////// MESH CLASS  //////////////////////////////////////////////////////////////////////
class MESH
{
public:
LPD3DXMESH              Mesh;
D3DMATERIAL8*           Materials;
LPDIRECT3DTEXTURE8*     Textures;
DWORD                   NumMaterials;
~MESH(){
	   if( Materials != NULL ) 
       delete[] Materials;
       if(Mesh != NULL )
          Mesh->Release();
	   if( Textures )
			{
            for( DWORD i = 0; i < NumMaterials; i++ )
				{
                if( Textures[i] )
                Textures[i]->Release();
				}
            delete[] Textures;
			}
		}
void LoadXFile(char* Filename)
	{ 
	//D3DXMESHSIMP MeshSimpl;
	//MeshSimpl.
	LPD3DXBUFFER pD3DXMtrlBuffer;

	D3DXLoadMeshFromX(Filename, 
				  D3DXMESH_MANAGED, 
                  Device, 
				  NULL, 
                  &pD3DXMtrlBuffer, 
				  &NumMaterials, 
                  &Mesh );

	D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();    
	Materials = new D3DMATERIAL8[NumMaterials];
	Textures  = new LPDIRECT3DTEXTURE8[NumMaterials];
	for( DWORD i=0; i<NumMaterials; i++ )
		{
        Materials[i] = d3dxMaterials[i].MatD3D;
		Materials[i].Ambient.r = Materials[i].Diffuse.r * 0.5f;
		Materials[i].Ambient.g = Materials[i].Diffuse.g * 0.5f;
		Materials[i].Ambient.b = Materials[i].Diffuse.b * 0.5f;
		Materials[i].Ambient.a = 0.3f;
		Materials[i].Specular.r = 1.0f;
		Materials[i].Specular.g = 1.0f;
		Materials[i].Specular.b = 1.0f;
        Materials[i].Specular.a = 1.0f;
		Materials[i].Diffuse.r *= 0.7f;
		Materials[i].Diffuse.g *= 0.7f;
	    Materials[i].Diffuse.b *= 0.7f;
		Materials[i].Diffuse.a = 1.0f;
	    //Materials[i].Emissive.r = 0.0f;
		//Materials[i].Emissive.g = 0.0f;
		//Materials[i].Emissive.b = 0.0f;
		//Materials[i].Emissive.a = 1.0f;
		Materials[i].Power = 20.0f;
		if( FAILED( D3DXCreateTextureFromFile( Device, 
                                               d3dxMaterials[i].pTextureFilename, 
                                               &Textures[i] 
			         /*D3DXCreateTextureFromFileEx(Device,
												 d3dxMaterials[i].pTextureFilename,
												 D3DX_DEFAULT, 
												 D3DX_DEFAULT, 
												 D3DX_DEFAULT,
												 D3DPOOL_MANAGED,
												 D3DFMT_A8R8G8B8,
												 D3DPOOL_MANAGED,
												 D3DX_DEFAULT,
												 D3DX_DEFAULT,
												 0,
												 NULL,
												 NULL,
												 &Textures[i]*/
											   ) ) )
			{
            Textures[i] = NULL;
			}    
		}
	pD3DXMtrlBuffer->Release();
	D3DXCleanMesh(Mesh, NULL, &Mesh, NULL, NULL);
	D3DXComputeNormals(Mesh, NULL);
	D3DXATTRIBUTEWEIGHTS AttributeWeights;

AttributeWeights.Position  = 0.5;
AttributeWeights.Boundary =  0.5;
AttributeWeights.Normal   =  1.0;
AttributeWeights.Diffuse  =  0.0;
AttributeWeights.Specular =  0.0;
//AttributeWeights.Tex[8]   =  {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

	//D3DXSimplifyMesh(Mesh, NULL, &AttributeWeights, NULL, 300, D3DXMESHSIMP_VERTEX, &Mesh);
	}
};


////
//////
//////// MODEL CLASS  /////////////////////////////////////////////////////////////////////
class MODEL
{
public:

//bool        start;
//float       xPos;
//float       yPos;
//float       zPos;
D3DXVECTOR3 Position;
D3DXQUATERNION QNAngle;
D3DXMATRIX  MatWorld;
D3DXMATRIX Rot, Trans, Size, Orient;
~MODEL()
	{
	
	}
MODEL* getPoint()
	{
	return this;
	}
void Render(MESH *Mesh, int FillMode)
	{
	Device->SetTransform( D3DTS_WORLD, &MatWorld );
	if(FillMode != NULL)
	{
	Device->SetRenderState(D3DRS_FILLMODE, FillMode ); 
												//D3DFILL_POINT
											    //D3DFILL_WIREFRAME
												//D3DFILL_SOLID
	}
	for( DWORD b=0; b<Mesh->NumMaterials; b += 1 )
		{
		Device->SetTexture( 0, Mesh->Textures[b] );
		Device->SetMaterial( &Mesh->Materials[b] );
		Mesh->Mesh->DrawSubset(b);		
		}				
	//delete[] Mesh;
	delete &Mesh;
	}
//
////
////// <<< World Matrix Functions >>> //////
void SetTransl(D3DXVECTOR3 Vector, bool erase)
	{
	
	if(!erase)
		{
		//xPos = X;yPos = Y; zPos = Z;
		D3DXMATRIX tempmat;
		D3DXMatrixTranslation(&tempmat, Vector.x, Vector.y, Vector.z);
		D3DXMatrixMultiply(&Trans, &tempmat, &Trans);		
		}
	else
        //xPos += X;yPos += Y;zPos += Z;		
        D3DXMatrixTranslation(&Trans, Vector.x, Vector.y, Vector.z);
	    D3DXVec3TransformCoord(&Position,  &D3DXVECTOR3(0, 0, 0), &Trans);
	}
void SetSize(D3DXVECTOR3 Vector, bool erase)
	{
	if(!erase)
		{
		D3DXMATRIX tempmat;
		D3DXMatrixScaling(&tempmat, Vector.x, Vector.y, Vector.z);
		D3DXMatrixMultiply(&Size, &tempmat, &Size);	
		}
	else
		D3DXMatrixScaling(&Size, Vector.x, Vector.y, Vector.z);
	}
void SetRotX(float angle, bool erase)
	{
	if(!erase)
		{
		D3DXMATRIX tempmat;
		D3DXMatrixRotationX(&tempmat, angle);
		D3DXMatrixMultiply(&Rot, &tempmat, &Rot);
		}
	else
		D3DXMatrixRotationX(&Rot, angle);
	}
void SetRotY(float angle, bool erase)
	{
	if(!erase)
		{
		D3DXMATRIX tempmat;
		D3DXMatrixRotationY(&tempmat, angle);
		D3DXMatrixMultiply(&Rot, &tempmat, &Rot);
		}
	else
		D3DXMatrixRotationY(&Rot, angle);
	}
void SetRotZ(float angle, bool erase)
	{
	if(!erase)
		{
		D3DXMATRIX tempmat;
		D3DXMatrixRotationZ(&Rot, angle);
		D3DXMatrixMultiply(&Rot, &tempmat, &Rot);
		}
	else	
		D3DXMatrixRotationZ(&Rot, angle);		
	}
	
void ResMat()
	{
	//D3DXMatrixRotationY(&matWorld, 0);
	D3DXMatrixIdentity(&MatWorld);
	}
void MergMat()
{
D3DXMATRIX tempmat;
D3DXMatrixMultiply(&MatWorld, &Size, &Rot);
D3DXMatrixMultiply(&MatWorld, &MatWorld, &Trans);
D3DXMatrixMultiply(&Orient, &Rot, &Trans);
}
};




