#include <stdexcept>
#include <string>
#include "OnlineDB/Oracle/interface/Oracle.h"

#include "OnlineDB/EcalCondDB/interface/LMFLaserBlueNormDat.h"
#include "OnlineDB/EcalCondDB/interface/RunTag.h"
#include "OnlineDB/EcalCondDB/interface/RunIOV.h"
#include "OnlineDB/EcalCondDB/interface/IDBObject.h"

using namespace std;
using namespace oracle::occi;

LMFLaserBlueNormDat::LMFLaserBlueNormDat()
{
  m_env = NULL;
  m_conn = NULL;
  m_writeStmt = NULL;
  m_readStmt = NULL;

  m_apdOverPNAMean = 0;
  m_apdOverPNARMS = 0;
  m_apdOverPNBMean = 0;
  m_apdOverPNMean = 0;
  m_apdOverPNRMS = 0;
}



LMFLaserBlueNormDat::~LMFLaserBlueNormDat()
{
}



void LMFLaserBlueNormDat::prepareWrite()
  throw(runtime_error)
{
  this->checkConnection();

  try {
    m_writeStmt = m_conn->createStatement();
    m_writeStmt->setSQL("INSERT INTO lmf_laser_blue_norm_dat (iov_id, logic_id, "
			"apd_over_pnA_mean, apd_over_pnA_rms, apd_over_pnB_mean, apd_over_pnB_rms, apd_over_pn_mean, apd_over_pn_rms) "
			"VALUES (:iov_id, :logic_id, "
			":3, :4, :5, :6, :7, :8)");
  } catch (SQLException &e) {
    throw(runtime_error("LMFLaserBlueNormDat::prepareWrite():  "+e.getMessage()));
  }
}



void LMFLaserBlueNormDat::writeDB(const EcalLogicID* ecid, const LMFLaserBlueNormDat* item, LMFRunIOV* iov)
  throw(runtime_error)
{
  this->checkConnection();
  this->checkPrepare();

  int iovID = iov->fetchID();
  if (!iovID) { throw(runtime_error("LMFLaserBlueNormDat::writeDB:  IOV not in DB")); }

  int logicID = ecid->getLogicID();
  if (!logicID) { throw(runtime_error("LMFLaserBlueNormDat::writeDB:  Bad EcalLogicID")); }
  
  try {
    m_writeStmt->setInt(1, iovID);
    m_writeStmt->setInt(2, logicID);

    m_writeStmt->setFloat(3, item->getAPDOverPNAMean() );
    m_writeStmt->setFloat(4, item->getAPDOverPNARMS() );
    m_writeStmt->setFloat(5, item->getAPDOverPNBMean() );
    m_writeStmt->setFloat(6, item->getAPDOverPNBRMS() );
    m_writeStmt->setFloat(7, item->getAPDOverPNMean() );
    m_writeStmt->setFloat(8, item->getAPDOverPNRMS() );


    m_writeStmt->executeUpdate();
  } catch (SQLException &e) {
    throw(runtime_error("LMFLaserBlueNormDat::writeDB():  "+e.getMessage()));
  }
}


