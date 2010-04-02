/*
 * \file EBStatusFlagsTask.cc
 *
 * $Date: 2010/04/02 08:18:10 $
 * $Revision: 1.27 $
 * \author G. Della Ricca
 *
*/

#include <iostream>
#include <fstream>
#include <vector>

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "DQMServices/Core/interface/MonitorElement.h"

#include "DQMServices/Core/interface/DQMStore.h"

#include "DataFormats/EcalRawData/interface/EcalRawDataCollections.h"

#include <DQM/EcalCommon/interface/Numbers.h>

#include <DQM/EcalBarrelMonitorTasks/interface/EBStatusFlagsTask.h>

EBStatusFlagsTask::EBStatusFlagsTask(const edm::ParameterSet& ps){

  init_ = false;

  dqmStore_ = edm::Service<DQMStore>().operator->();

  prefixME_ = ps.getUntrackedParameter<std::string>("prefixME", "");

  enableCleanup_ = ps.getUntrackedParameter<bool>("enableCleanup", false);

  mergeRuns_ = ps.getUntrackedParameter<bool>("mergeRuns", false);

  EcalRawDataCollection_ = ps.getParameter<edm::InputTag>("EcalRawDataCollection");

  for (int i = 0; i < 36; i++) {
    meEvtType_[i] = 0;

    meFEchErrors_[i][0] = 0;
    meFEchErrors_[i][1] = 0;
    meFEchErrors_[i][2] = 0;
  }

  meFEchErrorsByLumi_ = 0;

}

EBStatusFlagsTask::~EBStatusFlagsTask(){

}

void EBStatusFlagsTask::beginJob(void){

  ievt_ = 0;

  if ( dqmStore_ ) {
    dqmStore_->setCurrentFolder(prefixME_ + "/EBStatusFlagsTask");
    dqmStore_->rmdir(prefixME_ + "/EBStatusFlagsTask");
  }

}

void EBStatusFlagsTask::beginLuminosityBlock(const edm::LuminosityBlock& lumiBlock, const  edm::EventSetup& iSetup) {

  if ( meFEchErrorsByLumi_ ) meFEchErrorsByLumi_->Reset();

}

void EBStatusFlagsTask::endLuminosityBlock(const edm::LuminosityBlock&  lumiBlock, const  edm::EventSetup& iSetup) {
}

void EBStatusFlagsTask::beginRun(const edm::Run& r, const edm::EventSetup& c) {

  Numbers::initGeometry(c, false);

  if ( ! mergeRuns_ ) this->reset();

}

void EBStatusFlagsTask::endRun(const edm::Run& r, const edm::EventSetup& c) {

}

void EBStatusFlagsTask::reset(void) {

  for (int i = 0; i < 36; i++) {
    if ( meEvtType_[i] ) meEvtType_[i]->Reset();

    if ( meFEchErrors_[i][0] ) meFEchErrors_[i][0]->Reset();
    if ( meFEchErrors_[i][1] ) meFEchErrors_[i][1]->Reset();
    if ( meFEchErrors_[i][2] ) meFEchErrors_[i][2]->Reset();
  }
  if ( meFEchErrorsByLumi_ ) meFEchErrorsByLumi_->Reset();
}

