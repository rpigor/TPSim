#pragma once

#include "liberty/LibertyLibrary.hpp"
#include "CellLibrary.hpp"
#include <string>

class LibertyParser {
private:

    liberty::Library internalLib;
    liberty::CellLibraryBuilder libBuilder;

public:

    void read(const std::string& libFile);
    CellLibrary getLibrary() const;

};