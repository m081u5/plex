/*
*      Copyright (C) 2005-2008 Team XBMC
*      http://www.xbmc.org
*
*  This Program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2, or (at your option)
*  any later version.
*
*  This Program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with XBMC; see the file COPYING.  If not, write to
*  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
*  http://www.gnu.org/copyleft/gpl.html
*
*/


#include "stdafx.h"
#include "WinSystemWin32DX.h"

#ifdef HAS_DX

CWinSystemWin32DX g_Windowing;

CWinSystemWin32DX::CWinSystemWin32DX()
: CRenderSystemBase()
{
  m_pD3D = NULL;
  m_enumRenderingSystem = RENDERING_SYSTEM_DIRECTX;
  m_pD3DDevice = NULL;
  ZeroMemory(&m_D3DPP, sizeof(D3DPRESENT_PARAMETERS));
}

CWinSystemWin32DX::~CWinSystemWin32DX()
{
  //Destroy();
}

bool CWinSystemWin32DX::InitRenderSystem()
{
  m_pD3D = NULL;

  m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
  if(m_pD3D == NULL)
    return false;
    
  return true;
}

bool CWinSystemWin32DX::DestroyRenderSystem()
{
  return true;
}

bool CWinSystemWin32DX::ResetRenderSystem(int width, int height)
{
  return true;
}

void CWinSystemWin32DX::GetRenderVersion(unsigned int& major, unsigned int& minor)
{

}

const CStdString& CWinSystemWin32DX::GetRenderVendor()
{

}

const CStdString& CWinSystemWin32DX::GetRenderRenderer()
{
  
}

bool CWinSystemWin32DX::BeginRender()
{
  return true;
}

bool CWinSystemWin32DX::EndRender()
{
  return true;
}

bool CWinSystemWin32DX::PresentRender()
{
  return true;
}

bool CWinSystemWin32DX::ClearBuffers(DWORD color)
{
  return true;
}

bool CWinSystemWin32DX::ClearBuffers(float r, float g, float b, float a)
{
  return true;
}

bool CWinSystemWin32DX::IsExtSupported(CStdString strExt)
{
  return false;
}

void CWinSystemWin32DX::SetVSync(bool vsync)
{

}

void CWinSystemWin32DX::SetViewPort(CRect& viewPort)
{

}

void CWinSystemWin32DX::GetViewPort(CRect& viewPort)
{

}

bool CWinSystemWin32DX::NeedPower2Texture()
{
  return false;
}

void CWinSystemWin32DX::CaptureStateBlock()
{

}

void CWinSystemWin32DX::ApplyStateBlock()
{

}

void CWinSystemWin32DX::SetCameraPosition(const CPoint &camera, int screenWidth, int screenHeight)
{

}

/*
bool CRenderSystemDX::Destroy()
{
  m_bCreated = false;

  SAFE_RELEASE(m_pD3D);
  SAFE_RELEASE(m_pD3DDevice);

  return true;
}
*/

/*
bool CRenderSystemDX::AttachWindow(CWinSystem* winSystem)
{
  if(winSystem == NULL)
    return false;

  m_pWinSystem = winSystem;

  if(m_pWinSystem->GetWinSystem() != WINDOW_SYSTEM_WIN32)
    return false;

  if(!DeleteResources())
    return false;

  if(!CreateResources())
    return false;

  return true;
}
*/

/*
bool CRenderSystemDX::CreateResources()
{
  if(!CreateDevice())
    return false;

  m_bCreated = true;

  return true;
}
*/

/*
bool CRenderSystemDX::DeleteResources()
{
  return true;
}
*/

