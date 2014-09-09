#pragma once

#include <Rocket/Core.h>

class RocketSystemInterface : public Rocket::Core::SystemInterface
{
public:
	virtual float GetElapsedTime() override;
};