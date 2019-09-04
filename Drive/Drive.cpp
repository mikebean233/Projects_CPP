////// INCLUDE HEADER FILES //////
#include <d3dx8.h>
#include <Dinput.h>
#include <mmsystem.h>
#include <Math.h>
////   D3DXVECTOR3(
////// DECLARE GLOBAL VARIABLES //////
LPDIRECT3DDEVICE8       Device;
LPDIRECTINPUTDEVICE8 DiDevice;
#include "Drive.h"
LPDIRECT3D8             g_pD3D           = NULL;
LPDIRECTINPUT8  DirInput;
HINSTANCE Hinstance;
double Time;// = timeGetTime() / 1000;
bool LBDown = false;
bool RBDown = false;
int FrameCount = 0;
char CTime[10];
D3DLIGHT8 sun;
D3DLIGHT8 headlightL;
D3DLIGHT8 headlightR;
D3DXVECTOR3 CLook;
D3DXVECTOR3 CPos;
float CAngle = 0;
float angle = 0;
float Mangle = 0;
//MODEL ship;
//MESH Mship;
MODEL NGround;
MODEL MSun;
MESH MNGround;
MESH MNGround2;
MESH Sun;
MESH Arrow;
float Timer2;
///// DEFINE THE CAR STRUCTURE /////
class Automobile
{
public:
D3DXVECTOR3 Position;
D3DXVECTOR3 Vector;
D3DXVECTOR3 Movement; // Direction of the crafts movement
D3DXVECTOR3 Size;
MESH Mesh;
MODEL Model;
float Timer;
float MoveSpeed; // represents the crafts speed in points per second
  Automobile()
  {
  Position = D3DXVECTOR3(0,0,0);  
  //Movement = D3DXVECTOR3(0,0,1);
  Size = D3DXVECTOR3(1,1,1);
  //Mesh.LoadXFile("models/car5.x");
  Timer = timeGetTime() / 1000.0;
  }
  void move()
  {
  float TPas = (timeGetTime() / 1000.0) - Timer;
  Time = TPas;
  Timer = timeGetTime() / 1000.0;
  angle += TPas * Mangle;
  Movement.z = cos((angle * D3DX_PI) / 180);
  Movement.x = sin((angle * D3DX_PI) / 180);
  D3DXVec3Normalize(&Movement, &Movement);
  Position.x += TPas * (Movement.x * MoveSpeed);
  Position.y += TPas * (Movement.y * MoveSpeed);
  Position.z += TPas * (Movement.z * MoveSpeed);
  }
  void SetMat()
  {
  Model.SetTransl(Position, true);
  Model.SetSize(Size, true);
  //D3DXMatrixIdentity(&Model.Size);  
  Model.SetRotY((float)(angle * D3DX_PI) / 180 , true);
  Model.MergMat();
  }
  void RenderPers(CAMERA * camera)
  {
  D3DXMatrixPerspectiveFovLH( &camera->matProj,  (45.0f * D3DX_PI) / 180, 1.0f, 0.5f, 1000);
  Device->SetTransform( D3DTS_PROJECTION, &camera->matProj );
  ///////Device->SetRenderState( D3DRS_LIGHTING, TRUE );
  ///////D3DFILL_POINT
  ///////D3DFILL_WIREFRAME
  ///////D3DFILL_SOLID
  //Model.SetSize(Size.x, Size.y, Size.z, true);
  Model.Render(&Mesh, D3DFILL_SOLID);
  }
};

///// DEFINE DIFFERENT CAMERA STRUCTURES /////
struct FixedCam
{
public:
 D3DXVECTOR3 CurrentPosition;
 D3DXVECTOR3 TargetPosition;
 CAMERA camera;
 void SetToModel(D3DXMATRIX MatWorld, D3DXVECTOR3 CPos, D3DXVECTOR3 TPos)
 {
 CurrentPosition = CPos;
 TargetPosition = TPos;
 D3DXVECTOR3 CurrentPosition2;
 D3DXVECTOR3 TargetPosition2;
 D3DXVec3TransformCoord(&CurrentPosition2, &CurrentPosition, &MatWorld);
 D3DXVec3TransformCoord(&TargetPosition2, &TargetPosition, &MatWorld);
 D3DXMatrixLookAtLH(&camera.matView, &CurrentPosition2, &TargetPosition2, &D3DXVECTOR3(0,1,0));
 CurrentPosition = CurrentPosition2;
 TargetPosition = TargetPosition2;
 }
};

