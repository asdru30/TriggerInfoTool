// -*- C++ -*-
//
// Package:    TriggerPrescalesAnalyzer
// Class:      TriggerPrescalesAnalyzer
//
/**\class TriggerPrescalesAnalyzer TriggerPrescalesAnalyzer.cc TriggerInfoTool/TriggerPrescalesAnalyzer/src/TriggerPrescalesAnalyzer.cc
 Description: [one line class summary]
 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Edgar Carrera
//         Created:  Mon Jul  9 15:59:18 CEST 2019
// $Id$
//
//

//**


//classes to extract trigger information
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"

//classes to save data
#include "TH1.h"

//class to save the histograms
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include <vector>
#include <string>
#include <iostream>
#include <iomanip>

#include <iosfwd>

//**

// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

//Following the HLTEventAnalyzerAOD.h,
//include the following headers:
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"

//Also include headers from  HLTEventAnalyzerAOD.cc
#include "FWCore/Common/interface/TriggerNames.h"
#include "FWCore/Common/interface/TriggerResultsByName.h"
#include <cassert>

using namespace std;
using namespace edm;
using namespace trigger;
using namespace reco;

//
// class declaration
//

class TriggerPrescalesAnalyzer : public edm::EDAnalyzer {
	public:
	explicit TriggerPrescalesAnalyzer(const edm::ParameterSet&);
	~TriggerPrescalesAnalyzer();

	virtual void beginJob();
	virtual void beginRun(edm::Run const&, edm::EventSetup const&);
	//virtual void beginRun(edm::Run const&, edm::EventSetup const&, const edm::TriggerNames& triggerNames, const edm::TriggerResults& result);
	virtual void analyze(const edm::Event&, const edm::EventSetup&);
	virtual void endRun(edm::Run const&, edm::EventSetup const&);
	virtual void endJob();

	//the follwing are not being used here
	virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);
	virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);
	static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
	//Until here

	private:

	//Methods used for the analysis
	bool checkTriggerPass(const edm::Event& iEvent, const std::string& triggerName);//Check if the trigger passed
	//declare histograms and variables that will go into the root files
	TH1D *trighist_pt;

	edm::Service<TFileService> fs;//to access the TFileService object in a framework module

	std::string   filterName_;//declare de filter (module) of the trigger

	HLTConfigProvider hltConfig_;

};

//This should match your configuration python file
TriggerPrescalesAnalyzer::TriggerPrescalesAnalyzer(const edm::ParameterSet& iConfig):
filterName_(iConfig.getParameter<std::string>("filterName"))
{
	using namespace std;
	using namespace edm;
}

TriggerPrescalesAnalyzer::~TriggerPrescalesAnalyzer()
{
	// do anything here that needs to be done at desctruction time
	// (e.g. close files, deallocate resources etc.)
}

// -----method called once each job just before starting event loop-----
void TriggerPrescalesAnalyzer::beginJob()//Everything ROOT related
{
	trighist_pt = fs->make <TH1D>("trighist_pt", "obj pt ", 100,0,500);

}//-----------------------------------------------------------beginJob()

// ------------ method called when starting to processes a run  --------
//void TriggerPrescalesAnalyzer::beginRun(edm::Run const& iRun, edm::EventSetup const& iSetup, const edm::TriggerNames& triggerNames, const edm::TriggerResults& result)
void TriggerPrescalesAnalyzer::beginRun(edm::Run const& iRun, edm::EventSetup const& iSetup)
{
	///   "init" return value indicates whether intitialisation has succeeded
	///   "changed" parameter indicates whether the config has actually changed
		bool changed(true);
		hltConfig_.init(iRun,iSetup,"HLT",changed);

}//-----------------------------------------------------------beginRun()

// ------------ method called for each event  --------------------------
void TriggerPrescalesAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
	using namespace edm;
	using namespace std;

	InputTag triggerEventTag_("hltTriggerSummaryAOD","","HLT");
	InputTag triggerResultsTag_("TriggerResults","","HLT");
	//Declare the handle (container) to store trigger objects.
	//InputTag trigEventTag("hltTriggerSummaryAOD","","HLT"); //make sure have correct process on MC -> HLT
	//data process=HLT, MC depends, Spring11 is REDIGI311X

	Handle<TriggerEvent> triggerEventHandle_;
	iEvent.getByLabel(triggerEventTag_, triggerEventHandle_);
	Handle<TriggerResults>   triggerResultsHandle_;
	iEvent.getByLabel(triggerResultsTag_,triggerResultsHandle_);

	if (!triggerResultsHandle_.isValid()) {
		cout<<"HLTEventAnalyzerAOD::analyze: Error in getting TriggerResults product from Event!"<<endl;
		return;
	}
	if (!triggerEventHandle_.isValid()) {
    cout << "HLTEventAnalyzerAOD::analyze: Error in getting TriggerEvent product from Event!" << endl;
    return;
  }

	// Get all trigger names associated with the "Jet" dataset.
  const vector<string> triggerNames = hltConfig_.datasetContent("Jet");

	if (!checkTriggerPass(iEvent, triggerNames)){
		return;
	}


	for (unsigned i = 0; i < triggerNames.size(); i++) {
		 /*if (i == 0)
				output_ << "#   Trig                            Name      Prescale_1      Prescale_2          Fired?" << endl;

		 string name = triggerNames[i];

		 pair<int, int> prescale = hltConfig_.prescaleValues(iEvent, iSetup, name);

		 bool fired = triggerFired(name, ( * trigResults));

		 output_ << "    Trig"
								 << setw(32) << name
					 << setw(16) << prescale.first
					 << setw(16) << prescale.second
								 << setw(16) << fired
								 << endl;*/
	Handle<TriggerEvent> mytrigEvent;//Declare the handle (container) to store trigger objects.
	iEvent.getByLabel(triggerEventTag_,mytrigEvent);

 	const std::pair<int,int> prescales(hltConfig_.prescaleValues(iEvent,iSetup,triggerNames));

 	cout << "The trigger: "<< triggerNames<<" has prescale values L1T,HLT: "
 	<< prescales.first << "," << prescales.second<< endl
 	<<"The total prescale value is: "<<prescales.first*prescales.second<<endl;

 	trigger::size_type filterIndex = trigEvent->filterIndex(edm::InputTag(filterName_,"",triggerEventTag_.process()));

 	if(filterIndex < trigEvent->sizeFilters()) {
 	   const trigger::Keys& trigKeys = trigEvent->filterKeys(filterIndex);
 	   const trigger::TriggerObjectCollection & trigObjColl(trigEvent->getObjects());

 		//now loop of the trigger objects passing filter
 	  	for(trigger::Keys::const_iterator keyIt=trigKeys.begin();keyIt!=trigKeys.end();++keyIt) {
 			  const trigger::TriggerObject trigobj = trigObjColl[*keyIt];
 			  trighist_pt->Fill(trigobj.pt(), prescales.first*prescales.second);
 			  cout<<"Fired trigger Pt value: "<<trigobj.pt()<<endl;
 		}
 	}//end filter size check

	}


	return;
}//------------------------------------------------------------analyze()

