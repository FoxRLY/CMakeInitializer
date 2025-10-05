#include <CMakeInitializer/Errors.h>
#include <CMakeInitializer/CMakeInitializer.h>
#include <exception>
#include <iostream>

int main(int argc, char* argv[]) {
    try {
        CMakeInitializer initializer = CMakeInitializer(argc, argv);
        initializer.initialize();
    } catch (ExitSignal &e) {
        return 0;
    } catch (std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
