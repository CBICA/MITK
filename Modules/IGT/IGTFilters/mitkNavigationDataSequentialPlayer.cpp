/*=========================================================================

Program:   Medical Imaging & Interaction Toolkit
Language:  C++
Date:      $Date: 2009-02-10 18:08:54 +0100 (Di, 10 Feb 2009) $
Version:   $Revision: 16228 $

Copyright (c) German Cancer Research Center, Division of Medical and
Biological Informatics. All rights reserved.
See MITKCopyright.txt or http://www.mitk.org/copyright.html for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "mitkNavigationDataSequentialPlayer.h"

//for the pause
#include <itksys/SystemTools.hxx>

#include <mitkTimeStamp.h>
#include <fstream>
#include <sstream>

mitk::NavigationDataSequentialPlayer::NavigationDataSequentialPlayer()
  : m_Doc(new TiXmlDocument)
  , m_DataElem(0)
  , m_CurrentElem(0)
  , m_Repeat(false)
  , m_NumberOfSnapshots(0)
  , m_LastGoTo(0)
{
}


mitk::NavigationDataSequentialPlayer::~NavigationDataSequentialPlayer()
{
  delete m_Doc;
}

void mitk::NavigationDataSequentialPlayer::ReinitXML()
{
  m_DataElem = m_Doc->FirstChildElement("Data");
  int toolcount;
  if(!m_DataElem)
    MITK_WARN << "Data element not found";
  else
  {
    m_DataElem->QueryIntAttribute("ToolCount", &toolcount);
    this->SetNumberOfOutputs(toolcount);

    mitk::NavigationData::Pointer emptyNd = mitk::NavigationData::New();
    mitk::NavigationData::PositionType position;
    mitk::NavigationData::OrientationType orientation(0.0,0.0,0.0,0.0);
    position.Fill(0.0);

    emptyNd->SetPosition(position);
    emptyNd->SetOrientation(orientation);
    emptyNd->SetDataValid(false);

    mitk::NavigationData::Pointer tmp;
    for (unsigned int index = 0; index < this->GetNumberOfOutputs(); index++)
    {
      tmp = mitk::NavigationData::New();
      tmp->Graft(emptyNd);
      this->SetNthOutput(index, tmp);
    }

    // find out _NumberOfSnapshots
    m_NumberOfSnapshots = 0;
    TiXmlElement* nextND = m_DataElem->FirstChildElement("ND");
    while(nextND)
    {
      ++m_NumberOfSnapshots;
      nextND = nextND->NextSiblingElement("ND");
    }
    // e.g. 12 nd found and 2 tools used => number of snapshots is 12:2=6
    m_NumberOfSnapshots = m_NumberOfSnapshots/toolcount;

  }
}

void mitk::NavigationDataSequentialPlayer::GoToSnapshot(int i)
{
  assert(m_DataElem);

  int numOfUpdateCalls = 0;

  // i.e. number of snapshots 10
  // goto(7), m_LastGoTo=3 => numOfUpdateCalls = 4
  if(m_LastGoTo <= i)
    numOfUpdateCalls = i - m_LastGoTo;
  // goto(4), m_LastGoTo=7 => numOfUpdateCalls = 7
  else
  {
    if(!m_Repeat)
    {
      MITK_WARN << "cannot go back to snapshot " << i << " because the "
          << this->GetNameOfClass() << " is configured to not repeat the"
          << " navigation data";

    }
    else
    {
      numOfUpdateCalls = (m_NumberOfSnapshots - m_LastGoTo) + i;
    }
  }

  for(int j=0; j<numOfUpdateCalls; ++j)
    this->Update();

  m_LastGoTo = i;
}

void mitk::NavigationDataSequentialPlayer::
    SetFileName(const std::string& _FileName)
{
  m_FileName = _FileName;

  if(!m_Doc->LoadFile(m_FileName))
  {
    this->SetNumberOfOutputs(0);
    std::ostringstream s;
    s << "File " << _FileName << " could not be loaded";
    throw std::invalid_argument(s.str());
  }
  else
    this->ReinitXML();

  this->Modified();
}

void mitk::NavigationDataSequentialPlayer::
    SetXMLString(const std::string& _XMLString)
{
  m_XMLString = _XMLString;

  m_Doc->Parse( m_XMLString.c_str() );
  this->ReinitXML();

  this->Modified();
}

void mitk::NavigationDataSequentialPlayer::GenerateData()
{
  assert(m_DataElem);

  // very important: go through the tools (there could be more then one)
  mitk::NavigationData::Pointer tmp;
  for (unsigned int index = 0; index < this->GetNumberOfOutputs(); index++)
  {
    // go to the first element
    if(!m_CurrentElem)
      m_CurrentElem = m_DataElem->FirstChildElement("ND");
    // go to the next element
    else
      m_CurrentElem = m_CurrentElem->NextSiblingElement();

    // if repeat is on: go back to the first element (prior calls delivered NULL
    // elem)
    if(!m_CurrentElem && m_Repeat)
      m_CurrentElem = m_DataElem->FirstChildElement("ND");

    mitk::NavigationData* output = this->GetOutput(index);
    tmp = this->ReadVersion1();
    if(tmp.IsNotNull())
      output->Graft(tmp);
    else // no valid output
      output->SetDataValid(false);
  }
}

mitk::NavigationData::Pointer mitk::NavigationDataSequentialPlayer::ReadVersion1()
{
  mitk::NavigationData::Pointer nd = mitk::NavigationData::New();
  mitk::NavigationData::PositionType position;
  mitk::NavigationData::OrientationType orientation(0.0,0.0,0.0,0.0);
  mitk::NavigationData::TimeStampType timestamp = -1;
  mitk::NavigationData::CovarianceMatrixType matrix;

  bool hasPosition = true;
  bool hasOrientation = true;
  bool dataValid = false;

  position.Fill(0.0);
  matrix.SetIdentity();

  TiXmlElement* elem = m_CurrentElem;

  if(!elem)
    return NULL;

  //check here if EOF (the query don't change the timestamp value which should always be > 0)
  elem->QueryDoubleAttribute("Time",&timestamp);
  if (timestamp == -1)
  {
    return NULL;  //the calling method should check the return value if it is valid/not NULL
  }

  elem->QueryFloatAttribute("X", &position[0]);
  elem->QueryFloatAttribute("Y", &position[1]);
  elem->QueryFloatAttribute("Z", &position[2]);

  elem->QueryFloatAttribute("QX", &orientation[0]);
  elem->QueryFloatAttribute("QY", &orientation[1]);
  elem->QueryFloatAttribute("QZ", &orientation[2]);
  elem->QueryFloatAttribute("QR", &orientation[3]);

  elem->QueryFloatAttribute("C00", &matrix[0][0]);
  elem->QueryFloatAttribute("C01", &matrix[0][1]);
  elem->QueryFloatAttribute("C02", &matrix[0][2]);
  elem->QueryFloatAttribute("C03", &matrix[0][3]);
  elem->QueryFloatAttribute("C04", &matrix[0][4]);
  elem->QueryFloatAttribute("C05", &matrix[0][5]);
  elem->QueryFloatAttribute("C10", &matrix[1][0]);
  elem->QueryFloatAttribute("C11", &matrix[1][1]);
  elem->QueryFloatAttribute("C12", &matrix[1][2]);
  elem->QueryFloatAttribute("C13", &matrix[1][3]);
  elem->QueryFloatAttribute("C14", &matrix[1][4]);
  elem->QueryFloatAttribute("C15", &matrix[1][5]);

  int tmpval = 0;
  elem->QueryIntAttribute("Valid", &tmpval);
  if (tmpval == 0)
    dataValid = false;
  else
    dataValid = true;

  tmpval = 0;
  elem->QueryIntAttribute("hO", &tmpval);
  if (tmpval == 0)
    hasOrientation = false;
  else
    hasOrientation = true;

  tmpval = 0;
  elem->QueryIntAttribute("hP", &tmpval);
  if (tmpval == 0)
    hasPosition = false;
  else
    hasPosition = true;

  nd->SetTimeStamp(timestamp);
  nd->SetPosition(position);
  nd->SetOrientation(orientation);
  nd->SetCovErrorMatrix(matrix);
  nd->SetDataValid(dataValid);
  nd->SetHasOrientation(hasOrientation);
  nd->SetHasPosition(hasPosition);

  //delete elem;
  return nd;
}

void mitk::NavigationDataSequentialPlayer::UpdateOutputInformation()
{
  this->Modified();  // make sure that we need to be updated
  Superclass::UpdateOutputInformation();
}
