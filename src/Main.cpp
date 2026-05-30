import lysa;

import main_window;
import engine.camera;
import engine.scene_tree;
import engine.rotating_asset_scene;

using namespace mygame;

// Global engine configuration
lysa::ContextConfiguration contextConfiguration {
    .loggingConfiguration {
        .loggingMode = lysa::LOGGING_MODE_STDOUT,
        .logLevelMin = lysa::LogLevel::INFO,
    },
};

// Main application class
class Application {
public:
    Application():
        lysa(contextConfiguration),
        camera(window),
        scene(std::make_unique<RotatingAssetScene>(camera)) {

        if (!lysa::ctx().fs.directoryExists("app://shaders")) {
            lysa::Log::critical(
                "'shaders' directory not found – run the application from the "
                "project root or build the 'shaders' CMake target first.");
            return;
        }
    }

    void run() {
        // Attach the camera view to the window render target.
        auto view = lysa::RenderView(camera.getCamera(), *scene);
        window.getRenderTarget().addView(view);

        // Forward input events to the active scene.
        lysa::ctx().events.subscribe(
            lysa::RenderingWindowEvent::INPUT, window.id,
            [&](const lysa::Event& evt) {
                const auto& inputEvent =
                    std::any_cast<const lysa::InputEvent&>(evt.payload);
                scene->onInput(inputEvent);
            });

        // Rebuild the viewport/scissors when the window is resized.
        lysa::ctx().events.subscribe(
            lysa::RenderTargetEvent::RESIZED, window.getRenderTarget().id,
            [&](const lysa::Event& evt) {
                view.viewport = {};
                view.scissors = {};
                window.getRenderTarget().updateView(view);
                const auto extent = std::any_cast<const vireo::Extent>(evt.payload);
                scene->onResize(extent);
            });

        // Physics tick (not used here but wired for completeness).
        lysa::ctx().events.subscribe(
            lysa::MainLoopEvent::PHYSICS_PROCESS,
            [&](const lysa::Event& evt) {
                const auto delta = std::any_cast<double>(evt.payload);
                scene->onPhysicsProcess(delta);
            });

        // Main game loop tick: advance scene logic and present the frame.
        lysa::ctx().events.subscribe(
            lysa::MainLoopEvent::PROCESS,
            [&](const lysa::Event& evt) {
                const auto delta = std::any_cast<double>(evt.payload);
                scene->onProcess(delta);
                window.getRenderTarget().render();
            });

        lysa.run();
    }

private:
    lysa::Lysa lysa;
    MainWindow  window;
    Camera camera;
    std::unique_ptr<RotatingAssetScene> scene;
};

// Main entry point
int lysaMain() {
    // On Windows, prefer DirectX by default
    if constexpr (vireo::getPlatform() == vireo::Platform::WINDOWS) {
        contextConfiguration.backendConfiguration.backend = vireo::Backend::DIRECTX;
    }
    Application().run();
    return 0;
}
