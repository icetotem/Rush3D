
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
            for (auto [entity, camera, frustm, transform, tag] : view.each())
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
            auto view = EcsSystem::registry.view<Transform, Bounding, Tag>();
            for (auto [entity, transform, bounding, tag] : view.each())
            {
                bounding.m_AABB = AABB::Transform(bounding.m_InitAABB, transform.GetWorldMatrix());
                bounding.m_OBB = OBB::Transform(bounding.m_InitOBB, transform.GetWorldMatrix());
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
            for (auto [entity, flag, Tag] : view.each())
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
            }
        }
    }

    template <class Tag>
    void SceneManager::Render(Ref<Renderer> renderer, Ref<RSurface> surface)
    {
        using namespace rush;

        HMap<Camera*, Ref<RenderQueue>> renderContent;

        // fetch culled results
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
                renderQueue = CreateRef<RenderQueue>();
                renderQueue->camera = flag.camera;
                renderContent.insert({ cam, renderQueue });
            }
            else
            {
                renderQueue = iter->second;
            }

            RUSH_ASSERT(renderQueue != nullptr);

            // update global uniforms
            auto globalUniforms = RMaterialInst::GetGlobalUniformBuffer();

            Matrix4 buff[] = { cam->GetViewMatrix(), cam->GetProjMatrix() };
            globalUniforms->UpdateData(buff, sizeof(Matrix4) * 2);

            for (const auto& part : meshRdr.m_Primitives)
            {
                auto batch = renderQueue->NewBatch();
                batch->indexBuffer = part.indexBuffer;
                batch->vertexBuffers = part.vertexBuffers;
                batch->pipeline = part.material->m_Material->m_Pipeline;
                batch->bindGroup = part.material->m_BindGroup;
            }
        }

        // render
        for (auto& [cam, renderQueue] : renderContent)
        {
            renderer->BeginDraw();
            // TODO:
//             FrameBufferInfo info;
//             renderer->DrawQuad(info, nullptr);

            if (surface)
            {
                renderer->DrawSurface(*surface, "SurfaceTexture");
            }

            renderer->EndDraw();
        }
    }

}