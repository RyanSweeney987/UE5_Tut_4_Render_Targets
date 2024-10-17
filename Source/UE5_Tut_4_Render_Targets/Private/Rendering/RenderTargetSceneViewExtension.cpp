// Fill out your copyright notice in the Description page of Project Settings.


#include "Rendering/RenderTargetSceneViewExtension.h"

#include "PixelShaderUtils.h"
#include "RenderGraphEvent.h"
#include "SceneTexturesConfig.h"
#include "RenderGraphBuilder.h"
#include "RenderTargetPool.h"
#include "Engine/TextureRenderTarget2D.h"
#include "ShaderPasses/InvertColourComputePass.h"
#include "ShaderPasses/InvertColourRenderPass.h"

FRenderTargetSceneViewExtension::FRenderTargetSceneViewExtension(const FAutoRegister& AutoRegister) : FSceneViewExtensionBase(AutoRegister)
{
}

void FRenderTargetSceneViewExtension::PrePostProcessPass_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessingInputs& Inputs)
{
	FSceneViewExtensionBase::PrePostProcessPass_RenderThread(GraphBuilder, View, Inputs);

	if(RenderTargetSource == nullptr)
	{
		return;
	}
	
	if(!PooledRenderTarget.IsValid())
	{
		// Only needs to be done once
		// However, if you modify the render target asset, eg: change the resolution or pixel format, you may need to recreate the PooledRenderTarget object
		CreatePooledRenderTarget_RenderThread();
	}
	
	checkSlow(View.bIsViewInfo);
	const FIntRect Viewport = static_cast<const FViewInfo&>(View).ViewRect;
	const FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);

	const FScreenPassTexture SceneColourTexture((*Inputs.SceneTextures)->SceneColorTexture, Viewport);

	// Needs to be registered every frame
	FRDGTextureRef RenderTargetTexture = GraphBuilder.RegisterExternalTexture(PooledRenderTarget, TEXT("Bound Render Target"));

	// Since we're rendering to the render target, we're going to use the full size of the render target rather than the screen
	const FIntRect RenderViewport = FIntRect(0, 0, RenderTargetTexture->Desc.Extent.X, RenderTargetTexture->Desc.Extent.Y);
	
	// True for pixel shader, false for compute shader
#if true
	FInvertColourPS::FParameters* Parameters = GraphBuilder.AllocParameters<FInvertColourPS::FParameters>();
	Parameters->TextureSize = RenderTargetTexture->Desc.Extent;
	Parameters->SceneColorSampler = TStaticSamplerState<SF_Bilinear, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI();
	Parameters->SceneColorTexture = SceneColourTexture.Texture;
	// We're going to also clear the render target
	Parameters->RenderTargets[0] = FRenderTargetBinding(RenderTargetTexture, ERenderTargetLoadAction::EClear);
	
	TShaderMapRef<FInvertColourPS> PixelShader(GlobalShaderMap);
	FPixelShaderUtils::AddFullscreenPass(GraphBuilder, GlobalShaderMap, FRDGEventName(TEXT("Render Target Pass")), PixelShader, Parameters, RenderViewport);
#else
	FRDGTextureRef TempTexture = GraphBuilder.CreateTexture(RenderTargetTexture->Desc, TEXT("Temp Texture"));
	FRDGTextureUAVDesc TempUAVDesc = FRDGTextureUAVDesc(TempTexture);
	FRDGTextureUAVRef TempUAV = GraphBuilder.CreateUAV(TempUAVDesc);

	const FIntPoint ThreadCount = RenderViewport.Size();
	const FIntVector GroupCount = FComputeShaderUtils::GetGroupCount(ThreadCount, FIntPoint(16, 16));
	
	FInvertColourCS::FParameters* Parameters = GraphBuilder.AllocParameters<FInvertColourCS::FParameters>();
	Parameters->TextureSize = RenderTargetTexture->Desc.Extent;
	Parameters->SceneColorSampler = TStaticSamplerState<SF_Bilinear, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI();
	Parameters->SceneColorTexture = SceneColourTexture.Texture;
	Parameters->OutputTexture = TempUAV;
	
	TShaderMapRef<FInvertColourCS> ComputeShader(GlobalShaderMap);
	FComputeShaderUtils::AddPass(GraphBuilder, FRDGEventName(TEXT("Render Target Compute Pass")), ERDGPassFlags::Compute, ComputeShader, Parameters, GroupCount);
	
	AddCopyTexturePass(GraphBuilder, TempTexture, RenderTargetTexture);
#endif
}

void FRenderTargetSceneViewExtension::SetRenderTarget(UTextureRenderTarget2D* RenderTarget)
{
	RenderTargetSource = RenderTarget;
}

void FRenderTargetSceneViewExtension::CreatePooledRenderTarget_RenderThread()
{
	checkf(IsInRenderingThread() || IsInRHIThread(), TEXT("Cannot create from outside the rendering thread"));
	
	// Render target resources require the render thread
	const FTextureRenderTargetResource* RenderTargetResource = RenderTargetSource->GetRenderTargetResource();
	if(RenderTargetResource == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Render Target Resource is null"));
	}
	
	const FTexture2DRHIRef RenderTargetRHI = RenderTargetResource->GetRenderTargetTexture();
	if(RenderTargetRHI.GetReference() == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Render Target RHI is null"));
	}

	FSceneRenderTargetItem RenderTargetItem;
	RenderTargetItem.TargetableTexture = RenderTargetRHI;
	RenderTargetItem.ShaderResourceTexture = RenderTargetRHI;

	// Flags allow it to be used as a render target, shader resource, UAV 
	FPooledRenderTargetDesc RenderTargetDesc = FPooledRenderTargetDesc::Create2DDesc(RenderTargetResource->GetSizeXY(), RenderTargetRHI->GetDesc().Format, FClearValueBinding::Black, TexCreate_RenderTargetable | TexCreate_ShaderResource | TexCreate_UAV, TexCreate_None, false);
	
	GRenderTargetPool.CreateUntrackedElement(RenderTargetDesc, PooledRenderTarget, RenderTargetItem);

	UE_LOG(LogTemp,Warning, TEXT("Created untracked Pooled Render Target resource"));
}