#ifndef RECTANGLE_HPP__
#define RECTANGLE_HPP__

#include "../sprite.h"

class rectangle {
public:
	rectangle() = default;
	rectangle(glm::vec3 color, ItemFrame const& frame)
		: color{color}, frame{frame} {
		ResourceManager& rm = ResourceManager::get_instance();
		shader = rm.request_shader("basic-rectangle");
	}
	auto draw() noexcept
		-> void {
		shader.set("rect_color", color);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(frame.tlc_pos, 0.0f));
		if (fcomp(frame.angle, 0.0f)) {
			model = glm::translate(model, glm::vec3(0.5f * frame.size, 0.0f)); 
			model = glm::rotate(model, glm::radians(frame.angle), glm::vec3(0.0f, 0.0f, 1.0f)); 
			model = glm::translate(model, glm::vec3(-0.5f * frame.size, 0.0f));
		}
		model = glm::scale(model, glm::vec3(frame.size, 0.0f));
		shader.set("model", model);
		vao.draw();
	}

	auto scale_by(glm::vec2 val) 
		-> rectangle& {
		frame.size += val;
		return *this;
	}
	auto rescale(glm::vec2 val)
		-> rectangle& {
		frame.size = val;
		return *this;
	}
private:
	VAO vao{};
	Shader shader;
	glm::vec3 color;
	ItemFrame frame;
};

#endif