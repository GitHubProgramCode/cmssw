//
// Created: 2 Mar 2006
//          Shahram Rahatlou, University of Rome & INFN
//
#include "CalibCalorimetry/EcalTrivialCondModules/interface/EcalTrivialObjectAnalyzer.h"

#include <stdexcept>
#include <string>
#include <iostream>
#include <iomanip>
#include <map>
#include <vector>
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "DataFormats/EcalDetId/interface/EBDetId.h"

#include "CondFormats/EcalObjects/interface/EcalPedestals.h"
#include "CondFormats/DataRecord/interface/EcalPedestalsRcd.h"

#include "CondFormats/EcalObjects/interface/EcalXtalGroupId.h"
#include "CondFormats/EcalObjects/interface/EcalWeightXtalGroups.h"
#include "CondFormats/DataRecord/interface/EcalWeightXtalGroupsRcd.h"

#include "CondFormats/EcalObjects/interface/EcalWeightSet.h"
#include "CondFormats/EcalObjects/interface/EcalTBWeights.h"
#include "CondFormats/DataRecord/interface/EcalTBWeightsRcd.h"

#include "CondFormats/EcalObjects/interface/EcalIntercalibConstants.h"
#include "CondFormats/DataRecord/interface/EcalIntercalibConstantsRcd.h"

#include "CondFormats/EcalObjects/interface/EcalIntercalibErrors.h"
#include "CondFormats/DataRecord/interface/EcalIntercalibErrorsRcd.h"

#include "CondFormats/EcalObjects/interface/EcalTimeCalibConstants.h"
#include "CondFormats/DataRecord/interface/EcalTimeCalibConstantsRcd.h"

#include "CondFormats/EcalObjects/interface/EcalTimeCalibErrors.h"
#include "CondFormats/DataRecord/interface/EcalTimeCalibErrorsRcd.h"

#include "CondFormats/EcalObjects/interface/EcalTimeOffsetConstant.h"
#include "CondFormats/DataRecord/interface/EcalTimeOffsetConstantRcd.h"

#include "CondFormats/EcalObjects/interface/EcalMGPAGainRatio.h"
#include "CondFormats/EcalObjects/interface/EcalGainRatios.h"
#include "CondFormats/DataRecord/interface/EcalGainRatiosRcd.h"

#include "CondFormats/EcalObjects/interface/EcalADCToGeVConstant.h"
#include "CondFormats/DataRecord/interface/EcalADCToGeVConstantRcd.h"

#include "CondFormats/EcalObjects/interface/EcalLaserAlphas.h"
#include "CondFormats/DataRecord/interface/EcalLaserAlphasRcd.h"
 
#include "CondFormats/EcalObjects/interface/EcalLaserAPDPNRatiosRef.h"
#include "CondFormats/DataRecord/interface/EcalLaserAPDPNRatiosRefRcd.h"
 
#include "CondFormats/EcalObjects/interface/EcalLaserAPDPNRatios.h"
#include "CondFormats/DataRecord/interface/EcalLaserAPDPNRatiosRcd.h"

#include "CondFormats/EcalObjects/interface/EcalClusterLocalContCorrParameters.h"
#include "CondFormats/DataRecord/interface/EcalClusterLocalContCorrParametersRcd.h"
#include "CondFormats/EcalObjects/interface/EcalClusterCrackCorrParameters.h"
#include "CondFormats/DataRecord/interface/EcalClusterCrackCorrParametersRcd.h"
#include "CondFormats/EcalObjects/interface/EcalClusterEnergyCorrectionParameters.h"
#include "CondFormats/DataRecord/interface/EcalClusterEnergyCorrectionParametersRcd.h"
#include "CondFormats/EcalObjects/interface/EcalClusterEnergyUncertaintyParameters.h"
#include "CondFormats/DataRecord/interface/EcalClusterEnergyUncertaintyParametersRcd.h"
#include "CondFormats/EcalObjects/interface/EcalClusterEnergyCorrectionObjectSpecificParameters.h"
#include "CondFormats/DataRecord/interface/EcalClusterEnergyCorrectionObjectSpecificParametersRcd.h"

#include "CondFormats/EcalObjects/interface/EcalChannelStatus.h"
#include "CondFormats/DataRecord/interface/EcalChannelStatusRcd.h"

#include "CondFormats/EcalObjects/interface/EcalSampleMask.h"
#include "CondFormats/DataRecord/interface/EcalSampleMaskRcd.h"

#include "CLHEP/Matrix/Matrix.h"

using namespace std;

