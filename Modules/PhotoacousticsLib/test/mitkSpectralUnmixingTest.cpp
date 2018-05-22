///*===================================================================

//The Medical Imaging Interaction Toolkit (MITK)

//Copyright (c) German Cancer Research Center,
//Division of Medical and Biological Informatics.
//All rights reserved.

//This software is distributed WITHOUT ANY WARRANTY; without
//even the implied warranty of MERCHANTABILITY or FITNESS FOR
//A PARTICULAR PURPOSE.

//See LICENSE.txt or http://www.mitk.org for details.

//===================================================================*/
#include <mitkTestFixture.h>
#include <mitkTestingMacros.h>

#include <mitkPASpectralUnmixingFilterBase.h>
#include <mitkPALinearSpectralUnmixingFilter.h>
#include <mitkImageReadAccessor.h>

class mitkSpectralUnmixingTestSuite : public mitk::TestFixture
{
  CPPUNIT_TEST_SUITE(mitkSpectralUnmixingTestSuite);
  MITK_TEST(testSUAlgorithm);
  CPPUNIT_TEST_SUITE_END();

private:
  mitk::pa::SpectralUnmixingFilterBase::Pointer m_SpectralUnmixingFilter;
  mitk::Image::Pointer inputImage;
  std::vector<float> m_inputWavelengths;
  std::vector<float> m_CorrectResult;
  float threshold;

public:

  void setUp() override
  {
    MITK_INFO << "setUp ... ";
    //Set empty input image:
    inputImage = mitk::Image::New();
    mitk::PixelType pixelType = mitk::MakeScalarPixelType<float>();
    const int NUMBER_OF_SPATIAL_DIMENSIONS = 3;
    auto* dimensions = new unsigned int[NUMBER_OF_SPATIAL_DIMENSIONS];

    dimensions[0] = 1;
    dimensions[1] = 1;
    dimensions[2] = 3;

    inputImage->Initialize(pixelType, NUMBER_OF_SPATIAL_DIMENSIONS, dimensions);

    //Set wavelengths for unmixing:
    m_inputWavelengths.push_back(750);
    m_inputWavelengths.push_back(800);
    m_inputWavelengths.push_back(850);

    //Set fraction of Hb and HbO2 to unmix:
    float fracHb = 0.396579;
    float fracHbO2 = 0.594845;
    m_CorrectResult.push_back(fracHbO2);
    m_CorrectResult.push_back(fracHb);

    //Set threshold 1 digit
    threshold = 0.000001;

    //Multiply values of wavelengths (750,800,850 nm) with fractions to get pixel values:
    float px1 = fracHb * 7.52 + fracHbO2 * 2.77;
    float px2 = fracHb * 4.08 + fracHbO2 * 4.37;
    float px3 = fracHb * 3.7 + fracHbO2 * 5.67;
    std::vector<float> m_Value{px1,px2,px3};

    float* data = new float[3];
    data[0] = m_Value[0];
    data[1] = m_Value[1];
    data[2] = m_Value[2];

    inputImage->SetImportVolume(data, mitk::Image::ImportMemoryManagementType::CopyMemory);
    delete[] data;
    MITK_INFO << "[DONE]";
  }

  void testSUAlgorithm()
  {
    MITK_INFO << "START FILTER TEST ... ";
    // Set input image
    auto m_SpectralUnmixingFilter = mitk::pa::LinearSpectralUnmixingFilter::New();
    m_SpectralUnmixingFilter->SetInput(inputImage);

    // Set Algortihm to filter
    //m_SpectralUnmixingFilter->SetAlgorithm(mitk::pa::LinearSpectralUnmixingFilter::AlgortihmType::householderQr);
    //m_SpectralUnmixingFilter->SetAlgorithm(mitk::pa::LinearSpectralUnmixingFilter::AlgortihmType::ldlt);
    //m_SpectralUnmixingFilter->SetAlgorithm(mitk::pa::LinearSpectralUnmixingFilter::AlgortihmType::llt);
    m_SpectralUnmixingFilter->SetAlgorithm(mitk::pa::LinearSpectralUnmixingFilter::AlgortihmType::colPivHouseholderQr);
    //m_SpectralUnmixingFilter->SetAlgorithm(mitk::pa::LinearSpectralUnmixingFilter::AlgortihmType::jacobiSvd);
    //m_SpectralUnmixingFilter->SetAlgorithm(mitk::pa::LinearSpectralUnmixingFilter::AlgortihmType::fullPivLu);
    //m_SpectralUnmixingFilter->SetAlgorithm(mitk::pa::LinearSpectralUnmixingFilter::AlgortihmType::fullPivHouseholderQr);
    //m_SpectralUnmixingFilter->SetAlgorithm(mitk::pa::LinearSpectralUnmixingFilter::AlgortihmType::test);

    //Set wavelengths to filter
    for (unsigned int imageIndex = 0; imageIndex < m_inputWavelengths.size(); imageIndex++)
    {
      unsigned int wavelength = m_inputWavelengths[imageIndex];
      m_SpectralUnmixingFilter->AddWavelength(wavelength);
    }

    //Set Chromophores to filter
    m_SpectralUnmixingFilter->AddChromophore(
      mitk::pa::SpectralUnmixingFilterBase::ChromophoreType::OXYGENATED_HEMOGLOBIN);
    m_SpectralUnmixingFilter->AddChromophore(
      mitk::pa::SpectralUnmixingFilterBase::ChromophoreType::DEOXYGENATED_HEMOGLOBIN);

    m_SpectralUnmixingFilter->Update();

    for (int i = 0; i < 2; ++i)
    {
      mitk::Image::Pointer output = m_SpectralUnmixingFilter->GetOutput(i);
      mitk::ImageReadAccessor readAccess(output);
      const float* inputDataArray = ((const float*)readAccess.GetData());
      auto pixel = inputDataArray[0];
      /*For pixel values and results look at: [...]mitk-superbuild\MITK-build\Modules\PhotoacousticsLib\test\*/
      CPPUNIT_ASSERT(std::abs(pixel - m_CorrectResult[i])<threshold);
    }
    MITK_INFO << "FILTER TEST SUCCESFULL :)";
  }

  void tearDown() override
  {
    m_SpectralUnmixingFilter = nullptr;
    inputImage = nullptr;
    m_inputWavelengths.clear();
    m_CorrectResult.clear();
    MITK_INFO << "tearDown ... [DONE]";
  }
};

MITK_TEST_SUITE_REGISTRATION(mitkSpectralUnmixing)
