#pragma once
#include "Scene/Entity.h"

class PointLight : public Entity
{
public:
	PointLight(entt::entity handle, Scene* scene);

	virtual void OnUpdate() override;
};

