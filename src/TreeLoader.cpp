#include "TreeLoader.hpp"
#include "DateSelector.hpp"         

#include <TChain.h>
#include <TFile.h>
#include <TTree.h>
#include <TEnv.h>

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <regex>
#include <set>
#include <sstream>

namespace fs = std::filesystem;


static std::string extractDate(const std::string& prefix)

{ return fs::path(prefix).stem().string().substr(0, 8);}

static std::vector<std::string> listRootPrefixes(const std::string& dataDir)

{
    std::vector<std::string> pref;
    const std::string tag = "output.root";

    for (const auto& e : fs::directory_iterator(dataDir)) {
        const std::string p = e.path().string();
        if (p.size() >= tag.size() &&
            p.compare(p.size() - tag.size(), tag.size(), tag) == 0)
            pref.emplace_back(p.substr(0, p.size() - tag.size()));
    }
    std::sort(pref.begin(), pref.end());
    return pref;
}

static void previewTree(TTree* t)

{
    gEnv->SetValue("TTree.PrintPrecision", 20);
    gEnv->SetValue("TTree.PrintFormat",    "F");

    std::cout << "\nVista rápida del árbol:\n";
    t->Scan("ts:A1[0]:B1[0]:A2[0]:B2[0]:A3[0]:B3[0]:"
            "nA1:nB1:nA2:nB2:nA3:nB3:"
            "ts2_m101:ts2_m102:ts2_m103:evt",
            "", "colsize=22 precision=20 fmt=F");
}


TTree* loadRootChainInteractive(const PathConfig& cfg)
{
    
    auto prefixes = listRootPrefixes(cfg.dataDir);
    if (prefixes.empty()) {
        std::cout << "\nNo se encontraron .root en " << cfg.dataDir << '\n';
        return nullptr;
    }

    
    auto selected = selectDatePrefixes(prefixes);
    if (selected.empty()) {
        std::cout << "No se seleccionó ninguna fecha.\n";
        return nullptr;
    }

    
    std::cout << "\nSe han seleccionado " << selected.size()
              << " fecha(s). ¿Deseas continuar? [S/n]: ";
    char go; std::cin >> go;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (go == 'n' || go == 'N') {
        std::cout << "Operación cancelada.\n";
        return nullptr;
    }

    
    if (selected.size() == 1) {
        std::cout << "Solo hay un archivo. ¿Abrirlo individualmente? [s/N]: ";
        char solo; std::cin >> solo;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (solo == 's' || solo == 'S') {
            std::string fname = selected[0] + "output.root";
            auto* file = new TFile(fname.c_str(), "READ");
            if (!file || file->IsZombie()) {
                std::cerr << "No se pudo abrir " << fname << '\n';
                return nullptr;
            }
            auto* tree = static_cast<TTree*>(file->Get("tree"));
            if (!tree) {
                std::cerr << "No se encontró 'tree' en " << fname << '\n';
                return nullptr;
            }

            std::cout << "\nArchivo cargado. Eventos: "
                      << tree->GetEntries() << '\n';

            std::cout << "¿Ver Scan? [s/N]: ";
            char ver; std::cin >> ver;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (ver == 's' || ver == 'S') previewTree(tree);

            return tree;          
        }
    }

    
    auto* chain = new TChain("tree");
    for (const auto& pre : selected)
        chain->Add((pre + "output.root").c_str());

    std::cout << "\n⇒ Concatenado " << chain->GetEntries() << " eventos "
              << "de " << selected.size() << " archivo(s).\n";

    
    std::cout << "¿Ver Scan del árbol concatenado? [s/N]: ";
    char ver; std::cin >> ver;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (ver == 's' || ver == 'S') previewTree(chain);

    return chain;  
}
