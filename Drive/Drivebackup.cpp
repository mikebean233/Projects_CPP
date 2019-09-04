//
//// INLUCDE FILES
#include <d3dx8.h>
#include <Dinput.h>
#include <mmsystem.h>
#include <Math.h>
#include "Drive.h"

//
//// GLOBAL VARIABLES
LPDIRECT3DDEVICE8       Device;
LPDIRECTINPUTDEVICE8 DiDevice;


LPDIRECT3D8             D3D           = NULL;
LPDIRECTINPUT8  DirInput;

long Time = timeGetTime() / 1000;
int FrameCount = 0;
char CTime[50];

D3DLIGHT8 sun;
D3DLIGHT8 headlightL;
D3DLIGHT8 headlightR;
D3DXVECTOR3 CLook;
D3DXVECTOR3 CPos;
//
//// FUNCTION DECLARATIONS
void Slights();
void SetLDir(double xdir, double ydir, double zdir, D3DLIGHT8* sun, int index);
void InitMeshes();
void WINAPI ProcessKBInput();

 MODEL ship;
MESH Mship;
 MODEL NGround[2501];
 MODEL Sky;
MESH Mbg;


CAMERA camera;
int rows, cols;
HWND hWnd;
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL,"Drive", NULL };
//-----------------------------------------------------------------------------
// Name: InitD3D()
// Desc: Initializes Direct3D
//-----------------------------------------------------------------------------
HRESULT InitD3D( HWND hWnd )
{

//// DIRECT INPUT CODE /////
DirectInput8Create(wc.hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&DirInput, NULL); 
DirInput->CreateDevice(GUID_SysKeyboard, &DiDevice, NULL); 
DiDevice->SetDataFormat(&c_dfDIKeyboard); 
DiDevice->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
DiDevice->Acquire(); 

//// END OF DIRECT INPUT CODE ////

	if( NULL == ( D3D = Direct3DCreate8( D3D_SDK_VERSION ) ) )
 return E_FAIL;

D3DDISPLAYMODE d3ddm;
if( FAILED( D3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm ) ) )
 return E_FAIL;

D3DPRESENT_PARAMETERS d3dpp; 
ZeroMemory( &d3dpp, sizeof(d3dpp) );
d3dpp.Windowed = TRUE;
d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
d3dpp.BackBufferFormat = d3ddm.Format;
d3dpp.EnableAutoDepthStencil = TRUE;
d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

if( FAILED( D3D->CreateDevice(D3DADAPTER_DEFAULT, 
								 D3DDEVTYPE_HAL, 
								 hWnd,
                                 D3DCREATE_HARDWARE_VERTEXPROCESSING ,
                                 &d3dpp, 
								 &Device ) ) )
	{
    return E_FAIL;
    }

Device->SetRenderState( D3DRS_ZENABLE, TRUE );
Device->SetRenderState( D3DRS_AMBIENT, 0xFFFFFFFF );
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
// D3DRS_DIFFUSEMATERIALSOURCE  D3DMCS_MATERIAL 
// D3DRS_SPECULARMATERIALSOURCE
// D3DRS_AMBIENTMATERIALSOURCE
// D3DRS_EMISSIVEMATERIALSOURCE  
InitMeshes();
	   return S_OK;
}

//-----------------------------------------------------------------------------
// Name: InitGeometry()
// Desc: Load the mesh and build the material and texture arrays
//-----------------------------------------------------------------------------
HRESULT InitGeometry()
{
Sky.SetRotZ(180 * D3DX_PI / 180, true);
Sky.SetTransl(0, 0, -700, false);
for(int i = 0;i < 10; i++)
{
//NGround[i].SetTransl(0, -.84, 0, true);
}
//FGround.SetTransl(0,-.41,0, true);
//FGround.SetSize(4,1,4,false);
ship.SetSize(3,3,3, true);
ZeroMemory( &sun, sizeof(D3DLIGHT8) );
//ZeroMemory( &headlightL, sizeof(D3DLIGHT8) );
//ZeroMemory( &headlightR, sizeof(D3DLIGHT8) );
Slights();
SetLDir(0, .707, 1, &sun, 0);

SetLDir(0,0, -1, &headlightR, 1);
SetLDir(0,0, -1, &headlightL, 1);

Device->SetLight( 0, &sun );
Device->LightEnable( 0, TRUE );

Device->SetLight( 1, &headlightR );
Device->LightEnable( 1, TRUE );

Device->SetLight( 2, &headlightL );
Device->LightEnable( 2, TRUE );



Device->SetRenderState( D3DRS_LIGHTING, TRUE );
//Device->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_XRGB(20,20,20) );
//D3DCOLOR_XRGB(0,0,0)

Device->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_XRGB(27,27,27) );

