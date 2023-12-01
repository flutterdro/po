#include "resource-manager.h"


auto compile_shaders()
    -> void {
    engine::resource_manager& rm = engine::resource_manager::get_instance();
    for (auto const& dir_entry : fs::directory_iterator{fs::path{"shaders"}}) {
        if (dir_entry.is_directory()) {
            rm.submit<engine::shader>(dir_entry.path().stem().string(), engine::shader{dir_entry.path()});
        }
    }
}
