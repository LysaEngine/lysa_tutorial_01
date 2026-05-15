module engine.scene_instance;

import lysa.types;

namespace mygame {

    std::shared_ptr<SceneInstance> clone(const std::shared_ptr<SceneInstance>& orig) {
        std::shared_ptr<SceneInstance> root;
        if (const auto& mi = std::dynamic_pointer_cast<SceneMeshInstance>(orig)) {
            root = std::make_shared<SceneMeshInstance>(*mi);
        } else {
            root = std::make_shared<SceneInstance>(*orig);
        }
        for (const auto& child : orig->children) root->addChild(clone(child));
        return root;
    }

    std::shared_ptr<SceneInstance> load(const std::string& fileURI) {
        return lysa::AssetsPack::load<SceneInstance, SceneMeshInstance, DummyAnimationPlayer>(fileURI);
    }

}