return S_OK;
}

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
VOID Render()
{
Device->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(127,127,127), 1.0f, 0 );
Device->BeginScene();
Device->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
Device->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
//D3DXQUATERNION qn;	
if(LBDown)
	{
	xperc = 130 * (((double)LDMouse_X - (double)Mouse_X) / 300);
	zperc = 130 * (((double)LDMouse_X - (double)Mouse_X) / 300);
	};

CPos.x = 4 * sin(xperc * 3.14 / 180);
CPos.y = .5;
CPos.z = 4 * cos(zperc * 3.14 / 180);
double percent;
CLook.x = ship.xPos;
CLook.y = ship.yPos - 0.25f;
CLook.z = ship.zPos;

D3DXVec3Normalize(&camera.Vector,&D3DXVECTOR3((CPos.x - ship.xPos),(CPos.y - ship.yPos),(CPos.z - ship.zPos)));
//ship.QNAngle.x = camera.Vector.x;
//ship.QNAngle.y = camera.Vector.y;
//ship.QNAngle.z = camera.Vector.z;
//ship.QNAngle.w = 0;//90 / 180 * 3.14;

headlightL.Position.x = ship.xPos + 0.27f;
headlightL.Position.y = ship.yPos;
headlightL.Position.z = ship.zPos - 0.86f;

headlightR.Position.x = ship.xPos - 0.27f;
headlightR.Position.y = ship.yPos;
headlightR.Position.z = ship.zPos - 0.86f;

D3DXVECTOR3 LDir;	
D3DXVECTOR3 LDir2;
LDir.x = CPos.x - CLook.x;LDir.y = CPos.y - CLook.y;LDir.z = CPos.z - CLook.z;
LDir2.x = CPos.x - CLook.x;LDir2.y = CPos.y - CLook.y;LDir2.z = CPos.z - CLook.z;	
LDir.x *= -1;LDir.y *= -1;LDir.z *= -1;
LDir.y = 0;
LDir2.y = 0;
D3DXVec3Normalize(&LDir, &LDir);
D3DXVec3Normalize(&LDir2, &LDir2);
SetLDir(-1, -1.1, 0,  &sun,        0);
SetLDir(0,  -0.25f,    -1, &headlightL, 2);
SetLDir(0,  -0.25f,    -1, &headlightR, 1);
//Device->SetLight(2, &headlightR);
//Device->SetLight(1, &headlightR);


D3DXMatrixLookAtLH(&camera.matView,
				   &D3DXVECTOR3( 0.0f, 0.0f, 1.0f ) , 
                   &D3DXVECTOR3( Sky.xPos, Sky.yPos, Sky.zPos ) , 
                   &D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) );
Device->SetTransform( D3DTS_VIEW, &camera.matView );
D3DXMatrixOrthoLH(&camera.matProj, 100, 100, .5, 1000);
Device->SetTransform( D3DTS_PROJECTION, &camera.matProj );
Device->SetRenderState( D3DRS_LIGHTING, FALSE );
Sky.Render(&Mbg, D3DFILL_SOLID);

Device->Clear( 0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(127,127,127), 1.0f, 0 );

D3DXMatrixLookAtLH(&camera.matView,
				   &CPos, 
                   &CLook, 
                   &D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) );
Device->SetTransform( D3DTS_VIEW, &camera.matView );
D3DXMatrixPerspectiveFovLH( &camera.matProj,  (45.0f * D3DX_PI) / 180, 1.0f, 0.5f, 1000);
Device->SetTransform( D3DTS_PROJECTION, &camera.matProj );
Device->SetRenderState( D3DRS_ZENABLE, TRUE );
Device->SetRenderState( D3DRS_LIGHTING, TRUE );
ship.SetTransl(0,.25,0,true);
ship.SetSize(2.0f,2.0f,2.0f,false);

