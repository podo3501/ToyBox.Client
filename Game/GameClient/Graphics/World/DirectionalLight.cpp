#include "pch.h"
#include "DirectionalLight.h"
#include "Core/Math/Matrix.h"
#include "GameClient/Graphics/RenderData/CameraData.h"

namespace cm = Core::Math;

DirectionalLightData DirectionalLight::BuildLightData(const CameraData& mainCameraData) const
{
    DirectionalLightData data{};
    data.direction = m_direction;
    data.color = m_color;
    data.intensity = m_intensity;

    // 메인 카메라 위치를 기준으로 조명이 들어오는 반대 방향 벡터로 멀리 떨어뜨림
    //cm::Vector3 mainCamPos{ mainCameraData.position.x, mainCameraData.position.y, mainCameraData.position.z };
    cm::Vector3 targetCenter{ 0.0f, 0.0f, 0.0f };
    float lightDistance = 200.0f; // 씬 크기에 맞게 조절 가능
    cm::Vector3 lightPos = targetCenter - (m_direction * lightDistance);

    // 2. Light View Matrix 계산 (LookAt 변환 직접 유도) 기저 벡터 생성 (조명이 바라보는 방향이 곧 Forward 축)
    cm::Vector3 zAxis = m_direction.NormalizedOr({ 0.0f, -1.0f, 0.0f });

    // 업 벡터 기준 설정 (만약 조명이 수직으로 정방향 하강하면 임시로 Z축을 업벡터로 변경)
    cm::Vector3 upBasis{ 0.0f, 1.0f, 0.0f };
    if (std::abs(zAxis.y) > 0.99f)
        upBasis = cm::Vector3{ 0.0f, 0.0f, 1.0f };

    cm::Vector3 xAxis = upBasis.Cross(zAxis).NormalizedOr(cm::Vector3::Right());
    cm::Vector3 yAxis = zAxis.Cross(xAxis).NormalizedOr(cm::Vector3::Up());

    // Row-Major 카메라 뷰 변환 행렬 구성
    cm::Matrix lightView = cm::Matrix::Identity();
    lightView.m[0][0] = xAxis.x; lightView.m[1][0] = xAxis.y; lightView.m[2][0] = xAxis.z;
    lightView.m[0][1] = yAxis.x; lightView.m[1][1] = yAxis.y; lightView.m[2][1] = yAxis.z;
    lightView.m[0][2] = zAxis.x; lightView.m[1][2] = zAxis.y; lightView.m[2][2] = zAxis.z;

    // 카메라 이동(Translation) 성분 반영 (-Eye ∙ Axis)
    lightView.m[3][0] = -lightPos.Dot(xAxis);
    lightView.m[3][1] = -lightPos.Dot(yAxis);
    lightView.m[3][2] = -lightPos.Dot(zAxis);

    // 3. Light Projection Matrix 계산 (Orthographic) 조명이 커버할 공간의 가로세로 범위 (너무 크면 해상도가 뭉개지고 작으면 잘림)
    float shadowSizeX = 20.0f;
    float shadowSizeY = 20.0f;

    float halfWidth = shadowSizeX * 0.5f;
    float halfHeight = shadowSizeY * 0.5f;

    float nearZ = 0.1f;
    float farZ = 400.0f; // lightDistance 보다 커야 타겟 오브젝트들이 완전히 포함됨

    cm::Matrix lightProj = cm::Matrix::OrthographicOffCenter(-halfWidth, halfWidth, -halfHeight, halfHeight, nearZ, farZ);

    // 4. View * Proj 결합 후 구조체에 할당
    cm::Matrix lightViewProj = lightView * lightProj;
    data.viewProj = lightViewProj;

    return data;
}