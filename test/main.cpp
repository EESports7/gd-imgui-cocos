#include <Geode/Geode.hpp>
#include <imgui-cocos.hpp>
#include <DroidSans_ttf.h>

#ifdef GEODE_IS_WINDOWS
	#define strcpy strcpy_s
#endif

$on_mod(Loaded) {
	ImGuiCocos::get().setup([] {
		ImGui::StyleColorsDark();
		auto& io = ImGui::GetIO();
		ImFontConfig cfg;
		strcpy(cfg.Name, "DroidSans");
		cfg.FontDataOwnedByAtlas = false;
		io.Fonts->AddFontFromMemoryTTF(DroidSans_ttf_data, DroidSans_ttf_size, 18.f, &cfg);
		io.Fonts->AddFontDefault();
	}).draw([] {

		static float colors[4];
		static char chars[256];

		if (ImGui::Begin("So much text!")) {
			static float size = 18;
			ImGui::PushFont(nullptr, size);

			ImGui::Text("Wow! this window sure has a lot of text.");
			ImGui::Text("Ого! В этом окне и правда много текста.");

			ImGui::Separator();

			ImGui::DragFloat("Size", &size, 0.25f, 5.f, 100.f);
			ImGui::InputText("Text Input", chars, 256);

			ImGui::Separator();

			ImGui::ColorPicker4("Color Picker 4", colors);

			ImGui::PopFont();
		}
		ImGui::End();

		ImGui::ShowDemoWindow();
	});
}

#ifndef GEODE_IS_IOS
#include <Geode/modify/CCKeyboardDispatcher.hpp>
class $modify(ImGuiKeybindHook, cocos2d::CCKeyboardDispatcher) {
	bool dispatchKeyboardMSG(cocos2d::enumKeyCodes key, bool isKeyDown, bool isKeyRepeat, double time) {
		if (key == cocos2d::enumKeyCodes::KEY_F4 && isKeyDown) {
			ImGuiCocos::get().toggle();
		}
		return CCKeyboardDispatcher::dispatchKeyboardMSG(key, isKeyDown, isKeyRepeat, time);
	}
};
#endif