//NGround.Render(&MNGround);
rows = 50.0f; cols = 50.0f;

int i = 0;
D3DXMATRIX mat;
    D3DXMatrixMultiply( &mat, &camera.matView, &camera.matProj );
    D3DXMatrixTranspose( &mat, &mat );
	for(int h = 0; h < rows ; h++)
	{
	for(int v = 0; v < cols; v++)
		{
		i++;
		//NGround[i].SetTransl(((h / rows) * 30),0,((v / cols) * 10), true);
		NGround[i].SetTransl(((rows / 2.0f) * 10.0f) - (h * 10.0f) ,0,((cols / 2.0f) * 10.0f) - (v * 10.0f), true);
		//NGround[i].SetSize(0.001f, 1.0f, 0.001f, false);
		//if(((int)rows / 2) == h)
		//{
		//NGround[i].Render(&MNGround, D3DFILL_WIREFRAME);
		//}
		//else
		NGround[i].Render(&MNGround2, D3DFILL_SOLID);
		//((rows / 2) * 10) - 
		}
	}			 
//for(i = 0; i < 10; i++)
//	{
	//NGround[i].SetSize(1, -.84, 0, true);

//D3DFILL_POINT
//D3DFILL_WIREFRAME
//D3DFILL_SOLID					 
ship.Render(&Mship, D3DFILL_SOLID);
//wall.Render(&Mwall, D3DFILL_SOLID);
Device->SetRenderState( D3DRS_LIGHTING, FALSE );
//FGround.Render(&MFGround);

/*if((timeGetTime() / 1000) > Time + 1)
	{
	Time = timeGetTime() / 1000;
	_ltoa(FrameCount, CTime, 10);
	//_gcvt(CPos.z, 10, CTime );
	FrameCount = 0;
	}
FrameCount += 1;
*/
//HDC hdc = GetDC(hWnd);
//Rectangle(hdc, Mouse_X, Mouse_Y, RDMouse_X, LDMouse_Y);
//TextOut(hdc, 0, 0, CTime, 50);
//RECT rect;
//rect.top = 0;
//rect.bottom = 20; 
//rect.left = 0; 
//rect.right = 300;

//FrameRate->Begin();
//FrameRate->DrawText(CTime, sizeof(CTime), rect, DT_LEFT|DT_CALCRECT, 0xffffffff);
//FrameRate->End();
//ReleaseDC(hWnd, hdc);
	
Device->EndScene();
Device->Present( NULL, NULL, NULL, NULL );
}

//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
switch( msg )
	{
    case WM_DESTROY:
            PostQuitMessage( 0 );
    case WM_MOUSEMOVE:
      if(!LBDown && !RBDown)
		{
		Mouse_X = LOWORD(lParam);
		Mouse_Y = HIWORD(lParam);
		}	
	case WM_LBUTTONDOWN:
	  LDMouse_X = LOWORD(lParam);
	  LDMouse_Y = HIWORD(lParam);
	  if(wParam == MK_LBUTTON) LBDown = true;
	case WM_LBUTTONUP:
	  if(wParam != MK_LBUTTON) LBDown = false;
	case WM_RBUTTONDOWN:
	  RDMouse_X = LOWORD(lParam);
	  RDMouse_Y = HIWORD(lParam);
	  if(wParam == MK_RBUTTON) RBDown = true;
	case WM_RBUTTONUP:
	  if(wParam != MK_RBUTTON) RBDown = false;
	return 0;
    }

return DefWindowProc( hWnd, msg, wParam, lParam );
}

//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
//WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL,"Drive", NULL };
RegisterClassEx( &wc );
hWnd = CreateWindow("Drive", 
					"Drive", 
                     WS_OVERLAPPEDWINDOW, 
					 0, 0, 
					 1024, 768,
                     GetDesktopWindow(), 
					 NULL, 
					 wc.hInstance, 
					 NULL );
CLook.x = 0.0f;
CLook.y = 0.0f;
CLook.z = 0.0f;
CPos.x = 0.0f;
CPos.y = 3.0f;
CPos.z = -50.0f;
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
            if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
				{
                TranslateMessage( &msg );
                DispatchMessage( &msg );
                }
            else
            Render();
            }
        }
    }
if( Device != NULL )
    Device->Release();

if( D3D != NULL )
    D3D->Release();
