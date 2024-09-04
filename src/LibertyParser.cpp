#include "LibertyParser.hpp"
#include "liberty/LibertyGrammar.hpp"
#include <fstream>

void LibertyParser::read(const std::string& libFile) {
    std::ifstream in(libFile, std::fstream::in);
    std::vector<char> buffer{std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
    std::replace(buffer.begin(), buffer.end(), '\n', ' ');
    std::replace(buffer.begin(), buffer.end(), '\\', ' ');
    auto first = buffer.begin();
    liberty::liberty_parse(first, buffer.end(), internalLib);
    libBuilder(internalLib);
}

CellLibrary LibertyParser::getLibrary() const {
    return libBuilder.getLibrary();
}