#pragma once
#include <RDx12/Render/FrameWork/Fw_Fr.h>
#include <REcs/World.h>
#include "../../Include/Core/ShaderDesign/ResHandler.h"
#include "../Scene/Scene.h"

#include <RImgui/ImCmpt.h>
#include <RImgui/ImTimer.h>
#include <RImgui/ImWin.h>
#include <RImgui/ImHierachy.h>

class TestScene001_Base : public RDx12::Fw_Fr
{
public:
	TestScene001_Base(HINSTANCE hInst, HWND hWnd) :Fw_Fr(hInst, hWnd) {};
	ShaderManager sdMg{ baseSwch.GetDesc().BufferCount + 1,dx12Base.GetDevice()};
	
	//Layer 
	REcs::World world;
	Scene sc;
	bool layerDirty = true;
	bool meshDirty = true;
	std::vector<std::shared_ptr<SceneNode>> mVecRenderLayer[(int)RenderLayer::Count];//在此layer中的必须要有Cmpt_Mesh,Cmpt_ObjCb
	std::vector<std::shared_ptr<SceneNode>> mVecCamLayer[(int)CameraLayer::Count];//在此layer中的必须要有Cmpt_Camera,Cmpt_PassCb
	void SetAsRenderItem(std::shared_ptr<SceneNode> nd, int meshIdx, RenderLayer layer, int objTexCbIdx);
	void SetAsCamera(std::shared_ptr<SceneNode> nd, CameraLayer layerIdx);
	void SetAsSkinRenderItem(std::shared_ptr<SceneNode> nd, int meshIdx, RenderLayer layer, int objTexCbIdx);
	void SetAsSkinRoot(std::shared_ptr<SceneNode> nd, float endTime);
	void DeleteFromAllLayer(std::shared_ptr<SceneNode> nd);
	void ClearAllNode();

	//Ui
	std::queue<std::function<void(TestScene001_Base*)>> messageQue;
	void Ui_SetDragFilePath(std::string path);
	void UpdateByMessage();
protected:
	//Private Layer
	RDx12::Struct_MeshRes<RDx12::VertexData> vecMeshRes{ RDx12::BufferType::Default };
	RDx12::Struct_MeshRes<RDx12::SkinVertexData> vecSkinMeshRes{ RDx12::BufferType::Default };
	void UpdateLayer();
	void UpdateLayerRcs(std::shared_ptr<SceneNode> nd);
	void UpdateMesh();
	void UpdateMeshRcs(std::shared_ptr<SceneNode> nd, std::vector<int>& vecMeshResIdx, std::vector<int>& vecSkinMeshResIdx);
	void UpdateAnimRcs(std::shared_ptr<SceneNode> nd, DirectX::XMFLOAT4X4 skinRootWorldMat, std::vector<DirectX::XMFLOAT4X4>* mOffsetMulAnimWorld);
	void DeleteFromLayer(std::shared_ptr<SceneNode> nd, std::vector<std::shared_ptr<SceneNode>>* vecLayer, int layerCount);
	int CheckRepeat(std::vector<int>& vec, int num);

	//Private Ui
	std::string qtDragFilePath = "";//当Qt拖拽文件时，从外部修改该值为文件路径
	int vertexNum = 0; int indexNum = 0;
	int skinVertexNum = 0; int skinIndexNum = 0;
	RImgui::ImCmpt imCmpt;
	RImgui::ImTimer imTimer;
	RImgui::ImWin imMaterial{"Material"};
	RImgui::ImHierachy<SceneNode> imHierachy;
	void Set_ShowChildWin();
	void Set_ShowMainWinUi();

	//Resource
	std::unordered_map<std::string, RDx12::LdTex> ldTexDic;
	std::unordered_map<std::string, RDx12::MkTex_Cube> mkTexCubeDic;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> cubeMapPso;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> simpleTexPso;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> brdfPso;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> screenQuatPso;

	RDx12::FrameResAlloc_RootCb<LightCb> lightCb;
	RDx12::GpuHandler depthMap;
	std::vector<PipeLine_SkyBox> vecSkyBox;int nowSkyBoxIdx = 0;
	PipeLine_GBuffer pipeLine_Gbuffer;
	PipeLine_SSAO pipeLine_SSAO;
	PipeLine_RectBlur pipeLine_RectBlur{ frvec, sdMg, hpManager };
	PipeLine_Sobel pipeLine_Sobel{ sdMg, hpManager };

	virtual void InitDetailRes() override final;
	void MakeMeshRes();
	void MakeTexRes();
	void MakeMatRes();
	void AddNodes();
	void SystemOrder();

	virtual void ReSizeOtherRes() override final;
	virtual void InputHandle(float deltatime) override final;
	virtual void Update(float deltatime) override final;
	virtual void RenderLogic(ID3D12GraphicsCommandList* cl) override final;

	void DrawLayerItemByPso(ID3D12GraphicsCommandList* cl, RenderLayer layer, ID3D12PipelineState* pso);
	void DrawLayerSkinItemByPso(ID3D12GraphicsCommandList* cl, RenderLayer layer, ID3D12PipelineState* pso);
	void DrawScreenQuatByPso(ID3D12GraphicsCommandList* cl, ID3D12PipelineState* pso);

	friend class SaveLoad;
	friend class CmptSave;
	friend class Ui_CmptParser;
	friend class RFbxMiddle2MyScene;
};

