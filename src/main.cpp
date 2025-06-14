#include "PathConfig.hpp"
#include "DataValidator.hpp"
#include "DateSelector.hpp"
#include "DataProcessor.hpp"
#include "TreeLoader.hpp"
#include "DataFilter.hpp"
#include "HistogramMaker.hpp"
#include "RateFitter.hpp"          
#include <TFile.h>
#include <TTree.h>
#include <TROOT.h>
#include <iostream>
#include <sstream>
#include <ctime>
#include <limits>


static void clearCin()
{std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');}

static void saveTreeToFile(TTree* tree)
{
    std::ostringstream oss;
    oss << "concatenated_" << std::time(nullptr) << ".root";
    std::string outName = oss.str();

    auto* f = TFile::Open(outName.c_str(), "RECREATE");
    if (!f || f->IsZombie()) {
        std::cerr << "No se pudo crear '" << outName << "'.\n";
        if (f) delete f;
        return;
    }
    TTree* copy = tree->CloneTree(-1);
    copy->Write();
    copy->SetDirectory(nullptr);     
    gROOT->cd();
    f->Close();

    std::cout << "Árbol concatenado guardado en '" << outName
              << "'  (" << copy->GetEntries() << " eventos).\n";
}


int main(int argc, char** argv)
{

    PathConfig cfg;
    if (argc > 1) cfg.dataDir    = argv[1];
    if (argc > 2) cfg.badDir     = argv[2];
    if (argc > 3) cfg.reportFile = argv[3];
    cfg = PathConfig::makeInteractive(cfg);


    while (true)
    {
        std::cout << "\n=============== MENÚ ===============\n"
                     "  0) Salir\n"
                     "  1) Procesar .txt  →  .root\n"
                     "  2) Cargar / concatenar .root  (filtros / histos / rate)\n"
                     "Seleccione [0/1/2]: ";
        char opt; std::cin >> opt; clearCin();

 
        if (opt == '0') { std::cout << "Hasta luego.\n"; break; }

        
        if (opt == '1') {
            std::cout << "\n=== VALIDACIÓN DE TRIADAS (.txt) ===\n";
            auto ok = DataValidator::validateDirectory(cfg);
            if (ok.empty()) { std::cout<<"No hay fechas válidas.\n"; continue; }

            auto sel = selectDatePrefixes(ok);
            if (sel.empty()) { std::cout<<"No se seleccionó ninguna fecha.\n"; continue; }

            std::cout<<"\n=== CONVERSIÓN A TTree (DataProcessor) ===\n";
            for (auto& pre: sel) DataProcessor::processDate(cfg, pre);
            std::cout<<"Procesamiento finalizado.\n";
            continue;
        }

        
        if (opt == '2') {
            TTree* tree = loadRootChainInteractive(cfg);
            if (!tree) { std::cout<<"No se cargó ningún árbol.\n"; continue; }

            
            std::cout<<"¿Guardar árbol concatenado como un único .root? [s/N]: ";
            char gConc; std::cin>>gConc; clearCin();
            if (gConc=='s'||gConc=='S') saveTreeToFile(tree);

           
            std::cout<<"¿Aplicar filtros (DataFilter)? [s/N]: ";
            char f; std::cin>>f; clearCin();
            if (f=='s'||f=='S') {
                FilterResult fr = runInteractive(tree);
                tree = fr.tree;
            }

           
            while (true) {
                std::cout<<"¿Generar histogramas? [s/N]: ";
                char h; std::cin>>h; clearCin();
                if (h!='s' && h!='S') break;
                runHistograms(tree);
            }

         
            std::cout<<"¿Calcular ajuste exponencial del rate? [s/N]: ";
            char r; std::cin>>r; clearCin();
            if (r=='s'||r=='S') runRateFit(tree);

            std::cout<<"Operación con .root terminada.\n";
            continue;
        }

        std::cout<<"Opción inválida.\n";
    }
    return 0;
}
