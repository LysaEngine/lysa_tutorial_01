export module engine.scene_instance;

import lysa;

export namespace mygame {

    // Base scene node ─────────────────────────────────────────────────────────
    class SceneInstance {
    public:
        const SceneInstance* parent{nullptr};
        lysa::float4x4 globalTransform;
        std::vector<std::shared_ptr<SceneInstance>> children;
        const std::string name;
        bool visible{true};
        bool dirty{true};

        SceneInstance(
            const std::string&    name           = "",
            const lysa::float4x4& localTransform = lysa::float4x4::identity())
            : localTransform(localTransform), name(name) {
            SceneInstance::update();
        }

        SceneInstance(const SceneInstance& orig)
            : localTransform(orig.localTransform),
              globalTransform(orig.globalTransform),
              name(orig.name) {}

        void setVisible(bool isVisible) {
            visible = isVisible;
            for (const auto& child : children) child->setVisible(visible);
            dirty = true;
        }

        void addChild(const std::shared_ptr<SceneInstance>& child) {
            child->parent = this;
            children.push_back(child);
            update();
        }

        bool removeChild(const std::shared_ptr<SceneInstance>& child) {
            const auto it = std::ranges::find(children, child);
            if (it != children.end()) {
                (*it)->parent = nullptr;
                children.erase(it);
                update();
                return true;
            }
            return false;
        }

        virtual void update() {
            const auto parentTransform =
                parent ? parent->globalTransform : lysa::float4x4::identity();
            globalTransform = lysa::mul(localTransform, parentTransform);
            for (const auto& child : children) child->update();
        }

        void setTransform(const lysa::float4x4& tr) {
            localTransform = tr;
            dirty = true;
        }

        std::string getRelativePath(const std::shared_ptr<SceneInstance>& root) const {
            if (parent) {
                const auto path = parent->getRelativePath(root);
                return path.empty() ? name : path + "/" + name;
            }
            return "";
        }

        const auto& getTransform() const { return localTransform; }
        bool        haveParent()   const { return parent != nullptr; }

        virtual ~SceneInstance() = default;

    protected:
        lysa::float4x4 localTransform;
    };

    // Renderable scene node ───────────────────────────────────────────────────
    struct SceneMeshInstance : SceneInstance {
        lysa::MeshInstance meshInstance;

        SceneMeshInstance(
            const lysa::Mesh&     mesh,
            const std::string&    name,
            const lysa::float4x4& localTransform = lysa::float4x4::identity())
            : SceneInstance(name, localTransform),
              meshInstance(mesh, name) {
            SceneMeshInstance::update();
        }

        SceneMeshInstance(const SceneMeshInstance& orig)
            : SceneInstance(orig.name, orig.localTransform),
              meshInstance(orig.meshInstance) {
            SceneMeshInstance::update();
        }

        void update() override {
            SceneInstance::update();
            meshInstance.setTransform(globalTransform);
            meshInstance.setAABB(
                meshInstance.getMesh().getAABB().toGlobal(meshInstance.getTransform()));
            meshInstance.setVisible(visible);
            dirty = false;
        }

        void setCastShadows(bool castShadows) {
            meshInstance.setCastShadows(castShadows);
        }
    };

    // Dummy animation player (required by AssetsPack::load template)
    struct DummyAnimationPlayer : SceneInstance {
        auto add(const std::shared_ptr<lysa::AnimationLibrary>&, const std::string& = "") const {}
        std::shared_ptr<lysa::AnimationLibrary> getLibrary() const {
            return std::make_shared<lysa::AnimationLibrary>();
        }
        void setCurrentAnimation(const std::string&) const {}
    };

    //  Factory helpers ─────────────────────────────────────────────────────────
    std::shared_ptr<SceneInstance> load(const std::string& fileURI);
    std::shared_ptr<SceneInstance> clone(const std::shared_ptr<SceneInstance>& orig);

}
