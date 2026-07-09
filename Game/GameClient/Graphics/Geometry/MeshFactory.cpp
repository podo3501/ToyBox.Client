#include "pch.h"
#include "MeshFactory.h"

std::shared_ptr<MeshAsset> MeshFactory::CreateSphere(float radius, uint32_t sliceCount, uint32_t stackCount)
{
    auto mesh = std::make_shared<MeshAsset>();
    mesh->format = VertexFormat::Mesh;

    std::vector<MeshVertex> vertices;
    std::vector<uint32_t> indices;

    // sliceCount: 세로 분할 수 (경도, 축을 중심으로 도는 회전 분할)
    // stackCount: 가로 분할 수 (위도, 북극에서 남극으로 내려가는 층 분할)

    // 정점과 인덱스 공간 예약
    uint32_t vertexCount = (stackCount + 1) * (sliceCount + 1);
    uint32_t indexCount = stackCount * sliceCount * 6;
    vertices.reserve(vertexCount);
    indices.reserve(indexCount);

    const float PI = 3.14159265359f;
    const float TWO_PI = 6.28318530718f;

    // 1. Vertex (정점) 생성
    for (uint32_t i = 0; i <= stackCount; ++i)
    {
        // 북극(0)에서 남극(PI)까지의 각도 (위도)
        float phi = ((float)i / stackCount) * PI;
        float sinPhi = sinf(phi);
        float cosPhi = cosf(phi);

        for (uint32_t j = 0; j <= sliceCount; ++j)
        {
            // 한 바퀴 회전하는 각도 (경도)
            float theta = ((float)j / sliceCount) * TWO_PI;
            float sinTheta = sinf(theta);
            float cosTheta = cosf(theta);

            MeshVertex vert;

            // 법선 벡터 (Normal) : 원점에서 정점을 향하는 방향 단위 벡터
            vert.nx = sinPhi * cosTheta;
            vert.ny = cosPhi; // Y축을 위(Up)로 잡는 일반적인 3D 공간 기준
            vert.nz = sinPhi * sinTheta;

            // 위치 좌표 (Position) : 법선 벡터에 반지름을 곱함
            vert.px = radius * vert.nx;
            vert.py = radius * vert.ny;
            vert.pz = radius * vert.nz;

            // UV 좌표
            vert.u = (float)j / sliceCount;
            vert.v = (float)i / stackCount;

            // 접선 벡터 (Tangent) : U(경도 theta)가 증가하는 방향으로의 편미분 벡터
            vert.tx = -sinPhi * sinTheta;
            vert.ty = 0.0f;
            vert.tz = sinPhi * cosTheta;

            // Tangent 정규화
            float tLen = sqrtf(vert.tx * vert.tx + vert.ty * vert.ty + vert.tz * vert.tz);
            if (tLen > 0.00001f)
            {
                vert.tx /= tLen;
                vert.ty /= tLen;
                vert.tz /= tLen;
            }
            else
            {
                // 극점(북극/남극)처럼 수식이 깨지는 곳의 예외 처리 기본값
                vert.tx = 1.0f; vert.ty = 0.0f; vert.tz = 0.0f;
            }

            vertices.push_back(vert);
        }
    }

    // 2. Index (인덱스) 생성 - DirectX 표준 (CW 시계 방향)
    // 그리드 상의 정점 인덱스 계산을 위해 한 줄(링)에 들어가는 정점 수
    uint32_t ringVertexCount = sliceCount + 1;

    for (uint32_t i = 0; i < stackCount; ++i)
    {
        for (uint32_t j = 0; j < sliceCount; ++j)
        {
            // 사각형을 이루는 4개 모서리의 정점 인덱스 추출
            uint32_t current_row_left = i * ringVertexCount + j;
            uint32_t current_row_right = current_row_left + 1;
            uint32_t next_row_left = (i + 1) * ringVertexCount + j;
            uint32_t next_row_right = next_row_left + 1;

            // 사각형 면을 2개의 삼각형으로 쪼개서 CW 순서로 조립
            // 첫 번째 삼각형
            indices.push_back(current_row_left);
            indices.push_back(current_row_right);
            indices.push_back(next_row_left);

            // 두 번째 삼각형
            indices.push_back(current_row_right);
            indices.push_back(next_row_right);
            indices.push_back(next_row_left);
        }
    }

    // 데이터 복사 및 이동
    mesh->SetVertices(vertices);
    mesh->indices = std::move(indices);

    return mesh;
}

