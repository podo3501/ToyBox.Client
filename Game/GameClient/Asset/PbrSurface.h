#pragma once

struct PbrSurface
{
    float normalScale{ 1.f };      // 0.0 ~ N (0 = 평평, 1.0 = 원본, 1.0 초과 = 굴곡 과장. 실사용 권장 0~2)
    float roughnessScale{ 1.f };   // 0.0 ~ N (텍스처 값에 곱함, clamp(0.05, 1.0)로 최종 클램프. 실사용 권장 0~2)
    float metallicScale{ 1.f };    // 0.0 ~ N (텍스처 값에 곱함, saturate로 0~1 클램프. 1.0 초과는 사실상 의미 없음. 실사용 권장 0~2)
    float aoStrength{ 1.f };       // 0.0 ~ 1.0 (lerp 가중치: 0 = AO 미적용, 1 = AO 완전 적용)
};