/*
bool CRenderSystemDX::CreateDevice()
{
  // Code based on Ogre 3D engine

  HRESULT hr;

  if(m_pD3D == NULL)
    return false;

  if(m_pWinSystem == NULL)
    return false;

  D3DDEVTYPE devType = D3DDEVTYPE_HAL;

  ZeroMemory( &m_D3DPP, sizeof(D3DPRESENT_PARAMETERS) );
  m_D3DPP.Windowed					= !m_pWinSystem->IsFullScreen();
  m_D3DPP.SwapEffect				= D3DSWAPEFFECT_DISCARD;
  // triple buffer if VSync is on
  m_D3DPP.BackBufferCount			= 1;
  m_D3DPP.EnableAutoDepthStencil	= true;
  m_D3DPP.hDeviceWindow			= m_pWinSystem->GetHwnd();
  m_D3DPP.BackBufferWidth			= m_pWinSystem->GetWidth();
  m_D3DPP.BackBufferHeight			= m_pWinSystem->GetHeight();
  m_D3DPP.FullScreen_RefreshRateInHz = m_pWinSystem->GetDisplayFreq();

  if (m_bVSync)
  {
    m_D3DPP.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
  }
  else
  {
    m_D3DPP.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
  }

  m_D3DPP.BackBufferFormat = D3DFMT_X8R8G8B8;

  // Try to create a 32-bit depth, 8-bit stencil
  if( FAILED( m_pD3D->CheckDeviceFormat( D3DADAPTER_DEFAULT,
      devType,  m_D3DPP.BackBufferFormat,  D3DUSAGE_DEPTHSTENCIL, 
        D3DRTYPE_SURFACE, D3DFMT_D24S8 )))
  {
    // Bugger, no 8-bit hardware stencil, just try 32-bit zbuffer 
    if( FAILED( m_pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT,
      devType,  m_D3DPP.BackBufferFormat,  D3DUSAGE_DEPTHSTENCIL, 
      D3DRTYPE_SURFACE, D3DFMT_D32 )))
    {
      // Jeez, what a naff card. Fall back on 16-bit depth buffering
      m_D3DPP.AutoDepthStencilFormat = D3DFMT_D16;
    }
    else
      m_D3DPP.AutoDepthStencilFormat = D3DFMT_D32;
  }
  else
  {
    if( SUCCEEDED( m_pD3D->CheckDepthStencilMatch( D3DADAPTER_DEFAULT, devType,
          m_D3DPP.BackBufferFormat, m_D3DPP.BackBufferFormat, D3DFMT_D24S8 ) ) )
    {
      m_D3DPP.AutoDepthStencilFormat = D3DFMT_D24S8; 
    } 
    else 
      m_D3DPP.AutoDepthStencilFormat = D3DFMT_D24X8; 
  }


  m_D3DPP.MultiSampleType = D3DMULTISAMPLE_NONE;
  m_D3DPP.MultiSampleQuality = 0;

  hr = m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, devType, m_pWinSystem->GetHwnd(),
      D3DCREATE_HARDWARE_VERTEXPROCESSING, &m_D3DPP, &m_pD3DDevice );
  if (FAILED(hr))
  {
    // Try a second time, may fail the first time due to back buffer count,
    // which will be corrected down to 1 by the runtime
    hr = m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, devType, m_pWinSystem->GetHwnd(),
        D3DCREATE_HARDWARE_VERTEXPROCESSING, &m_D3DPP, &m_pD3DDevice );
    if( FAILED( hr ) )
    {
      hr = m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, devType, m_pWinSystem->GetHwnd(),
        D3DCREATE_MIXED_VERTEXPROCESSING, &m_D3DPP, &m_pD3DDevice );
      if( FAILED( hr ) )
      {
        hr = m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, devType, m_pWinSystem->GetHwnd(),
          D3DCREATE_SOFTWARE_VERTEXPROCESSING, &m_D3DPP, &m_pD3DDevice );
      }
      if(FAILED( hr ) )
        return false;
    }
  }

  m_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
  m_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

  return true;
}
*/

/*
void CRenderSystemDX::SetViewPort(CRect& viewPort)
{
  if(!m_bCreated)
    return;

  D3DVIEWPORT9 newviewport;

  newviewport.MinZ = 0.0f;
  newviewport.MaxZ = 1.0f;
  newviewport.X = (DWORD)viewPort.x1;
  newviewport.Y = (DWORD)viewPort.y1;
  newviewport.Width = (DWORD)(viewPort.x2 - viewPort.x1);
  newviewport.Height = (DWORD)(viewPort.y2 - viewPort.y1);
  m_pD3DDevice->SetViewport(&newviewport);
}
*/