std::shared_ptr<MeshAsset> MeshFactory::CreateCube(float size)
{
    auto mesh = std::make_shared<MeshAsset>();
    mesh->format = VertexFormat::Mesh;

    std::vector<MeshVertex> vertices;
    std::vector<uint32_t> indices;

    // 정육면체의 반 지름(중심에서 면까지의 거리)
    float h = size * 0.5f;

    // 6개 면 정의를 위한 구조체 데이터
    struct FaceInfo
    {
        float nx, ny, nz; // 면의 Normal
        float tx, ty, tz; // 면의 Tangent
        float p0[3], p1[3], p2[3], p3[3]; // 각 면의 4개 모서리 좌표 (CCW 순서)
    };

    // 현재 사용 중인 왼손 좌표계 규칙에 맞춘 6개 면 정의
    FaceInfo faces[6] = {
        // 1. 앞면 (Front Face: +Z 방향을 바라봄)
        {  0.f,  0.f,  1.f,   1.f,  0.f,  0.f,   { -h, -h,  h }, { -h,  h,  h }, {  h,  h,  h }, {  h, -h,  h } },
        // 2. 뒷면 (Back Face: -Z 방향을 바라봄)
        {  0.f,  0.f, -1.f,  -1.f,  0.f,  0.f,   {  h, -h, -h }, {  h,  h, -h }, { -h,  h, -h }, { -h, -h, -h } },
        // 3. 윗면 (Top Face: +Y 방향을 바라봄)
        {  0.f,  1.f,  0.f,   1.f,  0.f,  0.f,   { -h,  h,  h }, { -h,  h, -h }, {  h,  h, -h }, {  h,  h,  h } },
        // 4. 아랫면 (Bottom Face: -Y 방향을 바라봄)
        {  0.f, -1.f,  0.f,  -1.f,  0.f,  0.f,   { -h, -h, -h }, { -h, -h,  h }, {  h, -h,  h }, {  h, -h, -h } },
        // 5. 왼쪽면 (Left Face: -X 방향을 바라봄)
        { -1.f,  0.f,  0.f,   0.f,  0.f,  1.f,   { -h, -h, -h }, { -h,  h, -h }, { -h,  h,  h }, { -h, -h,  h } },
        // 6. 오른쪽면 (Right Face: +X 방향을 바라봄)
        {  1.f,  0.f,  0.f,   0.f,  0.f, -1.f,   {  h, -h,  h }, {  h,  h,  h }, {  h,  h, -h }, {  h, -h, -h } }
    };

    vertices.reserve(24);
    indices.reserve(36);

    // 각 면의 UV 매핑 기준 좌표 (사각형의 네 모서리)
    float uvs[4][2] = {
        { 0.f, 1.f }, // 좌측 하단
        { 0.f, 0.f }, // 좌측 상단
        { 1.f, 0.f }, // 우측 상단
        { 1.f, 1.f }  // 우측 하단
    };

    // 1. Vertex 및 Index 생성
    for (uint32_t f = 0; f < 6; ++f)
    {
        uint32_t startIndex = static_cast<uint32_t>(vertices.size());

        // 한 면당 4개의 정점 세팅
        float* points[4] = { faces[f].p0, faces[f].p1, faces[f].p2, faces[f].p3 };

        for (uint32_t i = 0; i < 4; ++i)
        {
            MeshVertex vert;

            // Position
            vert.px = points[i][0];
            vert.py = points[i][1];
            vert.pz = points[i][2];

            // Normal
            vert.nx = faces[f].nx;
            vert.ny = faces[f].ny;
            vert.nz = faces[f].nz;

            // Tangent
            vert.tx = faces[f].tx;
            vert.ty = faces[f].ty;
            vert.tz = faces[f].tz;

            // UV
            vert.u = uvs[i][0];
            vert.v = uvs[i][1];

            vertices.push_back(vert);
        }

        //
        indices.push_back(startIndex + 0);
        indices.push_back(startIndex + 2);
        indices.push_back(startIndex + 1);

        indices.push_back(startIndex + 0);
        indices.push_back(startIndex + 3);
        indices.push_back(startIndex + 2);
    }

    mesh->SetVertices(vertices);
    mesh->indices = std::move(indices);

    return mesh;
}

