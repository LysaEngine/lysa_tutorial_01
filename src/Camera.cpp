module engine.camera;

namespace mygame {

    Camera::Camera( const lysa::RenderingWindow& window):
        window(window),
        attachment(std::make_shared<SceneInstance>("Camera attachment", lysa::float4x4::identity())),
        pivot(std::make_shared<SceneInstance>("Camera pivot", lysa::float4x4::identity())),
        camera(lysa::float4x4::identity(),
               lysa::perspective(fov, window.getRenderTarget().getAspectRatio(), near, far),
               near, far) {

        attachment->addChild(pivot);

        onResize = lysa::ctx().events.subscribe(lysa::RenderTargetEvent::RESIZED, window.getRenderTarget().id, [&](const lysa::Event&) {
            camera.projection = lysa::perspective(fov, window.getRenderTarget().getAspectRatio(), near, far);
        });

        onInput = lysa::ctx().events.subscribe(lysa::RenderingWindowEvent::INPUT, window.id, [&](const lysa::Event& evt) {
            const auto& inputEvent = std::any_cast<const lysa::InputEvent&>(evt.payload);
            // capture the mouse on the first click if not already captured
           if ((inputEvent.type == lysa::InputEventType::MOUSE_BUTTON)) {
                const auto& event = std::get<lysa::InputEventMouseButton>(inputEvent.data);
               if (!event.pressed) {
                   if (mouseCaptured) {
                       releaseMouse();
                   } else {
                       captureMouse();
                   }
               }
           }
        });

        onPhysicsProcess = lysa::ctx().events.subscribe(lysa::MainLoopEvent::PHYSICS_PROCESS, [&](const lysa::Event& evt) {
            const auto delta = std::any_cast<double>(evt.payload);
            // if the mouse is captured, process the player keyboard events
            if (mouseCaptured && lysa::Input::isKeyJustPressed(lysa::KEY_ESCAPE)) {
                releaseMouse();
            }
            // reset the movement states
            previousState = currentState;
            currentState  = State{};
            // get the input vector, first from the game pad, second for the keyboard
            lysa::float2 input = lysa::FLOAT2ZERO;
            if (gamepad != -1) {
               input = lysa::Input::getGamepadVector(gamepad, lysa::GamepadAxisJoystick::LEFT);
                // capture the mouse if the player wants to move
                if (!mouseCaptured && any(input != lysa::FLOAT2ZERO)) {
                    captureMouse();
                }
            }
            if (mouseCaptured && all(input == lysa::FLOAT2ZERO)) {
               input = lysa::Input::getKeyboardVector(lysa::KEY_A, lysa::KEY_D, lysa::KEY_W, lysa::KEY_S);
            }
            // check if the player wants to move
            if (any(input != lysa::FLOAT2ZERO)) {
                // if the player is on the ground or on an object, we move in the input direction
                const auto direction = lysa::mul(lysa::TRANSFORM_BASIS, lysa::float3{input.x, 0, input.y});
                if (currentMovementSpeed == 0) {
                    // player start moving
                    currentMovementSpeed = minMovementsSpeed;
                } else {
                    // player already moving, accelerate
                    currentMovementSpeed += acceleration * delta;
                    currentMovementSpeed = std::min(currentMovementSpeed, maxMovementsSpeed);
                }
                // move
                currentState.displacement += direction * currentMovementSpeed;
            } else {
                // stop all movements
                currentMovementSpeed = 0;
            }
            // If the mouse is captured, we can use the game pad or the keyboard
            // to rotate the view
            if (mouseCaptured) {
                lysa::float2 inputDir = lysa::FLOAT2ZERO;
                if (gamepad != -1) {
                   inputDir = lysa::Input::getGamepadVector(gamepad, lysa::GamepadAxisJoystick::RIGHT);
                }
                if (all(inputDir == lysa::FLOAT2ZERO)) {
                   inputDir = lysa::Input::getKeyboardVector(lysa::KEY_LEFT, lysa::KEY_RIGHT, lysa::KEY_UP, lysa::KEY_DOWN);
                }
                if (any(inputDir != lysa::FLOAT2ZERO)) {
                   currentState.lookDir = inputDir * viewSensitivity * delta;
                }
            }
            attachment->setTransform(lysa::mul(
                lysa::float4x4::translation(previousState.displacement * (1.0f - delta) + currentState.displacement * delta),
                attachment->getTransform()));
            if (any(currentState.lookDir != lysa::FLOAT2ZERO)) {
               const auto interpolatedLookDir = previousState.lookDir * (1.0f - delta) + currentState.lookDir * delta;
               rotate(interpolatedLookDir.y * -keyboardInvertedAxisY, -interpolatedLookDir.x * 1.0f);
            }
        });

        onProcess = lysa::ctx().events.subscribe(lysa::MainLoopEvent::PROCESS, [&](const lysa::Event&evt) {
            if (mouseCaptured) {
                if (lysa::Input::isKeyPressed(lysa::KEY_Q)) {
                    currentState.displacement.y = minMovementsSpeed;
                } else if (lysa::Input::isKeyPressed(lysa::KEY_Z)) {
                    currentState.displacement.y = -minMovementsSpeed;
                }
                const auto mousePos = window.getMousePosition();
                auto offset = (lastMousePos - mousePos) * mouseSensitivity;
                if (any(offset != lysa::FLOAT2ZERO)) {
                    offset.y *= mouseInvertedAxisY;
                    rotate(offset.y, offset.x);
                }
                window.resetMousePosition();
                lastMousePos = window.getMousePosition();
            }

            attachment->update();
            camera.transform = lysa::mul(lysa::float4x4::identity(), pivot->globalTransform);
        });
    }

    void Camera::rotate(const float x, const float y) const {
        attachment->setTransform(lysa::mul(lysa::float4x4::rotation_y(y), attachment->getTransform()));
        pivot->setTransform(lysa::mul(
            lysa::float4x4::rotation_x(x),
            pivot->getTransform()));
        const auto pivotRotation = lysa::quaternion{lysa::float3x3{pivot->getTransform()}};
        const auto pivotRotationX = static_cast<float>(lysa::euler_angles(pivotRotation).x);
        const auto angle = std::clamp(
            pivotRotationX,
            maxCameraAngleDown,
            maxCameraAngleUp);
        pivot->setTransform(lysa::mul(
            lysa::float4x4::rotation_x(angle - pivotRotationX),
            pivot->getTransform()));
    }

    Camera::~Camera() {
        lysa::ctx().events.unsubscribe(onProcess);
        lysa::ctx().events.unsubscribe(onPhysicsProcess);
        lysa::ctx().events.unsubscribe(onInput);
        lysa::ctx().events.unsubscribe(onResize);
    }

    void Camera::setTransform(const lysa::float4x4& transform) const {
        attachment->setTransform(transform);
    }

    void Camera::releaseMouse() {
        window.setMouseMode(lysa::MouseMode::VISIBLE);
        mouseCaptured = false;
    }

    void Camera::captureMouse() {
        if (!mouseCaptured) {
            window.setMouseMode(lysa::MouseMode::HIDDEN_CAPTURED);
            mouseCaptured = true;
            lastMousePos = window.getMousePosition();
        }
    }

}
