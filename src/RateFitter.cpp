#include "RateFitter.hpp"

#include <TTree.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TH1D.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TPaveText.h>    
#include <TROOT.h>
#include <iostream>
#include <sstream>
#include <ctime>
#include <vector>


static TF1* fitExpo(TH1D& h)
{TF1* f = new TF1("expoFit", "expo", 0, h.GetXaxis()->GetXmax()); f->SetLineColor(kRed); h.Fit(f, "Q0"); return f;}


static Long64_t bestCandidate(const std::vector<Long64_t>& cands, Long64_t prev)
{Long64_t best = -1;
    for (auto c : cands) {
        if (c <= 0) continue;
        if (prev < 0 || c >= prev) {
            if (best < 0 || c < best) best = c;
        }}
    return best;}


FitResult runRateFit(TTree* tree, const std::string& /*unused*/, double histRangeS,int nBins)
{
  
    TTreeReader reader(tree);
    TTreeReaderValue<Int_t>  nA1(reader,"nA1");
    TTreeReaderValue<Int_t>  nB1(reader,"nB1");
    TTreeReaderValue<Int_t>  nA2(reader,"nA2");
    TTreeReaderValue<Int_t>  nB2(reader,"nB2");
    TTreeReaderValue<Int_t>  nA3(reader,"nA3");
    TTreeReaderValue<Int_t>  nB3(reader,"nB3");
    TTreeReaderValue<Long64_t> p1(reader,"ts2_m101");
    TTreeReaderValue<Long64_t> p2(reader,"ts2_m102");
    TTreeReaderValue<Long64_t> p3(reader,"ts2_m103");

    //HISTO DELTA T
    TH1D h("h_dt","Distribucion #Delta t;#Delta t (s);Conteos", nBins, 0, histRangeS);

    const double NS2S = 1e-7;   // 100 ns 
    Long64_t prev_ts = -1;

    
    while (reader.Next()){
        if (*nA1!=1||*nB1!=1||*nA2!=1||*nB2!=1||*nA3!=1||*nB3!=1)
            continue;
        Long64_t ts = -1;
        if (*p1 > 0 && (prev_ts < 0 || *p1 >= prev_ts)) {
            ts = *p1;} else {
            std::vector<Long64_t> cands = {*p2, *p3};
            ts = bestCandidate(cands, prev_ts);}
        if (ts < 0) continue;

        if (prev_ts > 0) {
            double dt = (ts - prev_ts) * NS2S; if (dt > 0 && dt < histRangeS) h.Fill(dt);}
        prev_ts = ts;}

    if (h.GetEntries() < 10) {
        std::cerr << "Insuficientes eventos ("<<h.GetEntries()
                  <<") para ajustar el rate.\n";
        return {0,0,0,0,0,0};
    }

    //AJUSTE EXPONENCIAL
    TF1* fexp = fitExpo(h);
    double A   = fexp->GetParameter(0);
    double B= fexp->GetParameter(1);  
    double errA = fexp->GetParError(0);
    double errB= fexp->GetParError(1);
    double lambda =-B;
    double err_lambda=  errB;
    double period =1.0 / lambda;
    double err_period= err_lambda/(lambda*lambda);
    TCanvas c("cRate","Rate fit",800,600);
    h.SetFillColor(kAzure+1);
    h.Draw("hist");
    h.GetYaxis()->SetRangeUser(0, h.GetMaximum()*1.1);
    fexp->Draw("same");
    h.SetTitle("Distribucion #Delta t");
    h.GetXaxis()->SetTitle("#Delta t (s)");
    TPaveText box(0.60, 0.60, 0.88, 0.83, "NDC");
    box.SetFillColor(0);
    box.SetBorderSize(1);
    box.SetTextAlign(12);
    box.AddText(Form("A        = %.2f #pm %.2f", A, errA));
    box.AddText(Form("#lambda  = %.4f #pm %.4f Hz", lambda, err_lambda));
    box.AddText(Form("#mu = 1/#lambda = %.4f #pm %.4f s", period, err_period));
    box.Draw();

    std::string pdfName = "rate_fit.pdf";
    c.Print(pdfName.c_str(), "pdf");
    std::cout << "Grafico guardado en " << pdfName << "\n";

 
    std::cout << "\nParámetros del ajuste exponencial\n"
              << "----------------------------------\n"
              << "A               = " << A << " ± " << errA << "\n"
              << "λ (rate)   [Hz] = " << lambda << " ± " << err_lambda << "\n"
              << "μ (periodo) [s] = " << period << " ± " << err_period << "\n";

    return {A, errA, lambda, err_lambda, period, err_period};
}