std::shared_ptr<MeshAsset> MeshFactory::CreateTorus(
    float radius,
    float tubeRadius,
    uint32_t radialSegments,
    uint32_t tubularSegments)
{
    auto mesh = std::make_shared<MeshAsset>();
    mesh->format = VertexFormat::Mesh;

    std::vector<MeshVertex> vertices;
    std::vector<uint32_t> indices;

    vertices.reserve((radialSegments + 1) * (tubularSegments + 1));

    const float TWO_PI = 6.28318530718f;

    // 1. Vertex 생성
    for (uint32_t j = 0; j <= radialSegments; ++j)
    {
        float v = (float)j / (float)radialSegments;
        float phi = v * TWO_PI;

        float cosPhi = cosf(phi);
        float sinPhi = sinf(phi);

        for (uint32_t i = 0; i <= tubularSegments; ++i)
        {
            float u = (float)i / (float)tubularSegments;
            float theta = u * TWO_PI;

            float cosTheta = cosf(theta);
            float sinTheta = sinf(theta);

            // 중심 원 (major circle)
            float cx = radius * cosPhi;
            float cy = radius * sinPhi;

            // torus position
            MeshVertex vert;

            vert.px = (radius + tubeRadius * cosTheta) * cosPhi;
            vert.py = (radius + tubeRadius * cosTheta) * sinPhi;
            vert.pz = tubeRadius * sinTheta;

            // normal (center - vertex 방향)
            float nx = vert.px - cx;
            float ny = vert.py - cy;
            float nz = vert.pz;

            float len = sqrtf(nx * nx + ny * ny + nz * nz);
            if (len > 0.00001f)
            {
                nx /= len;
                ny /= len;
                nz /= len;
            }

            vert.nx = nx;
            vert.ny = ny;
            vert.nz = nz;

            // Tangent 계산: U(theta)가 증가하는 방향으로의 편미분 벡터
            float tx = -sinTheta * cosPhi;
            float ty = -sinTheta * sinPhi;
            float tz = cosTheta;

            float tLen = sqrtf(tx * tx + ty * ty + tz * tz);
            if (tLen > 0.00001f)
            {
                tx /= tLen;
                ty /= tLen;
                tz /= tLen;
            }

            vert.tx = tx;
            vert.ty = ty;
            vert.tz = tz;

            vert.u = u;
            vert.v = v;

            vertices.push_back(vert);
        }
    }

    // 2. Index 생성
    const uint32_t ring = tubularSegments + 1;

    for (uint32_t j = 0; j < radialSegments; ++j)
    {
        for (uint32_t i = 0; i < tubularSegments; ++i)
        {
            uint32_t a = j * ring + i;
            uint32_t b = (j + 1) * ring + i;
            uint32_t c = (j + 1) * ring + (i + 1);
            uint32_t d = j * ring + (i + 1);

            // triangle 1
            indices.push_back(a);
            indices.push_back(b);
            indices.push_back(d);

            // triangle 2
            indices.push_back(b);
            indices.push_back(c);
            indices.push_back(d);
        }
    }

    // 3. MeshAsset 세팅
    mesh->SetVertices(vertices);
    mesh->indices = std::move(indices);

    return mesh;
}

std::shared_ptr<MeshAsset> MeshFactory::CreateGrid(
    float cellSize,
    uint32_t halfExtent,
    float r, float g, float b)
{
    auto mesh = std::make_shared<MeshAsset>();
    mesh->format = VertexFormat::Grid;

    std::vector<GridVertex> vertices;
    std::vector<uint32_t> indices;

    const uint32_t lineCount = (halfExtent * 2 + 1); // 라인 개수
    vertices.reserve(lineCount * 4); // 각 라인은 vertex 2개
    indices.reserve(lineCount * 4); // 각 라인은 index 2개

    uint32_t index = 0;

    const float min = -static_cast<float>(halfExtent) * cellSize;
    const float max = static_cast<float>(halfExtent) * cellSize;

    // Z 방향 라인 (세로줄)
    for (uint32_t i = 0; i < lineCount; ++i)
    {
        float x =
            min + static_cast<float>(i) * cellSize;

        GridVertex v0{};
        v0.px = x;
        v0.py = 0.0f;
        v0.pz = min;
        v0.r = r;
        v0.g = g;
        v0.b = b;

        GridVertex v1{};
        v1.px = x;
        v1.py = 0.0f;
        v1.pz = max;
        v1.r = r;
        v1.g = g;
        v1.b = b;

        vertices.push_back(v0);
        vertices.push_back(v1);

        indices.push_back(index++);
        indices.push_back(index++);
    }

    // X 방향 라인 (가로줄)
    for (uint32_t i = 0; i < lineCount; ++i)
    {
        float z = min + static_cast<float>(i) * cellSize;

        GridVertex v0{};
        v0.px = min;
        v0.py = 0.0f;
        v0.pz = z;
        v0.r = r;
        v0.g = g;
        v0.b = b;

        GridVertex v1{};
        v1.px = max;
        v1.py = 0.0f;
        v1.pz = z;
        v1.r = r;
        v1.g = g;
        v1.b = b;

        vertices.push_back(v0);
        vertices.push_back(v1);

        indices.push_back(index++);
        indices.push_back(index++);
    }

    mesh->SetVertices(vertices);
    mesh->indices = std::move(indices);

    return mesh;
}