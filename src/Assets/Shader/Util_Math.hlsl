#ifndef __UTIL_MATH_HLSL__
#define __UTIL_MATH_HLSL__

static const float PI = 3.14159265359f;

/*Random*/
/* 
Hammersley Point Set(2dRandom)
ǰ���λ�����ǽ��е���
������16λ��������
1 2 3 4 5 6 7 8 a b c d e f g h

a b c d e f g h 1 2 3 4 5 6 7 8  ÿ��8������������
b a d c f e h g 2 1 4 3 6 5 8 7  ÿ��1������������
d c b a h g f e 4 3 2 1 8 7 6 5  ÿ��2������������
h g f e d c b a 8 7 6 5 4 3 2 1  ÿ��4������������(��ɵ���)
*/
float RadicalInverse_VdC(uint bits)
{
	// http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html

    bits = (bits << 16u) | (bits >> 16u); //ÿ��16������������
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u); //ÿ��1������������
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u); //ÿ��2������������
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u); //ÿ��4������������
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u); //ÿ��8������������(��ɵ���)
    return float(bits) * 2.3283064365386963e-10; // /2.3283064365386963e-10 = 2^-32    
    /*
    uint���޷��Ÿ�������
    ԭ��                         ������                                          �����
      1          0000 0000 0000 0000 0000 0000 0000 0001           1000 0000 0000 0000 0000 0000 0000 0000 2^31 * 2^-32 = 0.5
      2          0000 0000 0000 0000 0000 0000 0000 0010           0100 0000 0000 0000 0000 0000 0000 0000 2^30 * 2^-32 = 0.25
      3          0000 0000 0000 0000 0000 0000 0000 0011           1100 0000 0000 0000 0000 0000 0000 0000 ��2^31+2^30��* 2^-32 = 0.75  
      4          0000 0000 0000 0000 0000 0000 0000 0100           0010 0000 0000 0000 0000 0000 0000 0000 2^29 * 2^-32  = 0.125
    
    */
}

float2 Random_Hammersley2d(uint i, uint N)//N��ʾ�������,i��ʾ��i���㣬���ص�yΪ�����
{
    return float2(float(i) / float(N), RadicalInverse_VdC(i)); //��x���й�һ�����õ����磨1/5,0.5��(2/5,0.25)(3/5,0.75)(4/5,0.125)�ڶ�ά�Ͼ��ȷֲ��ĵ㼯
}


/*˥������*/
/*
https://learnopengl.com/Lighting/Multiple-lights
�ص㣺�������½��죬Զ���뻺���½�
*/
float Att_Clq(float dis, float lValue, float qValue)
{
    return 1 / (1 + lValue * dis + qValue * dis * dis); //�ֲ���һ��������֤�������ֵΪ1
}


/*
����ƽ���½������޽ӽ�0
*/
float Att_Dis(float dis)
{
    return 1 / (1 + dis * dis);
}

/*
�����½���Զ��Ϊ0
*/
float Att_Linear(float dis,float fallStart,float fallEnd)
{
    return saturate((fallEnd - dis) / (fallEnd - fallStart));
}

/*
��Ԫ�������ŵ�λ����u��תtheta����
      ʵ��            �鲿
��cos(theta/2),sin(theta/2)*u��
*/
float4 Quat(float3 u,float theta)
{
    float halfTheta = theta / 2;
    return float4(
    sin(halfTheta) * u,
    cos(halfTheta)
    );
}

/*��Ԫ������=����*/
float4 Quat_Inverse(float4 q)
{
    return float4(-q.xyz, q.w);
}

/*
q1=(a,bi,cj,dk);(a,v)
q2=(e,fi,gj,hk);(e,u)

q1*q2=(a,v)*(e,u)=(ae-v*u��a*u+e*v+cross(v,u)
*/
float4 Quat_Mul(float4 q1,float4 q2)
{
    return float4(
    q1.w * q2.xyz + q2.w * q1.xyz + cross(q1.xyz, q2.xyz),
    q1.w * q2.w - dot(q1.xyz, q2.xyz)
    );
}

/*
v��u��תtheta���ȵõ�v'
v=(0,v)
q=(cos(theta/2),sin(theta/2)u)
v'= qvq*
*/
float3 Quat_RotVec(float4 q,float3 v)
{
    return Quat_Mul(Quat_Mul(q, float4(v, 0)), Quat_Inverse(q)).xyz;
}

/*
0<= z_ndc <=1
zndc = f/(f-n)-fn/(f-n)/viewZ (��viewZ=n zndc=0 ��viewz=f zndc = 1)
ProjMat(row)
   1/aspect*cot(fov/2)         0             0         0
            0             cot(fov/2)         0         0
            0                  0          f/(f-n)      1
            0                  0         -fn/(f-n)     0  

*/
float NdcDepthToViewDepth(float z_ndc,float4x4 projMat)
{
    
    // z_ndc = A + B/viewZ, where gProj[2,2]=A and gProj[3,2]=B.
    float viewZ = projMat[3][2] / (z_ndc - projMat[2][2]);
    return viewZ;
}

float OcclusionFunction(float eps,float dis, float fallStart, float fallEnd)
{
	//
	// If depth(q) is "behind" depth(p), then q cannot occlude p.  Moreover, if 
	// depth(q) and depth(p) are sufficiently close, then we also assume q cannot
	// occlude p because q needs to be in front of p by Epsilon to occlude p.
	//
	// We use the following function to determine the occlusion.  
	// 
	//
	//       1.0     -------------\
	//               |           |  \
	//               |           |    \
	//               |           |      \ 
	//               |           |        \
	//               |           |          \
	//               |           |            \
	//  ------|------|-----------|-------------|---------|--> distZ
	//        0     Eps          z0            z1        
	//
	
    float occlusion = 0.0f;

    if (dis > eps)
    {
        occlusion = Att_Linear(dis, fallStart, fallEnd);
    }
	
    return occlusion;
}
#endif