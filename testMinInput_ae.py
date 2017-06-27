import FWCore.ParameterSet.Config as cms

process = cms.Process("TEST")
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(10000))

#process.source = cms.Source("HcalTBSource",
#                            fileNames = cms.untracked.vstring('/store/group/comm_hcal/LS1/USC_228407.root'),
#) #222756
process.source = cms.Source("HcalTBSource",
                  #          fileNames = cms.untracked.vstring('file:/home/hep/jmmans/BHM_102_Jun5_2015.root'),
                            fileNames = cms.untracked.vstring('file:FNAL_000000.root'),

)

process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1
process.load("Configuration.Geometry.GeometryIdeal_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = '90X_upgrade2017_realistic_v20'
#process.Tracer=cms.Service("Tracer")
process.es_ascii = cms.ESSource('HcalTextCalibrations',
    input = cms.VPSet(
        cms.PSet(
            object = cms.string('ElectronicsMap'),
            file = cms.FileInPath('dummyEmap.txt')
            ),
        )
    )
process.es_prefer = cms.ESPrefer('HcalTextCalibrations','es_ascii')
#process.esContent = cms.EDAnalyzer("PrintEventSetupContent")
#process.histoUnpack = cms.EDProducer("HcalUTCAhistogramUnpacker", 
#          fedNumber = cms.int32(63),
#          fedRawDataCollectionTag = cms.InputTag("source")
#          )
process.hcalDigis = cms.EDProducer("HcalRawToDigi",
   ComplainEmptyData = cms.untracked.bool(True),
   ElectronicsMap = cms.string(''),
   ExpectedOrbitMessageTime = cms.untracked.int32(-1),
   FEDs = cms.untracked.vint32(1776),
   FilterDataQuality = cms.bool(True),
   HcalFirstFED = cms.untracked.int32(700),
   InputLabel = cms.InputTag("source"),
   UnpackCalib = cms.untracked.bool(True),
   UnpackTTP = cms.untracked.bool(True),
   UnpackZDC = cms.untracked.bool(True),
   UnpackerMode = cms.untracked.int32(0),
   firstSample = cms.int32(0),
   lastSample = cms.int32(9),
   silent = cms.untracked.bool(False)
)


#process.histoView = cms.EDAnalyzer("histoPrint", tag = cms.InputTag("histoUnpack"))
#process.dump  = cms.EDAnalyzer("HcalDigiDump")
process.out = cms.OutputModule("PoolOutputModule",
      fileName = cms.untracked.string("FNAL_000000_unpacked.root")
         )
process.end = cms.EndPath(process.out)
process.p=cms.Path( process.hcalDigis
 #           process.esContent+
		 #   process.histoView+
 #                   process.dump
		    )
#process.p1=cms.Path(process.histoUnpack)
process.schedule = cms.Schedule(#process.p1,
                                process.p,process.end)
