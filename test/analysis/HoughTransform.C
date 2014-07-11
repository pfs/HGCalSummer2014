#include "TH2.h"
#include "TMath.h"

/**
   @short apply simple hough transformation
 */
std::pair<TH2F *,TH2F *> HoughTransform(TH2F *h)
{
  Int_t nx( h->GetXaxis()->GetNbins() ); 
  Int_t ny( h->GetYaxis()->GetNbins() );
  ny = int(ny/2)*2;
 
  Float_t rmax( TMath::Sqrt(nx*nx+ny*ny) );
  Float_t dtheta( TMath::Pi() / nx );
  
  TH2F *houghH=new TH2F(TString("hough_")+h->GetName(),"Counts;#theta [rad];#rho",nx,0,TMath::Pi(),ny,-rmax,rmax);
  houghH->Sumw2();
  houghH->SetDirectory(0);
  TH2F *wgtHoughH=(TH2F *) houghH->Clone(TString("weighted")+houghH->GetName());
  wgtHoughH->SetDirectory(0);

  for(Int_t xbin=1; xbin<=nx; xbin++)
    {
      for(Int_t ybin=1; ybin<=ny; ybin++)
	{      
	  Float_t val=h->GetBinContent(xbin+1,ybin+1);
	  if(val==0) continue;

	  //check all the lines which can pass at this point
	  for(Int_t jtx=0; jtx<nx; jtx++)
	    {
	      Float_t theta = dtheta * jtx;
	      Float_t r = xbin*TMath::Cos(theta) + ybin*TMath::Sin(theta);
	      houghH->Fill(theta,r);
	      wgtHoughH->Fill(theta,r,val);
	    }
	}
    }
  
  return std::pair<TH2F *,TH2F *>(houghH,wgtHoughH);
}
