#pragma once

#include "ModelManager.h"
#include "Animator.h"

namespace DgEngine::Graphics::AnimationUtil
{
	// defining a vector of bone matrices to use for skeleton calculations
	using BoneTransforms = std::vector<Math::Matrix4>;

	//compute the matrices for all the bones in the hierarchy
	void ComputeBoneTransforms(ModelId modelId, BoneTransforms& boneTransforms, const Animator* animator = nullptr);

	// to be called after ComputeBoneTransforms, draws the skeleton hierarchy
	void DrawSkeleton(ModelId modelId, const BoneTransforms& boneTransforms);
	// to be called to apply bone offsets for skinning data
	void ApplyBoneOffset(ModelId modelId, BoneTransforms& boneTransforms);
}