
namespace rush
{
    template <class Tag>
    Scene<Tag>::~Scene()
    {
        Unload();
    }

    template <class Tag>
    Entity Scene<Tag>::CreateEntity(const StringView& name /*= ""*/)
    {
        Entity entity = Entity::Create(name);
        entity.Add<Tag>();
        return entity;
    }

    template <typename Tag>
    void Scene<Tag>::Update(float deltaTime)
    {

    }

    template <typename Tag>
    void Scene<Tag>::LoadFromScn(const StringView& fileName)
    {

    }

    template <typename Tag>
    void Scene<Tag>::LoadFromGltf(const StringView& fileName)
    {

    }

    template <typename Tag>
    void Scene<Tag>::Unload()
    {

    }

    //////////////////////////////////////////////////////////////////////////

    template <typename Tag>
    Ref<Scene<Tag>> SceneManager::CreateScene()
    {
        return CreateRef<Scene<Tag>>();
    }

    template <class Tag>
    void SceneManager::Update(Ref<Renderer> renderer, Ref<RSurface> surface)
    {
        // update cameras and frustums 
        {
            auto view = EcsSystem::registry.view<Camera, Frustum, Transform, Tag>();
            for (auto [entity, camera, frustum, transform, tag] : view.each())
            {
                camera.Update(renderer);
                frustum.UpdateFromCamera(Entity::Find(entity));
            }
        }

        // update bondings transforms
        {
            auto view = EcsSystem::registry.view<Transform, Bounding, Tag>();
            for (auto [entity, transform, bounding, tag] : view.each())
            {
                bounding.Update();
            }
        }

        {
            auto view = EcsSystem::registry.view<Transform, Light, Tag>();
            for (auto [entity, transform, light, tag] : view.each())
            {
                light.UpdateBoundings();
            }
        }

        Cull<Tag>();
        Render<Tag>(renderer, surface);
    }

    template <class Tag>
    void SceneManager::Cull()
    {
        using namespace rush;

        // clear cull flags
        {
            auto view = EcsSystem::registry.view<InFrustumFlag, Tag>();
            for (auto [entity, flag, tag] : view.each())
            {
                Entity ent = Entity::Find(entity);
                ent.Remove<InFrustumFlag>();
            }
        }

        // todo octree
        // do culling
        {
            auto view1 = EcsSystem::registry.view<Camera, Frustum, Tag>();
            for (auto [entity1, camera, frustum, tag] : view1.each())
            {
                auto view2 = EcsSystem::registry.view<Transform, Bounding, Tag>();
                for (auto [entity2, transform, bounding, tag] : view2.each())
                {
                    if (frustum.CullAABB(bounding.m_AABB) == FrustumCullResult::Inside)
                    {
                        Entity ent = Entity::Find(entity2);
                        auto flag = ent.Add<InFrustumFlag>();
                        flag->camera = Entity::Find(entity1);
                    }
                }

                auto view3 = EcsSystem::registry.view<Transform, Light, Tag>();
                for (auto [entity2, transform, light, tag] : view3.each())
                {
                    if (frustum.CullAABB(light.GetAABB()) == FrustumCullResult::Inside)
                    {
                        Entity ent = Entity::Find(entity2);
                        auto flag = ent.Add<InFrustumFlag>();
                        flag->camera = Entity::Find(entity1);
                    }
                }
            }
        }
    }

    template <class Tag>
    void SceneManager::Render(Ref<Renderer> renderer, Ref<RSurface> surface)
    {
        HMap<Camera*, Ref<RenderQueue>> renderContent;

        // fetch culled results
        {
            auto view = EcsSystem::registry.view<InFrustumFlag, MeshRenderer>();
            for (auto [entity, flag, meshRdr] : view.each())
            {
                Entity ent = Entity::Find(entity);
                auto cam = flag.camera.Get<Camera>();
                cam->Update(renderer);
                auto iter = renderContent.find(cam);
                Ref<RenderQueue> renderQueue;
                if (iter == renderContent.end())
                {
                    renderQueue = CreateRef<RenderQueue>(flag.camera);
                    renderContent.insert({ cam, renderQueue });
                }
                else
                {
                    renderQueue = iter->second;
                }

                RUSH_ASSERT(renderQueue != nullptr);

                meshRdr.SubmitRenderQueue(renderQueue);
            }
        }
        // lights
        {
            auto view = EcsSystem::registry.view<InFrustumFlag, Light>();
            for (auto [entity, flag, light] : view.each())
            {
                Entity ent = Entity::Find(entity);
                auto cam = flag.camera.Get<Camera>();
                auto iter = renderContent.find(cam);
                RUSH_ASSERT(iter != renderContent.end());
                Ref<RenderQueue> renderQueue = iter->second;
                renderQueue->AddLight(&light);
            }
        }

        // render
        for (auto& [cam, renderQueue] : renderContent)
        {
            renderQueue->MergeAndSortBatch();
            renderer->Render(renderQueue, surface);
        }
    }

}