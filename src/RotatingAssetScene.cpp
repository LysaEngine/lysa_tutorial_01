module engine.rotating_asset_scene;

import engine.scene_instance;

namespace mygame {

    RotatingAssetScene::RotatingAssetScene(Camera& camera)
        : SceneTree(camera) {

        // Ambient light keeps unlit faces from going pitch-black.
        setEnvironment({lysa::float3{1.0f, 1.0f, 1.0f}, 0.20f});

        // Place the asset at the world origin.
        const auto asset = load("app://res/models/crate.assets");
        root.setTransform(lysa::float4x4::identity());
        root.addChild(asset);
        addInstance(root);

        // Step back a few units so the asset is fully visible on startup.
        camera.setTransform(lysa::float4x4::translation(0.0f, 0.0f, 4.0f));

        // Lights ────────────────────────────────────────────────────────────────
        directionalLight.castShadows = true;
        directionalLight.shadowMapSize = 2048;
        directionalLight.shadowMapCascadesCount = 3;
        directionalLight.shadowMapCascadesSplitLambda = 0.75f;
        addLight(directionalLight);
        addLight(omniLight);
    }

    void RotatingAssetScene::onProcess(const double alpha) {
        // alpha is the elapsed time in seconds for this frame.
        rotationAngle += static_cast<float>(alpha) * ROTATION_SPEED;

        // Keep the angle in [0, 2π) to avoid floating-point drift over time.
        if (rotationAngle > lysa::radians(360.0f)) {
            rotationAngle -= lysa::radians(360.0f);
        }

        // Apply the rotation around the world Y-axis.
        root.setTransform(lysa::float4x4::rotation_y(rotationAngle));

        // Let the base class propagate the transform and submit render instances.
        SceneTree::onProcess(alpha);
    }

}
