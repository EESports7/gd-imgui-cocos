#include <imgui-cocos.hpp>

#include <Geode/Geode.hpp>
#include <Geode/modify/CCMouseDispatcher.hpp>
#include <Geode/modify/CCIMEDispatcher.hpp>
#include <Geode/modify/CCTouchDispatcher.hpp>
#include <Geode/modify/CCKeyboardDispatcher.hpp>

#include <imgui.h>

using namespace geode::prelude;

#ifndef GEODE_IS_IOS
class $modify(CCMouseDispatcher) {
	bool dispatchScrollMSG(float x, float y) {
		if (!ImGuiCocos::get().isInitialized())
			return CCMouseDispatcher::dispatchScrollMSG(x, y);

		auto& io = ImGui::GetIO();
		static constexpr float scrollMult = 1.f / 10.f;
		io.AddMouseWheelEvent(y * scrollMult, -x * scrollMult);

		if (!io.WantCaptureMouse) {
			return CCMouseDispatcher::dispatchScrollMSG(x, y);
		}
		return true;
	}
};
#endif

// 2.2 adds some new arguments to the dispatchers
#if GEODE_COMP_GD_VERSION >= 22000
	#define IF_2_2(...) __VA_ARGS__
#else
	#define IF_2_2(...)
#endif

#if GEODE_COMP_GD_VERSION >= 22070
	#define IF_2_207(...) __VA_ARGS__
#else
	#define IF_2_207(...)
#endif

#if GEODE_COMP_GD_VERSION >= 22080
	#define IF_2_208(...) __VA_ARGS__
#else
	#define IF_2_208(...)
#endif

class $modify(CCIMEDispatcher) {
	void dispatchInsertText(const char* text, int len IF_2_2(, enumKeyCodes keys)) {
		if (!ImGuiCocos::get().isInitialized())
			return CCIMEDispatcher::dispatchInsertText(text, len IF_2_2(, keys));

		auto& io = ImGui::GetIO();
		if (!io.WantCaptureKeyboard) {
			CCIMEDispatcher::dispatchInsertText(text, len IF_2_2(, keys));
		}
		std::string str(text, len);
		io.AddInputCharactersUTF8(str.c_str());
	}

	void dispatchDeleteBackward() {
		if (!ImGuiCocos::get().isInitialized())
			return CCIMEDispatcher::dispatchDeleteBackward();

		auto& io = ImGui::GetIO();
		if (!io.WantCaptureKeyboard) {
			CCIMEDispatcher::dispatchDeleteBackward();
		}
		// is this really how youre supposed to do this
		io.AddKeyEvent(ImGuiKey_Backspace, true);
		io.AddKeyEvent(ImGuiKey_Backspace, false);
	}
};

ImGuiKey cocosToImGuiKey(cocos2d::enumKeyCodes key) {
	if (key >= KEY_A && key <= KEY_Z) {
		return static_cast<ImGuiKey>(ImGuiKey_A + (key - KEY_A));
	}
	if (key >= KEY_Zero && key <= KEY_Nine) {
		return static_cast<ImGuiKey>(ImGuiKey_0 + (key - KEY_Zero));
	}
	switch (key) {
		case KEY_Up: return ImGuiKey_UpArrow;
		case KEY_Down: return ImGuiKey_DownArrow;
		case KEY_Left: return ImGuiKey_LeftArrow;
		case KEY_Right: return ImGuiKey_RightArrow;

		case KEY_Control: return ImGuiMod_Ctrl;
		case KEY_LeftWindowsKey: return ImGuiMod_Super;
		case KEY_Shift: return ImGuiMod_Shift;
		case KEY_Alt: return ImGuiMod_Alt;
		case KEY_Enter: return ImGuiKey_Enter;

		case KEY_Home: return ImGuiKey_Home;
		case KEY_End: return ImGuiKey_End;

		#ifndef GEODE_IS_MACOS
			case KEY_Delete: return ImGuiKey_Delete;
		#endif
		case KEY_Escape: return ImGuiKey_Escape;

		#ifdef GEODE_IS_ANDROID
			case KEY_LeftControl: return ImGuiKey_ModCtrl;
			case KEY_RightContol: return ImGuiKey_ModCtrl;
			case KEY_LeftShift: return ImGuiKey_ModShift;
			case KEY_RightShift: return ImGuiKey_ModShift;
		#endif

		default: return ImGuiKey_None;
	}
}

bool shouldBlockInput() {
	auto& inst = ImGuiCocos::get();
	return inst.isVisible() && inst.getInputMode() == ImGuiCocos::InputMode::Blocking;
}

#ifndef GEODE_IS_IOS
class $modify(CCKeyboardDispatcher) {
	bool dispatchKeyboardMSG(enumKeyCodes key, bool down IF_2_2(, bool repeat) IF_2_208(, double time)) {
		if (!ImGuiCocos::get().isInitialized())
			return CCKeyboardDispatcher::dispatchKeyboardMSG(key, down IF_2_2(, repeat) IF_2_208(, time));

		const bool shouldEatInput = ImGui::GetIO().WantCaptureKeyboard || shouldBlockInput();
		if (shouldEatInput || !down) {
			if (const auto imKey = cocosToImGuiKey(key); imKey != ImGuiKey_None) {
				ImGui::GetIO().AddKeyEvent(imKey, down);
			}
		}

		#ifdef GEODE_IS_MOBILE
        if (down) {
            char c = 0;
            if (key >= KEY_A && key <= KEY_Z) {
                c = static_cast<char>(key);
                if (!io.KeyShift) {
                    c = static_cast<char>(tolower(c));
                }
            } else if (key >= KEY_Zero && key <= KEY_Nine) {
                c = static_cast<char>('0' + (key - KEY_Zero));
            } else if (key == KEY_Space) {
                c = ' ';
            }

            if (c != 0) {
                std::string str(1, c);
                io.AddInputCharactersUTF8(str.c_str());
            }
        }
        if (key == KEY_Backspace) {
            io.AddKeyEvent(ImGuiKey_Backspace, true);
            io.AddKeyEvent(ImGuiKey_Backspace, false);
        }
        #endif

		if (shouldEatInput) {
			return false;
		} else {
			return CCKeyboardDispatcher::dispatchKeyboardMSG(key, down IF_2_2(, repeat) IF_2_208(, time));
		}
	}

