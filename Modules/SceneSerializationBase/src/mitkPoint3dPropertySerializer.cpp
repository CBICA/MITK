/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#ifndef mitkPoint3dPropertySerializer_h_included
#define mitkPoint3dPropertySerializer_h_included

#include "mitkBasePropertySerializer.h"
#include "mitkProperties.h"
#include "mitkStringsToNumbers.h"
#include <mitkLocaleSwitch.h>

namespace mitk
{
  class Point3dPropertySerializer : public BasePropertySerializer
  {
  public:
    mitkClassMacro(Point3dPropertySerializer, BasePropertySerializer);
    itkFactorylessNewMacro(Self) itkCloneMacro(Self)

      TiXmlElement *Serialize() override
    {
      if (const Point3dProperty *prop = dynamic_cast<const Point3dProperty *>(m_Property.GetPointer()))
      {
        LocaleSwitch localeSwitch("C");

        auto element = new TiXmlElement("point");
        Point3D point = prop->GetValue();
        element->SetAttribute("x", boost::lexical_cast<std::string>(point[0]));
        element->SetAttribute("y", boost::lexical_cast<std::string>(point[1]));
        element->SetAttribute("z", boost::lexical_cast<std::string>(point[2]));
        return element;
      }
      else
        return nullptr;
    }

    BaseProperty::Pointer Deserialize(TiXmlElement *element) override
    {
      if (!element)
        return nullptr;

      LocaleSwitch localeSwitch("C");

      std::string v_str[3];
      if (element->QueryStringAttribute("x", &v_str[0]) != TIXML_SUCCESS)
        return nullptr;
      if (element->QueryStringAttribute("y", &v_str[1]) != TIXML_SUCCESS)
        return nullptr;
      if (element->QueryStringAttribute("z", &v_str[2]) != TIXML_SUCCESS)
        return nullptr;
      Point3D v;
      try
      {
        StringsToNumbers<double>(3, v_str, v);
      }
      catch (boost::bad_lexical_cast &e)
      {
        MITK_ERROR << "Could not parse string as number: " << e.what();
        return nullptr;
      }
      return Point3dProperty::New(v).GetPointer();
    }

  protected:
    Point3dPropertySerializer() {}
    ~Point3dPropertySerializer() override {}
  };

} // namespace

// important to put this into the GLOBAL namespace (because it starts with 'namespace mitk')
MITK_REGISTER_SERIALIZER(Point3dPropertySerializer);

#endif
