#include "stdafx.h"
#include "SceneManager.h"

namespace rush
{
    
    SceneManager::SceneManager()
    {
        m_MainScene = CreateScene<MainSceneTag>();
    }

}