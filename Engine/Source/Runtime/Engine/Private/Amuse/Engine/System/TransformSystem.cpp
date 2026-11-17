//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/Engine/System/TransformSystem.h>

namespace Amuse {

	//! @brief 親の変換を合成して子階層まで再帰的に更新する。
	static void UpdateTransformRecursively(entt::registry& world, Entity entity, const TransformComponent* parent = nullptr);
	
	SystemAccess TransformSystem::Access = SystemAccess{}
		.read<TransformComponent,HierarchyComponent>()
		.write<TransformComponent>();


	void TransformSystem::Update(entt::registry& world) {

		HashSet<Entity> roots;

		for (auto [entity] : world.view<TransformDirtyFlag>().each()) {

			Entity root = entity;

			do {
				auto& hierarchy = world.get<HierarchyComponent>(entity);

				if (world.all_of<TransformDirtyFlag>(hierarchy.parent)) {
					root = entt::null;
					break;
				}

				entity = hierarchy.parent;
			} while (entity != entt::null);

			if (root != entt::null) {
				roots.emplace(root);
			}
		}

		for (auto entity : roots) {
			UpdateTransformRecursively(world,entity);
		}

	}

	//! @brief 親の変換を合成して子階層まで再帰的に更新する。
	static void UpdateTransformRecursively(entt::registry& world, Entity entity,const TransformComponent* parent) {

		auto& hierarchy = world.get<HierarchyComponent>(entity);
		auto& child = world.get<TransformComponent>(entity);
		
		// 階層解決

		child.world = parent ? parent->world * child.local : child.local;
		
		// 再帰更新
		entity = hierarchy.child;

		while (entity != entt::null) {
			UpdateTransformRecursively(world, entity, &child);
			entity = world.get<HierarchyComponent>(entity).next;
		}
	}

}