/*
void CRenderSystemDX::GetViewPort(CRect& viewPort)
{
  if(!m_bCreated)
    return;

  D3DVIEWPORT9 d3dviewport;
  m_pD3DDevice->GetViewport(&d3dviewport);

  viewPort.x1 = (float)d3dviewport.X;
  viewPort.y2 = (float)d3dviewport.Y;
  viewPort.y1 = (float)d3dviewport.X + d3dviewport.Width;
  viewPort.x2 = (float)d3dviewport.Y + d3dviewport.Height;
}
*/

/*
bool CRenderSystemDX::BeginRender()
{
  if(!m_bCreated)
    return false;

  if(FAILED (m_pD3DDevice->BeginScene()))
    return false;

  return true;
}
*/

/*
bool CRenderSystemDX::EndRender()
{
  if(!m_bCreated)
    return false;

  if(FAILED (m_pD3DDevice->EndScene()))
    return false;

  return true;
}
*/

/*
bool CRenderSystemDX::Present()
{
  if(!m_bCreated)
    return false;
  
  HRESULT hr;
      
  hr = m_pD3DDevice->Present( NULL, NULL, 0, NULL );
 
  if( D3DERR_DEVICELOST == hr )
    return false;

  if(FAILED(hr))
    return false;

  return true;
}
*/

/*
bool CRenderSystemDX::ClearBuffers(DWORD color)
{
  if(!m_bCreated)
    return false;

  HRESULT hr;

  if( FAILED( hr = m_pD3DDevice->Clear( 
    0, 
    NULL, 
    D3DCLEAR_TARGET,
    color, 
    1.0, 
    0 ) ) )
    return false;

  return true;
}
*/

/*
bool CRenderSystemDX::ClearBuffers(float r, float g, float b, float a)
{
  D3DXCOLOR color(r, g, b, a);

  return ClearBuffers((DWORD)color); 
}
*/

/*
bool CRenderSystemDX::Test()
{
  static DWORD lastTime = 0;
  static float delta = 0;

  DWORD thisTime = timeGetTime();
  
  if(thisTime - lastTime > 10)
  {
    lastTime = thisTime;
    delta++;
  }

  CLog::Log(LOGINFO, "Delta =  %d", delta);

  if(delta > m_pWinSystem->GetWidth())
    delta = 0;

  LPDIRECT3DVERTEXBUFFER9 pVB = NULL;

  // A structure for our custom vertex type
  struct CUSTOMVERTEX
  {
    FLOAT x, y, z, rhw; // The transformed position for the vertex
    DWORD color;        // The vertex color
  };

  // Our custom FVF, which describes our custom vertex structure
  #define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)

  // Initialize three vertices for rendering a triangle
  CUSTOMVERTEX vertices[] =
  {
    { delta + 100.0f,  50.0f, 0.5f, 1.0f, 0xffff0000, }, // x, y, z, rhw, color
    { delta+200.0f, 250.0f, 0.5f, 1.0f, 0xff00ff00, },
    {  delta, 250.0f, 0.5f, 1.0f, 0xff00ffff, },
  };

  // Create the vertex buffer. Here we are allocating enough memory
  // (from the default pool) to hold all our 3 custom vertices. We also
  // specify the FVF, so the vertex buffer knows what data it contains.
  if( FAILED( m_pD3DDevice->CreateVertexBuffer( 3 * sizeof( CUSTOMVERTEX ),
    0, D3DFVF_CUSTOMVERTEX,
    D3DPOOL_DEFAULT, &pVB, NULL ) ) )
  {
    return false;;
  }

  // Now we fill the vertex buffer. To do this, we need to Lock() the VB to
  // gain access to the vertices. This mechanism is required becuase vertex
  // buffers may be in device memory.
  VOID* pVertices;
  if( FAILED( pVB->Lock( 0, sizeof( vertices ), ( void** )&pVertices, 0 ) ) )
    return false;
  memcpy( pVertices, vertices, sizeof( vertices ) );
  pVB->Unlock();

  m_pD3DDevice->SetStreamSource( 0, pVB, 0, sizeof( CUSTOMVERTEX ) );
  m_pD3DDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
  m_pD3DDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 1 );

  pVB->Release();

  return true;
}
*/

#endif