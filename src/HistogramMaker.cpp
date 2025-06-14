#include "HistogramMaker.hpp"

#include <TTree.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <limits>

static void clearCin()
{std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');}


static TH1D* makeH1(TTree* tree, const std::string& var, const std::string& hname)
{auto* h = new TH1D(hname.c_str(), (hname+";Barra;Frecuencia (hits)").c_str(), 12, 1, 13); //12 BINS
    tree->Draw((var+">>"+hname).c_str(), "", "goff");
    return h;}
struct PlaneInfo { std::string A, B, title; };
static TH2D* makeH2(TTree* tree,
                    const PlaneInfo& p,
                    const std::string& hname,
                    const std::string& cut)
{auto* h = new TH2D(hname.c_str(), (p.title+";A;B").c_str(), 12, 1, 13, 12, 1, 13);
    tree->Draw((p.B+"[0]:"+p.A+"[0]>>"+hname).c_str(), cut.c_str(), "goff");
    return h;}


void runHistograms(TTree* tree)
{
    std::cout << "\n== Histogramas ==\n"
                 "  1) 1D  (A1,B1,A2,B2,A3,B3)  – varias coma-separadas\n"
                 "  2) 2D  (B1vsA1, B2vsA2, B3vsA3)  corte obligatorio\n"
                 "  0) Volver\n"
                 "Opción [0/1/2]: ";
    char mode; std::cin >> mode; clearCin();
    if (mode=='0') return;


    if (mode=='1')
    {
        std::cout << "Variables (ej. A1,B2,A3): ";
        std::string line; std::getline(std::cin,line);

        const std::vector<std::string> allowed = {"A1","B1","A2","B2","A3","B3"};
        std::vector<std::string> vars;
        std::stringstream ss(line); std::string tok;
        while (std::getline(ss,tok,',')) {
            tok.erase(remove_if(tok.begin(),tok.end(),::isspace),tok.end());
            if (std::find(allowed.begin(),allowed.end(),tok)!=allowed.end())
                vars.push_back(tok);
        }
        if (vars.empty()) { std::cout<<"Sin variables válidas.\n"; return; }
        std::string pdfName="hist1d.pdf";
        std::cout<<"Nombre PDF (ENTER hist1d.pdf): ";
        std::string tmp; std::getline(std::cin,tmp);
        if(!tmp.empty()) pdfName = tmp;
        int n = vars.size();
        TCanvas* c = new TCanvas("c1d","Histogramas 1D",800,300*n);
        c->Divide(1,n,0.01,0.01);

        for (int i=0;i<n;++i)
        {
            std::string hname = "h1_"+vars[i];
            TH1D* h = makeH1(tree, vars[i], hname);
            c->cd(i+1);
            gPad->SetGridx(true); gPad->SetGridy(true);
            h->SetStats(false);
            // EJE Y DESDE 0
            double ymax = h->GetMaximum();
            h->GetYaxis()->SetRangeUser(0.0, ymax*1.1)
            //Eje X ticks cada 1
            h->GetXaxis()->SetNdivisions(12, kFALSE);
            h->GetXaxis()->CenterLabels();
            h->SetFillColor(kAzure+1);
            h->SetLineColor(kAzure+2);
            h->SetBarWidth(0.8);     
            h->SetBarOffset(0.1);    

            h->Draw("bar");}

        c->Print(pdfName.c_str(), "pdf");
        std::cout<<"PDF guardado: "<<pdfName<<"\n";
        return;
    }

    //HISTOGRAMAS 2D
    if (mode=='2')
    {
        const std::string mandatoryCut =
            "nA1==1 && nB1==1 && nA2==1 && nB2==1 && nA3==1 && nB3==1";

        PlaneInfo planes[3] = {
            {"A1","B1","B1 vs A1 (m101)"},
            {"A2","B2","B2 vs A2 (m102)"},
            {"A3","B3","B3 vs A3 (m103)"}
        };

        std::string pdfName="hist2d_planes.pdf";
        std::cout<<"Nombre PDF (ENTER hist2d_planes.pdf): ";
        std::string tmp; std::getline(std::cin,tmp);
        if(!tmp.empty()) pdfName = tmp;

        TCanvas* c = new TCanvas("cPlanes","Planes 2D",1200,400);
        c->Divide(3,1);

        for (int i=0;i<3;++i) {
            std::string hname="h2_"+planes[i].A;
            TH2D* h = makeH2(tree, planes[i], hname, mandatoryCut);
            c->cd(i+1);
            h->Draw("COLZ");
        }

        c->Print(pdfName.c_str(), "pdf");
        std::cout<<"PDF guardado: "<<pdfName<<"\n";
        return;
    }

    std::cout<<"Opción inválida.\n";
}
