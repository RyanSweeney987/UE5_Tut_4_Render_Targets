// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShaderParameterStruct.h"

// This can be included in your FGlobalShader class
// Handy to keep them separate as you can use the same Params for multiple shaders
BEGIN_SHADER_PARAMETER_STRUCT(FInvertColourComputeParams,)
	SHADER_PARAMETER(FVector2f, TextureSize)
	SHADER_PARAMETER_SAMPLER(SamplerState, SceneColorSampler)
	SHADER_PARAMETER_RDG_TEXTURE(Texture2D, SceneColorTexture)
	SHADER_PARAMETER_RDG_TEXTURE_UAV(Texture2D, OutputTexture)
END_SHADER_PARAMETER_STRUCT()

class FInvertColourCS : public FGlobalShader
{
	DECLARE_EXPORTED_SHADER_TYPE(FInvertColourCS, Global, );
	using FParameters = FInvertColourComputeParams;
	SHADER_USE_PARAMETER_STRUCT(FInvertColourCS, FGlobalShader);
};