void EBStatusFlagsTask::setup(void){

  init_ = true;

  char histo[200];

  if ( dqmStore_ ) {
    dqmStore_->setCurrentFolder(prefixME_ + "/EBStatusFlagsTask");

    dqmStore_->setCurrentFolder(prefixME_ + "/EBStatusFlagsTask/EvtType");
    for (int i = 0; i < 36; i++) {
      sprintf(histo, "EBSFT EVTTYPE %s", Numbers::sEB(i+1).c_str());
      meEvtType_[i] = dqmStore_->book1D(histo, histo, 31, -1., 30.);
      meEvtType_[i]->setBinLabel(1, "UNKNOWN", 1);
      meEvtType_[i]->setBinLabel(2+EcalDCCHeaderBlock::COSMIC, "COSMIC", 1);
      meEvtType_[i]->setBinLabel(2+EcalDCCHeaderBlock::BEAMH4, "BEAMH4", 1);
      meEvtType_[i]->setBinLabel(2+EcalDCCHeaderBlock::BEAMH2, "BEAMH2", 1);
      meEvtType_[i]->setBinLabel(2+EcalDCCHeaderBlock::MTCC, "MTCC", 1);
      meEvtType_[i]->setBinLabel(2+EcalDCCHeaderBlock::LASER_STD, "LASER_STD", 1);
      meEvtType_[i]->setBinLabel(2+EcalDCCHeaderBlock::LASER_POWER_SCAN, "LASER_POWER_SCAN", 1);
      meEvtType_[i]->setBinLabel(2+EcalDCCHeaderBlock::LASER_DELAY_SCAN, "LASER_DELAY_SCAN", 1);
      meEvtType_[i]->setBinLabel(2+EcalDCCHeaderBlock::TESTPULSE_SCAN_MEM, "TESTPULSE_SCAN_MEM", 1);
      meEvtType_[i]->setBinLabel(2+EcalDCCHeaderBlock::TESTPULSE_MGPA, "TESTPULSE_MGPA", 1);
      meEvtType_[i]->setBinLabel(2+EcalDCCHeaderBlock::PEDESTAL_STD, "PEDESTAL_STD", 1);
      meEvtType_[i]->setBinLabel(2+EcalDCCHeaderBlock::PEDESTAL_OFFSET_SCAN, "PEDESTAL_OFFSET_SCAN", 1);
      meEvtType_[i]->setBinLabel(2+EcalDCCHeaderBlock::PEDESTAL_25NS_SCAN, "PEDESTAL_25NS_SCAN", 1);
      meEvtType_[i]->setBinLabel(2+EcalDCCHeaderBlock::LED_STD, "LED_STD", 1);
      meEvtType_[i]->setBinLabel(2+EcalDCCHeaderBlock::PHYSICS_GLOBAL, "PHYSICS_GLOBAL", 1);
      meEvtType_[i]->setBinLabel(2+EcalDCCHeaderBlock::COSMICS_GLOBAL, "COSMICS_GLOBAL", 1);
      meEvtType_[i]->setBinLabel(2+EcalDCCHeaderBlock::HALO_GLOBAL, "HALO_GLOBAL", 1);
      meEvtType_[i]->setBinLabel(2+EcalDCCHeaderBlock::LASER_GAP, "LASER_GAP", 1);
      meEvtType_[i]->setBinLabel(2+EcalDCCHeaderBlock::TESTPULSE_GAP, "TESTPULSE_GAP");
      meEvtType_[i]->setBinLabel(2+EcalDCCHeaderBlock::PEDESTAL_GAP, "PEDESTAL_GAP");
      meEvtType_[i]->setBinLabel(2+EcalDCCHeaderBlock::LED_GAP, "LED_GAP", 1);
      meEvtType_[i]->setBinLabel(2+EcalDCCHeaderBlock::PHYSICS_LOCAL, "PHYSICS_LOCAL", 1);
      meEvtType_[i]->setBinLabel(2+EcalDCCHeaderBlock::COSMICS_LOCAL, "COSMICS_LOCAL", 1);
      meEvtType_[i]->setBinLabel(2+EcalDCCHeaderBlock::HALO_LOCAL, "HALO_LOCAL", 1);
      dqmStore_->tag(meEvtType_[i], i+1);
    }

    dqmStore_->setCurrentFolder(prefixME_ + "/EBStatusFlagsTask/FEStatus");
    for (int i = 0; i < 36; i++) {
      sprintf(histo, "EBSFT front-end status %s", Numbers::sEB(i+1).c_str());
      meFEchErrors_[i][0] = dqmStore_->book2D(histo, histo, 17, 0., 17., 4, 0., 4.);
      meFEchErrors_[i][0]->setAxisTitle("ieta'", 1);
      meFEchErrors_[i][0]->setAxisTitle("iphi'", 2);
      dqmStore_->tag(meFEchErrors_[i][0], i+1);

      for ( int ie = 1; ie <= 17; ie++ ) {
        for ( int ip = 1; ip <= 4; ip++ ) {
          meFEchErrors_[i][0]->setBinContent( ie, ip, -1. );
        }
      }
      meFEchErrors_[i][0]->setEntries( 0 );

      sprintf(histo, "EBSFT MEM front-end status %s", Numbers::sEB(i+1).c_str());
      meFEchErrors_[i][1] = dqmStore_->book2D(histo, histo, 2, 0., 2., 1, 0., 1.);
      meFEchErrors_[i][1]->setAxisTitle("pseudo-strip", 1);
      meFEchErrors_[i][1]->setAxisTitle("channel", 2);
      dqmStore_->tag(meFEchErrors_[i][1], i+1);

      meFEchErrors_[i][1]->setBinContent( 1, 1, -1. );
      meFEchErrors_[i][1]->setBinContent( 2, 1, -1. );
      meFEchErrors_[i][1]->setEntries( 0 );

      sprintf(histo, "EBSFT front-end status bits %s", Numbers::sEB(i+1).c_str());
      meFEchErrors_[i][2] = dqmStore_->book1D(histo, histo, 16, 0., 16.);
      meFEchErrors_[i][2]->setBinLabel(1+0, "ACTIVE", 1);
      meFEchErrors_[i][2]->setBinLabel(1+1, "DISABLED", 1);
      meFEchErrors_[i][2]->setBinLabel(1+2, "TIMEOUT", 1);
      meFEchErrors_[i][2]->setBinLabel(1+3, "HEADER", 1);
      meFEchErrors_[i][2]->setBinLabel(1+4, "CHANNEL ID", 1);
      meFEchErrors_[i][2]->setBinLabel(1+5, "LINK", 1);
      meFEchErrors_[i][2]->setBinLabel(1+6, "BLOCKSIZE", 1);
      meFEchErrors_[i][2]->setBinLabel(1+7, "SUPPRESSED", 1);
      meFEchErrors_[i][2]->setBinLabel(1+8, "FIFO FULL", 1);
      meFEchErrors_[i][2]->setBinLabel(1+9, "L1A SYNC", 1);
      meFEchErrors_[i][2]->setBinLabel(1+10, "BX SYNC", 1);
      meFEchErrors_[i][2]->setBinLabel(1+11, "L1A+BX SYNC", 1);
      meFEchErrors_[i][2]->setBinLabel(1+12, "FIFO+L1A", 1);
      meFEchErrors_[i][2]->setBinLabel(1+13, "H PARITY", 1);
      meFEchErrors_[i][2]->setBinLabel(1+14, "V PARITY", 1);
      meFEchErrors_[i][2]->setBinLabel(1+15, "FORCED ZS", 1);
      dqmStore_->tag(meFEchErrors_[i][2], i+1);
    }

    // checking the number of front-end errors in each DCC for each lumi
    // tower error is weighted by 1/68
    // bin 0 contains the number of processed events in the lumi (for normalization)
    sprintf(histo, "EBSFT weighted frontend errors by lumi");
    meFEchErrorsByLumi_ = dqmStore_->book1D(histo, histo, 36, 1., 37.);
    meFEchErrorsByLumi_->setLumiFlag();
    for (int i = 0; i < 36; i++) {
      meFEchErrorsByLumi_->setBinLabel(i+1, Numbers::sEB(i+1).c_str(), 1);
    }

  }

}

