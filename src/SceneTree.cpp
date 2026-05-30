module engine.scene_tree;

namespace mygame {

    lysa::SceneConfiguration sceneConfiguration{};

    SceneTree::SceneTree(Camera& camera):
        Scene(sceneConfiguration),
        camera(camera) {}

    void SceneTree::addInstance(SceneInstance& instance) {
        if (auto* mi = dynamic_cast<SceneMeshInstance*>(&instance)) {
            Scene::addInstance(mi->meshInstance);
        }
        for (const auto& child : instance.children) addInstance(*child);
    }

    void SceneTree::updateInstance(SceneInstance& instance) {
        if (auto* mi = dynamic_cast<SceneMeshInstance*>(&instance)) {
            if (haveInstance(mi->meshInstance)) {
                Scene::updateInstance(mi->meshInstance);
            } else {
                Scene::addInstance(mi->meshInstance);
            }
        }
        for (const auto& child : instance.children) updateInstance(*child);
    }

    void SceneTree::onProcess(const double /*alpha*/) {
        root.update();
        updateInstance(root);
    }

}