struct InsideCam
{
CAMERA camera;
};
FixedCam FCFollow;
Automobile MyCar;
int rows, cols;
HWND hWnd;
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );


////// DECLARE FUNCTIONS ///////
void StUpLights();
void SetLDir(float xdir, float ydir, float zdir, D3DLIGHT8* sun, int index);
void InitMeshes();
void WINAPI ProcessKBInput();
void Render();
char* FixPath(char *FileName, HINSTANCE Handle); //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT InitGeometry();
HRESULT InitD3D( HWND hWnd );

////
////// WINMAIN FUNCTION - THIS IS WHERE THE PROGRAM STARTS EXECUTION //////////////////////
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
Hinstance = hInst;
WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L, hInst, NULL, NULL, NULL, NULL,"Drive", NULL };
	//GetModuleHandle(NULL)
		/// REGISTER THE WINDOWS CLASS
RegisterClassEx( &wc );
//// CREATE THE MAIN WINDOW 
hWnd = CreateWindow("Drive", 
					"Drive", 
                     WS_OVERLAPPEDWINDOW, 
					 0, 0, 
					 1024, 768,
                     GetDesktopWindow(), 
					 NULL, 
					 wc.hInstance, 
					 NULL );

if( SUCCEEDED( InitD3D( hWnd ) ) )
	{ 
    if( SUCCEEDED( InitGeometry() ) )
		{
        ShowWindow(hWnd, 
			       SW_SHOWDEFAULT );
            UpdateWindow( hWnd );
            MSG msg; 
            ZeroMemory( &msg, sizeof(msg) );
            
			while( msg.message!=WM_QUIT )
			{
            ProcessKBInput(); 
			if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
				{
                TranslateMessage( &msg );
                DispatchMessage( &msg );
                }
            
			else
            ProcessKBInput();
				Render();
            }
        }
    }
if( Device != NULL )
    Device->Release();

if( g_pD3D != NULL )
    g_pD3D->Release();

if( DiDevice != NULL )
    DiDevice->Unacquire();
DiDevice->Release();

DirInput->Release();
DirInput = NULL;
UnregisterClass( "Drive", wc.hInstance );
return 0;
}

////
////// INITD3D FUNCTION  //////////////////////////////////////////////////////////////////
HRESULT InitD3D( HWND hWnd )
{

//// DIRECT INPUT CODE /////
DirectInput8Create(Hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&DirInput, NULL);
DirInput->CreateDevice(GUID_SysKeyboard, &DiDevice, NULL);
DiDevice->SetDataFormat(&c_dfDIKeyboard);
DiDevice->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
HRESULT HR = DiDevice->Acquire();
//// END OF DIRECT INPUT CODE ////

g_pD3D = Direct3DCreate8(D3D_SDK_VERSION);
D3DDISPLAYMODE d3ddm;
g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);

D3DPRESENT_PARAMETERS d3dpp; 
ZeroMemory( &d3dpp, sizeof(d3dpp) );

d3dpp.Windowed = TRUE;
d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
d3dpp.BackBufferFormat = d3ddm.Format;
d3dpp.EnableAutoDepthStencil = TRUE;
d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

////// CREATE THE DIRECT3D DEVICE //////
g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, 
					 D3DDEVTYPE_HAL, 
					 hWnd,
                     D3DCREATE_HARDWARE_VERTEXPROCESSING ,
                     &d3dpp, 
		             &Device);


/// SET THE DEVICE RENDER STATES ///
Device->SetRenderState( D3DRS_ZENABLE, TRUE );
Device->SetRenderState( D3DRS_AMBIENT, 0x0000FFFF );
Device->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
									  //D3DCULL_NONE
								      //D3DCULL_CW
									  //D3DCULL_CCW
Device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD );
									  //D3DSHADE_FLAT
									  //D3DSHADE_GOURAUD
									  //D3DSHADE_PHONG
Device->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID ); 
									 //D3DFILL_POINT
									 //D3DFILL_WIREFRAME
									 //D3DFILL_SOLID
Device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);                     
							      //D3DCMP_NEVER
								  //D3DCMP_LESS
								  //D3DCMP_EQUAL
								  //D3DCMP_LESSEQUAL
								  //D3DCMP_GREATER
								  //D3DCMP_NOTEQUAL
								  //D3DCMP_GREATEREQUAL
								  //D3DCMP_ALWAYS               