if( DiDevice != NULL )
    DiDevice->Unacquire;
DiDevice->Release;
DirInput->Release;
DirInput = NULL;

UnregisterClass( "Drive", wc.hInstance );
return 0;
}
void SetLDir(double xdir, double ydir, double zdir, D3DLIGHT8* light, int index)
{
D3DXVECTOR3 vecDir;
vecDir = D3DXVECTOR3(xdir, ydir, zdir);
D3DXVec3Normalize( (D3DXVECTOR3*)&light->Direction, &vecDir );
Device->SetLight( index, light );
}
void Slights()
{
sun.Type       = D3DLIGHT_DIRECTIONAL;
sun.Diffuse.r  = 0.5f;
sun.Diffuse.g  = 0.5f;
sun.Diffuse.b  = 0.5f;
sun.Specular.r = 0.7f;
sun.Specular.g = 0.7f;
sun.Specular.b = 0.7f;
sun.Ambient.r = 0.3f;
sun.Ambient.g = 0.3f;
sun.Ambient.b = 0.3f;
sun.Attenuation0 = 0.0f;
sun.Attenuation1 = 0.0f;
sun.Attenuation2 = 0.0f;
sun.Range       = 1000.0f;
headlightL.Type       = D3DLIGHT_SPOT;
headlightL.Diffuse.r  = 0.7f;
headlightL.Diffuse.g  = 0.7f;
headlightL.Diffuse.b  = 0.6f;
headlightL.Theta = (9.0f * D3DX_PI) / 180;
headlightL.Phi = (30.0f * D3DX_PI) / 180;
headlightL.Specular.r = 0.9;
headlightL.Specular.g = 0.9;
headlightL.Specular.b = 0.9;
headlightL.Ambient.r = 0.7f;
headlightL.Ambient.g = 0.7f;
headlightL.Ambient.b = 0.6f;
headlightL.Attenuation0 = 0.0;
headlightL.Attenuation1 = 0.5;
headlightL.Attenuation2 = 1.0;
headlightL.Falloff = 5.0f;
headlightL.Range= 20.0f;

headlightR.Type       = D3DLIGHT_SPOT;
headlightR.Diffuse.r  = 0.7f;
headlightR.Diffuse.g  = 0.7f;
headlightR.Diffuse.b  = 0.6f;
headlightR.Theta = (9.0f * D3DX_PI) / 180;
headlightR.Phi = (30.0f * D3DX_PI) / 180;
headlightR.Specular.r = 0.9;
headlightR.Specular.g = 0.9;
headlightR.Specular.b = 0.9;
headlightR.Ambient.r = 0.7f;
headlightR.Ambient.g = 0.7f;
headlightR.Ambient.b = 0.6f;
headlightR.Attenuation0 = 0.0;
headlightR.Attenuation1 = 0.5;
headlightR.Attenuation2 = 1.0;
headlightR.Falloff = 5.0f;
headlightR.Range= 20.0f;
}
void InitMeshes()
{
MNGround.LoadXFile("models/grass.x");
MNGround2.LoadXFile("models/grass.x");
//MFGround.LoadXFile("ground.x");
Mship.LoadXFile("models/car5.x");
Mbg.LoadXFile("models/sky.x");
}

void WINAPI ProcessKBInput() 
{ 
    #define KEYDOWN(name, key) (name[key] & 0x80) 
 
    char     buffer[256]; 
    HRESULT  hr; 
 
    hr = DiDevice->GetDeviceState(sizeof(buffer),(LPVOID)&buffer); 
    if FAILED(hr) 
    { 
         // If it failed, the device has probably been lost. 
         // Check for (hr == DIERR_INPUTLOST) 
         // and attempt to reacquire it here. 
         return; 
    } 
 
    // Turn the spaceship right or left 
    if (KEYDOWN(buffer, DIK_RIGHT)); 
        // Turn right. 
    //(buffer[DIK_RIGHT] & 0x80)
	
	
	else if(KEYDOWN(buffer, DIK_LEFT)); 
        // Turn left. 
 
    // Thrust or stop the spaceship 
    if (KEYDOWN(buffer, DIK_UP)) ; 
        // Move the spaceship forward. 
    else if (KEYDOWN(buffer, DIK_DOWN)); 
        // Stop the spaceship. 
} 