bool TriggerPrescalesAnalyzer::checkTriggerPass(const edm::Event& iEvent, const std::string& triggerName)
{
	const unsigned int triggerIndex(hltConfig_.triggerIndex(triggerName));//Get the trigger index for the current trigger

	assert(triggerIndex==iEvent.triggerNames(*triggerResultsHandle_).triggerIndex(triggerName));//check that the trigger in the event and in the configuration agree

	bool acceptedTrigger = triggerResultsHandle_->accept(triggerIndex);//Was the trigger accepted? 1 == True, 0 == False.

	cout<<"Currently analyzing trigger: "<<triggerName<<endl;//Prints trigger name

	return acceptedTrigger;
}//---------------------------------------------------checkTriggerPass()


// ------------ method called when ending the processing of a run  ------------
void TriggerPrescalesAnalyzer::endRun(edm::Run const&, edm::EventSetup const&)
{

}

// ------------ method called once each job just after ending the event loop  ------------
void TriggerPrescalesAnalyzer::endJob()
{
	myfile->Write();
}//-------------------------------------------------------------endJob()

// ------------ method called when starting to processes a luminosity block  ------------
void
TriggerPrescalesAnalyzer::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{

}//-----------------------------------------------beginLuminosityBlock()

// ------------ method called when ending the processing of a luminosity block  ------------
void
TriggerPrescalesAnalyzer::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{

}//---------------------------------------------------endLuminosityBlock

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
TriggerPrescalesAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions){
	//The following says we do not know what parameters are allowed so do no validation
	//Please change this to state exactly what you do use, even if it is no parameters
	edm::ParameterSetDescription desc;
	desc.setUnknown();
	descriptions.addDefault(desc);
}//---------------------------------------------------fillDescriptions()

//define this as a plug-in
DEFINE_FWK_MODULE(TriggerPrescalesAnalyzer);
