#include "mechanism.hpp"

#include <fmt/format.h>

#include "gui.hpp"

void make_mechanism(mechanism_def& data,
                    const std::string& cat_name, const std::string& name,
                    const std::unordered_map<std::string, double>& values) {
    data.name = fmt::format("{}/{}", cat_name, name);
    log_debug("Fetching cat {}", cat_name);
    auto cat = catalogues.at(cat_name);
    log_debug("Fetching mach {}", name);
    auto info = cat[name];
    data.globals.clear();
    data.parameters.clear();
    data.states.clear();
    log_debug("Setting values");
    for (const auto& [k, v]: info.globals)    data.globals[k]    = values.contains(k) ? values.at(k) : v.default_value;
    for (const auto& [k, v]: info.parameters) data.parameters[k] = values.contains(k) ? values.at(k) : v.default_value;
    for (const auto& [k, v]: info.state)      data.states[k]     = values.contains(k) ? values.at(k) : v.default_value;
}

void gui_mechanism(id_type id, mechanism_def& data, event_queue& evts) {
    ZoneScopedN(__FUNCTION__);
    with_id mech_guard{id};
    auto open = gui_tree("##mechanism-tree");
    ImGui::SameLine();
    if (ImGui::BeginCombo("##mechanism-choice", data.name.c_str())) {
        for (const auto& [cat_name, cat]: catalogues) {
            ImGui::Selectable(cat_name.c_str(), false);
            with_indent ind{};
            for (const auto& name: cat.mechanism_names()) {
                if (gui_select(fmt::format("  {}##{}", name, cat_name), data.name)) make_mechanism(data, cat_name, name);
            }
        }
        ImGui::EndCombo();
    }
    gui_right_margin();
    if (ImGui::Button(icon_delete)) evts.push_back(evt_del_mechanism{id});
    if (open) {
        if (!data.globals.empty()) {
            ImGui::BulletText("Global Values");
            with_indent ind{};
            for (auto& [k, v]: data.globals) gui_input_double(k, v);
        }
        if (!data.states.empty()) {
            ImGui::BulletText("State Variables");
            with_indent ind{};
            for (auto& [k, v]: data.states) gui_input_double(k, v);
        }
        if (!data.parameters.empty()) {
            ImGui::BulletText("Parameters");
            with_indent ind{};
            for (auto& [k, v]: data.parameters) gui_input_double(k, v);
        }
        ImGui::TreePop();
    }
}
