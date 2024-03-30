#include "PointLight.h"

PointLight::PointLight(entt::entity handle, Scene* scene)
	: Entity(handle, scene)
{
	AddComponent<PointLightComponent>();
	AddComponent<ShadowMapComponent>();
}

void PointLight::OnUpdate()
{
	LOG_T("Point Light Updated!")
}
