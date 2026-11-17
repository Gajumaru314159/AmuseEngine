//***********************************************************
//! @file
//! @author		Gajumaru
//***********************************************************
#include <Amuse/Core/Core.h>
#include <Amuse/RHI/Forward.h>
#include <entt/entt.hpp>
#include <gtest/gtest.h>

using namespace Amuse;

class Material;


class World {
public:
    template<typename... TComponents>
	auto spawn(TComponents&&... components) {
		auto entity = m_registry.create();
        {
            m_registry.emplace(components),
            ...
        }
		return entity;
	}
private:
	entt::registry m_registry;
};



class WorldTest: public ::testing::Test {
protected:
    virtual void SetUp() {
    };
    virtual void TearDown() {
    };
protected:
    World m_world;
};




struct Mesh {
    Ref<Mesh> mesh;
};
struct OverriteMaterial {
	Vector<Ref<Material>> materials;
};
struct Camera {
    // Window
    // Texture
    s32 display = 0;
    Ref<RenderTexture> texture;


};
struct SkyLight {
};
struct PointLight {
    Color color;
};


TEST_F(WorldTest, World2) {

    m_world.spawn(
        Transform::Identity,
        Mesh()
    );
    m_world.spawn(
        Transform{ .position {0,0,-10} },
        Camera{ }
    );
    m_world.spawn(
        Transform{
            .position {0,0,-10} 
        },
        PointLight{ 
            .color = Color{1,1,1,1}
        }
    );


    m_world.spawn(
        Transform::Identity,
        Mesh{},
        OverriteMaterial{}
    );

    m_world.spawn(
        Transform{ .position {0,0,-10} },
        Camera{ .display = 1 }
    );
    m_world.spawn(
        SkyLight{}
    );

}


template<class T>
class Res {
public:
    Res(T&& resource) {
        m_resource = std::make_shared(resource);
    }
    const T& operator*() const {
        return *m_resource;
    }
    const T* operator->() const {
        return m_resource.get();
    }
    const T* get() const {
        return m_resource.get();
    }
protected:
    SPtr<T> m_resource;
};





struct MeshLodDesc
{
    Vector<Vec3>			positions;		//!< 位置配列
    Vector<Vec3>			normals;		//!< 法線配列
    Vector<Vec3>			binormals;		//!< 従法線配列
    Vector<Vec3>			tangents;		//!< 接線配列
    Vector<Vec2>			uvs;			//!< UV 配列
    Vector<Color>			colors;			//!< 頂点色配列
};


void AssetTest() {
    Res<Camera> cam = Camera{
        .display = 0
    };

    auto camera = Res(Camera{
	    .display = 0,
    });
}