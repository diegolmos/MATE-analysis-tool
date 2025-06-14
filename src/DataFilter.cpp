#include "DataFilter.hpp"
#include <TTree.h>
#include <TEntryList.h>
#include <TTreeFormula.h>
#include <TFile.h>
#include <TDirectory.h>
#include <TROOT.h>
#include <ctime>
#include <iostream>
#include <limits>
#include <sstream>


static bool isCutValid(TTree* tree, const std::string& cut)
{
    TTreeFormula tf("tmpFormula", cut.c_str(), tree);
    return tf.GetNdim() >= 0 && tf.GetTree();
}


FilterResult runInteractive(TTree* tree)
{std::string globalCut;


    while (true) {
        std::cout << "\nCorte actual: "
                  << (globalCut.empty() ? "(ninguno)" : globalCut) << '\n';

        std::cout << "Ingrese nuevo corte (ENTER para continuar): ";
        std::string input;
        std::getline(std::cin, input);
        if (input.empty()) break;

        if (!isCutValid(tree, input)) {
            std::cout << "  ✖ Corte inválido. Revise la sintaxis.\n";
            continue;
        }

        if (!globalCut.empty()) {
            std::cout << "Combinar con 1) AND  o  2) OR  [1/2]: ";
            char op; std::cin >> op;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            globalCut = "(" + globalCut + ")"
                      + (op=='2' ? "||" : "&&")
                      + "(" + input + ")";
        } else {
            globalCut = input;
        }

        Long64_t pass = tree->GetEntries(globalCut.c_str());
        std::cout << "   → Pasan " << pass << " / "
                  << tree->GetEntries() << " eventos.\n";
    }

    if (globalCut.empty()) {
        std::cout << "No se aplicó ningún filtro.\n";
        return {tree, false};
    }


    std::cout << "\nOpciones de salida:\n"
                 "  1) Aplicar EntryList en memoria (rápido)\n"
                 "  2) Crear nuevo archivo .root con árbol filtrado\n"
                 "Seleccione [1/2]: ";
    char mode; std::cin >> mode;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // ===== 1) EntryList =====
    if (mode == '1') {
        tree->SetEntryList(nullptr);
        tree->Draw(">>elist_tmp", globalCut.c_str(), "entrylist");
        auto* elist = static_cast<TEntryList*>(gDirectory->Get("elist_tmp"));
        tree->SetEntryList(elist);

        std::cout << "EntryList aplicado (" << elist->GetN()
                  << " eventos seleccionados).\n";
        return {tree, false};
    }


    std::ostringstream oss;
    oss << "filtered_" << std::time(nullptr) << ".root";
    std::string fname = oss.str();

    auto* outFile = TFile::Open(fname.c_str(), "RECREATE");
    if (!outFile || outFile->IsZombie()) {
        std::cerr << "No se pudo crear '" << fname << "'.\n";
        if (outFile) delete outFile;
        return {tree, false};
    }

    TTree* newTree = tree->CopyTree(globalCut.c_str());
    newTree->Write();                  

    newTree->SetDirectory(nullptr);   
    gROOT->cd();                      
    outFile->Close();                 
    std::cout << "Árbol filtrado guardado en '" << fname
              << "'  (" << newTree->GetEntries() << " eventos).\n";

    return { newTree, true };        
}
