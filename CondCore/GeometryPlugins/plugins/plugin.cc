#include "CondCore/PluginSystem/interface/registration_macros.h"

#include "CondFormats/GeometryObjects/interface/GeometryFile.h"
#include "Geometry/Records/interface/GeometryFileRcd.h"

#include "CondFormats/GeometryObjects/interface/PGeometricDet.h"
#include "Geometry/Records/interface/PGeometricDetRcd.h"

#include "CondFormats/GeometryObjects/interface/PCaloGeometry.h"
#include "Geometry/Records/interface/PEcalBarrelRcd.h"
#include "Geometry/Records/interface/PEcalEndcapRcd.h"
#include "Geometry/Records/interface/PEcalPreshowerRcd.h"
#include "Geometry/Records/interface/PHcalRcd.h"

#include "CondFormats/GeometryObjects/interface/CSCRecoDigiParameters.h"
#include "Geometry/Records/interface/CSCRecoDigiParametersRcd.h"

#include "CondFormats/GeometryObjects/interface/RecoIdealGeometry.h"    
#include "Geometry/Records/interface/CSCRecoGeometryRcd.h"
#include "Geometry/Records/interface/DTRecoGeometryRcd.h"

#include "Geometry/Records/interface/IdealGeometryRecord.h" 

DEFINE_SEAL_MODULE();
REGISTER_PLUGIN(GeometryFileRcd,GeometryFile);
REGISTER_PLUGIN(PGeometricDetRcd,PGeometricDet);
REGISTER_PLUGIN(PEcalBarrelRcd,PCaloGeometry);
REGISTER_PLUGIN(PEcalEndcapRcd,PCaloGeometry);
REGISTER_PLUGIN(PEcalPreshowerRcd,PCaloGeometry);
REGISTER_PLUGIN(PHcalRcd,PCaloGeometry);
REGISTER_PLUGIN(CSCRecoDigiParametersRcd,CSCRecoDigiParameters);
REGISTER_PLUGIN(CSCRecoGeometryRcd,RecoIdealGeometry);
REGISTER_PLUGIN(DTRecoGeometryRcd,RecoIdealGeometry);
