#include "../../../Include/Core/EcsCmpt/Cmpt_SkinMesh.h"
#include <RWin32/FileHandler.h>

std::function<void(Cmpt_SkinMesh*)> Cmpt_SkinMesh::Update(RDx12::FrVec& fv, ID3D12Device4* dev, std::vector<DirectX::XMFLOAT4X4>& vecSkinMeshBindWorldMat)
{
	return std::function
	(
		[&fv, dev, &vecSkinMeshBindWorldMat](Cmpt_SkinMesh* skinMeshCt)
		{
			if (skinMeshCt->mOffsetMulAnimWorld!=nullptr)
			{
				for (int i = 0; i < skinMeshCt->mOffsetMulAnimWorld->size(); i++)
				{
					skinMeshCt->mSkinnedCb.data.gBoneTransforms[i] = RDx12::MathHelper::MulMat(RDx12::MathHelper::MulMat(vecSkinMeshBindWorldMat[skinMeshCt->mSkinMeshIdx], (*skinMeshCt->mOffsetMulAnimWorld)[i]),skinMeshCt->mSkinRootWorldMat);
					skinMeshCt->mSkinnedCb.data.gBoneTransforms[i] = RDx12::MathHelper::TMat(skinMeshCt->mSkinnedCb.data.gBoneTransforms[i]);
				}
				skinMeshCt->mSkinnedCb.Update(fv.GetNowFrIdx());

				/*
				for (int i = 0; i < sc.vecSkinMesh[skinMeshCt->mSkinMeshIdx].vecVertex.size(); i++)
				{
					RDx12::SkinVertexData vt = sc.vecSkinMesh[skinMeshCt->mSkinMeshIdx].vecVertex[i];
					DirectX::XMFLOAT3 pos(0, 0, 0);

					if (vt.boneIdx.x != -1)
					{
						float weight[4];
						weight[0] = vt.boneWeight.x;
						weight[1] = vt.boneWeight.y;
						weight[2] = vt.boneWeight.z;
						weight[3] = 1 - weight[0] - weight[1] - weight[2];
						int boneIdx[4];
						boneIdx[0] = vt.boneIdx.x;
						boneIdx[1] = vt.boneIdx.y;
						boneIdx[2] = vt.boneIdx.z;
						boneIdx[3] = vt.boneIdx.w;
						for (int j = 0; j < 4; j++)
						{
							if (boneIdx[j] == -1) { break; }
							pos = RDx12::MathHelper::F3AddF3(
								RDx12::MathHelper::F3MulF(
									RDx12::MathHelper::F3MulMat(
										vt.pos,
										RDx12::MathHelper::TMat(skinMeshCt->mSkinnedCb.data.gBoneTransforms[boneIdx[j]])
									),
									weight[j]
								),
								pos
							);
						}
					}
					WIN32DEBUG(L"vertex: %d   src: %.2f %.2f %.2f   weight: %.2f %.2f %.2f   ", i, vt.pos.x, vt.pos.y, vt.pos.z, vt.boneWeight.x, vt.boneWeight.y, vt.boneWeight.z);
					WIN32DEBUG(L"boneIdx: %d %d %d %d res: %.2f %.2f %.2f\n ", vt.boneIdx.x, vt.boneIdx.y, vt.boneIdx.z, vt.boneIdx.w, pos.x, pos.y, pos.z);
				}
				*/
			}
		}
	);
}

void Cmpt_SkinMesh::AllocRes(RDx12::Sign_RootCb<SkinnedCb>& sd_SkinnedCb)
{
	mSkinnedCb = sd_SkinnedCb.Get();
}

void Cmpt_SkinMesh::SetGraphicsRootConstantBufferView(ID3D12GraphicsCommandList* cl, RDx12::FrVec& fv)
{
	mSkinnedCb.SetGraphicsRootConstantBufferView(fv.GetNowFrIdx(), cl);
}

void Cmpt_SkinMesh::RSerialize_Save(std::ofstream& outFile)
{
	RWin32::Bin_WriteData(outFile, mSkinMeshIdx);
}

void Cmpt_SkinMesh::RSerialize_Load(std::ifstream& inFile)
{
	mSkinMeshIdx = RWin32::Bin_LoadData<int>(inFile);
}