Device->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
Device->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL);
Device->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);
Device->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);
// D3DRS_DIFFUSEMATERIALSOURCE  
// D3DRS_SPECULARMATERIALSOURCE
// D3DRS_AMBIENTMATERIALSOURCE
// D3DRS_EMISSIVEMATERIALSOURCE  
float Start = 3.0f,    // For linear mode
End   = 13.0f,
Density = 0.05f;  // For exponential modes

Device->SetRenderState(D3DRS_FOGENABLE, TRUE);
Device->SetRenderState(D3DRS_FOGCOLOR, D3DCOLOR_XRGB(200,200,255));
Device->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_EXP);
Device->SetRenderState(D3DRS_FOGSTART, *(DWORD *) (&Start));
Device->SetRenderState(D3DRS_FOGEND,   *(DWORD *)(&End));
Device->SetRenderState(D3DRS_FOGDENSITY, *((DWORD*) (&Density)));
Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
Device->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
Device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
Device->SetRenderState(D3DRS_ALPHAREF, 0xFF);
Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE); 
//Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);


InitMeshes();
return S_OK;
}


////
////// INITGEOMETRY FUNCTION  /////////////////////////////////////////////////////////////
HRESULT InitGeometry()
{
ZeroMemory( &sun, sizeof(D3DLIGHT8) );
ZeroMemory( &headlightL, sizeof(D3DLIGHT8) );
ZeroMemory( &headlightR, sizeof(D3DLIGHT8) );

StUpLights();
SetLDir(0.0f, 0.707f, 1, &sun, 0);

SetLDir(0,0, -1, &headlightR, 1);
SetLDir(0,0, -1, &headlightL, 1);

Device->SetLight( 0, &sun );
Device->LightEnable( 0, TRUE );
 
Device->SetLight( 1, &headlightR );
Device->LightEnable( 1, TRUE );
 
Device->SetLight( 2, &headlightL );
Device->LightEnable( 2, TRUE );
 
Device->SetRenderState( D3DRS_LIGHTING, TRUE );
Device->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_XRGB(27,27,27) );

return S_OK;
}


////
//////
//////// RENDER FUNCTION //////////////////////////////////////////////////////////////////
void Render()
{
Device->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(200,200,255), 1.0f, 0 );
 
/// BEGIN RENDERING THE SCNENE //////
Device->BeginScene();
Device->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
Device->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);

//headlightL.Position.x = ship.xPos + 0.27f;
//headlightL.Position.y = ship.yPos;
//headlightL.Position.z = ship.zPos - 0.86f;
MyCar.move();
MyCar.SetMat();
D3DXVECTOR3 CPos;
CPos.x = cos((CAngle * D3DX_PI) / 180);
CPos.z = sin((CAngle * D3DX_PI) / 180);
CPos.x *= 1.2;
CPos.z *= 1.2;
CPos.y = .4;

FCFollow.SetToModel(MyCar.Model.Orient,CPos , D3DXVECTOR3(0,0,0));
D3DXMATRIX tempmat = MyCar.Model.Orient;

D3DXVECTOR3 temp1, temp2, temp3;

D3DXVec3TransformCoord(&temp1 ,
              &D3DXVECTOR3(
			  0.27f, 
			  0, 
			  -0.86f), 
			  &MyCar.Model.Orient);
D3DXVec3TransformCoord(&temp2 ,
              &D3DXVECTOR3(
			  -0.27f, 
			  0, 
			  -0.86f), 
			  &MyCar.Model.Orient);



headlightL.Position = temp1;
//headlightL.Position.y = temp1.y;
//headlightL.Position.z = temp1.z;

headlightR.Position = temp2;
//headlightR.Position.y = temp2.y;
//headlightR.Position.z = temp2.z;
//headlightR.Position.x = ship.xPos - 0.27f;
//headlightR.Position.y = ship.yPos;
//headlightR.Position.z = ship.zPos - 0.86f;

