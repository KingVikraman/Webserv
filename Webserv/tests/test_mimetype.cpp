#include "../includes/FileHandler.hpp"

int main() {
    FileHandler fh;
    std::cout << "MIME test results:\n";
    std::cout << "index.html -> " << fh.getMimeType("public/index.html") << "\n";
    std::cout << "style.css -> " << fh.getMimeType("public/style.css") << "\n";
    std::cout << "images/logo.png -> " << fh.getMimeType("public/images/logo.png") << "\n";
    return 0;
}

