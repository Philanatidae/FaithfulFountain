/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/render/CameraRenderNode.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <ff/Locator.hpp>

namespace ff {

CameraRenderNode::CameraRenderNode(std::string const& camera)
    :RenderNode("Camera"),_camera(camera) {
    getOutput(OUTPUT_COMBINED_MAT) = std::make_unique<MatrixRenderNodeIO>("combined", glm::identity<glm::mat4x4>());
    getOutput(OUTPUT_TRANSFORM_MAT) = std::make_unique<MatrixRenderNodeIO>("transform", glm::identity<glm::mat4x4>());
    getOutput(OUTPUT_PERSPECTIVE_MAT) = std::make_unique<MatrixRenderNodeIO>("perspective", glm::identity<glm::mat4x4>());
}

void CameraRenderNode::setCamera(std::string const& camera) {
    _camera = camera;
}
std::string CameraRenderNode::getCamera() const {
    return _camera;
}

void CameraRenderNode::prepare() {
}
void CameraRenderNode::process(float const& alpha) {
    Camera* cam = Locator::getCameraManager().getCamera(_camera);
    if(cam == nullptr) {
        ((MatrixRenderNodeIO*)getOutput(OUTPUT_COMBINED_MAT).get())->setValue(glm::identity<glm::mat4x4>());
        ((MatrixRenderNodeIO*)getOutput(OUTPUT_TRANSFORM_MAT).get())->setValue(glm::identity<glm::mat4x4>());
        ((MatrixRenderNodeIO*)getOutput(OUTPUT_PERSPECTIVE_MAT).get())->setValue(glm::identity<glm::mat4x4>());
    } else {
        ((MatrixRenderNodeIO*)getOutput(OUTPUT_COMBINED_MAT).get())->setValue(cam->getCombined(alpha));
        ((MatrixRenderNodeIO*)getOutput(OUTPUT_TRANSFORM_MAT).get())->setValue(cam->getTransformMatrix());
        ((MatrixRenderNodeIO*)getOutput(OUTPUT_PERSPECTIVE_MAT).get())->setValue(cam->getProjectionMatrix());
    }
}

void CameraRenderNode::drawImGuiNodeContents() {
    ImGui::PushItemWidth(200);

    std::vector<std::string> cameras = Locator::getCameraManager().getCameras();
    if(ImGui::BeginCombo("Camera", _camera.c_str())) {
        for(int i = 0; i < cameras.size(); i++) {
            bool isSelected = cameras[i] == _camera;
            if(ImGui::Selectable(cameras[i].c_str(),
                isSelected)) {
                setCamera(cameras[i]);
            }

            if(isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
}
    
}

