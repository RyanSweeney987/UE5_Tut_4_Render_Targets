// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PostProcess/PostProcessInputs.h"

// This can be included in your FGlobalShader class
// Handy to keep them separate as you can use the same Params for multiple shaders
BEGIN_SHADER_PARAMETER_STRUCT(FInvertColourParams,)
	SHADER_PARAMETER(FVector2f, TextureSize)
	SHADER_PARAMETER_SAMPLER(SamplerState, SceneColorSampler)
	SHADER_PARAMETER_RDG_TEXTURE(Texture2D, SceneColorTexture)

	RENDER_TARGET_BINDING_SLOTS()
END_SHADER_PARAMETER_STRUCT()

class FInvertColourPS : public FGlobalShader
{
	DECLARE_EXPORTED_SHADER_TYPE(FInvertColourPS, Global, );
	using FParameters = FInvertColourParams;
	SHADER_USE_PARAMETER_STRUCT(FInvertColourPS, FGlobalShader);
};