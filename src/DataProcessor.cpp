#include "DataProcessor.hpp"
#include "PathConfig.hpp"   //RUTAS

#include <TFile.h>
#include <TTree.h>

#include <algorithm>
#include <bitset>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>

namespace fs = std::filesystem;


void DataProcessor::processAll(const PathConfig& cfg)
{ std::vector<std::string> prefixes;
    const std::string tag = "mate-m101.txt";

    for (const auto& e : fs::directory_iterator(cfg.dataDir)) {
        const std::string p = e.path().string();
        if (p.size() >= tag.size() &&
            p.compare(p.size() - tag.size(), tag.size(), tag) == 0)
            prefixes.emplace_back(p.substr(0, p.size() - tag.size()));
    }
    std::sort(prefixes.begin(), prefixes.end());

    for (const auto& pre : prefixes) {
        std::cout << ">> Procesando fecha: " << pre << " ...\n";
        if (!processDate(cfg, pre))
            std::cerr << "   ! Se omitió por error.\n";
    }}

bool DataProcessor::processDate(const PathConfig& /*cfg*/,
                                const std::string& prefix)
{
    //RUTAS 
    const std::string f101 = prefix + "mate-m101.txt";
    const std::string f102 = prefix + "mate-m102.txt";
    const std::string f103 = prefix + "mate-m103.txt";

    std::ifstream in101(f101), in102(f102), in103(f103);
    if (!in101.is_open() || !in102.is_open() || !in103.is_open()) {
        std::cerr << "Error abriendo alguno de los archivos: "
                  << f101 << ", " << f102 << ", " << f103 << '\n';
        return false;
    }

    //OUTPUTS
    const std::string txtOut  = prefix + "combined_output.txt";
    const std::string rootOut = prefix + "output.root";

    //VARIABLES TREE
    Long64_t ts, ts2_m101, ts2_m102, ts2_m103;
    int      evt;
    int nA1, nB1, nA2, nB2, nA3, nB3;
    std::vector<int> A1, B1, A2, B2, A3, B3;

    TFile rootFile(rootOut.c_str(), "RECREATE");
    TTree tree("tree", "Tree con datos procesados");

    tree.Branch("ts",        &ts,        "ts/L");
    tree.Branch("ts2_m101",  &ts2_m101,  "ts2_m101/L");
    tree.Branch("ts2_m102",  &ts2_m102,  "ts2_m102/L");
    tree.Branch("ts2_m103",  &ts2_m103,  "ts2_m103/L");
    tree.Branch("evt",       &evt,       "evt/I");

    tree.Branch("A1", &A1); tree.Branch("B1", &B1);
    tree.Branch("A2", &A2); tree.Branch("B2", &B2);
    tree.Branch("A3", &A3); tree.Branch("B3", &B3);

    tree.Branch("nA1", &nA1, "nA1/I"); tree.Branch("nB1", &nB1, "nB1/I");
    tree.Branch("nA2", &nA2, "nA2/I"); tree.Branch("nB2", &nB2, "nB2/I");
    tree.Branch("nA3", &nA3, "nA3/I"); tree.Branch("nB3", &nB3, "nB3/I");

    
    std::unordered_map<std::string, std::string> binCache;

    auto hexToBinLocal = [&](const std::string& h, int ln) {
        return DataProcessor::hexToBin(h, ln, binCache);
    };
    auto binToPos = DataProcessor::binToPositions;
    auto vecStr   = DataProcessor::positionsToString;


    std::ofstream out(txtOut);

    std::string l101, l102, l103, tmp;

    while ( std::getline(in101,l101) &&
            std::getline(in102,l102) &&
            std::getline(in103,l103) ) {

        //PARSEAR
        std::stringstream s1(l101), s2(l102), s3(l103);
        std::string ts1, BB1, BA1, AA1, ts21, evt1;
        std::string      BB2, BA2, AA2, ts22;
        std::string      BB3, BA3, AA3, ts23;

        std::getline(s1, ts1, ',');  std::getline(s1, BB1, ',');
        std::getline(s1, BA1, ',');  std::getline(s1, AA1, ',');
        std::getline(s1, ts21, ','); std::getline(s1, evt1, ',');

        std::getline(s2, tmp, ',');  std::getline(s2, BB2, ',');
        std::getline(s2, BA2, ',');  std::getline(s2, AA2, ',');
        std::getline(s2, ts22, ',');

        std::getline(s3, tmp, ',');  std::getline(s3, BB3, ',');
        std::getline(s3, BA3, ',');  std::getline(s3, AA3, ',');
        std::getline(s3, ts23, ',');

        int ln = __LINE__;

        //hexToBin
        auto bBB1 = hexToBinLocal(BB1,ln);
        auto bBA1 = hexToBinLocal(BA1,ln);
        auto bAA1 = hexToBinLocal(AA1,ln);

        auto bBB2 = hexToBinLocal(BB2,ln);
        auto bBA2 = hexToBinLocal(BA2,ln);
        auto bAA2 = hexToBinLocal(AA2,ln);

        auto bBB3 = hexToBinLocal(BB3,ln);
        auto bBA3 = hexToBinLocal(BA3,ln);
        auto bAA3 = hexToBinLocal(AA3,ln);

        //CONCATENAR
        std::string B1b = bBB1 + bBA1.substr(0,4);
        std::string A1b = bBA1.substr(4,4) + bAA1;

        std::string B2b = bBB2 + bBA2.substr(0,4);
        std::string A2b = bBA2.substr(4,4) + bAA2;

        std::string B3b = bBB3 + bBA3.substr(0,4);
        std::string A3b = bBA3.substr(4,4) + bAA3;

        //VECTORES
        auto vB1 = binToPos(B1b);
        auto vA1 = binToPos(A1b);
        auto vB2 = binToPos(B2b);
        auto vA2 = binToPos(A2b);
        auto vB3 = binToPos(B3b);
        auto vA3 = binToPos(A3b);

        //STRINGS
        std::string sB1 = vecStr(vB1);
        std::string sA1 = vecStr(vA1);
        std::string sB2 = vecStr(vB2);
        std::string sA2 = vecStr(vA2);
        std::string sB3 = vecStr(vB3);
        std::string sA3 = vecStr(vA3);

        //CONTEO BITS
        nB1 = vB1.size();  nA1 = vA1.size();
        nB2 = vB2.size();  nA2 = vA2.size();
        nB3 = vB3.size();  nA3 = vA3.size();

        
        out << ts1 << ','
            << sB1 << ',' << sA1 << ','
            << sB2 << ',' << sA2 << ','
            << sB3 << ',' << sA3 << ','
            << evt1 << ',' << ts21 << ',' << ts22 << ',' << ts23 << ','
            << nB1 << ',' << nA1 << ',' << nB2 << ',' << nA2 << ','
            << nB3 << ',' << nA3 << '\n';

        //TREE FILL
        ts        = std::stoll(ts1);
        ts2_m101  = std::stoll(ts21);
        ts2_m102  = std::stoll(ts22);
        ts2_m103  = std::stoll(ts23);
        evt       = std::stoi(evt1);

        A1 = vA1;  B1 = vB1;
        A2 = vA2;  B2 = vB2;
        A3 = vA3;  B3 = vB3;

        tree.Fill();
    }

    in101.close(); in102.close(); in103.close();
    out.close();

    rootFile.Write();
    rootFile.Close();
    std::cout << "   ✔ Archivos creados: "
              << txtOut << " , " << rootOut << '\n';
    return true;
}


std::string DataProcessor::hexToBin(const std::string& hex,
                                    int ln,
                                    std::unordered_map<std::string,std::string>& cache)
{auto it = cache.find(hex);
    if (it != cache.end()) return it->second;

    try {
        std::bitset<8> bits(std::stoi(hex,nullptr,16));
        return cache[hex] = bits.to_string();
    } catch (...) {
        std::cerr << "Warn: hex '" << hex << "' inválido (ln " << ln << ")\n";
        return cache[hex] = std::string(8,'0');
    }}

std::vector<int> DataProcessor::binToPositions(const std::string& bin)
{
    std::vector<int> pos;
    for (int i=0;i<(int)bin.size();++i)
        if (bin[bin.size()-1-i]=='1') pos.push_back(i+1);
    return pos;
}

std::string DataProcessor::positionsToString(const std::vector<int>& v)
{
    if (v.empty()) return "(0)";
    std::string s = '(' + std::to_string(v[0]);
    for (size_t i=1;i<v.size();++i) s += ',' + std::to_string(v[i]);
    s += ')';
    return s;
}