D3DXVec3TransformCoord(&temp3, &D3DXVECTOR3(0.0f, -0.25f, -1.0f), &MyCar.Model.Rot);
//D3DXVec3TransformCoord((&D3DXVECTOR3)&headlightL.Position, &D3DXVECTOR3(0.0f, -0.25f, -1.0f), &MyCar.Model.Orient);
headlightL.Direction = temp3;
headlightR.Direction = temp3;
SetLDir(0.0f, -1.1f, -1.0f,  &sun,        0);
D3DXVECTOR3 SunDir, tempa, tempb, tempc;
//tempa = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
//tempb = FCFollow.CurrentPosition;
//tempb *= -1;
SunDir = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
//tempa.x -= FCFollow.CurrentPosition2.x;

 //D3DXVec3TransformCoord(&tempa,  &D3DXVECTOR3(0, 0, 0), &tempmat);
tempb = FCFollow.CurrentPosition;
//tempb = FCFollow.TargetPosition;
tempa = D3DXVECTOR3(0, 0, 0);
//SunDir = tempa - tempb;
//tempb.x -= FCFollow.CurrentPosition2.x;
//tempa.z -= FCFollow.CurrentPosition2.z;
//tempb.z -= FCFollow.CurrentPosition2.z;
//tempa.x = sun.Direction.x * -3;
//tempa.y = sun.Direction.y * -3;
//tempa.z = sun.Direction.z * -3;;

//D3DXMatrixLookAtLH(&MSun.MatWorld, 
//				   &tempa, 
//				   &tempb,
//				   &SunDir);
tempa = D3DXVECTOR3(0,0,0);
tempb = D3DXVECTOR3(FCFollow.CurrentPosition);
tempc = tempa - tempb;
D3DXVec3Normalize(&tempc, &tempc);
D3DXMatrixRotationAxis(&MSun.Rot, &tempc, 3);
MSun.SetSize(D3DXVECTOR3(0.025f,0.025f,0.025f), true);
MSun.SetTransl(tempa, true);
//D3DXMatrixIdentity(&MSun.Trans);
MSun.MergMat();
//MSun.MatWorld = tempmat;
//MSun.MatWorld = FCFollow.camera.matView;
Device->SetLight(2, &headlightL );
Device->SetLight(1, &headlightR );
//SetLDir(temp3.x,  temp3.y,    temp3.z, &headlightL, 2);
//SetLDir(temp3.x,  temp3.y,    temp3.z, &headlightR, 1);

//-----------------------------------------------
/// RENDER THE REST OF THE SCENE ///
///// CREATE A LEFT HAND LOOK-AT MATRIX //////

//ship.SetRotY((float)(angle * D3DX_PI) / 180 , true);

Device->SetRenderState( D3DRS_LIGHTING, TRUE );
Device->SetTransform( D3DTS_VIEW, &FCFollow.camera.matView );
D3DXMatrixPerspectiveFovLH( &FCFollow.camera.matProj,  (45.0f * D3DX_PI) / 180, 1.0f, 0.5f, 1000);
Device->SetTransform( D3DTS_PROJECTION, &FCFollow.camera.matProj );
Device->SetRenderState( D3DRS_LIGHTING, TRUE );
Device->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
Device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);

rows = 6; cols = 6;
int i = 0;
CAMERA camera;
D3DXMATRIX mat;
    D3DXMatrixMultiply( &mat, &camera.matView, &camera.matProj );
    D3DXMatrixTranspose( &mat, &mat );
	//Device->SetRenderState( D3DRS_LIGHTING, FALSE);
	for(float h = 0; h < rows ; h += 1.0f)
	{
	for(float v = 0; v < cols; v += 0.99f)
		{
		i++;
		NGround.SetTransl(D3DXVECTOR3(((rows / 2.0f) * 10.0f) - (h * 10.0f) ,-0.70f,((cols / 2.0f) * 10.0f) - (v * 10.0f)), true);
		NGround.SetSize(D3DXVECTOR3(1.0f, 1.0f, 1.0f), true);
		NGround.SetRotY(0, true);
		NGround.MergMat();
		if((rows / 2) == h)
		{
	    NGround.Render(&MNGround, D3DFILL_SOLID);
		}
		else
		NGround.Render(&MNGround2, D3DFILL_SOLID);
		//((rows / 2) * 10) - 
		//}
		}
	}			 
Device->SetRenderState( D3DRS_LIGHTING, TRUE );
///////D3DFILL_POINT
///////D3DFILL_WIREFRAME
///////D3DFILL_SOLID					 
//ship.Render(&Mship, D3DFILL_SOLID);

