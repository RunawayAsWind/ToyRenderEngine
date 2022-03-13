#pragma once
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <RWin32/Win32Debug.h>
#include "Quat2Euler.h"
#include <cmath>
namespace RDx12
{
	class MathHelper
	{
	public:

		/*
			----------------Base----------------
		*/
		static DirectX::XMFLOAT3 Foward()
		{
			return DirectX::XMFLOAT3(0, 0, 1);
		}

		static DirectX::XMFLOAT3 Right()
		{
			return DirectX::XMFLOAT3(1, 0, 0);
		}

		static DirectX::XMFLOAT3 Up()
		{
			return DirectX::XMFLOAT3(0, 1, 0);
		}

		static DirectX::XMFLOAT4 FowardF4()
		{
			return DirectX::XMFLOAT4(0, 0, 1, 1);
		}

		static DirectX::XMFLOAT4 RightF4()
		{
			return DirectX::XMFLOAT4(1, 0, 0, 1);
		}

		static DirectX::XMFLOAT4 UpF4()
		{
			return DirectX::XMFLOAT4(0, 1, 0, 1);
		}

		static DirectX::XMVECTOR FowardVec()
		{
			auto vec = DirectX::XMFLOAT4(0, 0, 1, 1);
			return DirectX::XMLoadFloat4(&vec);
		}

		static DirectX::XMVECTOR RightVec()
		{
			auto vec = DirectX::XMFLOAT4(1, 0, 0, 1);
			return DirectX::XMLoadFloat4(&vec);
		}

		static DirectX::XMVECTOR UpVec()
		{
			auto vec = DirectX::XMFLOAT4(0, 1, 0, 1);
			return DirectX::XMLoadFloat4(&vec);
		}

		static bool F3Equal(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b)
		{
			return DirectX::XMVector3Equal(DirectX::XMLoadFloat3(&a), DirectX::XMLoadFloat3(&b));
		}

		static bool F4Equal(DirectX::XMFLOAT4 a, DirectX::XMFLOAT4 b)
		{
			return DirectX::XMVector4Equal(DirectX::XMLoadFloat4(&a), DirectX::XMLoadFloat4(&b));
		}

		static DirectX::XMFLOAT3 F3MulF(DirectX::XMFLOAT3 a, float b)
		{
			DirectX::XMFLOAT3 res;
			DirectX::XMStoreFloat3(
				&res, 
				DirectX::XMVectorMultiply(
					DirectX::XMLoadFloat3(&a),
					DirectX::XMVECTORF32{ b,b,b,b }
				)
			);
			return res;
		}

		static DirectX::XMFLOAT3 F3AddF3(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b)
		{
			DirectX::XMFLOAT3 res;
			DirectX::XMStoreFloat3(
				&res,
				DirectX::XMVectorAdd(
					DirectX::XMLoadFloat3(&a),
					DirectX::XMLoadFloat3(&b)
				)
			);
			return res;
		}

		static DirectX::XMFLOAT3 F3VAddScale(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b, float s)
		{
			DirectX::XMFLOAT3 res;
			DirectX::XMStoreFloat3(
				&res,
				DirectX::XMVectorAdd(
					DirectX::XMLoadFloat3(&a),
					DirectX::XMVectorScale(DirectX::XMLoadFloat3(&b),s)
				)
			);
			return res;
		}

		static DirectX::XMFLOAT3 F3SubF3(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b)
		{
			DirectX::XMFLOAT3 res;
			DirectX::XMStoreFloat3(
				&res,
				DirectX::XMVectorSubtract(
					DirectX::XMLoadFloat3(&a),
					DirectX::XMLoadFloat3(&b)
				)
			);
			return res;
		}

		static DirectX::XMFLOAT3 F3CrossF3(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b)
		{
			DirectX::XMFLOAT3 res;
			DirectX::XMStoreFloat3(
				&res,
				DirectX::XMVector3Cross(
					DirectX::XMLoadFloat3(&a),
					DirectX::XMLoadFloat3(&b)
				)
			);
			return res;
		}

		static DirectX::XMFLOAT3 F3MulF3(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b)
		{
			DirectX::XMFLOAT3 res;
			DirectX::XMStoreFloat3(
				&res,
				DirectX::XMVectorMultiply(
					DirectX::XMLoadFloat3(&a),
					DirectX::XMLoadFloat3(&b)
				)
			);
			return res;
		}

		static DirectX::XMFLOAT3 F3DivideF3(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b)
		{
			DirectX::XMFLOAT3 res;
			DirectX::XMStoreFloat3(
				&res,
				DirectX::XMVectorDivide(
					DirectX::XMLoadFloat3(&a),
					DirectX::XMLoadFloat3(&b)
				)
			);
			return res;
		}

