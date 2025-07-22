#include "Entity.h"

Entity::Entity(entt::entity entity, Scene* scene)
	: m_EntityHandle{ entity }, m_Scene{ scene }
{
}
