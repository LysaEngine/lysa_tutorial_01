export module main_window;

import lysa;

export namespace mygame {

    // Window & renderer configuration ────────────────────────────────────────
    lysa::RenderingWindowConfiguration renderingWindowConfiguration {
        .title  = "Lysa – Tutorial 1",
        .mode   = lysa::RenderingWindowMode::WINDOWED,
        .width  = 1280,
        .height = 720,
        .renderTargetConfiguration = {
            .presentMode = vireo::PresentMode::VSYNC,
            .rendererConfiguration = {
                .rendererType          = lysa::RendererType::DEFERRED,
                .depthStencilFormat    = vireo::ImageFormat::D32_SFLOAT,
                .clearColor            = lysa::float3{0.0f, 0.2f, 0.4f},
                .toneMappingType       = lysa::ToneMappingType::ACES,
                .postProcessAntiAliasingType = lysa::PostProcessAntiAliasingType::SMAA,
                .bloomEnabled          = true,
                .ambientOcclusionType  = lysa::AmbientOcclusionType::GTAO,
            }
        }
    };

    // Main application window ─────────────────────────────────────────────────
    class MainWindow : public lysa::RenderingWindow {
    public:
        MainWindow() : RenderingWindow(renderingWindowConfiguration) {
            // Show the window once the engine is ready
            lysa::ctx().events.subscribe(lysa::RenderingWindowEvent::READY, id,
                [&](const lysa::Event&) {
                    auto title = renderingWindowConfiguration.title;
                    title += (lysa::ctx().config.backendConfiguration.backend == vireo::Backend::VULKAN)
                             ? " (Vulkan)" : " (DirectX)";
                    setTitle(title);
                    show();
                });
            // Let the engine know the user wants to quit.
            lysa::ctx().events.subscribe(lysa::RenderingWindowEvent::CLOSING, id,
                [&](const lysa::Event&) { lysa::ctx().exit = true; });
        }
    };

} // namespace mygame
