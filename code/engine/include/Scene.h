#ifndef Scene_h__
#define Scene_h__


namespace rush
{

    class Scene
    {
    public:
        Scene();
        ~Scene();

        void Update(float deltaTime);

        void LoadFromScn(const StringView& fileName);

        void LoadFromGltf(const StringView& fileName);

        void Unload();

    protected:
    };

}

#endif // Scene_h__
