#include "DataFormats/HcalDigi/interface/QIE11DataFrame.h"
#include "DataFormats/HcalDetId/interface/HcalGenericDetId.h"

int QIE11DataFrame::Sample::tdc() { return (frame_[i_]>>OFFSET_TDC)&(detFlavor == 0 ? MASK_TDC_HE : MASK_TDC_HB); }
int QIE11DataFrame::Sample::capid() { return (detFlavor == 0 ? ((((frame_[0]>>OFFSET_CAPID)&MASK_CAPID_HE)+i_-HEADER_WORDS)&MASK_CAPID_HE) : (frame_[i_]>>OFFSET_CAPID)&MASK_CAPID_HB ); }

void QIE11DataFrame::setCapid0(int cap0) {
  m_data[0]&=0xFCFF; // inversion of the capid0 mask
  m_data[0]|=((cap0&Sample::MASK_CAPID_HE)<<Sample::OFFSET_CAPID);  
}

void QIE11DataFrame::setFlags(uint16_t v) {
  m_data[size()-1]=v;
}

void QIE11DataFrame::copyContent(const QIE11DataFrame& digi) {
  for (edm::DataFrame::size_type i=0; i<size() && i<digi.size();i++){
    Sample sam = digi[i];
	setSample(i,sam.adc(),sam.tdc(),sam.soi());
  }
}

int QIE11DataFrame::presamples() const {
  for (int i=0; i<samples(); i++) {
    if ((*this)[i].soi()) return i;
  }
  return -1;
}
bool QIE11DataFrame::capidError() const {
  if (flavor() == FLAVOR_6CHANNEL) return m_data[0]&MASK_CAPIDERROR;
  //CALCULATE FOR NGHB
  int capid_guess = begin()->capid();
  for(edm::DataFrame::const_iterator sample = begin(); sample != end(); sample++) {
    if(capid_guess != sample->capid()) return true; //GUESS DOES NOT MATCH CAPID, THIS IS A ROTATION ERROR
    if(capid_guess == 3) capid_guess = -1;          //LOOP BACK 0,1,2,3->0,1,2,3
    capid_guess++;
  }                                 
  return false;
}

void QIE11DataFrame::setZSInfo(bool markAndPass){
	if(markAndPass) m_data[0] |= (markAndPass&MASK_FLAVOR)<<OFFSET_FLAVOR;
}

void QIE11DataFrame::setSample(edm::DataFrame::size_type isample, int adc, int tdc, bool soi) {
  if (isample>=size()) return;
  if (detFlavor == 0) m_data[isample+1]=(adc&Sample::MASK_ADC)|(soi?(Sample::MASK_SOI):(0))|((tdc&Sample::MASK_TDC_HE)<<Sample::OFFSET_TDC);
  if (detFlavor == 1) m_data[isample+1]=(adc&Sample::MASK_ADC)|(soi?(Sample::MASK_SOI):(0))|((tdc&Sample::MASK_TDC_HB)<<Sample::OFFSET_TDC);
}

std::ostream& operator<<(std::ostream& s, QIE11DataFrame& digi) {
  if (digi.detid().det()==DetId::Hcal) {
    s << HcalGenericDetId(digi.detid());
  } else {
    s << "DetId(" << digi.detid().rawId() << ")";    
  }
  s << " " << digi.samples() << " samples";
  if (digi.linkError()) s << " LinkError ";
  if (digi.capidError()) s << " CapIdError ";
  if (digi.zsMarkAndPass()) s << " M&P ";
  s << std::endl;
  for (int i=0; i<digi.samples(); i++) {
    QIE11DataFrame::Sample sam = digi[i];
    s << "  ADC=" << sam.adc() << " TDC=" << sam.tdc() << " CAPID=" << sam.capid();
    if (sam.soi()) s << " SOI ";
    s << std::endl;
  }
  return s;
}