Device->SetRenderState(D3DRS_FOGENABLE, FALSE);
MyCar.RenderPers(&FCFollow.camera);
Device->SetRenderState( D3DRS_LIGHTING, FALSE );
MSun.Render(&Arrow, D3DFILL_WIREFRAME);
MSun.Render(&Sun, D3DFILL_SOLID);
Device->SetRenderState(D3DRS_FOGENABLE, TRUE);
//char APath[40];

//Device->SetRenderState( D3DRS_LIGHTING, TRUE );
////// DISPLAY THE FRAME RATE //////
//if((timeGetTime() / 1000) > Time + 1)
//	{
	//Time = timeGetTime() / 1000;
	//_ltoa(MyCar.Timer, CTime, 10);
	//_ltoa(sizeof(APath), APath, 10);
	
	//_gcvt( Time, 10, CTime );
	//FrameCount = 0;
//	}
//FrameCount += 1;
//HDC hdc = GetDC(hWnd);
//char APath[50] = "                                                 ";
//GetModuleFileName(Hinstance, APath, 50);
//TextOut(hdc, 0, 0, APath, 50);
//TextOut(hdc, 0,0, FixPath("file.ext", Hinstance), 50);
//ReleaseDC(hWnd, hdc);
	
Device->EndScene();
Device->Present( NULL, NULL, NULL, NULL );
}

/////
////// MSGPROC FUNCTION - THIS IS THE MAIN WINDOWS MESSAGE LOOP ///////////////////////////
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
switch( msg )
	{
    case WM_DESTROY:
            PostQuitMessage( 0 );
    case WM_MOUSEMOVE:
      if(!LBDown && !RBDown)
		{
		
		}	
	case WM_LBUTTONDOWN:
	  if(wParam == MK_LBUTTON) LBDown = true;
	   
	case WM_LBUTTONUP:
	  if(wParam != MK_LBUTTON) LBDown = false;
	case WM_RBUTTONDOWN:
	  if(wParam == MK_RBUTTON) RBDown = true;
	case WM_RBUTTONUP:
	  if(wParam != MK_RBUTTON) RBDown = false;
	return 0;
    }

return DefWindowProc( hWnd, msg, wParam, lParam );
}




////
////// SET LIGHT DIRECTION FUNCTION  //////////////////////////////////////////////////////
void SetLDir(float xdir, float ydir, float zdir, D3DLIGHT8* light, int index)
{
D3DXVECTOR3 vecDir;
vecDir = D3DXVECTOR3(xdir, ydir, zdir);
D3DXVec3Normalize( (D3DXVECTOR3*)&light->Direction, &vecDir );
Device->SetLight( index, light );
}


////
////// SET UP LIGHTS FUNCTION  ////////////////////////////////////////////////////////////
void StUpLights()
{
sun.Type       = D3DLIGHT_DIRECTIONAL;
sun.Diffuse.r  = 0.5f;
sun.Diffuse.g  = 0.5f;
sun.Diffuse.b  = 0.5f;
sun.Diffuse.a = 0.0f;
sun.Specular.r = 0.8f;
sun.Specular.g = 0.8f;
sun.Specular.b = 0.8f;
sun.Specular.a = 1.0f;
sun.Ambient.r = 0.7f;
sun.Ambient.g = 0.7f;
sun.Ambient.b = 0.7f;
sun.Ambient.a = 1.0f;
sun.Attenuation0 = 0.0f;
sun.Attenuation1 = 0.0f;
sun.Attenuation2 = 0.0f;
sun.Range       = 1000.0f;
headlightL.Type       = D3DLIGHT_SPOT;
headlightL.Diffuse.r  = 0.7f;
headlightL.Diffuse.g  = 0.7f;
headlightL.Diffuse.b  = 0.6f;
headlightL.Diffuse.a = 1.0f;
headlightL.Theta = (9.0f * D3DX_PI) / 180;
headlightL.Phi = (30.0f * D3DX_PI) / 180;
headlightL.Specular.r = 0.9f;
headlightL.Specular.g = 0.9f;
headlightL.Specular.b = 0.9f;
headlightL.Specular.a = 1.0f;
headlightL.Ambient.r = 0.7f;
headlightL.Ambient.g = 0.7f;
headlightL.Ambient.b = 0.6f;
headlightL.Ambient.a = 1.0f;
headlightL.Attenuation0 = 0.0;
headlightL.Attenuation1 = 0.5;
headlightL.Attenuation2 = 1.0;
headlightL.Falloff = 5.0f;
headlightL.Range= 20.0f;

headlightR.Type       = D3DLIGHT_SPOT;
headlightR.Diffuse.r  = 0.9f;
headlightR.Diffuse.g  = 0.9f;
headlightR.Diffuse.b  = 0.6f;
headlightR.Theta = (9.0f * D3DX_PI) / 180;
headlightR.Phi = (30.0f * D3DX_PI) / 180;
headlightR.Specular.r = 0.9f;
headlightR.Specular.g = 0.9f;
headlightR.Specular.b = 0.9f;
headlightR.Ambient.r = 0.9f;
headlightR.Ambient.g = 0.9f;
headlightR.Ambient.b = 0.6f;
headlightR.Attenuation0 = 0.0;
headlightR.Attenuation1 = 0.5;
headlightR.Attenuation2 = 1.0;
headlightR.Falloff = 5.0f;
headlightR.Range= 20.0f;
}