	#if defined(GEODE_IS_MACOS)
	static void onModify(auto& self) {
		Result<> res = self.setHookPriorityBeforePre("CCKeyboardDispatcher::updateModifierKeys", "geode.custom-keybinds");
		if (!res) {
			log::warn("Failed to set hook priority for CCKeyboardDispatcher::updateModifierKeys: {}", res.unwrapErr());
		}
	}

	void updateModifierKeys(bool shft, bool ctrl, bool alt, bool cmd) {
        auto& io = ImGui::GetIO();

        io.AddKeyEvent(ImGuiMod_Shift, shft);
        io.AddKeyEvent(ImGuiMod_Ctrl, ctrl);
        io.AddKeyEvent(ImGuiMod_Alt, alt);
        io.AddKeyEvent(ImGuiMod_Super, cmd);
        CCKeyboardDispatcher::updateModifierKeys(shft, ctrl, alt, cmd);
    }
	#endif
};
#endif

class $modify(CCTouchDispatcher) {
	static void onModify(auto& self) {
		if (!self.setHookPriorityPre("cocos2d::CCTouchDispatcher::touches", Priority::First)) {
			log::warn("Failed to set hook priority for touches");
		}
	}

	void touches(CCSet* touches, CCEvent* event, unsigned int type) {
		if (!ImGuiCocos::get().isInitialized() || !touches)
			return CCTouchDispatcher::touches(touches, event, type);

		auto& io = ImGui::GetIO();
		auto* touch = static_cast<CCTouch*>(touches->anyObject());

		if (!touch) return CCTouchDispatcher::touches(touches, event, type);

		// add mouse source events, so imgui can handle touches right
		if (geode::cocos::getMousePos().isZero()) {
			// touch->getLocation() can be different from geode::cocos::getMousePos()!
			const auto pos = ImGuiCocos::cocosToFrame(touch->getLocation());
			io.AddMouseSourceEvent(ImGuiMouseSource_TouchScreen);
			io.AddMousePosEvent(pos.x, pos.y);
		}

		if (io.WantCaptureMouse || shouldBlockInput()) {
			if (type == CCTOUCHBEGAN) {
				io.AddMouseSourceEvent(ImGuiMouseSource_TouchScreen);
				io.AddMouseButtonEvent(0, true);
			} else if (type == CCTOUCHENDED || type == CCTOUCHCANCELLED) {
				io.AddMouseSourceEvent(ImGuiMouseSource_TouchScreen);
				io.AddMouseButtonEvent(0, false);
			}
			if (type == CCTOUCHMOVED) {
				CCTouchDispatcher::touches(touches, event, CCTOUCHCANCELLED);
			}
		} else {
			if (type != CCTOUCHMOVED) {
				io.AddMouseSourceEvent(ImGuiMouseSource_TouchScreen);
				io.AddMouseButtonEvent(0, false);
			}
			CCTouchDispatcher::touches(touches, event, type);
		}
	}
};

// need imgui to be drawn inbetween glClear and swapBuffers:
// drawScene() {
//   glClear();
//   draw current scene();
//   <- here!
//   swapBuffers();
// }
// swapBuffers on android and macos doesnt do anything, so hooking it might not work,
// and because it doesnt do anything just drawing imgui at the end of drawScene works fine

#if defined(GEODE_IS_WINDOWS) || defined(GEODE_IS_IOS)

#include <Geode/modify/CCEGLView.hpp>

class $modify(CCEGLView) {
#ifdef IMGUI_COCOS_HOOK_EARLY
	static void onModify(auto& self) {
		if (!self.setHookPriorityPre("cocos2d::CCEGLView::swapBuffers", Priority::Early)) {
			log::warn("Failed to set hook priority for swapBuffers");
		}
	}
#endif

	void swapBuffers() {
		if (ImGuiCocos::get().isInitialized())
			ImGuiCocos::get().drawFrame();

		CCEGLView::swapBuffers();
	}

#ifdef GEODE_IS_WINDOWS
	void toggleFullScreen(bool value IF_2_2(, bool borderless) IF_2_207(, bool fix)) {
		if (!ImGuiCocos::get().isInitialized())
			return CCEGLView::toggleFullScreen(value IF_2_2(, borderless) IF_2_207(, fix));

		ImGuiCocos::get().destroy();
		CCEGLView::toggleFullScreen(value IF_2_2(, borderless) IF_2_207(, fix));
		ImGuiCocos::get().setup();
	}
#endif
};

#else

#include <Geode/modify/CCDirector.hpp>

class $modify(CCDirector) {
	void drawScene() {
		CCDirector::drawScene();
		if (ImGuiCocos::get().isInitialized())
			ImGuiCocos::get().drawFrame();
	}
};

#endif
