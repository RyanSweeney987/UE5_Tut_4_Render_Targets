// Fill out your copyright notice in the Description page of Project Settings.

#include "ShaderPasses/InvertColourRenderPass.h"

// MainPS is the entry point for the pixel shader - You can have multiple in a file but you have to specify separately
IMPLEMENT_SHADER_TYPE(, FInvertColourPS, TEXT("/RenderTargetTutorialShaders/private/RenderTargetTutorial.usf"), TEXT("MainPS"), SF_Pixel);