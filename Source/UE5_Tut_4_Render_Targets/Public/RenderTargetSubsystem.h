// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "Rendering/RenderTargetSceneViewExtension.h"
#include "RenderTargetSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class UE5_TUT_4_RENDER_TARGETS_API URenderTargetSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

private:
	TSharedPtr< class FRenderTargetSceneViewExtension, ESPMode::ThreadSafe > CustomSceneViewExtension;

	// The source render target texture asset
	UPROPERTY()
	TObjectPtr<UTextureRenderTarget2D> RenderTargetSource = nullptr;
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
};
