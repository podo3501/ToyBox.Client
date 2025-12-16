#include "pch.h"
#include "UIHelperClass.h"
#include "Shared/System/StepTimer.h"

enum class MovementType
{
	None,
	Lerp,
};

static double GetMovementController(MovementType type, double current, double target, double deltaTime) noexcept
{
	switch (type)
	{
	case MovementType::Lerp: return lerp(current, target, deltaTime);
	}

	return target;
}

BoundedValue::BoundedValue() noexcept :
	m_min{ 0 }, m_max{ 0 }, m_unit{ 0 }
{}

BoundedValue::BoundedValue(int min, int max, int unit) noexcept :
	m_min{ min }, m_max{ max }, m_unit{ unit }
{}

void BoundedValue::Reset() noexcept
{
	m_target = 0;
	m_current = 0.f;
	m_previous = 0;
}

void BoundedValue::SetValue(int value) noexcept
{
	m_target += (value * m_unit);
}

void BoundedValue::SetBounds(int min, int max, int unit) noexcept
{
	m_min = min;
	m_max = max;
	m_unit = unit;
}

int BoundedValue::UpdateLerpedValue(const DX::StepTimer& timer) noexcept
{
	m_target = clamp(m_target, m_min, m_max);

	m_current = GetMovementController(MovementType::Lerp, m_current, m_target, 30.f * timer.GetElapsedSeconds());
	m_current = clamp(m_current, static_cast<double>(m_min), static_cast<double>(m_max));

	int current = static_cast<int>(m_current);
	int gap = current - m_previous;
	m_previous = current;
	return gap;
}

void BoundedValue::SetPositionRatio(float ratio) noexcept
{
	m_target = m_previous = static_cast<int>(ratio * static_cast<float>(m_min));
	m_current = m_target;
}

