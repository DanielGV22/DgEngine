#include "Precompiled.h"
#include "AnimationUtil.h"

#include "Color.h"
#include "SimpleDraw.h"

using namespace DgEngine;
using namespace DgEngine::Graphics;

// empty namespace for global functions isolated in the cpp file
namespace
{
	void ComputeBoneTransformRecursive(const Bone* bone, AnimationUtil::BoneTransforms& boneTransforms, const Animator* animator)
	{
		if (bone != nullptr)
		{

			// set the bone transform to the array of matrices 
			// if no animator or the bone does not have any animations use the regular to parent transform otherwise get the transform from the animator
			if (animator == nullptr || !animator->GetToParentTransform(bone, boneTransforms[bone->index]))
			{
				boneTransforms[bone->index] = bone->toParentTransform;
			}
			// if there is a parent, apply the parent's transform as well
			if (bone->parent != nullptr)
			{
				boneTransforms[bone->index] = boneTransforms[bone->index] * boneTransforms[bone->parentIndex];
			}
			// go through the children and apply their transforms
			for (const Bone* child : bone->children)
			{
				ComputeBoneTransformRecursive(child, boneTransforms, animator);
			}
		}
	}
}

void AnimationUtil::ComputeBoneTransforms(ModelId modelId, BoneTransforms& boneTransforms,const Animator* animator)
{
	const Model* model = ModelManager::Get()->GetModel(modelId);
	if (model != nullptr && model->skeleton != nullptr)
	{
		// resize to aync the number of bones with the matrices
		boneTransforms.resize(model->skeleton->bones.size());
		// generate the matrices
		ComputeBoneTransformRecursive(model->skeleton->root, boneTransforms, animator);
	}
}

void AnimationUtil::DrawSkeleton(ModelId modelId, const BoneTransforms& boneTransforms)
{
	const Model* model = ModelManager::Get()->GetModel(modelId);
	if (model != nullptr && model->skeleton != nullptr)
	{
		// iterate through the unique bone pointers
		for (const auto& bone : model->skeleton->bones)
		{
			if (bone->parent != nullptr)
			{
				// gets the bone and parent bone positions
				const Math::Vector3 bonePos = Math::GetTranslation(boneTransforms[bone->index]);
				const Math::Vector3 parentPos = Math::GetTranslation(boneTransforms[bone->parentIndex]);
				// draws a line from the bone to its parent 
				SimpleDraw::AddLine(parentPos, bonePos, Colors::HotPink);
				// adds a sphere at the bone (joint)
				SimpleDraw::AddSphere(10, 10, 0.03F,Colors::Red, bonePos);
			}
		}
	}
}

void AnimationUtil::ApplyBoneOffset(ModelId modelId, BoneTransforms& boneTransforms)
{
	const Model* model = ModelManager::Get()->GetModel(modelId);
	if (model != nullptr && model->skeleton != nullptr)
	{
		// iterate through the unique bone pointers
		for (const auto& bone : model->skeleton->bones)
		{
			// apply the offset transform to the bone transform
			boneTransforms[bone->index] = boneTransforms[bone->index] * bone->offsetTransform;
		}
	}
}
