#include "stdafx.h"
#include "components/CameraCtrl.h"
#include "Engine.h"

namespace rush
{

    //     gainput::InputMap map(m_InputManager);
    //         enum Button
    //         {
    //             ButtonConfirm
    //         };

    CameraCtrl::CameraCtrl(Entity owner)
        : Component(owner)
    {
        m_InputBinding = CreateRef<gainput::InputMap>(Engine::instance().m_InputManager);
        //         map.MapBool(ButtonConfirm, keyboardId, gainput::KeyReturn);
//         map.MapBool(ButtonConfirm, mouseId, gainput::MouseButtonLeft);
//         map.MapBool(ButtonConfirm, padId, gainput::PadButtonA);
//         map.MapBool(ButtonConfirm, touchId, gainput::Touch0Down);
    }

    void CameraCtrl::Update()
    {
        //         map.MapBool(ButtonConfirm, keyboardId, gainput::KeyReturn);
//         map.MapBool(ButtonConfirm, mouseId, gainput::MouseButtonLeft);
//         map.MapBool(ButtonConfirm, padId, gainput::PadButtonA);
//         map.MapBool(ButtonConfirm, touchId, gainput::Touch0Down);
    }

}