void EcalTrivialObjectAnalyzer::analyze(const edm::Event& e, const edm::EventSetup& context)
{
    using namespace edm::eventsetup;
    // Context is not used.
    edm::LogInfo(">>> EcalTrivialObjectAnalyzer: processing run ")<<e.id().run() << " event: " << e.id().event();

    edm::ESHandle<EcalPedestals> pPeds;
    context.get<EcalPedestalsRcd>().get(pPeds);

    // ADC -> GeV Scale
    edm::ESHandle<EcalADCToGeVConstant> pAgc;
    context.get<EcalADCToGeVConstantRcd>().get(pAgc);
    const EcalADCToGeVConstant* agc = pAgc.product();
    edm::LogInfo("Global ADC->GeV scale: EB ") << std::setprecision(6) << agc->getEBValue() << " GeV/ADC count" 
	      " EE " << std::setprecision(6) << agc->getEEValue() << " GeV/ADC count";

    // use a channel to fetch values from DB
    double r1 = (double)std::rand()/( double(RAND_MAX)+double(1) );
    int ieta =  int( 1 + r1*85 );
    r1 = (double)std::rand()/( double(RAND_MAX)+double(1) );
    int iphi =  int( 1 + r1*20 );

    EBDetId ebid(ieta,iphi); //eta,phi
    edm::LogInfo("EcalTrivialObjectAnalyzer: using EBDetId: ") << ebid;

    const EcalPedestals* myped=pPeds.product();
    EcalPedestals::const_iterator it = myped->find(ebid.rawId());
    if( it!=myped->end() ){
      edm::LogInfo("EcalPedestal: ")
                << "  mean_x1:  " << std::setprecision(8) << (*it).mean_x1 << " rms_x1: " << (*it).rms_x1
                << "  mean_x6:  " <<(*it).mean_x6 << " rms_x6: " << (*it).rms_x6
                << "  mean_x12: " <<(*it).mean_x12 << " rms_x12: " << (*it).rms_x12
                ;
    } else {
      edm::LogInfo("No pedestal found for this xtal! something wrong with EcalPedestals in your DB? ")
               ;
    }

    // fetch map of groups of xtals
    edm::ESHandle<EcalWeightXtalGroups> pGrp;
    context.get<EcalWeightXtalGroupsRcd>().get(pGrp);
    const EcalWeightXtalGroups* grp = pGrp.product();

    EcalXtalGroupsMap::const_iterator git = grp->getMap().find( ebid.rawId() );
    EcalXtalGroupId gid;
    if( git != grp->getMap().end() ) {
      edm::LogInfo("XtalGroupId.id() = ") << std::setprecision(3) << (*git).id() ;
      gid = (*git);
    } else {
      edm::LogInfo("No group id found for this crystal. something wrong with EcalWeightXtalGroups in your DB?")
                ;
   }

    // Gain Ratios
    edm::ESHandle<EcalGainRatios> pRatio;
    context.get<EcalGainRatiosRcd>().get(pRatio);
    const EcalGainRatios* gr = pRatio.product();

    EcalGainRatioMap::const_iterator grit=gr->getMap().find(ebid.rawId());
    EcalMGPAGainRatio mgpa;
    if( grit!=gr->getMap().end() ){
      mgpa = (*grit);

      edm::LogInfo("EcalMGPAGainRatio: ")
                << "gain 12/6 :  " << std::setprecision(4) << mgpa.gain12Over6() << " gain 6/1: " << mgpa.gain6Over1()
                ;
    } else {
      edm::LogInfo("No MGPA Gain Ratio found for this xtal! something wrong with EcalGainRatios in your DB? ")
               ;
    }

    // Intercalib constants
    edm::ESHandle<EcalIntercalibConstants> pIcal;
    context.get<EcalIntercalibConstantsRcd>().get(pIcal);
    const EcalIntercalibConstants* ical = pIcal.product();

    EcalIntercalibConstantMap::const_iterator icalit=ical->getMap().find(ebid.rawId());
    EcalIntercalibConstant icalconst;
    if( icalit!=ical->getMap().end() ){
      icalconst = (*icalit);

      edm::LogInfo("EcalIntercalibConstant: ")
                <<std::setprecision(6)
                << icalconst
                ;
    } else {
      edm::LogInfo("No intercalib const found for this xtal! something wrong with EcalIntercalibConstants in your DB? ")
               ;
    }

    // Intercalib errors
    edm::ESHandle<EcalIntercalibErrors> pIcalErr;
    context.get<EcalIntercalibErrorsRcd>().get(pIcalErr);
    const EcalIntercalibErrors* icalErr = pIcalErr.product();

    EcalIntercalibErrorMap::const_iterator icalitErr=icalErr->getMap().find(ebid.rawId());
    EcalIntercalibError icalconstErr;
    if( icalitErr!=icalErr->getMap().end() ){
      icalconstErr = (*icalitErr);

      edm::LogInfo("EcalIntercalibError: ")
                <<std::setprecision(6)
                << icalconstErr
                ;
    } else {
      edm::LogInfo("No intercalib const found for this xtal! something wrong with EcalIntercalibErrors in your DB? ")
               ;
    }



    { // quick and dirty for cut and paste ;) it is a test program...
            // TimeCalib constants
            edm::ESHandle<EcalTimeCalibConstants> pIcal;
            context.get<EcalTimeCalibConstantsRcd>().get(pIcal);
            const EcalTimeCalibConstants* ical = pIcal.product();

            EcalTimeCalibConstantMap::const_iterator icalit=ical->getMap().find(ebid.rawId());
            EcalTimeCalibConstant icalconst;
            if( icalit!=ical->getMap().end() ){
                    icalconst = (*icalit);

                    edm::LogInfo("EcalTimeCalibConstant: ")
                            <<std::setprecision(6)
                            << icalconst
                            ;
            } else {
	      edm::LogInfo("No intercalib const found for this xtal! something wrong with EcalTimeCalibConstants in your DB? ")
                            ;
            }

            // TimeCalib errors
            edm::ESHandle<EcalTimeCalibErrors> pIcalErr;
            context.get<EcalTimeCalibErrorsRcd>().get(pIcalErr);
            const EcalTimeCalibErrors* icalErr = pIcalErr.product();

            EcalTimeCalibErrorMap::const_iterator icalitErr=icalErr->getMap().find(ebid.rawId());
            EcalTimeCalibError icalconstErr;
            if( icalitErr!=icalErr->getMap().end() ){
                    icalconstErr = (*icalitErr);

                    edm::LogInfo("EcalTimeCalibError: ")
                            <<std::setprecision(6)
                            << icalconstErr
                            ;
            } else {
	      edm::LogInfo("No intercalib const found for this xtal! something wrong with EcalTimeCalibErrors in your DB? ")
                            ;
            }
    }

   // fetch Time Offset
   //std::cout <<"Fetching TimeOffsetConstant from DB " << std::endl;

   // Time Offset constants
   edm::ESHandle<EcalTimeOffsetConstant> pTOff;
   context.get<EcalTimeOffsetConstantRcd>().get(pTOff);
   const EcalTimeOffsetConstant* TOff = pTOff.product();

   edm::LogInfo("EcalTimeOffsetConstant: ")
	     << " EB " << TOff->getEBValue()
	     << " EE " << TOff->getEEValue()
	     ;

   // fetch TB weights
   edm::LogInfo("Fetching EcalTBWeights from DB ");
   edm::ESHandle<EcalTBWeights> pWgts;
   context.get<EcalTBWeightsRcd>().get(pWgts);
   const EcalTBWeights* wgts = pWgts.product();
   edm::LogInfo("EcalTBWeightMap.size(): ") << std::setprecision(3) << wgts->getMap().size();


   // look up the correct weights for this  xtal
   //EcalXtalGroupId gid( (*git) );
   EcalTBWeights::EcalTDCId tdcid(1);

   edm::LogInfo("Lookup EcalWeightSet for groupid: ") << std::setprecision(3) 
             << gid.id() << " and TDC id " << tdcid ;
   EcalTBWeights::EcalTBWeightMap::const_iterator wit = wgts->getMap().find( std::make_pair(gid,tdcid) );
   EcalWeightSet  wset;
   if( wit != wgts->getMap().end() ) {
      wset = wit->second;
      edm::LogInfo("check size of data members in EcalWeightSet") ;
      //wit->second.print(std::cout);


      //typedef std::vector< std::vector<EcalWeight> > EcalWeightMatrix;
      const EcalWeightSet::EcalWeightMatrix& mat1 = wit->second.getWeightsBeforeGainSwitch();
      const EcalWeightSet::EcalWeightMatrix& mat2 = wit->second.getWeightsAfterGainSwitch();

//       std::cout << "WeightsBeforeGainSwitch.size: " << mat1.size()
//                 << ", WeightsAfterGainSwitch.size: " << mat2.size() << std::endl;


      CLHEP::HepMatrix clmat1(3,10,0);
      CLHEP::HepMatrix clmat2(3,10,0);
      for(int irow=0; irow<3; irow++) {
       for(int icol=0; icol<10; icol++) {
         clmat1[irow][icol] = mat1(irow,icol);
         clmat2[irow][icol] = mat2(irow,icol);
       }
     }
      edm::LogInfo("weight matrix before gain switch:") << clmat1;
      edm::LogInfo("weight matrix after gain switch:") << clmat2;

   } else {
     edm::LogInfo("No weights found for EcalGroupId: ") << gid.id() << " and  EcalTDCId: " << tdcid ;
  }

   // cluster functions/corrections
   edm::ESHandle<EcalClusterLocalContCorrParameters> pLocalCont;
   context.get<EcalClusterLocalContCorrParametersRcd>().get(pLocalCont);
   const EcalClusterLocalContCorrParameters* paramLocalCont = pLocalCont.product();
   edm::LogInfo("LocalContCorrParameters:");
   for ( EcalFunctionParameters::const_iterator it = paramLocalCont->params().begin(); it != paramLocalCont->params().end(); ++it ) {
     // edm::LogInfo(" ") << *it;
   }
   // std::cout << "\n";
   edm::ESHandle<EcalClusterCrackCorrParameters> pCrack;
   context.get<EcalClusterCrackCorrParametersRcd>().get(pCrack);
   const EcalClusterCrackCorrParameters* paramCrack = pCrack.product();
   edm::LogInfo("CrackCorrParameters:");
   for ( EcalFunctionParameters::const_iterator it = paramCrack->params().begin(); it != paramCrack->params().end(); ++it ) {
     //  edm::LogInfo(" ") << *it;
   }
   // std::cout << "\n";
   edm::ESHandle<EcalClusterEnergyCorrectionParameters> pEnergyCorrection;
   context.get<EcalClusterEnergyCorrectionParametersRcd>().get(pEnergyCorrection);
   const EcalClusterEnergyCorrectionParameters* paramEnergyCorrection = pEnergyCorrection.product();
   edm::LogInfo("EnergyCorrectionParameters:");
   for ( EcalFunctionParameters::const_iterator it = paramEnergyCorrection->params().begin(); it != paramEnergyCorrection->params().end(); ++it ) {
     // edm::LogInfo(" ") << *it;
   }
   // std::cout << "\n";
   edm::ESHandle<EcalClusterEnergyUncertaintyParameters> pEnergyUncertainty;
   context.get<EcalClusterEnergyUncertaintyParametersRcd>().get(pEnergyUncertainty);
   const EcalClusterEnergyUncertaintyParameters* paramEnergyUncertainty = pEnergyUncertainty.product();
   edm::LogInfo("EnergyCorrectionParameters:");
   for ( EcalFunctionParameters::const_iterator it = paramEnergyUncertainty->params().begin(); it != paramEnergyUncertainty->params().end(); ++it ) {
     // edm::LogInfo(" ") << *it;
   }
   // std::cout << "\n";
   edm::ESHandle<EcalClusterEnergyCorrectionObjectSpecificParameters> pEnergyCorrectionObjectSpecific;
   context.get<EcalClusterEnergyCorrectionObjectSpecificParametersRcd>().get(pEnergyCorrectionObjectSpecific);
   const EcalClusterEnergyCorrectionObjectSpecificParameters* paramEnergyCorrectionObjectSpecific = pEnergyCorrectionObjectSpecific.product();
   edm::LogInfo("EnergyCorrectionObjectSpecificParameters:");
   for ( EcalFunctionParameters::const_iterator it = paramEnergyCorrectionObjectSpecific->params().begin(); it != paramEnergyCorrectionObjectSpecific->params().end(); ++it ) {
     //  edm::LogInfo(" ") << *it;
   }
   // std::cout << "\n";


   // laser correction
   
   // laser alphas
   edm::ESHandle<EcalLaserAlphas> pAlpha; 
   context.get<EcalLaserAlphasRcd>().get(pAlpha);
   const EcalLaserAlphas* lalpha = pAlpha.product();
   
   EcalLaserAlphaMap::const_iterator lalphait;
   lalphait = lalpha->getMap().find(ebid.rawId());
   if( lalphait!=lalpha->getMap().end() ){
     edm::LogInfo("EcalLaserAlpha: ")
	       <<std::setprecision(6)
	       << (*lalphait)
	       ;
   } else {
     edm::LogInfo("No laser alpha found for this xtal! something wrong with EcalLaserAlphas in your DB? ")
               ;
   }

   // laser apdpnref
   edm::ESHandle<EcalLaserAPDPNRatiosRef> pAPDPNRatiosRef;          
   context.get<EcalLaserAPDPNRatiosRefRcd>().get(pAPDPNRatiosRef);  
   const EcalLaserAPDPNRatiosRef* lref = pAPDPNRatiosRef.product(); 
   
   EcalLaserAPDPNRatiosRef::const_iterator lrefit;
   lrefit = lref->getMap().find(ebid.rawId());
   if( lrefit!=lref->getMap().end() ){
     //  edm::LogInfo("EcalLaserAPDPNRatiosRef: ")
     //	       <<std::setprecision(6)
     //	       << (*lrefit)
  	       ;
   } else {
     edm::LogInfo("No laser apd/pn ref found for this xtal! something wrong with EcalLaserAPDPNRatiosRef in your DB? ")
	       ;
   }

   // laser apdpn ratios 
   edm::ESHandle<EcalLaserAPDPNRatios> pAPDPNRatios;          
   context.get<EcalLaserAPDPNRatiosRcd>().get(pAPDPNRatios);  
   const EcalLaserAPDPNRatios* lratio = pAPDPNRatios.product();

   EcalLaserAPDPNRatios::EcalLaserAPDPNRatiosMap::const_iterator lratioit; 
   lratioit = lratio->getLaserMap().find(ebid.rawId());
   EcalLaserAPDPNRatios::EcalLaserAPDPNpair lratioconst;
   
   if( lratioit!=lratio->getLaserMap().end() ){
     lratioconst = (*lratioit);
     
     //  edm::LogInfo("EcalLaserAPDPNRatios: ")
      //	       <<e.id().run() << " " << e.id().event() << " " 
     //	       << std::setprecision(6)
     //	       << lratioconst.p1 << " " << lratioconst.p2
    	       ;
   } else {
     edm::LogInfo("No laser apd/pn ratio found for this xtal! something wrong with EcalLaserAPDPNRatios in your DB? ")
  	       ;
   }

   // laser timestamps
   EcalLaserAPDPNRatios::EcalLaserTimeStamp ltimestamp;
   // EcalLaserAPDPNRatios::EcalLaserTimeStampMap::const_iterator ltimeit;
   for (int i=1; i<=92; i++) {     
     ltimestamp = lratio->getTimeMap()[i];
     //  edm::LogInfo("i = ") << std::setprecision(6) << i
     //         << ltimestamp.t1.value() << " " << ltimestamp.t2.value() << " : " ;
   }
   // edm::LogInfo("Tests finished.");

   // channel status
   edm::ESHandle<EcalChannelStatus> pChannelStatus;
   context.get<EcalChannelStatusRcd>().get(pChannelStatus);
   const EcalChannelStatus *ch_status = pChannelStatus.product();

   EcalChannelStatusMap::const_iterator chit;
   chit = ch_status->getMap().find(ebid.rawId());
   if( chit != ch_status->getMap().end() ){
           EcalChannelStatusCode ch_code = (*chit);
           edm::LogInfo("EcalChannelStatus: ")
                   <<std::setprecision(6)
                   << ch_code.getStatusCode()
                   ;
   } else {
     edm::LogInfo("No channel status found for this xtal! something wrong with EcalChannelStatus in your DB? ")
                   ;
   }


   // laser transparency correction

   // Mask to ignore sample
   edm::ESHandle<EcalSampleMask> pSMask;
   context.get<EcalSampleMaskRcd>().get(pSMask);
   const EcalSampleMask* smask = pSMask.product();
   edm::LogInfo("Sample Mask EB ") << std::hex << smask->getEcalSampleMaskRecordEB() 
	     << " EE " << std::hex << smask->getEcalSampleMaskRecordEE();
   

/*
    std::cout << "make CLHEP matrices from vector<vector<Ecalweight>>" << std::endl;
    CLHEP::HepMatrix clmat1(3,8,0);
    CLHEP::HepMatrix clmat2(3,8,0);
    for(int irow=0; irow<3; irow++) {
     for(int icol=0; icol<8; icol++) {
       clmat1[irow][icol] = (mat1[irow])[icol]();
       clmat2[irow][icol] = (mat2[irow])[icol]();
     }
   }
   std::cout << clmat1 << std::endl;
   std::cout << clmat2 << std::endl;
*/

} //end of ::Analyze()
  //DEFINE_FWK_MODULE(EcalTrivialObjectAnalyzer);
