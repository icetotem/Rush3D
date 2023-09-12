#include "stdafx.h"
#include "SceneManager.h"
#include "components/Camera.h"
#include "components/Frustum.h"
#include "components/Transform.h"
#include "components/Bounding.h"
#include "components/CommonComponents.h"

namespace rush
{

    Scene::Scene()
    {
        
    }

    Scene::~Scene()
    {
        Unload();
    }


    void Scene::Update(float deltaTime)
    {

    }

    void Scene::LoadFromScn(const StringView& fileName)
    {

    }

    void Scene::LoadFromGltf(const StringView& fileName)
    {

    }

    void Scene::Unload()
    {

    }

    //////////////////////////////////////////////////////////////////////////

    SceneManager::SceneManager()
    {

    }

    SceneManager::~SceneManager()
    {

    }

    void SceneManager::Update()
    {
        // update cameras and frustums 
        {
            auto view = EcsSystem::registry.view<Camera, Frustum, Transform>();
            for (auto [entity, camera, frustm, transform] : view.each())
            {
                // update frustums
                const auto& position = transform.GetPosition();
                const auto& front = transform.GetForward();
                const auto& left = transform.GetLeft();
                const auto& up = transform.GetUp();

                const float halfVSide = camera.GetFarClip() * tanf(degToRad(camera.GetFov()) * 0.5f);
                const float halfHSide = halfVSide * camera.GetAspect();
                const Vector3 frontMultFar = camera.GetFarClip() * front;

                frustm.SetPlane(FrustumSide::FS_Near, Plane(front, position + camera.GetNearClip() * front)); // near
                frustm.SetPlane(FrustumSide::FS_Far, Plane(-front, position + frontMultFar)); // far
                frustm.SetPlane(FrustumSide::FS_Right, Plane(cross(frontMultFar + left * halfHSide, up), position)); // right
                frustm.SetPlane(FrustumSide::FS_Left, Plane(cross(up, frontMultFar - left * halfHSide), position)); // left
                frustm.SetPlane(FrustumSide::FS_Bottom, Plane(cross(frontMultFar - up * halfVSide, left), position)); // bottom
                frustm.SetPlane(FrustumSide::FS_Top, Plane(cross(left, frontMultFar + up * halfVSide), position)); // top
            }
        }

        // update bondings transforms
        {
            auto view = EcsSystem::registry.view<Transform, Bounding>();
            for (auto [entity, transform, bounding] : view.each())
            {
                bounding.m_AABB = AABB::Transform(bounding.m_InitAABB, transform.GetWorldMatrix());
                bounding.m_OBB = OBB::Transform(bounding.m_InitOBB, transform.GetWorldMatrix());
            }
        }

        Cull();
    }

    void SceneManager::Cull()
    {
        // clear cull flags
        {
            auto view = EcsSystem::registry.view<InFrustumFlag>();
            for (auto [entity, flag] : view.each())
            {
                Entity ent = Entity::Find(entity);
                ent.Remove<InFrustumFlag>();
            }
        }

        // todo octree
        // do culling
        {
            auto view1 = EcsSystem::registry.view<Camera, Frustum>();
            for (auto [entity1, camera, frustum] : view1.each())
            {
                auto view2 = EcsSystem::registry.view<Transform, Bounding>();
                for (auto [entity2, transform, bounding] : view2.each())
                {
                    if (frustum.CullAABB(bounding.m_AABB) == FrustumCullResult::Inside)
                    {
                        Entity ent = Entity::Find(entity2);
                        auto flag = ent.Add<InFrustumFlag>();
                        flag->camera = Entity::Find(entity1);
                    }
                }
            }
        }
    }

}