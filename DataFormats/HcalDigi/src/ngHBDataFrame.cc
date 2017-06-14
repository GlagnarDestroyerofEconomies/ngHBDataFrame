#include "DataFormats/HcalDigi/interface/ngHBDataFrame.h"
#include "DataFormats/HcalDetId/interface/HcalGenericDetId.h"

//void ngHBDataFrame::setCapid0(int cap0) {
//  m_data[0]&=0xFCFF; // inversion of the capid0 mask
//  m_data[0]|=((cap0&Sample::MASK_CAPID)<<Sample::OFFSET_CAPID);  
//}

void ngHBDataFrame::setFlags(uint16_t v) {
  m_data[size()-1]=v;
}

void ngHBDataFrame::copyContent(const ngHBDataFrame& digi) {
  for (edm::DataFrame::size_type i=0; i<size() && i<digi.size();i++){
    Sample sam = digi[i];
	setSample(i,sam.adc(),sam.tdc(),sam.soi());
  }
}

int ngHBDataFrame::presamples() const {
  for (int i=0; i<samples(); i++) {
    if ((*this)[i].soi()) return i;
  }
  return -1;
}

void ngHBDataFrame::setZSInfo(bool markAndPass){
	if(markAndPass) m_data[0] |= (markAndPass&MASK_FLAVOR)<<OFFSET_FLAVOR;
}

void ngHBDataFrame::setSample(edm::DataFrame::size_type isample, int adc, int tdc, int capid, bool soi, bool le) {
  if (isample>=size()) return;
  m_data[isample+1]=(adc&Sample::MASK_ADC)|(soi?(Sample::MASK_SOI):(0))|((tdc&Sample::MASK_TDC)<<Sample::OFFSET_TDC)|((capid&Sample::MASK_CAPID)<<Sample::OFFSET_CAPID)|(le?(Sample::MASK_LE):(0));
}

std::ostream& operator<<(std::ostream& s, const ngHBDataFrame& digi) {
  if (digi.detid().det()==DetId::Hcal) {
    s << HcalGenericDetId(digi.detid());
  } else {
    s << "DetId(" << digi.detid().rawId() << ")";    
  }
  s << " " << digi.samples() << " samples";
  if (digi.linkError()) s << " LinkError ";
//  if (digi.capidError()) s << " CapIdError ";
  if (digi.zsMarkAndPass()) s << " M&P ";
  s << std::endl;
  for (int i=0; i<digi.samples(); i++) {
    ngHBDataFrame::Sample sam = digi[i];
    s << "  ADC=" << sam.adc() << " TDC=" << sam.tdc() << " CAPID=" << sam.capid();
    if (sam.soi()) s << " SOI ";
    if (sam.le()) s << " SAMPLE_LINK_ERROR ";
    s << std::endl;
  }
  return s;
}
