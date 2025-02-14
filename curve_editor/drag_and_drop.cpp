#include "config.hpp"
#include "constants.hpp"
#include "curve_editor.hpp"
#include "drag_and_drop.hpp"
#include "global.hpp"
#include "util.hpp"
#include <mkaul/util_hook.hpp>
#include <mkaul/aviutl.hpp>
#include <regex>



namespace curve_editor {
	int16_t DragAndDrop::get_track_script_idx() noexcept {
		const std::regex regex_script_name{ std::format(R"(^Type1@{}(\x01.+)?$)", global::PLUGIN_DISPLAY_NAME) };
		auto tra_script_names = global::exedit_internal.get<const char*>(0x231488u);
		int16_t script_idx = 0;

		while (true) {
			if (!tra_script_names[script_idx]) return -1;
			if (std::regex_match(tra_script_names[script_idx], regex_script_name)) {
				break;
			}
			script_idx++;
		}
		return script_idx;
	}

	// イージングが適用されるトラックバーのインデックスの配列を取得する
	void DragAndDrop::get_applied_track_idcs(int32_t obj_idx, int32_t track_idx, std::vector<int32_t>& ret) noexcept {
		ret.clear();
		ret.emplace_back(track_idx);

		auto offset_idcs = global::exedit_internal.call_cdecl<int*>(0x2ca70u, obj_idx, track_idx);
		for (size_t i = 0u; i < 2u; i++) {
			if (offset_idcs[i] != 0) {
				ret.emplace_back(track_idx + offset_idcs[i]);
			}
		}
		// 重複削除
		std::sort(ret.begin(), ret.end());
		ret.erase(std::unique(ret.begin(), ret.end()), ret.end());
	}

	// 指定したトラックバーにCEのイージングを適用する
	void DragAndDrop::apply_easing_to_track(int32_t obj_idx, int32_t track_idx) noexcept {
		int16_t type_idx = 0;
		int32_t param;
		auto obj_array = *(global::exedit_internal.p_array_obj());
		auto p_obj = &obj_array[obj_idx];
		EditMode mode = EditMode::Normal;

		auto curve = global::id_manager.get_curve<NumericGraphCurve>(curve_id_);
		if (curve) {
			mode = curve->get_type();
			param = curve->encode();
		}
		else {
			mode = global::config.get_edit_mode();
			param = global::editor.track_param();
		}

		switch (mode) {
		case EditMode::Elastic:
		case EditMode::Bounce:
			type_idx = 1;
			break;

		default:
			type_idx = 0;
		}
		
		int16_t apply_mode = (int16_t)global::config.get_apply_mode(mode);
		int16_t script_idx_offset = type_idx * (int16_t)ApplyMode::NumApplyMode + apply_mode;

		// track_mode, track_paramの該当要素の値を変更
		mkaul::replace_var(&(p_obj->track_mode[track_idx].num), ExEdit::Object::TrackMode::isScript);
		mkaul::replace_var(
			&(p_obj->track_mode[track_idx].script_idx),
			(int16_t)(get_track_script_idx() + script_idx_offset)
		);
		mkaul::replace_var(&(p_obj->track_param[track_idx]), param);
	}

	void DragAndDrop::apply_easing_to_tracks(int32_t obj_idx, int32_t track_idx) noexcept {
		std::vector<int32_t> idcs;
		get_applied_track_idcs(obj_idx, track_idx, idcs);
		for (const auto idx : idcs) {
			apply_easing_to_track(obj_idx, idx);
		}
	}

	bool DragAndDrop::init() noexcept {
		auto hwnd_objdialog = *global::exedit_internal.p_hwnd_objdialog();

		for (int i = 0; i < ExEdit::Object::MAX_TRACK; i++) {
			if (!buttons_[i].init(hwnd_objdialog, i)) {
				return false;
			}
		}

		auto hr = ::D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &p_factory_);
		if (FAILED(hr)) {
			d2d_init_failed_ = true;
			return false;
		}
		D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(
				DXGI_FORMAT_B8G8R8A8_UNORM,
				D2D1_ALPHA_MODE_IGNORE
			),
			96.f, 96.f,
			D2D1_RENDER_TARGET_USAGE_NONE,
			D2D1_FEATURE_LEVEL_DEFAULT
		);
		hr = p_factory_->CreateDCRenderTarget(&props, &p_render_target_);
		if (FAILED(hr)) {
			d2d_init_failed_ = true;
			return false;
		}
		return true;
	}

	void DragAndDrop::exit() noexcept {
		if (p_render_target_) {
			p_render_target_->Release();
			p_render_target_ = nullptr;
		}
		if (p_factory_) {
			p_factory_->Release();
			p_factory_ = nullptr;
		}
	}

	bool DragAndDrop::drag(uint32_t curve_id) noexcept {
		if (!dragging_) {
			curve_id_ = curve_id;
			dragging_ = true;
			return true;
		}
		else return false;
	}

	void DragAndDrop::update() noexcept {
		if (dragging_ and !d2d_init_failed_) {
			for (const auto& button : buttons_) {
				if (button.is_visible() and button.is_hovered()) {
					auto current_obj_idx = *(global::exedit_internal.p_current_obj_idx());
					if (0 <= current_obj_idx) {
						std::vector<int32_t> track_idcs;
						get_applied_track_idcs(current_obj_idx, button.track_idx(), track_idcs);
						// ハイライト対象のインデックスが変化したときにのみ再描画
						if (track_idcs != track_idcs_buffer_) {
							for (const auto idx : track_idcs_buffer_) {
								buttons_[idx].unhighlight();
							}
							for (const auto idx : track_idcs) {
								buttons_[idx].highlight(p_render_target_, curve_id_);
							}
							track_idcs_buffer_ = track_idcs;
						}
					}
					return;
				}
			}
			for (const auto idx : track_idcs_buffer_) {
				buttons_[idx].unhighlight();
			}
			track_idcs_buffer_.clear();
		}
	}

	bool DragAndDrop::drop() noexcept {
		if (dragging_) {
			dragging_ = false;
			if (!d2d_init_failed_) {
				for (const auto idx : track_idcs_buffer_) {
					buttons_[idx].unhighlight();
					track_idcs_buffer_.clear();
				}
			}
			for (int i = 0; i < ExEdit::Object::MAX_TRACK; i++) {
				if (buttons_[i].is_visible() and buttons_[i].is_hovered()) {
					auto current_obj_idx = *(global::exedit_internal.p_current_obj_idx());
					auto selected_obj_num = *(global::exedit_internal.p_selected_obj_num());
					auto selected_obj_idcs = global::exedit_internal.array_selected_obj_idcs();
					auto obj_array = *(global::exedit_internal.p_array_obj());
					
					if (obj_array) {
						// オブジェクト設定ダイアログが表示されている時
						if (0 <= current_obj_idx) {
							apply_easing_to_tracks(current_obj_idx, i);
						}
						// オブジェクトが選択されている時
						for (int j = 0; j < selected_obj_num; j++) {
							if (i < obj_array[selected_obj_idcs[j]].track_n) {
								apply_easing_to_tracks(selected_obj_idcs[j], i);
							}
						}
						// オブジェクト設定ダイアログの更新
						global::exedit_internal.call_cdecl<BOOL>(0x305e0u, current_obj_idx);
					}
					return true;
				}
			}
		}
		return false;
	}
} // namespace curve_editor