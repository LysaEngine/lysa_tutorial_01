export module engine.rotating_asset_scene;

import lysa;
import engine.camera;
import engine.scene_tree;

export namespace mygame {

    class RotatingAssetScene : public SceneTree {
    public:
        explicit RotatingAssetScene(Camera& camera);

        void onProcess(double alpha) override;

    private:
        // Accumulated rotation angle in radians
        float rotationAngle{0.0f};

        // Rotation speed
        static constexpr float ROTATION_SPEED{0.025f};

        // Directional light coming from the upper-right
        lysa::Light directionalLight{
            lysa::LightType::LIGHT_DIRECTIONAL,
            /*color*/    {1.00f, 0.95f, 0.85f},
            /*intensity*/ 1.5f,
            lysa::mul(
                lysa::float4x4::rotation_x(lysa::radians(-50.0f)),
                lysa::mul(
                    lysa::float4x4::rotation_y(lysa::radians(35.0f)),
                    lysa::float4x4::translation(0.0f, 0.0f, 0.0f))),
        };

        // Omni light from below to soften shadows
        lysa::Light omniLight{
            lysa::LightType::LIGHT_OMNI,
            /*color*/    {0.30f, 0.40f, 0.60f},
            /*intensity*/ 0.6f,
            lysa::float4x4::translation(0.0f, -2.0f, 3.0f),
            /*range*/ 12.0f,
        };
    };

}