		static DirectX::XMFLOAT3 F3VSubScale(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b, float s)
		{
			DirectX::XMFLOAT3 res;
			DirectX::XMStoreFloat3(
				&res,
				DirectX::XMVectorSubtract(
					DirectX::XMLoadFloat3(&a),
					DirectX::XMVectorScale(DirectX::XMLoadFloat3(&b), s)
				)
			);
			return res;
		}

		static DirectX::XMFLOAT3 F3Normalize(DirectX::XMFLOAT3 f3)
		{
			DirectX::XMFLOAT3 res;
			DirectX::XMStoreFloat3(&res, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&f3)));
			return res;
		}

		static DirectX::XMFLOAT4 F4Normalize(DirectX::XMFLOAT4 f4)
		{
			DirectX::XMFLOAT4 res;
			DirectX::XMStoreFloat4(&res, DirectX::XMVector4Normalize(DirectX::XMLoadFloat4(&f4)));
			return res;
		}

		/*
			----------------Matrix---------------
		*/
		static DirectX::XMFLOAT4X4 Identity4x4()
		{
			DirectX::XMFLOAT4X4 mat;
			DirectX::XMStoreFloat4x4(&mat, DirectX::XMMatrixIdentity());
			return mat;
		}

		static bool F4x4Equal(DirectX::XMFLOAT4X4 a, DirectX::XMFLOAT4X4 b)
		{
			return (
				a._11 == b._11 && a._12 == b._12 && a._13 == b._13 && a._14 == b._14 &&
				a._21 == b._21 && a._22 == b._22 && a._23 == b._23 && a._24 == b._24 &&
				a._31 == b._31 && a._32 == b._32 && a._33 == b._33 && a._34 == b._34 &&
				a._41 == b._41 && a._42 == b._42 && a._43 == b._43 && a._44 == b._44
			);
		}

		static DirectX::XMFLOAT4X4 InverMat(DirectX::XMFLOAT4X4 mat)
		{
			DirectX::XMFLOAT4X4 res;
			DirectX::XMMATRIX matVec = DirectX::XMLoadFloat4x4(&mat);
			DirectX::XMStoreFloat4x4(
				&res,
				DirectX::XMMatrixInverse(
					&DirectX::XMMatrixDeterminant(matVec),
					matVec
				)
			);
			return res;
		}

		static DirectX::XMFLOAT4X4 TMat(DirectX::XMFLOAT4X4 mat)
		{
			DirectX::XMFLOAT4X4 res;
			DirectX::XMStoreFloat4x4(&res, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&mat)));
			return res;
		}

		//法向量进行到世界坐标的变换，需要乘以worldmat的逆转置矩阵
		static DirectX::XMMATRIX InverTMat(DirectX::FXMMATRIX mat)
		{
			DirectX::XMMATRIX matVec = mat;
			matVec.r[3] = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
			DirectX::XMVECTOR det = DirectX::XMMatrixDeterminant(matVec);
			
			return DirectX::XMMatrixTranspose(
				DirectX::XMMatrixInverse(
					&DirectX::XMMatrixDeterminant(matVec),
					matVec
				)
			);
		}

		static DirectX::XMFLOAT4 F4MulMat(DirectX::XMFLOAT4 vec, DirectX::XMFLOAT4X4 mat)
		{
			DirectX::XMFLOAT4 res;
			DirectX::XMStoreFloat4(
				&res,
				DirectX::XMVector4Transform(
					DirectX::XMLoadFloat4(&vec),
					DirectX::XMLoadFloat4x4(&mat)
				)
			);
			return res;
		}

		static DirectX::XMFLOAT3 F3MulMat(DirectX::XMFLOAT3 vec, DirectX::XMFLOAT4X4 mat)
		{
			DirectX::XMFLOAT3 res;
			DirectX::XMStoreFloat3(
				&res, 
				DirectX::XMVector3Transform(
					DirectX::XMLoadFloat3(&vec),
					DirectX::XMLoadFloat4x4(&mat)
				)
			);
			return res;
		}

		static DirectX::XMFLOAT4X4 MulMat(DirectX::XMFLOAT4X4 a, DirectX::XMFLOAT4X4 b)
		{
			DirectX::XMFLOAT4X4 res;
			DirectX::XMStoreFloat4x4(
				&res,
				DirectX::XMMatrixMultiply(
					DirectX::XMLoadFloat4x4(&a),
					DirectX::XMLoadFloat4x4(&b)
				)
			);
			return res;
		}

		static DirectX::XMFLOAT4X4 MulMat(DirectX::XMFLOAT4X4 mat1, DirectX::XMMATRIX mat2)
		{
			DirectX::XMFLOAT4X4 res;
			DirectX::XMStoreFloat4x4(&res, DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&mat1),mat2));
			return res;
		}

		static DirectX::XMFLOAT4X4 MulMat(DirectX::XMMATRIX mat1, DirectX::XMMATRIX mat2)
		{
			DirectX::XMFLOAT4X4 res;
			DirectX::XMStoreFloat4x4(&res, DirectX::XMMatrixMultiply(mat1, mat2));
			return res;
		}

		static DirectX::XMFLOAT4X4 MulMat(DirectX::XMMATRIX mat1, DirectX::XMFLOAT4X4 mat2)
		{
			DirectX::XMFLOAT4X4 res;
			DirectX::XMStoreFloat4x4(&res, DirectX::XMMatrixMultiply(mat1, DirectX::XMLoadFloat4x4(&mat2)));
			return res;
		}

		static void MatDeCompose(DirectX::XMFLOAT4X4 mat, DirectX::XMFLOAT3& pos, DirectX::XMFLOAT3& scale, DirectX::XMFLOAT4& quat)
		{
			DirectX::XMVECTOR posVec, scaleVec, quatVec;
			DirectX::XMMatrixDecompose(&scaleVec, &quatVec, &posVec, DirectX::XMLoadFloat4x4(&mat));
			DirectX::XMStoreFloat3(&pos, posVec);
			DirectX::XMStoreFloat3(&scale, scaleVec);
			DirectX::XMStoreFloat4(&quat, quatVec);
		}

		static DirectX::XMFLOAT4X4 GetRotMatByQuat(DirectX::XMFLOAT4 quat)
		{
			DirectX::XMFLOAT4X4 rotMat;
			DirectX::XMStoreFloat4x4(&rotMat, DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&quat)));
			return rotMat;
		}



		/*
			Rotation
		*/
		static float Rad2Deg(float rad)
		{
			return DirectX::XMConvertToDegrees(rad);
		}

		static float Deg2Rad(float deg)
		{
			return  DirectX::XMConvertToRadians(deg);
		}

		static DirectX::XMFLOAT3 Rad2Deg(DirectX::XMFLOAT3 rad)
		{
			return DirectX::XMFLOAT3(Rad2Deg(rad.x), Rad2Deg(rad.y), Rad2Deg(rad.z));
		}

		static DirectX::XMFLOAT3 Deg2Rad(DirectX::XMFLOAT3 deg)
		{
			return DirectX::XMFLOAT3(Deg2Rad(deg.x), Deg2Rad(deg.y), Deg2Rad(deg.z));
		}

		static DirectX::XMFLOAT4 GetQuatVecA2VecB(DirectX::XMFLOAT3 vecA, DirectX::XMFLOAT3 vecB)
		{
			DirectX::XMFLOAT4 quat{0,0,0,1};
			DirectX::XMVECTOR unitA = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&vecA));
			DirectX::XMVECTOR unitB = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&vecB));

			float cosRad = DirectX::XMVectorGetX(DirectX::XMVector3AngleBetweenVectors(unitA, unitB));
			DirectX::XMStoreFloat4(&quat, DirectX::XMQuaternionRotationAxis(DirectX::XMVector3Cross(unitA, unitB), cosRad));
			return quat;
		}

		static DirectX::XMFLOAT4 GetRollPitchYawQuatByVec(DirectX::XMFLOAT3 endP, DirectX::XMFLOAT3 startP)
		{
			DirectX::XMFLOAT4 quat;
			DirectX::XMVECTOR vec = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&endP), DirectX::XMLoadFloat3(&startP)));
			DirectX::XMFLOAT3 unitVec;
			DirectX::XMStoreFloat3(&unitVec, vec);
			DirectX::XMFLOAT3 forwd = Right();
			DirectX::XMStoreFloat4(&quat, DirectX::XMQuaternionRotationAxis(DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&forwd),vec),acos(unitVec.x)));
			DirectX::XMFLOAT3 test = RotVecByQuat(forwd, quat);

			std::cout << std::endl << endP.x << " " << endP.y << " " << endP.z << " -  " << startP.x << " " << startP.y << " " << startP.z <<"  =  "<<unitVec.x<<" "<< unitVec.y<<" "<< unitVec.z<<"          Test: "<<test.x<<" "<<test.y<<" "<<test.z<< std::endl;
			return quat;
		}

		static DirectX::XMFLOAT4 GetRollPitchYawQuatByVec(DirectX::XMFLOAT3 vec)
		{
			DirectX::XMFLOAT4 quat;
			DirectX::XMStoreFloat4(&quat,DirectX::XMQuaternionRotationRollPitchYawFromVector(DirectX::XMLoadFloat3(&vec)));
			return quat;
		}

		static DirectX::XMFLOAT4 GetAngleAxiQuat(DirectX::XMFLOAT3 axis, float angleRad)
		{
			DirectX::XMFLOAT4 quat;
			DirectX::XMStoreFloat4(&quat, DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat3(&axis), angleRad));
			return quat;
		}

		static DirectX::XMFLOAT4 GetRollPitchYawQuatByDeg(float pitchDeg, float yawDeg, float rollDeg)
		{
			DirectX::XMFLOAT4 quat;
			DirectX::XMStoreFloat4(&quat, DirectX::XMQuaternionRotationRollPitchYaw(Deg2Rad(pitchDeg), Deg2Rad(yawDeg), Deg2Rad(rollDeg)));
			return quat;
		}

		static DirectX::XMFLOAT4X4 GetRollPitchYawMatByDeg(float pitchDeg, float yawDeg, float rollDeg)
		{
			DirectX::XMFLOAT4X4 rotMat;
			DirectX::XMStoreFloat4x4(&rotMat, DirectX::XMMatrixRotationRollPitchYaw(Deg2Rad(pitchDeg), Deg2Rad(yawDeg), Deg2Rad(rollDeg)));
			return rotMat;
		}

		static DirectX::XMFLOAT4 GetYawPitchQuat(float yawDeg, DirectX::XMFLOAT3 yawAxi, float pitchDeg, DirectX::XMFLOAT3 pitchAxi)
		{
			DirectX::XMVECTOR yawQuat = DirectX::XMQuaternionRotationAxis(
				DirectX::XMLoadFloat3(&yawAxi),
				DirectX::XMConvertToRadians(yawDeg)
			);
			DirectX::XMVECTOR pitchQuat = DirectX::XMQuaternionRotationAxis(
				DirectX::XMLoadFloat3(&pitchAxi),
				DirectX::XMConvertToRadians(pitchDeg)
			);
			DirectX::XMFLOAT4 res;
			DirectX::XMStoreFloat4(&res, DirectX::XMQuaternionMultiply(yawQuat, pitchQuat));
			return res;
		}

		static DirectX::XMFLOAT4 QuatMulQuat(DirectX::XMFLOAT4 quat1, DirectX::XMFLOAT4 quat2)
		{
			DirectX::XMFLOAT4 res;
			DirectX::XMStoreFloat4(&res,DirectX::XMQuaternionMultiply(DirectX::XMLoadFloat4(&quat1),DirectX::XMLoadFloat4(&quat2)));
			return res;
		}

		static DirectX::XMFLOAT4 QuatInver(DirectX::XMFLOAT4 quat)
		{
			DirectX::XMFLOAT4 res;
			DirectX::XMStoreFloat4(
				&res,
				DirectX::XMQuaternionInverse(DirectX::XMLoadFloat4(&quat))
			);
			return res;
		}

		static DirectX::XMFLOAT4 QuatMulQuatInver(DirectX::XMFLOAT4 quat1, DirectX::XMFLOAT4 quat2)
		{
			DirectX::XMFLOAT4 res;
			DirectX::XMStoreFloat4(
				&res,
				DirectX::XMQuaternionMultiply(
					DirectX::XMLoadFloat4(&quat1),
					DirectX::XMQuaternionInverse(DirectX::XMLoadFloat4(&quat2))
				)
			);
			return res;
		}

		static DirectX::XMFLOAT3 Quat2EluerDeg(DirectX::XMFLOAT4 q)
		{
			Quaternion qt(q.x, q.y, q.z, q.w);
			qt.normalize();
			double res[3];
			quaternion2Euler(qt, res, RotSeq::yxz);//得到得结果是enum的倒序（zxy）
			return Rad2Deg(DirectX::XMFLOAT3(res[1], res[2], res[0]));
		}

		static DirectX::XMFLOAT3 RotVecByQuat(DirectX::XMFLOAT3 vec, DirectX::XMFLOAT4 quat)
		{
			DirectX::XMFLOAT3 res;
			DirectX::XMStoreFloat3(
				&res,
				DirectX::XMVector3Transform(
					DirectX::XMLoadFloat3(&vec),
					DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&quat))
				)
			);

			return res;
		}

		//D3DXVec3TransformNormal( ) 用于矢量的变换，第四维分量为0。
		//D3DXVec3TransformCoord( )  用于坐标的变换，第四维分量为1。
		static DirectX::XMFLOAT3 TransNormal(DirectX::XMFLOAT3 vec,DirectX::XMFLOAT4X4 mat)
		{
			DirectX::XMFLOAT3 res;
			DirectX::XMStoreFloat3(&res, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&vec), DirectX::XMLoadFloat4x4(&mat)));
			return res;
		}

		/*
		当前坐标系下的方向向量转化成mat坐标系下的方向向量
		TransformNormal解决了不等比缩放时法线不垂直的问题
		设 a2bMat是变换顶点从a到b的变换矩阵，为了保证法线变换后仍然垂直，normalMat = Transpose（Inver（a2bMat））
		这里使用normalMat的逆矩阵进行坐标变换，保证转化为目标坐标系后通过乘以mat可以解压复原出完全相同的方向向量
		https://www.scratchapixel.com/lessons/mathematics-physics-for-computer-graphics/geometry/transforming-normals
		*/
		static DirectX::XMFLOAT3 VecTransCoord(DirectX::XMFLOAT3 aVec, DirectX::XMFLOAT4X4 a2bMat,bool normalize)
		{
			DirectX::XMMATRIX mat = DirectX::XMLoadFloat4x4(&a2bMat);
			DirectX::XMMATRIX inverMat = DirectX::XMMatrixInverse(&DirectX::XMMatrixDeterminant(mat), mat);//乘以变换矩阵的逆矩阵=变换当前坐标到变换矩阵对应的坐标系
			DirectX::XMVECTOR res = DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&aVec), inverMat);
			DirectX::XMFLOAT3 bVec;
			if (normalize == true)
			{
				DirectX::XMStoreFloat3(&bVec, DirectX::XMVector3Normalize(res));
			}
			else
			{
				DirectX::XMStoreFloat3(&bVec, res);
			}
			
			return bVec;
		}

		//将当前坐标系下的四元数转化成mat坐标系下的四元数
		static DirectX::XMFLOAT4 QuatTransCoord(DirectX::XMFLOAT4 aQuat, DirectX::XMFLOAT4X4 a2bMat)
		{
			DirectX::XMVECTOR aQuatAxis; float quatAngle;
			DirectX::XMQuaternionToAxisAngle(&aQuatAxis, &quatAngle, DirectX::XMLoadFloat4(&aQuat));

			//当前坐标系下的方向向量转化成mat坐标系下的方向向量
			DirectX::XMMATRIX mat = DirectX::XMLoadFloat4x4(&a2bMat);
			DirectX::XMMATRIX inverMat = DirectX::XMMatrixInverse(&DirectX::XMMatrixDeterminant(mat), mat);
			DirectX::XMVECTOR bQuatAxis = DirectX::XMVector3Normalize(DirectX::XMVector3TransformNormal(aQuatAxis,inverMat));//乘以变换矩阵的逆矩阵=变换当前坐标到变换矩阵对应的坐标系

			DirectX::XMFLOAT4 bQuat = { 0,0,0,1 };
			if (!DirectX::XMVector3Equal(bQuatAxis, DirectX::XMVectorZero()))
			{
				DirectX::XMStoreFloat4(&bQuat, DirectX::XMQuaternionRotationAxis(bQuatAxis, quatAngle));
			}
			return bQuat;
		}

		static DirectX::XMMATRIX GetTransMatByF(
			DirectX::XMFLOAT3 ScalingOrigin,
			DirectX::XMFLOAT4 ScalingOrientationQuaternion,
			DirectX::XMFLOAT3 Scaling,
			DirectX::XMFLOAT3 RotationOrigin,
			DirectX::XMFLOAT4 RotationQuaternion,
			DirectX::XMFLOAT3 Translation
		)
		{

			return DirectX::XMMatrixTransformation(
				DirectX::XMLoadFloat3(&ScalingOrigin),
				DirectX::XMLoadFloat4(&ScalingOrientationQuaternion),
				DirectX::XMLoadFloat3(&Scaling),
				DirectX::XMLoadFloat3(&RotationOrigin),
				DirectX::XMLoadFloat4(&RotationQuaternion),
				DirectX::XMLoadFloat3(&Translation)
			);
		}

		static DirectX::XMFLOAT4 TransMatPrint(
			DirectX::XMFLOAT4 Vec,
			DirectX::XMFLOAT3 ScalingOrigin,
			DirectX::XMFLOAT4 ScalingOrientationQuaternion,
			DirectX::XMFLOAT3 Scaling,
			DirectX::XMFLOAT3 RotationOrigin,
			DirectX::XMFLOAT4 RotationQuaternion,
			DirectX::XMFLOAT3 Translation,
			bool debug = false
		)
		{
			DirectX::XMMATRIX mat = GetTransMatByF(
				ScalingOrigin,
				ScalingOrientationQuaternion,
				Scaling,
				RotationOrigin,
				RotationQuaternion,
				Translation
			);
			DirectX::XMFLOAT4 res;
			DirectX::XMStoreFloat4(&res, DirectX::XMVector4Transform(DirectX::XMLoadFloat4(&Vec), mat));

			if (debug)
			{
				ShowMat(mat);
				ShowFloat4(res);
			}
			return res;
		}

		/*
		根据一个三角面计算三个共面顶点的副切线，副法线
		输入：三个顶点的pos,uv;
		输出：tan binorm
		| T.x T.y T.z |               1              |  dACv  -dABv | | AB.x AB.y AB.z |
		|             | = ------------------------   |              | |                |
		| B.x B.y B.z |  dABu * dACv - dABv * dACu   | -dACu   dABu | | AC.x AC.y AC.z |
		*/
		static void ComputeTanAndBiNorm(
			DirectX::XMFLOAT3& tan, DirectX::XMFLOAT3& biNorm,
			DirectX::XMFLOAT3 posA, DirectX::XMFLOAT3 posB, DirectX::XMFLOAT3 posC,
			DirectX::XMFLOAT2 uvA, DirectX::XMFLOAT2 uvB, DirectX::XMFLOAT2 uvC)
		{
			float dACv = uvC.y - uvA.y;float dABv = uvB.y - uvA.y;     DirectX::XMFLOAT3 vecAB = DirectX::XMFLOAT3(posB.x - posA.x, posB.y - posA.y, posB.z - posA.z);
			float dACu = uvC.x - uvA.x;float dABu = uvB.x - uvA.x;     DirectX::XMFLOAT3 vecAC = DirectX::XMFLOAT3(posC.x - posA.x, posC.y - posA.y, posC.z - posA.z);
			
			tan = DirectX::XMFLOAT3(dACv * vecAB.x - dABv * vecAC.x, dACv * vecAB.y - dABv * vecAC.y, dACv * vecAB.z - dABv * vecAC.z);
			biNorm = DirectX::XMFLOAT3(-dACu * vecAB.x + dABu * vecAC.x, -dACu * vecAB.y + dABu * vecAC.y, -dACu * vecAB.z + dABu * vecAC.z);

			DirectX::XMStoreFloat3(&tan, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&tan)));
			DirectX::XMStoreFloat3(&biNorm, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&biNorm)));
		}


		/*
		获取CubeMap摄像头的6个角度
		out用来控制角度是朝正方体内还是朝正方体外
		*/
		static std::vector<DirectX::XMFLOAT4> GetCubeCameraQuat(DirectX::XMFLOAT3 pos, bool out)
		{
			std::vector<DirectX::XMFLOAT4> quatVec;

			int k = 1;
			if (out == false)
				k = -1;

			// Look along each coordinate axis.
			DirectX::XMFLOAT3 targets[6] =
			{
				F3VAddScale(pos,Right(),k),   //look at right
				F3VSubScale(pos,Right(),k),   //look at left
				F3VAddScale(pos,Up(),k),      //look at up
				F3VSubScale(pos,Up(),k),	  //look at down
				F3VAddScale(pos,Foward(),1),  //look at forward
				F3VSubScale(pos,Foward(),1),  //look at back
			};

			DirectX::XMFLOAT3 ups[6] =
			{
				Up(),                  //look at right
				Up(), 				   //look at left
				F3MulF(Foward(), -k),  //look at up
				F3MulF(Foward(), k),   //look at down
				Up(),				   //look at forward
				Up(),				   //look at back
			};

			for (int i = 0; i < 6; ++i)
			{
				DirectX::XMMATRIX mat = DirectX::XMMatrixLookAtLH(XMLoadFloat3(&pos), XMLoadFloat3(&targets[i]), XMLoadFloat3(&ups[i]));
				DirectX::XMVECTOR quat = DirectX::XMQuaternionRotationMatrix(mat);
				DirectX::XMFLOAT4 quatf4;
				DirectX::XMStoreFloat4(&quatf4, quat);

				quatVec.push_back(quatf4);
			}
			return quatVec;
		}



		static DirectX::XMFLOAT4X4 FbxTrans2DxMat(
			bool YuporZup,
			float posx, float posy, float posz,
			float scalex, float scaley, float scalez,
			float eulerx, float eulery, float eulerz
		)
		{
			DirectX::XMFLOAT4X4 mat = RDx12::MathHelper::Identity4x4();
			DirectX::XMFLOAT3 pos(0, 0, 0);
			DirectX::XMFLOAT3 scale(1, 1, 1);

			if (YuporZup)
			{
				pos = DirectX::XMFLOAT3(posx, posy, -posz);
			}
			else
			{
				pos = DirectX::XMFLOAT3(posx, posz, posy);
			}
			scale = DirectX::XMFLOAT3(scalex, scalez, scaley);
			
			DirectX::XMStoreFloat4x4(
				&mat,
				RDx12::MathHelper::GetTransMatByF
				(
					{ 0,0,0 },
					{ 0,0,0,1 },
					scale,
					{ 0,0,0 },
					FbxEuler2DxQuat(eulerx,eulery,eulerz),
					pos
				)
			);
			return mat;
		}

		static DirectX::XMFLOAT4 FbxEuler2DxQuat(float eulerx, float eulery, float eulerz)
		{
			/*
				3dMax                       MyDx
				Forward = Y                 Forward = Z
				Right = X                   Right = X
				Up = Z                      Up = Y
				右手坐标系                  左手坐标系
			*/
			DirectX::XMFLOAT3 euler = DirectX::XMFLOAT3(-eulerx, -eulerz, -eulery);
			/*
				转化后，不能直接用DirectX::XMQuaternionRotationRollPitchYaw来生成quat
				因为fbx原始欧拉角旋转顺序是 延right旋转--->延forward旋转---->延up旋转
				而DirectX::XMQuaternionRotationRollPitchYaw是  延forward旋转--->延right旋转--->延up旋转
				直接用单个欧拉角形成的矩阵按序相乘，将乘积矩阵再反过来转成RollPitchYaw的Quat
			*/
			DirectX::XMFLOAT4X4 fbxPitchMat = RDx12::MathHelper::GetRollPitchYawMatByDeg(euler.x, 0, 0);
			DirectX::XMFLOAT4X4 fbxYawMat = RDx12::MathHelper::GetRollPitchYawMatByDeg(0, euler.y, 0);
			DirectX::XMFLOAT4X4 fbxRollMat = RDx12::MathHelper::GetRollPitchYawMatByDeg(0, 0, euler.z);
			
			DirectX::XMFLOAT4X4 resRotMat = RDx12::MathHelper::MulMat(RDx12::MathHelper::MulMat(fbxPitchMat, fbxRollMat), fbxYawMat);
			
			DirectX::XMFLOAT4 dxQuat(0, 0, 0, 1);
			DirectX::XMStoreFloat4(&dxQuat, DirectX::XMQuaternionRotationMatrix(DirectX::XMLoadFloat4x4(&resRotMat)));

			return dxQuat;
		}

		static void ShowMat(DirectX::XMFLOAT4X4 mat)
		{
			WIN32DEBUG(L"\n");
			WIN32DEBUG(L"%.4f %.4f %.4f %.4f\n", mat._11, mat._12, mat._13, mat._14);
			WIN32DEBUG(L"%.4f %.4f %.4f %.4f\n", mat._21, mat._22, mat._23, mat._24);
			WIN32DEBUG(L"%.4f %.4f %.4f %.4f\n", mat._31, mat._32, mat._33, mat._34);
			WIN32DEBUG(L"%.4f %.4f %.4f %.4f\n", mat._41, mat._42, mat._43, mat._44);
			WIN32DEBUG(L"\n");
		}

		static void ShowMat(DirectX::XMMATRIX mat)
		{
			DirectX::XMFLOAT4X4 f44;
			DirectX::XMStoreFloat4x4(&f44, mat);
			ShowMat(f44);
		}

		static void ShowFloat4(DirectX::XMFLOAT4 vec)
		{
			WIN32DEBUG(L"\n");
			WIN32DEBUG(L"%.2f %.2f %.2f %.2f\n", vec.x, vec.y, vec.z, vec.w);
			WIN32DEBUG(L"\n");
		}

		static void ShowFloat4(DirectX::XMVECTOR vec)
		{
			DirectX::XMFLOAT4 res;
			DirectX::XMStoreFloat4(&res, vec);
			ShowFloat4(res);
		}

		static void ShowFloat3(DirectX::XMFLOAT3 vec)
		{
			WIN32DEBUG(L"\n");
			WIN32DEBUG(L"%.2f %.2f %.2f\n", vec.x, vec.y, vec.z);
			WIN32DEBUG(L"\n");
		}

		static void CoutFloat3(DirectX::XMVECTOR vec)
		{
			DirectX::XMFLOAT3 res;
			DirectX::XMStoreFloat3(&res, vec);
			CoutFloat3(res);
		}

		static void CoutFloat3(DirectX::XMFLOAT3 vec)
		{
			std::cout<<vec.x<<","<<vec.y<<","<<vec.z;
		}

		static void ShowFloat3(DirectX::XMVECTOR vec)
		{
			DirectX::XMFLOAT3 res;
			DirectX::XMStoreFloat3(&res, vec);
			ShowFloat3(res);
		}

		static void ShowAngleAndAxis(DirectX::XMFLOAT4 quat)
		{
			DirectX::XMVECTOR axis;
			float angle;
			DirectX::XMQuaternionToAxisAngle(&axis, &angle, DirectX::XMLoadFloat4(&quat));
			WIN32DEBUG(L"axis");
			RDx12::MathHelper::ShowFloat4(axis);
			WIN32DEBUG(L"normalize axis");
			RDx12::MathHelper::ShowFloat4(DirectX::XMVector3Normalize(axis));
			WIN32DEBUG(L"angle %f\n", angle);
		}

		static void BaseTest()
		{
			ShowFloat3(F3MulF({ 1,2,3 }, 2));
			ShowFloat3(F3AddF3({ 1,2,3 }, { -1,-2,-3 }));
			ShowFloat3(F3VAddScale({ 1,2,3 }, { -1,-2,-3 }, 2));
			ShowFloat3(F3SubF3({ 1,2,3 }, { -1,-2,-3 }));
			ShowFloat3(F3MulF3({ 1,2,3 }, { -1,-2,-3 }));
			ShowFloat3(F3DivideF3({ 1,2,3 }, { -1,-2,-3 }));
			ShowFloat3(F3VSubScale({ 1,2,3 }, { -1,-2,-3 }, 2));
			ShowFloat3(F3MulMat({ 1,0,0 }, GetRollPitchYawMatByDeg(0, 0, 90)));
			ShowFloat3(Quat2EluerDeg(GetQuatVecA2VecB({ 0, 1, 0 }, { 0,0,-1 })));
			ShowFloat3(VecTransCoord({ -1,0,0 }, GetRollPitchYawMatByDeg(0, 0, 90), false));
			ShowFloat3(
				Quat2EluerDeg(
					QuatTransCoord(
						GetRollPitchYawQuatByDeg(0, 0, 90),
						GetRollPitchYawMatByDeg(90, 0, 0)
					)
				)
			);
		}

		static void TransTest()
		{
			/*DirectX::XMFLOAT4 testVec(0, 1, 0, 1);
			DirectX::XMFLOAT3 testScale(1,2,1);
			DirectX::XMFLOAT4 testQuat = GetRollPitchYawQuatByDeg(0, 0, 45);
			TransMatPrint(
				testVec,
				{ 0,0,0 },
				testQuat,
				testScale,
				{ 0,0,0 },
				{ 0,0,0,1 },
				{ 0,0,0 },
				true
			);

			auto mat1 = GetRollPitchYawMatByDeg(0, 0, 45);
			auto mat2 = InverMat(mat1);
			auto scaleMat = DirectX::XMMatrixScaling(1, 2, 1);
			auto scaleQuatMat = MulMat(RDx12::MathHelper::MulMat(mat2, scaleMat), mat1);
			ShowMat(mat2);
			ShowMat(mat1);
			ShowMat(scaleQuatMat);

			ShowFloat4(F4MulMat(testVec,scaleQuatMat));*/

			DirectX::XMMATRIX mat = GetTransMatByF({ 10,23,55 }, GetRollPitchYawQuatByDeg(0, 0, 0), { 1,2,3 }, { 0,0,0 }, GetRollPitchYawQuatByDeg(10, 20, 30), { 4,5,6 });
			ShowMat(mat);
			DirectX::XMVECTOR posVec, scaleVec, quatVec;
			DirectX::XMMatrixDecompose(&scaleVec, &quatVec, &posVec, mat);
			ShowFloat3(posVec);
			ShowFloat3(scaleVec);
			ShowFloat4(quatVec);
			ShowFloat4(GetRollPitchYawQuatByDeg(10, 20, 30));

		}

		static void RotTest()
		{
			ShowMat(GetRollPitchYawMatByDeg(30, 45, 60));
			ShowMat(GetRollPitchYawMatByDeg(30, 0, 0));
			ShowMat(GetRollPitchYawMatByDeg(0, 45, 0));
			ShowMat(GetRollPitchYawMatByDeg(0, 0, 60));
			/*旋转矩阵顺序测试*/
			ShowMat(
				MulMat(
					MulMat(
						GetRollPitchYawMatByDeg(0, 0, 60),
						GetRollPitchYawMatByDeg(30, 0, 0)
					),
					GetRollPitchYawMatByDeg(0, 45, 0)
				)
			);
			/*四元数与欧拉角之间的转换测试*/
			auto deg = Quat2EluerDeg(GetRollPitchYawQuatByDeg(230, 145, 260));
			ShowFloat3(deg);
			ShowFloat4(GetRollPitchYawQuatByDeg(deg.x, deg.y, deg.z));
			ShowFloat4(GetRollPitchYawQuatByDeg(230, 145, 260));
		}
	};
}