////
////// KEYOARD MESSAGE LOOP FUNCTION  /////////////////////////////////////////////////////
void WINAPI ProcessKBInput() 
{ 
    #define KEYDOWN(name, key) (name[key] & 0x80) 
 
    char     buffer[256]; 
    HRESULT  hr; 
 
    hr = DiDevice->GetDeviceState(sizeof(buffer),(LPVOID)&buffer); 
    if FAILED(hr) 
    { 
        ::MessageBox(NULL, "GetDeviceState Failed", "Drive", MB_ICONSTOP);
		return; 
    } 
    
	/*for(int i = 0; i < 257; i++)
	{
	if(buffer[i]& 0x80)
	{CTime[i] = '1';
	}
	else
		CTime[i] = '0';
	}*/
    float TPas = (float) (timeGetTime() / 1000.0f) - Timer2;
    Timer2 = timeGetTime() / 1000.0f;
    if (KEYDOWN(buffer, DIK_EQUALS))
	{
	CAngle += (25 * TPas);
	if (CAngle > 360) CAngle -= 360;
	}
    if (KEYDOWN(buffer, DIK_MINUS))
	{
	CAngle -= (25 * TPas);
	if (CAngle < 0) CAngle += 360;
	}
    if (KEYDOWN(buffer, DIK_RIGHT))
	{
    Mangle = 25.0f;
	//angle += 0.25f;
	//if(angle > 360) angle -= 360;
	//if(angle < 0) angle += 360;
	};
	
	if(KEYDOWN(buffer, DIK_LEFT)) 
    {
	//angle -= 0.25f;
	//if(angle > 360) angle -= 360;
	//if(angle < 0) angle += 360;
	Mangle = -25.0f;
	};
	
	if (KEYDOWN(buffer, DIK_UP)) 
	{
	MyCar.MoveSpeed = -5.0f;
	}
	if (KEYDOWN(buffer, DIK_DOWN)) 
    {
	MyCar.MoveSpeed = 5.0f;
	}
	if  (((buffer[DIK_UP]& 0x80) == false) && ((buffer[DIK_DOWN]& 0x80) == false))
	{
	MyCar.MoveSpeed = 0.0f;
	}
	if  (((buffer[DIK_LEFT]& 0x80) == false) && ((buffer[DIK_RIGHT]& 0x80) == false))
	{
	Mangle = 0.0f;
	}
	//DIK_MINUS           0x0C    /* - on main keyboard */
//DIK_EQUALS          0x0D


} 

////
////// MESH LOADING FUNCTION  /////////////////////////////////////////////////////////////
void InitMeshes()
{
MNGround.LoadXFile("road.x");
MNGround2.LoadXFile("grass.x");
MyCar.Mesh.LoadXFile("car5.x");
Arrow.LoadXFile("arrow2.x");
Sun.LoadXFile("Sun.x");
}

char* FixPath(char FileName[], HINSTANCE Handle)
{
char APath[50];
char FPath[] = "101010101010101010101010101010101010101";
GetModuleFileName(Handle, APath, 50);
bool done = false;
 for(int i = 40; i > 0; i -= 1)
 {
 if(APath[i] == (int)"/" && done == false)
 {
 bool done = true;
 
   for(int ii = 0;ii <= ((40 - i) + sizeof(FileName)); i += 1)
   {
    if(ii <= i)  
	{	  
    FPath[i] = APath[ii];
	};
    if(ii > i)
	{
	FPath[i] = FileName[(ii - i)];
	}
   };
};
};
//char* pfpath = APath;
 return APath;
}
