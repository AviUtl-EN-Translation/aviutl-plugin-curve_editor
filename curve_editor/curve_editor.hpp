#pragma once

#include <vector>
#include "curve.hpp"
#include "enum.hpp"
#include "curve_editor_graph.hpp"
#include "curve_editor_script.hpp"



namespace cved {
	namespace global {
		inline class CurveEditor {
		private:
			GraphCurveEditor editor_graph_;
			ScriptCurveEditor editor_script_;

		public:
			CurveEditor() {}

			int32_t track_param() noexcept;
			Curve* current_curve() noexcept;
			size_t current_idx() noexcept;
			bool set_idx(size_t idx) noexcept;
			bool advance_idx(int n) noexcept;
			void jump_to_last_idx() noexcept;
			bool is_idx_first() noexcept { return current_idx() == 0u; }
			bool is_idx_last() noexcept;
			void delete_last_idx() noexcept;
			void reset_id_curves() noexcept;

			auto& editor_graph() noexcept { return editor_graph_; }
			auto& editor_script() noexcept { return editor_script_; }

			template <class Archive>
			void serialize(Archive& archive, const std::uint32_t) {
				archive(
					editor_graph_,
					editor_script_
				);
			}
		} editor;
	}
}

CEREAL_CLASS_VERSION(cved::global::CurveEditor, 0)