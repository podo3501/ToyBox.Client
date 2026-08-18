#pragma once

struct PhongSurface
{
    float normalScale{ 1.f };      // 0.0 ~ N (0 = 평평, 1.0 = 원본, 1.0 초과 = 굴곡 과장. 실사용 권장 0~2)
    float ambientScale{ 0.1f };   // 0.0 ~ N (환경광 밝기 배율, 상한 없음)
    float specularScale{ 0.2f };    // 0.0 ~ N (스펙큘러 밝기 배율, 상한 없음. 일반적으로 0~수십 범위에서 사용)
    float shininess{ 8.f };       // 하이라이트 지수: 보통 4.0 ~ 256.0 (값이 클수록 하이라이트가 좁고 날카로워짐)
};
