#include <iostream>

template<typename Enum>
	requires std::is_enum_v<Enum>
std::string stringify(Enum enum) {

}

int main() {
	std::cout << "magic\n";
}

