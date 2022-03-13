#include "../../../Include/RDx12/Geomertry/PureMesh.h"


namespace RDx12
{
	PureMesh<VertexData> GeoGen::GetPlane(std::string&& name, float x2, float y2)
	{
		PureMesh<VertexData> pureMesh;


		DirectX::XMFLOAT3 pos[4] =
		{
			DirectX::XMFLOAT3(-x2, -y2, 0) ,DirectX::XMFLOAT3(-x2, +y2, 0) ,DirectX::XMFLOAT3(+x2, +y2,0) ,DirectX::XMFLOAT3(+x2, -y2, 0) ,//Front
		};

		DirectX::XMFLOAT3 nor[4] =
		{
			DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f),DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f),DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f),DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f)
		};

		DirectX::XMFLOAT2 texc[4] =
		{
			DirectX::XMFLOAT2(0.0f, 1.0f),DirectX::XMFLOAT2(0.0f, 0.0f),DirectX::XMFLOAT2(1.0f, 0.0f),DirectX::XMFLOAT2(1.0f, 1.0f),//Front
		};

		DirectX::XMFLOAT4 col[4] =
		{
			DirectX::XMFLOAT4{DirectX::Colors::Blue},DirectX::XMFLOAT4{DirectX::Colors::Red},DirectX::XMFLOAT4{DirectX::Colors::Green},DirectX::XMFLOAT4{DirectX::Colors::Yellow}
		};

		IndexType index[6] =
		{
			0,1,2,0,2,3,
		};

		pureMesh.name = name;
		for (int i = 0; i < 4; i++)
		{
			VertexData vert;
			vert.pos = pos[i];
			vert.nor = nor[i];
			vert.col = col[i];
			vert.texc = texc[i];
			pureMesh.vecVertex.push_back(vert);
		}

		for (int i = 0; i < 6; i++)
		{
			pureMesh.vecIndex.push_back(index[i]);
		}
		pureMesh.topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		return pureMesh;
	}



	PureMesh<VertexData> GeoGen::GetCube(std::string&& name, float x2, float y2, float z2)
	{
		PureMesh<VertexData> pureMesh;
		DirectX::XMFLOAT3 pos[24] =
		{
			DirectX::XMFLOAT3(-x2, -y2, -z2) ,DirectX::XMFLOAT3(-x2, +y2, -z2) ,DirectX::XMFLOAT3(+x2, +y2, -z2) ,DirectX::XMFLOAT3(+x2, -y2, -z2) ,//Front
			DirectX::XMFLOAT3(-x2, -y2, +z2) ,DirectX::XMFLOAT3(+x2, -y2, +z2) ,DirectX::XMFLOAT3(+x2, +y2, +z2) ,DirectX::XMFLOAT3(-x2, +y2, +z2) ,//Back
			DirectX::XMFLOAT3(-x2, +y2, -z2) ,DirectX::XMFLOAT3(-x2, +y2, +z2) ,DirectX::XMFLOAT3(+x2, +y2, +z2) ,DirectX::XMFLOAT3(+x2, +y2, -z2) ,//top
			DirectX::XMFLOAT3(-x2, -y2, -z2) ,DirectX::XMFLOAT3(+x2, -y2, -z2) ,DirectX::XMFLOAT3(+x2, -y2, +z2) ,DirectX::XMFLOAT3(-x2, -y2, +z2) ,//bottom
			DirectX::XMFLOAT3(-x2, -y2, +z2) ,DirectX::XMFLOAT3(-x2, +y2, +z2) ,DirectX::XMFLOAT3(-x2,	+y2, -z2) ,DirectX::XMFLOAT3(-x2, -y2, -z2) ,//left
			DirectX::XMFLOAT3(+x2, -y2, -z2) ,DirectX::XMFLOAT3(+x2, +y2, -z2) ,DirectX::XMFLOAT3(+x2, +y2, +z2) ,DirectX::XMFLOAT3(+x2, -y2, +z2) ,//bottom

		};

		DirectX::XMFLOAT2 texc[24] =
		{
			DirectX::XMFLOAT2(0.0f, 1.0f),DirectX::XMFLOAT2(0.0f, 0.0f),DirectX::XMFLOAT2(1.0f, 0.0f),DirectX::XMFLOAT2(1.0f, 1.0f),//Front
			DirectX::XMFLOAT2(1.0f, 1.0f),DirectX::XMFLOAT2(0.0f, 1.0f),DirectX::XMFLOAT2(0.0f, 0.0f),DirectX::XMFLOAT2(1.0f, 0.0f),//Back
			DirectX::XMFLOAT2(0.0f, 1.0f),DirectX::XMFLOAT2(0.0f, 0.0f),DirectX::XMFLOAT2(1.0f, 0.0f),DirectX::XMFLOAT2(1.0f, 1.0f),//top
			DirectX::XMFLOAT2(1.0f, 1.0f),DirectX::XMFLOAT2(0.0f, 1.0f),DirectX::XMFLOAT2(0.0f, 0.0f),DirectX::XMFLOAT2(1.0f, 0.0f),//bottom
			DirectX::XMFLOAT2(0.0f, 1.0f),DirectX::XMFLOAT2(0.0f, 0.0f),DirectX::XMFLOAT2(1.0f, 0.0f),DirectX::XMFLOAT2(1.0f, 1.0f),//left
			DirectX::XMFLOAT2(0.0f, 1.0f),DirectX::XMFLOAT2(0.0f, 0.0f),DirectX::XMFLOAT2(1.0f, 0.0f),DirectX::XMFLOAT2(1.0f, 1.0f),//bottom
		};

		DirectX::XMFLOAT3 nor[24] =
		{
			DirectX::XMFLOAT3(0.0f, 0.0f,-1.0f),DirectX::XMFLOAT3(0.0f, 0.0f,-1.0f),DirectX::XMFLOAT3(0.0f, 0.0f,-1.0f),DirectX::XMFLOAT3(0.0f, 0.0f,-1.0f),//Front
			DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f),DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f),DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f),DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f),//Back
			DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),//top
			DirectX::XMFLOAT3(0.0f,-1.0f, 0.0f),DirectX::XMFLOAT3(0.0f,-1.0f, 0.0f),DirectX::XMFLOAT3(0.0f,-1.0f, 0.0f),DirectX::XMFLOAT3(0.0f,-1.0f, 0.0f),//bottom
			DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f),DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f),DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f),DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f),//left
			DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f),DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f),DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f),DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f),//bottom
		};

		DirectX::XMFLOAT4 col[24] =
		{
			DirectX::XMFLOAT4{DirectX::Colors::Blue},DirectX::XMFLOAT4{DirectX::Colors::Blue},DirectX::XMFLOAT4{DirectX::Colors::Blue},DirectX::XMFLOAT4{DirectX::Colors::Blue},
			DirectX::XMFLOAT4{DirectX::Colors::Red},DirectX::XMFLOAT4{DirectX::Colors::Red},DirectX::XMFLOAT4{DirectX::Colors::Red},DirectX::XMFLOAT4{DirectX::Colors::Red},
			DirectX::XMFLOAT4{DirectX::Colors::Green},DirectX::XMFLOAT4{DirectX::Colors::Green},DirectX::XMFLOAT4{DirectX::Colors::Green},DirectX::XMFLOAT4{DirectX::Colors::Green},
			DirectX::XMFLOAT4{DirectX::Colors::Yellow},DirectX::XMFLOAT4{DirectX::Colors::Yellow},DirectX::XMFLOAT4{DirectX::Colors::Yellow},DirectX::XMFLOAT4{DirectX::Colors::Yellow},
			DirectX::XMFLOAT4{DirectX::Colors::Brown},DirectX::XMFLOAT4{DirectX::Colors::Brown},DirectX::XMFLOAT4{DirectX::Colors::Brown},DirectX::XMFLOAT4{DirectX::Colors::Brown},
			DirectX::XMFLOAT4{DirectX::Colors::Purple},DirectX::XMFLOAT4{DirectX::Colors::Purple},DirectX::XMFLOAT4{DirectX::Colors::Purple},DirectX::XMFLOAT4{DirectX::Colors::Purple}
		};

		IndexType index[36] =
		{
			0,1,2,0,2,3,
			4,5,6,4,6,7,
			8,9,10,8,10,11,
			12,13,14,12,14,15,
			16,17,18,16,18,19,
			20,21,22,20,22,23
		};

		pureMesh.name = name;
		for (int i = 0; i < 24; i++)
		{
			VertexData vert;
			vert.pos = pos[i];
			vert.nor = nor[i];
			vert.col = col[i];
			vert.texc = texc[i];
			pureMesh.vecVertex.push_back(vert);
		}

		for (int i = 0; i < 36; i++)
		{
			pureMesh.vecIndex.push_back(index[i]);
		}

		pureMesh.topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		//这里 D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST会影响index的解读，List意味着3个不重复的顶点一个三角形：0,1,2,0,2,3->0,1,2  0,2,3 两个三角形
		//如果改成TRIANGELSTRIP，意味着0,1,2,0,2,3->0,1,2 1,2,0 2,0,2 ,0,2,3 四个三角形
		//一般来说List要灵活一点，因此大多数模型用的是LIST

		pureMesh.CalcuTangent();
		return pureMesh;
	}

	PureMesh<VertexData> GeoGen::GetSphere(std::string&& name, float radius, uint32_t sliceCount, uint32_t stackCount)
	{
		PureMesh<VertexData> pureMesh;
		pureMesh.name = name;
		pureMesh.topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		VertexData topVertex;
		topVertex.pos = DirectX::XMFLOAT3(0.0f, +radius, 0.0f);
		topVertex.nor = DirectX::XMFLOAT3(0.0f, +1.0f, 0.0f);
		topVertex.tan = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
		topVertex.texc = DirectX::XMFLOAT2(0.0f, 0.0f);

		VertexData bottomVertex; 
		bottomVertex.pos = DirectX::XMFLOAT3(0.0f, -radius, 0.0f);
		bottomVertex.nor = DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f);
		bottomVertex.tan = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
		bottomVertex.texc = DirectX::XMFLOAT2(0.0f, 1.0f);

		pureMesh.vecVertex.push_back(topVertex);

		float phiStep = DirectX::XM_PI / stackCount;
		float thetaStep = 2.0f * DirectX::XM_PI / sliceCount;

		// Compute vertices for each stack ring (do not count the poles as rings).
		for (uint32_t i = 1; i <= stackCount - 1; ++i)
		{
			float phi = i * phiStep;

			// Vertices of ring.
			for (uint32_t j = 0; j <= sliceCount; ++j)
			{
				float theta = j * thetaStep;

				VertexData v;

				// spherical to cartesian
				v.pos.x = radius * sinf(phi) * cosf(theta);
				v.pos.y = radius * cosf(phi);
				v.pos.z = radius * sinf(phi) * sinf(theta);

				// Partial derivative of P with respect to theta
				v.tan.x = -radius * sinf(phi) * sinf(theta);
				v.tan.y = 0.0f;
				v.tan.z = +radius * sinf(phi) * cosf(theta);

				DirectX::XMVECTOR T = DirectX::XMLoadFloat3(&v.tan);
				DirectX::XMStoreFloat3(&v.tan, DirectX::XMVector3Normalize(T));

				DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&v.pos);
				DirectX::XMStoreFloat3(&v.nor, DirectX::XMVector3Normalize(p));

				v.texc.x = theta / DirectX::XM_2PI;
				v.texc.y = phi / DirectX::XM_PI;

				pureMesh.vecVertex.push_back(v);
			}
		}

		pureMesh.vecVertex.push_back(bottomVertex);

		//
		// Compute indices for top stack.  The top stack was written first to the vertex buffer
		// and connects the top pole to the first ring.
		//

		for (uint32_t i = 1; i <= sliceCount; ++i)
		{
			pureMesh.vecIndex.push_back(0);
			pureMesh.vecIndex.push_back(i + 1);
			pureMesh.vecIndex.push_back(i);
		}

		//
		// Compute indices for inner stacks (not connected to poles).
		//

		// Offset the indices to the index of the first vertex in the first ring.
		// This is just skipping the top pole vertex.
		uint32_t baseIndex = 1;
		uint32_t ringVertexCount = sliceCount + 1;
		for (uint32_t i = 0; i < stackCount - 2; ++i)
		{
			for (uint32_t j = 0; j < sliceCount; ++j)
			{
				pureMesh.vecIndex.push_back(baseIndex + i * ringVertexCount + j);
				pureMesh.vecIndex.push_back(baseIndex + i * ringVertexCount + j + 1);
				pureMesh.vecIndex.push_back(baseIndex + (i + 1) * ringVertexCount + j);

				pureMesh.vecIndex.push_back(baseIndex + (i + 1) * ringVertexCount + j);
				pureMesh.vecIndex.push_back(baseIndex + i * ringVertexCount + j + 1);
				pureMesh.vecIndex.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
			}
		}

		//
		// Compute indices for bottom stack.  The bottom stack was written last to the vertex buffer
		// and connects the bottom pole to the bottom ring.
		//

		// South pole vertex was added last.
		uint32_t southPoleIndex = (uint32_t)pureMesh.vecVertex.size() - 1;

		// Offset the indices to the index of the first vertex in the last ring.
		baseIndex = southPoleIndex - ringVertexCount;

		for (uint32_t i = 0; i < sliceCount; ++i)
		{
			pureMesh.vecIndex.push_back(southPoleIndex);
			pureMesh.vecIndex.push_back(baseIndex + i);
			pureMesh.vecIndex.push_back(baseIndex + i + 1);
		}

		return pureMesh;
	}
}