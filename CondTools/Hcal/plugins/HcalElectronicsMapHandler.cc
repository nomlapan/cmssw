#include "CondTools/Hcal/interface/HcalElectronicsMapHandler.h"
#include "DataFormats/DetId/interface/DetId.h"

HcalElectronicsMapHandler::HcalElectronicsMapHandler(edm::ParameterSet const & ps)
{
  m_name = ps.getUntrackedParameter<std::string>("name","HcalElectronicsMapHandler");
  sinceTime = ps.getUntrackedParameter<unsigned>("IOVRun",0);
  fFile = ps.getUntrackedParameter<std::string>("CondFile","");
}

HcalElectronicsMapHandler::~HcalElectronicsMapHandler()
{
}

void HcalElectronicsMapHandler::getNewObjects()
{
  edm::LogInfo   ("HcalElectronicsMapHandler") << "------- " << m_name 
					  << " - > getNewObjects\n" << 
    //check whats already inside of database
    "got offlineInfo"<<
    tagInfo().name << ", size " << tagInfo().size 
					  << ", last object valid since " 
					  << tagInfo().lastInterval.first << std::endl;  
  

  HcalElectronicsMap* myobject = new HcalElectronicsMap();
  std::cout << "Using file: " << fFile << std::endl;

  std::ifstream inStream(fFile.c_str() );
  bool g = HcalDbASCIIIO::getObject(inStream, myobject);

  std::cout << "bool=" << g << std::endl;
  std::cout << myobject << std::endl;

  //  IOV information
  cond::Time_t myTime = sinceTime;

  std::cout << "Using IOV run " << sinceTime << std::endl;

  // prepare for transfer:
  m_to_transfer.push_back(std::make_pair(myobject,myTime));

  edm::LogInfo("HcalElectronicsMapHandler") << "------- " << m_name << " - > getNewObjects" << std::endl;

}