void LMFLaserBlueNormDat::writeArrayDB(const std::map< EcalLogicID, LMFLaserBlueNormDat >* data, LMFRunIOV* iov)
  throw(runtime_error)
{
  this->checkConnection();
  this->checkPrepare();

  int iovID = iov->fetchID();
  if (!iovID) { throw(runtime_error("LMFLaserBlueNormDat::writeArrayDB:  IOV not in DB")); }


  int nrows=data->size(); 
  int* ids= new int[nrows];
  int* iovid_vec= new int[nrows];
  float* xx= new float[nrows];
  float* yy= new float[nrows];
  float* zz= new float[nrows];
  float* ww= new float[nrows];
  float* uu= new float[nrows];
  float* tt= new float[nrows];

  ub2* ids_len= new ub2[nrows];
  ub2* iov_len= new ub2[nrows];
  ub2* x_len= new ub2[nrows];
  ub2* y_len= new ub2[nrows];
  ub2* z_len= new ub2[nrows];
  ub2* w_len= new ub2[nrows];
  ub2* u_len= new ub2[nrows];
  ub2* t_len= new ub2[nrows];

  const EcalLogicID* channel;
  const LMFLaserBlueNormDat* dataitem;
  int count=0;
  typedef map< EcalLogicID, LMFLaserBlueNormDat >::const_iterator CI;
  for (CI p = data->begin(); p != data->end(); ++p) {
        channel = &(p->first);
	int logicID = channel->getLogicID();
	if (!logicID) { throw(runtime_error("LMFLaserBlueNormDat::writeArrayDB:  Bad EcalLogicID")); }
	ids[count]=logicID;
	iovid_vec[count]=iovID;

	dataitem = &(p->second);
	// dataIface.writeDB( channel, dataitem, iov);
	float x=dataitem->getAPDOverPNAMean();
	float y=dataitem->getAPDOverPNARMS();
	float z=dataitem->getAPDOverPNBMean();
	float w=dataitem->getAPDOverPNBRMS();
	float u=dataitem->getAPDOverPNMean();
	float t=dataitem->getAPDOverPNRMS();



	xx[count]=x;
	yy[count]=y;
	zz[count]=z;
	ww[count]=w;
	uu[count]=u;
	tt[count]=t;


	ids_len[count]=sizeof(ids[count]);
	iov_len[count]=sizeof(iovid_vec[count]);
	
	x_len[count]=sizeof(xx[count]);
	y_len[count]=sizeof(yy[count]);
	z_len[count]=sizeof(zz[count]);
	w_len[count]=sizeof(ww[count]);
	u_len[count]=sizeof(uu[count]);
	t_len[count]=sizeof(tt[count]);

	count++;
     }


  try {
    m_writeStmt->setDataBuffer(1, (dvoid*)iovid_vec, OCCIINT, sizeof(iovid_vec[0]),iov_len);
    m_writeStmt->setDataBuffer(2, (dvoid*)ids, OCCIINT, sizeof(ids[0]), ids_len );
    m_writeStmt->setDataBuffer(3, (dvoid*)xx, OCCIFLOAT , sizeof(xx[0]), x_len );
    m_writeStmt->setDataBuffer(4, (dvoid*)yy, OCCIFLOAT , sizeof(yy[0]), y_len );
    m_writeStmt->setDataBuffer(5, (dvoid*)zz, OCCIFLOAT , sizeof(zz[0]), z_len );
    m_writeStmt->setDataBuffer(6, (dvoid*)ww, OCCIFLOAT , sizeof(ww[0]), w_len );
    m_writeStmt->setDataBuffer(7, (dvoid*)uu, OCCIFLOAT , sizeof(uu[0]), u_len );
    m_writeStmt->setDataBuffer(8, (dvoid*)tt, OCCIFLOAT , sizeof(tt[0]), t_len );


    m_writeStmt->executeArrayUpdate(nrows);

    delete [] ids;
    delete [] iovid_vec;
    delete [] xx;
    delete [] yy;
    delete [] zz;
    delete [] ww;
    delete [] uu;
    delete [] tt;

    delete [] ids_len;
    delete [] iov_len;
    delete [] x_len;
    delete [] y_len;
    delete [] z_len;
    delete [] w_len;
    delete [] u_len;
    delete [] t_len;



  } catch (SQLException &e) {
    throw(runtime_error("LMFLaserBlueNormDat::writeArrayDB():  "+e.getMessage()));
  }
}

void LMFLaserBlueNormDat::fetchData(std::map< EcalLogicID, LMFLaserBlueNormDat >* fillMap, LMFRunIOV* iov)
  throw(runtime_error)
{
  this->checkConnection();
  fillMap->clear();

  iov->setConnection(m_env, m_conn);
  int iovID = iov->fetchID();
  if (!iovID) { 
    //  throw(runtime_error("LMFLaserBlueNormDat::writeDB:  IOV not in DB")); 
    return;
  }

  try {
  
    m_readStmt->setSQL("SELECT cv.name, cv.logic_id, cv.id1, cv.id2, cv.id3, cv.maps_to, "
		 "d.apd_over_pnA_mean, d.apd_over_pnA_rms, d.apd_over_pnB_mean, d.apd_over_pnB_rms, d.apd_over_pn_mean, d.apd_over_pn_rms "
		 "FROM channelview cv JOIN lmf_laser_blue_norm_dat d "
		 "ON cv.logic_id = d.logic_id AND cv.name = cv.maps_to "
		 "WHERE d.iov_id = :iov_id");
 
    m_readStmt->setInt(1, iovID);
    
    ResultSet* rset = m_readStmt->executeQuery();
     
    std::pair< EcalLogicID, LMFLaserBlueNormDat > p;
    LMFLaserBlueNormDat dat;
    while(rset->next()) {
      p.first = EcalLogicID( rset->getString(1),     // name
			     rset->getInt(2),        // logic_id
			     rset->getInt(3),        // id1
			     rset->getInt(4),        // id2
			     rset->getInt(5),        // id3
			     rset->getString(6));    // maps_to

      dat.setAPDOverPNAMean( rset->getFloat(7) );
      dat.setAPDOverPNARMS( rset->getFloat(8) );
      dat.setAPDOverPNBMean( rset->getFloat(9) );
      dat.setAPDOverPNBRMS( rset->getFloat(10) );
      dat.setAPDOverPNMean( rset->getFloat(11) );
      dat.setAPDOverPNRMS( rset->getFloat(12) );


      p.second = dat;
      fillMap->insert(p);
    }

  } catch (SQLException &e) {
    throw(runtime_error("LMFLaserBlueNormDat::fetchData():  "+e.getMessage()));
  }
}
