// Fill out your copyright notice in the Description page of Project Settings.


#include "RenderTargetSubsystem.h"

#include "EngineUtils.h"
#include "SceneViewExtension.h"
#include "Engine/TextureRenderTarget2D.h"

void URenderTargetSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	CustomSceneViewExtension = FSceneViewExtensions::NewExtension<FRenderTargetSceneViewExtension>();

	if(UTextureRenderTarget2D* RenderTarget = LoadObject<UTextureRenderTarget2D>(nullptr, TEXT("/Script/Engine.TextureRenderTarget2D'/UE5_Tut_4_Render_Targets/RT_Target.RT_Target'"))) {
		CustomSceneViewExtension->SetRenderTarget(RenderTarget);
	}
}

void URenderTargetSubsystem::Deinitialize()
{
	Super::Deinitialize();
	
	CustomSceneViewExtension.Reset();
	CustomSceneViewExtension = nullptr;
}
