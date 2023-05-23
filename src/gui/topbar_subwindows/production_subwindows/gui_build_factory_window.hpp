#pragma once

#include "gui_element_types.hpp"

namespace ui {

namespace build_factory {


			//move_child_to_front(new_factory);
			//new_factory->set_visible(state, true);
			//new_factory->impl_set(state, payload);

class cancel_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		parent->set_visible(state, false);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("production_close_tooltip"));
		text::close_layout_box(contents, box);
	}
};

class build_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::factory_type_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::factory_type_id>(payload);
			auto fat = dcon::fatten(state.world, content);

			if(!fat.is_valid()) {
				disabled = true;
			} else {
				disabled = false;
			}
		}
	}
};

class build_output_name_text : public simple_text_element_base {
public:
	std::string get_text(sys::state& state, dcon::factory_type_id ftid) noexcept {
		auto fat = dcon::fatten(state.world, ftid);
		auto name = fat.get_name();
		return text::produce_simple_string(state, name);
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::factory_type_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::factory_type_id>(payload);
			set_text(state, get_text(state, content));
		}
	}
};

class build_cost_text : public simple_text_element_base {
public:
	std::string get_text(sys::state& state, dcon::factory_type_id ftid) noexcept {
		auto fat = dcon::fatten(state.world, ftid);
		auto& name = fat.get_construction_costs();
		auto total = 0.0f;
		for(uint32_t i = 0; i < economy::commodity_set::set_size; i++) {
			auto cid = name.commodity_type[i];
			if(bool(cid)) {
				total += state.world.commodity_get_current_price(cid) * name.commodity_amounts[i];
			}
		}	// Credit to leaf for this code :3
		return text::format_money(total);
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::factory_type_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::factory_type_id>(payload);
			set_text(state, get_text(state, content));
		}
	}
};

class build_time_text : public simple_text_element_base {
public:
	std::string get_text(sys::state& state, dcon::factory_type_id ftid) noexcept {
		auto fat = dcon::fatten(state.world, ftid);
		auto name = fat.get_construction_time();
		return (text::prettify(name) + "d");
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::factory_type_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::factory_type_id>(payload);
			set_text(state, get_text(state, content));
		}
	}
};

class new_factory_option_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::factory_type_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::factory_type_id>(payload);
			Cyto::Any payload2 = element_selection_wrapper<dcon::factory_type_id>{content};
			parent->impl_get(state, payload2);
		}
	}
};

class new_factory_option : public listbox_row_element_base<dcon::factory_type_id> {
private:
	image_element_base* output_icon = nullptr;
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "bg") {
			//return make_element_by_type<button_element_base>(state, id);
			auto ptr = make_element_by_type<new_factory_option_button>(state, id);
			ptr->base_data.size.x = base_data.size.x;
			return ptr;

		} else if(name == "output") {
			//auto ptr = make_element_by_type<image_element_base>(state, id);
			//output_icon = ptr.get();
			//return ptr;
			return make_element_by_type<commodity_factory_image>(state, id);

		} else if(name == "name") {
			return make_element_by_type<build_output_name_text>(state, id);


		} else if(name == "total_build_cost") {
			return make_element_by_type<build_cost_text>(state, id);

		} else if(name == "buildtime") {
			return make_element_by_type<build_time_text>(state, id);

		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::factory_type_id>()) {
			payload.emplace<dcon::factory_type_id>(content);
			return message_result::consumed;
		} else if(payload.holds_type<dcon::commodity_id>()) {
			payload.emplace<dcon::commodity_id>(dcon::fatten(state.world, content).get_output().id);
			return message_result::consumed;
		}
		return listbox_row_element_base<dcon::factory_type_id>::get(state, payload);
	}
};

class new_factory_list : public listbox_element_base<new_factory_option, dcon::factory_type_id> {
protected:
	std::string_view get_row_element_name() override {
		return "new_factory_option";
	}
public:
	void on_update(sys::state& state) noexcept override{
		if(parent) {
			row_contents.clear();
			state.world.for_each_factory_type([&](dcon::factory_type_id ident) {
				row_contents.push_back(ident);
			});
			update(state);
		}
	}
};

class state_name : public simple_text_element_base {
public:
	std::string get_text(sys::state& state, dcon::state_instance_id sid) noexcept {
		return "The Glorious State of UwU";
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::state_instance_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::state_instance_id>(payload);
			set_text(state, get_text(state, content));
		}
	}
};



class build_new_factory_window : public window_element_base {
private:
	dcon::state_instance_id state_id;
	dcon::factory_type_id factory_to_build;
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		set_visible(state, false);
	}

	/*
	 * There are only two hard things in Computer Science:
	 * Cache invalidation and naming thing -Phil Karlton
	 */
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "bg") {
			return make_element_by_type<image_element_base>(state, id);
		} else
		if(name == "state_name") {
			return make_element_by_type<state_name>(state, id);
		} else
		if(name == "factory_type") {
			return make_element_by_type<new_factory_list>(state, id);
		} else
		if(name == "input_label") {
			return make_element_by_type<image_element_base>(state, id);
		} else
			// input_0_amount
			// input_1_amount
			// input_2_amount
			// input_3_amount
		if(name == "output") {
			return make_element_by_type<commodity_factory_image>(state, id);
		} else
		if(name == "building_name") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else
		if(name == "output_label") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else
		if(name == "output_ammount") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else
			// input_0
			// input_1
			// input_2
			// input_3
		if(name == "description_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else
		if(name == "needed_workers") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else
		if(name == "needed_workers_count") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else
		if(name == "available_workers") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else
		if(name == "resource_cost_label") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else
		if(name == "in_stockpile_label") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else
		if(name == "base_price_label") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else
		if(name == "input_price_label") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else
		if(name == "total_label") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else
		if(name == "current_funds_label") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else
		if(name == "base_price") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else
		if(name == "input_price") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else
		if(name == "total_price") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else
		if(name == "you_have") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else
		if(name == "prod_label") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else
		if(name == "prod_cost") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else
		if(name == "cancel") {
			return make_element_by_type<cancel_button>(state, id);
		} else
		if(name == "build") {
			return make_element_by_type<build_button>(state, id);
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<element_selection_wrapper<dcon::state_instance_id>>()) {
			state_id = any_cast<dcon::state_instance_id>(payload);
			return message_result::consumed;
		} else if(payload.holds_type<element_selection_wrapper<dcon::factory_type_id>>()) {
			//factory_to_build = any_cast<dcon::factory_type_id>(payload).data;
			factory_to_build = any_cast<element_selection_wrapper<dcon::factory_type_id>>(payload).data;
			return message_result::consumed;
		} else if(payload.holds_type<dcon::factory_type_id>()) {
			payload.emplace<dcon::factory_type_id>(factory_to_build);
			return message_result::consumed;
		} else if(payload.holds_type<dcon::commodity_id>()) {
			payload.emplace<dcon::commodity_id>(dcon::fatten(state.world, factory_to_build).get_output());
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

}

}