void EBStatusFlagsTask::cleanup(void){

  if ( ! init_ ) return;

  if ( dqmStore_ ) {
    dqmStore_->setCurrentFolder(prefixME_ + "/EBStatusFlagsTask");

    dqmStore_->setCurrentFolder(prefixME_ + "/EBStatusFlagsTask/EvtType");
    for (int i = 0; i < 36; i++) {
      if ( meEvtType_[i] ) dqmStore_->removeElement( meEvtType_[i]->getName() );
      meEvtType_[i] = 0;
    }

    dqmStore_->setCurrentFolder(prefixME_ + "/EBStatusFlagsTask/FEStatus");
    for (int i = 0; i < 36; i++) {
      if ( meFEchErrors_[i][0] ) dqmStore_->removeElement( meFEchErrors_[i][0]->getName() );
      meFEchErrors_[i][0] = 0;
      if ( meFEchErrors_[i][1] ) dqmStore_->removeElement( meFEchErrors_[i][1]->getName() );
      meFEchErrors_[i][1] = 0;
      if ( meFEchErrors_[i][2] ) dqmStore_->removeElement( meFEchErrors_[i][2]->getName() );
      meFEchErrors_[i][2] = 0;
    }

    if ( meFEchErrorsByLumi_ ) dqmStore_->removeElement( meFEchErrorsByLumi_->getName() );
    meFEchErrorsByLumi_ = 0;

  }

  init_ = false;

}

