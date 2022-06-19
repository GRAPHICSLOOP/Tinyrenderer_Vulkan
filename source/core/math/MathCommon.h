#pragma once

namespace tiny
{
	struct Rotation
	{
	public:
		Rotation(float y = 0.f, float r = 0.f, float p = 0.f)
			:yaw(y),roll(r),pitch(p)
		{

		}

	public:
		float yaw;
		float roll;
		float pitch;
	};
}
