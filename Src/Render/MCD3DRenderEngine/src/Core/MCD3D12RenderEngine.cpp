#include "MCD3D12RenderEngine.h"
#include "MCREGlobals.h"
#include "../../../../Common/MCCommon/src/Data/MCThreads.h"
#include "../RootSignature/MCRootSignatureManager.h"
#include "../PipelineStateObject/MCPipelineStateObjectManager.h"
#include "../Shader/MCShaderManager.h"
#include "../Memory/MCResourceManager.h"
#include "../Render/Utility/MCSceneLoader.h"
#include "../Core/MCRenderObjects.h"
#include "MCD3D.h"
#include "MCDXGI.h"
#include "MCRenderWindow.h"
#include "../../../../Common/MCCommon/src/Global/MCCOGlobals.h"
#include "../Messaging/D3D12RenderEngineDispatcher.h"
#include "../Messaging/D3D12RenderEngineDispatchTarget.h"
#include "../Messaging/D3D12RenderSystemDispatchTarget.h"


namespace MC {

  MCD3D12RenderEngine::MCD3D12RenderEngine(const RENDER_CONFIG *pConfiguration) {
	  assert(!MCREGlobals::pRenderEngine);
	  	  
	  // Register the thread that is calling this thread as the main thread.
	  MCThreads::RegisterCurrentThread("Main", MC_THREAD_CLASS::MAIN);

	  _pRenderEngineDispatcher = std::make_unique<D3D12RenderEngineDispatcher>(MCCOGlobals::pRouter);
	  MCREGlobals::pRenderEngineDispatcher = _pRenderEngineDispatcher.get();

	  MCREGlobals::InitialConfiguration = *pConfiguration;
	  _pRenderWindow = std::make_unique<MCRenderWindow>();
	  MCREGlobals::pRenderWindow = _pRenderWindow.get();

	  _pMCDXGI = std::make_unique<MCDXGI>();
	  MCREGlobals::pMCDXGI = _pMCDXGI.get();

	  _pMCD3D = std::make_unique<MCD3D>();
	  MCREGlobals::pMCD3D = _pMCD3D.get();

	  _pResourceManager = std::make_unique<MCResourceManager>();
	  MCREGlobals::pResourceManager = _pResourceManager.get();

	  _pShaderManager = std::make_unique<MCShaderManager>();
	  MCREGlobals::pShaderManager = _pShaderManager.get();

	  _pPSOManager = std::make_unique<MCPipelineStateObjectManager>();
	  MCREGlobals::pPSOManager = _pPSOManager.get();

	  _pRSManager = std::make_unique<MCRootSignatureManager>();
	  MCREGlobals::pRSManager = _pRSManager.get();

	  /******************************************************************/

	  /************ CREATE SYSTEM AND ENGING DISPATCH TARGETS ***********/
	  _pSystemDispatchTarget = std::make_unique<D3D12RenderSystemDispatchTarget>(_pRenderEngineDispatcher.get(), MCCOGlobals::pRouter);
	  _pEngineDispatchTarget = std::make_unique<D3D12RenderEngineDispatchTarget>(_pRenderEngineDispatcher.get(), MCCOGlobals::pRouter);

	  _pRenderer = std::make_unique<MCRenderer>();

	  MCREGlobals::pRenderEngine = this;
  }

  MCD3D12RenderEngine::~MCD3D12RenderEngine() {}

#pragma region Public API

  void MCD3D12RenderEngine::Update() {
	  _pRenderer->Update();
  }

  MC_RESULT MCD3D12RenderEngine::ScheduleFrame(MCFrame *pFrame) {
	  return _pRenderer->ScheduleFrame(pFrame);
  }

  MC_RESULT MCD3D12RenderEngine::SetRenderState(MC_RENDER_STATE renderState) {
	  return _pRenderer->SetState(renderState);	  
  }

  MCRENDER_ITEM_HANDLE MCD3D12RenderEngine::LoadGeometry(const MCGEOMETRY_MESH_DESC& desc) {
	  return _pRenderer->GetSceneLoader()->LoadGeometry<MCVertex1Color, DXGI_FORMAT_R32_UINT>(desc);
  }

  MC_RESULT MCD3D12RenderEngine::SetPauseState(bool pauseState) {
	  if (!_pRenderer)
		  return MC_RESULT::FAIL_NO_OBJECT;

	  if (pauseState)
		  return _pRenderer->SuspendExecution();
	  else
		  return _pRenderer->UnsuspendExecution();
  }

#pragma endregion

}
