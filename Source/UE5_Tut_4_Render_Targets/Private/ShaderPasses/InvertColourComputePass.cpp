// Fill out your copyright notice in the Description page of Project Settings.


#include "ShaderPasses/InvertColourComputePass.h"

// MainCS is the entry point for the compute shader - You can have multiple in a file but you have to specify separately
IMPLEMENT_SHADER_TYPE(, FInvertColourCS, TEXT("/RenderTargetTutorialShaders/private/RenderTargetTutorial.usf"), TEXT("MainCS"), SF_Compute);
