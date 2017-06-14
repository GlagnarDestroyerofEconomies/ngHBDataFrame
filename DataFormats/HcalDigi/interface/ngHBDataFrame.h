#ifndef DATAFORMATS_HCALDIGI_NGHBDATAFRAME_H
#define DATAFORMATS_HCALDIGI_NGHBDATAFRAME_H

#include "DataFormats/HcalDetId/interface/HcalDetId.h"
#include "DataFormats/Common/interface/DataFrame.h"
#include <ostream>

/** Precision readout digi from ngHB including TDC information

 */
class ngHBDataFrame {
public:

  static const int WORDS_PER_SAMPLE = 1;
  static const int HEADER_WORDS = 1;
  static const int FLAG_WORDS = 1;

  ngHBDataFrame() { }
  ngHBDataFrame(edm::DataFrame const & df) : m_data(df) { }

  class Sample {
  public:
    Sample(const edm::DataFrame& frame, edm::DataFrame::size_type i) : frame_(frame),i_(i) { }
    static const int MASK_ADC = 0xFF;
    static const int MASK_TDC = 0x3; //different for ngHB
    static const int OFFSET_TDC = 8; // 8 bits
    static const int MASK_SOI = 0x4000;
    static const int MASK_CAPID = 0xC;//different for ngHB
    static const int OFFSET_CAPID = 8;
    static const int MASK_LE = 0x2000;//feature of ngHB data
    int adc() const { return frame_[i_]&MASK_ADC; }
    int tdc() const { return (frame_[i_]>>OFFSET_TDC)&MASK_TDC; }
    bool soi() const { return frame_[i_]&MASK_SOI; }
    int capid() const { return (frame_[i_]>>OFFSET_CAPID)&MASK_CAPID; } //different as each sample has its own 2 bits of CapID
    bool le() const { return frame_[i_]&MASK_LE; } //link error per sample
  private:
    const edm::DataFrame& frame_;
    edm::DataFrame::size_type i_;
  };

  void copyContent(const ngHBDataFrame&);

  /// Get the detector id
  DetId detid() const { return DetId(m_data.id()); }
  edm::DataFrame::id_type id() const { return m_data.id(); }
  /// more accessors
  edm::DataFrame::size_type size() const { return m_data.size(); }
  /// iterators
  edm::DataFrame::iterator begin() { return m_data.begin(); }
  edm::DataFrame::iterator end() { return m_data.end(); }
  edm::DataFrame::const_iterator begin() const { return m_data.begin(); }
  edm::DataFrame::const_iterator end() const { return m_data.end(); }
  /// total number of samples in the digi
  int samples() const { return (size()-HEADER_WORDS-FLAG_WORDS)/WORDS_PER_SAMPLE; }
  /// for backward compatibility
  int presamples() const;
  /// get the flavor of the frame
  static const int OFFSET_FLAVOR = 12;
  static const int MASK_FLAVOR = 0x7;
  int flavor() const { return ((m_data[0]>>OFFSET_FLAVOR)&MASK_FLAVOR); }
  /// was there a link error?
  static const int MASK_LINKERROR = 0x800;
  bool linkError() const { return m_data[0]&MASK_LINKERROR; } 
  /// was there a capid rotation error?  NO BIT SET FOR THIS IN NGHB CAPID SPECIFIED FOR EACH SAMPLE.
  //static const int MASK_CAPIDERROR = 0x400;
  //bool capidError() const { return m_data[0]&MASK_CAPIDERROR; } 
  /// was this a mark-and-pass ZS event?
  static const int MASK_MARKPASS = 0x100;
  bool zsMarkAndPass() const {return m_data[0]&MASK_MARKPASS; }
  /// set ZS params
  void setZSInfo(bool markAndPass);
  /// get the sample
  inline Sample operator[](edm::DataFrame::size_type i) const { return Sample(m_data,i+HEADER_WORDS); }
  //void setCapid0(int cap0);
  /// set the sample contents
  void setSample(edm::DataFrame::size_type isample, int adc, int tdc, int capid, bool soi=false, bool le=false);
  /// get the flag word
  uint16_t flags() const { return m_data[size()-1]; }
  /// set the flag word
  void setFlags(uint16_t v);

  private:
   edm::DataFrame m_data;

};

std::ostream& operator<<(std::ostream&, const ngHBDataFrame&);


#endif // DATAFORMATS_HCALDIGI_NGHBDATAFRAME_H