void EBStatusFlagsTask::endJob(void){

  edm::LogInfo("EBStatusFlagsTask") << "analyzed " << ievt_ << " events";

  if ( enableCleanup_ ) this->cleanup();

}

void EBStatusFlagsTask::analyze(const edm::Event& e, const edm::EventSetup& c){

  if ( ! init_ ) this->setup();

  ievt_++;

  // fill bin 0 with number of events in the lumi
  if ( meFEchErrorsByLumi_ ) meFEchErrorsByLumi_->Fill(0.);

  edm::Handle<EcalRawDataCollection> dcchs;

  if ( e.getByLabel(EcalRawDataCollection_, dcchs) ) {

    for ( EcalRawDataCollection::const_iterator dcchItr = dcchs->begin(); dcchItr != dcchs->end(); ++dcchItr ) {

      if ( Numbers::subDet( *dcchItr ) != EcalBarrel ) continue;

      int ism = Numbers::iSM( *dcchItr, EcalBarrel );
      float xism = ism + 0.5;

      if ( meEvtType_[ism-1] ) meEvtType_[ism-1]->Fill(dcchItr->getRunType()+0.5);

      const std::vector<short> status = dcchItr->getFEStatus();

      for ( unsigned int itt=1; itt<=status.size(); itt++ ) {

        if ( itt > 70 ) continue;

        if ( itt >= 1 && itt <= 68 ) {

          int iet = (itt-1)/4 + 1;
          int ipt = (itt-1)%4 + 1;

          float xiet = iet - 0.5;
          float xipt = ipt - 0.5;

          if ( meFEchErrors_[ism-1][0] ) {
            if ( meFEchErrors_[ism-1][0]->getBinContent(iet, ipt) == -1 ) {
              meFEchErrors_[ism-1][0]->setBinContent(iet, ipt, 0);
            }
          }

          if ( ! ( status[itt-1] == 0 || status[itt-1] == 1 || status[itt-1] == 7 || status[itt-1] == 8 || status[itt-1] == 15 ) ) {
            if ( meFEchErrors_[ism-1][0] ) meFEchErrors_[ism-1][0]->Fill(xiet, xipt);
            if ( meFEchErrorsByLumi_ ) meFEchErrorsByLumi_->Fill(xism, 1./68.);
          }

        } else if ( itt == 69 || itt == 70 ) {

          if ( meFEchErrors_[ism-1][1] ) {
            if ( meFEchErrors_[ism-1][1]->getBinContent(itt-68, 1) == -1 ) {
              meFEchErrors_[ism-1][1]->setBinContent(itt-68, 1, 0);
            }
          }

          if ( ! ( status[itt-1] == 0 || status[itt-1] == 1 || status[itt-1] == 7 || status[itt-1] == 8 || status[itt-1] == 15 ) ) {
            if ( meFEchErrors_[ism-1][1] ) meFEchErrors_[ism-1][1]->Fill(itt-68-0.5, 0);
          }

        }

        if ( meFEchErrors_[ism-1][2] ) meFEchErrors_[ism-1][2]->Fill(status[itt-1]+0.5); 

      }

    }

  } else {

    edm::LogWarning("EBStatusFlagsTask") << EcalRawDataCollection_ << " not available";

  }

}

