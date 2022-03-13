#pragma once
#include <REcs/World.h>
#include "../Core/EcsCmpt/Cmpt_Trans.h"
#include "../Core/EcsCmpt/Cmpt_ObjCb.h"
#include "../Core/EcsCmpt/Cmpt_Camera.h"
#include "../Core/EcsCmpt/Cmpt_PassCb.h"
#include "../Core/EcsCmpt/Cmpt_Mesh.h"
#include "../Core/EcsCmpt/Cmpt_SkinMesh.h"
#include "../Core/EcsCmpt/Cmpt_SkinRoot.h"
#include "../Core/EcsCmpt/Cmpt_CmdParser.h"

class SceneNode :public std::enable_shared_from_this<SceneNode>
{
public:
	SceneNode(std::string name, REcs::World& world) :name(name), world(&world)
	{
		entity = world.etm.CreateEntity<Cmpt_Trans>();
	};

	std::string GetCmptStr() { return world->etm.GetEntityArType(entity)->name; }

	template<typename Cmpt>
	Cmpt* Get() { return world->etm.Get<Cmpt>(entity); }

	template<typename Cmpt>
	void Set(Cmpt* cmpt) { world->etm.Set<Cmpt>(entity, cmpt); }

	template<typename... Cmpt>
	void Add() { world->etm.Add<Cmpt...>(entity); }

	template<typename... Cmpt>
	void Remove() { world->etm.Remove<Cmpt...>(entity); }

	REcs::Archetype* GetArch() { return world->etm.GetEntityArType(entity); }

	virtual ~SceneNode()
	{
		world->etm.DestoryEntity(entity);
		std::cout << name << " Release" << std::endl;
	}

	/*
	对物体进行世界坐标系的Tran更改，将世界坐标系的更改反映到相对坐标系进行记录
	*/
	void ChangeWorldTrans(DirectX::XMFLOAT3 dPos, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT4 dQuat)
	{
		Cmpt_Trans* trans = Get<Cmpt_Trans>();
		DirectX::XMFLOAT4X4 toRoot = RDx12::MathHelper::Identity4x4();
		if (parent != nullptr)
		{
			toRoot = parent->Get<Cmpt_Trans>()->mWorldMat;
		}

		DirectX::XMFLOAT3 dOffsetPos = RDx12::MathHelper::VecTransCoord(dPos, toRoot, false);
		DirectX::XMFLOAT4 dOffsetQuat = RDx12::MathHelper::QuatTransCoord(dQuat, toRoot);


		SetOffsetTrans(
			RDx12::MathHelper::F3AddF3(trans->mOffsetPos, dOffsetPos),
			RDx12::MathHelper::F3MulF3(trans->mOffsetScale, scale),
			RDx12::MathHelper::QuatMulQuat(trans->mOffsetQuat, dOffsetQuat)
		);
	}

	void SetOffsetTrans(DirectX::XMFLOAT3 offsetPos, DirectX::XMFLOAT3 offsetScale, DirectX::XMFLOAT4 offsetQuat)
	{
		Cmpt_Trans* trans = Get<Cmpt_Trans>();
		trans->mOffsetPos = offsetPos;
		trans->mOffsetScale = offsetScale;
		trans->mOffsetQuat = offsetQuat;
		UpdateWorldTranByOffset();
	}

	void UpdateWorldTranByOffset()
	{
		Cmpt_Trans* trans = Get<Cmpt_Trans>();
		if (parent != nullptr)
		{
			trans->ComputeGobalTrans(parent->Get<Cmpt_Trans>());
		}
		else
		{
			trans->ComputeGobalTrans(nullptr);
		}
		for (auto childNd : child)
		{
			childNd->UpdateWorldTranByOffset();
		}
	}

	void SetWorldTrans(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 euler)
	{
		DirectX::XMFLOAT4 quat = RDx12::MathHelper::GetRollPitchYawQuatByDeg(euler.x, euler.y, euler.z);
		if (parent != nullptr)
		{
			Cmpt_Trans* trans = Get<Cmpt_Trans>();
			
			DirectX::XMStoreFloat4x4(
				&trans->mWorldMat,
				RDx12::MathHelper::GetTransMatByF(
					{ 0,0,0 },
					{ 0,0,0,1 },
					scale,
					{ 0,0,0 },
					quat,
					pos
				)
			);
			trans->mQuat = quat;
			trans->mPos = pos;

			UpdateOffsetByWorld();
		}
		else
		{
			SetOffsetTrans(pos, scale, quat);
		}
	}

	void UpdateOffsetByWorld()
	{
		if (parent != nullptr)
		{
			Cmpt_Trans* trans = Get<Cmpt_Trans>();
			Cmpt_Trans* fatherTrans = parent->Get<Cmpt_Trans>();

			//更改父子关系后保留子节点的offset，世界姿态发生更改，如何在更改父类后保持世界姿态有待后续开发
			DirectX::XMFLOAT4X4 newOffSetMat = RDx12::MathHelper::MulMat(trans->mWorldMat, RDx12::MathHelper::InverMat(fatherTrans->mWorldMat));
			DirectX::XMFLOAT3 pos;
			DirectX::XMFLOAT4 quat;
			RDx12::MathHelper::MatDeCompose(newOffSetMat, pos, trans->mOffsetScale, quat);

			trans->mOffsetPos = RDx12::MathHelper::F3MulMat(trans->mPos, RDx12::MathHelper::InverMat(fatherTrans->mWorldMat));
			trans->mOffsetQuat = RDx12::MathHelper::QuatMulQuat(trans->mQuat, RDx12::MathHelper::QuatInver(fatherTrans->mQuat));

			DirectX::XMFLOAT4X4 SRTMat;
			DirectX::XMStoreFloat4x4(
				&SRTMat,
				RDx12::MathHelper::GetTransMatByF(
					{ 0,0,0 },
					{ 0,0,0,1 },
					trans->mOffsetScale,
					{ 0,0,0 },
					trans->mOffsetQuat,
					trans->mOffsetPos
				)
			);

			trans->mKeepWorldMat = RDx12::MathHelper::MulMat(newOffSetMat, RDx12::MathHelper::InverMat(SRTMat));

			UpdateWorldTranByOffset();
		}
	}

	void ChangeSelect(bool newSelected)
	{
		is_selected = newSelected;
		if (Get<Cmpt_ObjCb>() != nullptr)
		{
			Get<Cmpt_ObjCb>()->mObjCb.data.gIsSelected = newSelected;
			Get<Cmpt_ObjCb>()->MustDirty();
		}
	}

	bool IsRenderNode()
	{
		if((Get<Cmpt_Mesh>() != nullptr || Get<Cmpt_SkinMesh>() != nullptr) && Get<Cmpt_ObjCb>() != nullptr)
		{
			return true;
		}
		return false;
	}

	bool IsCameraNode()
	{
		if (Get<Cmpt_Camera>() != nullptr && Get<Cmpt_PassCb>() != nullptr)
		{
			return true;
		}
		return false;
	}

	std::string name;
	std::shared_ptr<SceneNode> parent;
	std::list<std::shared_ptr<SceneNode>> child;
	bool is_selected = false;
protected:
	REcs::World* world;
	REcs::Entity entity;
};