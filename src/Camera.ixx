export module engine.camera;

import lysa;
import engine.scene_instance;

export namespace mygame {

    class Camera {
    public:
        Camera( const lysa::RenderingWindow& window);

        ~Camera();

        SceneInstance& getAttachment() const { return *attachment; }

        lysa::Camera& getCamera() { return camera; }

        const lysa::float4x4& getTransform() const { return attachment->getTransform(); }

        void setTransform(const lysa::float4x4& transform) const;

    private:
        struct State {
            lysa::float3 displacement{lysa::FLOAT3ZERO};
            lysa::float2 lookDir{lysa::FLOAT2ZERO};

            State &operator=(const State &other) = default;
        };

        const lysa::RenderingWindow& window;
        static constexpr float near{0.01};
        static constexpr float far{100.0};
        const float fov{lysa::radians(75.0)};

        // view rotation speed when the gamepad or keyboard is used
        const float viewSensitivity{2.0f};
        // view rotation speed when the mouse is used
        const float mouseSensitivity{0.002f};
        // invert the vertical mouse axis
        const float mouseInvertedAxisY{-1.0};
        // invert the vertical keyboard axis
        const float keyboardInvertedAxisY{-1.0};
        // max camera vertical angle
        const float maxCameraAngleUp{lysa::radians(60.0)};
        // min camera vertical angle
        const float maxCameraAngleDown{-lysa::radians(45.0)};
        // movement starting speed
        const float minMovementsSpeed{0.05f};
        // maximum movement speed
        const float maxMovementsSpeed{0.75f};
        // acceleration from minMovementsSpeed to maxMovementsSpeed
        const float acceleration{0.05f};

        // did the mouse is captured ?
        bool mouseCaptured{false};
        // current game pad id
        int gamepad{-1};
        // current movement speed
        float currentMovementSpeed{minMovementsSpeed};
        // Mouse position in last frame
        lysa::float2 lastMousePos;

        // Camera entities
        std::shared_ptr<SceneInstance> attachment;
        std::shared_ptr<SceneInstance> pivot;
        lysa::Camera camera;
        // events callbacks
        lysa::unique_id onResize;
        lysa::unique_id onInput;
        lysa::unique_id onPhysicsProcess;
        lysa::unique_id onProcess;

        State previousState;
        State currentState;

        void captureMouse();

        void releaseMouse();

        void rotate(float x, float y) const;
    };

}
