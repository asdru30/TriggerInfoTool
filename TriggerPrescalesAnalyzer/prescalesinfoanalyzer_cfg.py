import FWCore.ParameterSet.Config as cms
import FWCore.PythonUtilities.LumiList as LumiList
import FWCore.ParameterSet.Types as CfgTypes

process = cms.Process("TriggerInfo")

process.load("FWCore.MessageService.MessageLogger_cfi")

#process.options = cms.untracked.PSet(
    #wantSummary = cms.untracked.bool(True)
#)

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring( *(
#'root://eospublic.cern.ch//eos/opendata/cms/Run2011A/Jet/AOD/12Oct2013-v1/10000/06E59C16-0A40-E311-B387-02163E008D9F.root', #Not good quality
'root://eospublic.cern.ch//eos/opendata/cms/Run2011A/Jet/AOD/12Oct2013-v1/20000/000D4260-D23E-E311-A850-02163E008D77.root',
'root://eospublic.cern.ch//eos/opendata/cms/Run2011A/Jet/AOD/12Oct2013-v1/20000/00E41BFD-D93E-E311-A91D-0025901D5DEC.root',
'root://eospublic.cern.ch//eos/opendata/cms/Run2011A/Jet/AOD/12Oct2013-v1/20000/022B6AEB-173F-E311-B17D-0025904A90CA.root',
'root://eospublic.cern.ch//eos/opendata/cms/Run2011A/Jet/AOD/12Oct2013-v1/20000/02484AF2-E63E-E311-A3F6-003048D2BEE6.root',
'root://eospublic.cern.ch//eos/opendata/cms/Run2011A/Jet/AOD/12Oct2013-v1/20000/024E0F96-B93E-E311-8A3C-02163E008DA1.root',
'root://eospublic.cern.ch//eos/opendata/cms/Run2011A/Jet/AOD/12Oct2013-v1/20000/027308C4-C73E-E311-8A0C-003048F23FA8.root',
'root://eospublic.cern.ch//eos/opendata/cms/Run2011A/Jet/AOD/12Oct2013-v1/20000/02751E29-D73E-E311-B249-02163E00CD78.root',
'root://eospublic.cern.ch//eos/opendata/cms/Run2011A/Jet/AOD/12Oct2013-v1/20000/0278353F-F23E-E311-AEED-02163E00C482.root',
'root://eospublic.cern.ch//eos/opendata/cms/Run2011A/Jet/AOD/12Oct2013-v1/20000/02C290BF-E03E-E311-913B-C860001BD86A.root',
'root://eospublic.cern.ch//eos/opendata/cms/Run2011A/Jet/AOD/12Oct2013-v1/20000/02D19806-DD3E-E311-BC8A-002481E0D1F2.root',
'root://eospublic.cern.ch//eos/opendata/cms/Run2011A/Jet/AOD/12Oct2013-v1/20000/02E16C94-0B3F-E311-9F2F-02163E00C4FA.root',

#    'file:00082EAF-C03D-E311-8E53-003048F00B1C.root'
    ) )
)

#goodJSON = 'Cert_160404-180252_7TeV_ReRecoNov08_Collisions11_JSON.txt'
goodJSON = 'JsonRun163337.json'
myLumis = LumiList.LumiList(filename = goodJSON).getCMSSWString().split(',')


process.source.lumisToProcess = cms.untracked.VLuminosityBlockRange()
process.source.lumisToProcess.extend(myLumis)

#needed to get the actual prescale values used from the global tag
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.GlobalTag.connect = cms.string('sqlite_file:/cvmfs/cms-opendata-conddb.cern.ch/FT_53_LV5_AN1_RUNA.db')
process.GlobalTag.globaltag = 'FT_53_LV5_AN1::All'


#configure the analyzer
process.gettriggerinfo = cms.EDAnalyzer('TriggerPrescalesAnalyzer'
    ,filterName = cms.vstring("hltSingleJet60Regional","hltSingleJet70Regional","hltSingleJet80Regional","hltSingleJet100Regional","hltSingleJet110Regional")
)

#configure the TFileservice, in order to save histograms.
process.TFileService = cms.Service("TFileService",
              fileName = cms.string('histo.root')
                                   )

process.triggerinfo = cms.Path(process.gettriggerinfo)
process.schedule = cms.Schedule(process.triggerinfo)
