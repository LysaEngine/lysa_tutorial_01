export module engine.scene_tree;

import lysa;
import engine.camera;
import engine.scene_instance;

export namespace mygame {

    class SceneTree : public lysa::Scene {
    public:
        explicit SceneTree(Camera& camera);

        virtual void onPhysicsProcess(double delta) {}
        virtual void onProcess(double alpha);
        virtual void onInput(const lysa::InputEvent& event) {}
        virtual void onResize(const vireo::Extent& extent) {}

    protected:
        Camera& camera;
        SceneInstance root{"Root"};

        void addInstance(SceneInstance& instance);
        void updateInstance(SceneInstance& instance);
    };

}
