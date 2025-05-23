/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkGDCMImageIO.h"
#include "itkMetaDataObject.h"
#include "itkRandomImageSource.h"
#include <string>
#include <sstream>
#include <vector>
#include "itkMath.h"
#include "itkTestingMacros.h"

int
itkGDCMImageOrientationPatientTest(int argc, char * argv[])
{

  if (argc < 2)
  {
    std::cerr << "Usage: " << itkNameOfTestExecutableMacro(argv) << " OutputTestDirectory" << std::endl;
    return EXIT_FAILURE;
  }

  using Image3DType = itk::Image<short, 3>;
  using Image2DType = itk::Image<short, 2>;
  using ReaderType = itk::ImageFileReader<Image3DType>;
  using RandomImageSource2DType = itk::RandomImageSource<Image2DType>;
  using Writer2DType = itk::ImageFileWriter<Image2DType>;
  using ImageIOType = itk::GDCMImageIO;

  using DictionaryType = itk::MetaDataDictionary;
  const DictionaryType dict;

  // Create a 2D image
  Image2DType::SpacingType spacing2D;
  spacing2D[0] = 10.0;
  spacing2D[1] = 100.0;

  auto size2D = Image2DType::SizeType::Filled(16);

  auto src2D = RandomImageSource2DType::New();
  src2D->SetMin(0);
  src2D->SetMax(255);
  src2D->SetSpacing(spacing2D);
  src2D->SetSize(size2D);

  auto           gdcmIO = ImageIOType::New();
  DictionaryType dictionary;

  // Set all required DICOM fields
  std::ostringstream value;

  double origin3D[3];
  origin3D[0] = 1.0;
  origin3D[1] = 2.0;
  origin3D[2] = 3.0;
  value.str("");
  value << origin3D[0] << '\\' << origin3D[1] << '\\' << origin3D[2];
  itk::EncapsulateMetaData<std::string>(dictionary, "0020|0032", value.str());

  Image3DType::DirectionType direction3D;
  direction3D[0][0] = 0.5;
  direction3D[1][0] = 0.2;
  direction3D[2][0] = 0.2;
  direction3D[0][1] = 0.5;
  direction3D[1][1] = 0.4;
  direction3D[2][1] = 0.3;
  direction3D[0][2] = 0.6;
  direction3D[1][2] = 0.6;
  direction3D[2][2] = 0.6;
  value.str("");
  value << direction3D[0][0] << '\\' << direction3D[1][0] << '\\' << direction3D[2][0] << '\\' << direction3D[0][1]
        << '\\' << direction3D[1][1] << '\\' << direction3D[2][1];
  itk::EncapsulateMetaData<std::string>(dictionary, "0020|0037", value.str());

  // GDCM will not write IPP unless the modality is one of CT, MR or RT.
  const std::string modality("MR");
  itk::EncapsulateMetaData<std::string>(dictionary, "0008|0060", modality);

  src2D->GetOutput()->SetMetaDataDictionary(dictionary);

  auto               writer2D = Writer2DType::New();
  std::ostringstream filename;
  filename.str("");
  filename << argv[1] << "/itkGDCMImageOrientationPatientTest.dcm";
  writer2D->SetInput(src2D->GetOutput());
  writer2D->SetFileName(filename.str().c_str());

  // Cause intentional error, non-orthogonal direction cosine,
  // write should fail.
  writer2D->SetImageIO(gdcmIO);
  ITK_TRY_EXPECT_EXCEPTION(writer2D->Update());

  // Now write using valid direction cosine
  direction3D[0][0] = .6;
  direction3D[1][0] = .0;
  direction3D[2][0] = .8;
  direction3D[0][1] = -.8;
  direction3D[1][1] = .0;
  direction3D[2][1] = .6;
  direction3D[0][2] = 0;
  direction3D[1][2] = 1;
  direction3D[2][2] = 0;
  value.str("");
  value << direction3D[0][0] << '\\' << direction3D[1][0] << '\\' << direction3D[2][0] << '\\' << direction3D[0][1]
        << '\\' << direction3D[1][1] << '\\' << direction3D[2][1];
  itk::EncapsulateMetaData<std::string>(dictionary, "0020|0037", value.str());
  src2D->GetOutput()->SetMetaDataDictionary(dictionary);
  ITK_TRY_EXPECT_NO_EXCEPTION(writer2D->Update());

  // Now read the dicom back and check its origin
  auto reader = ReaderType::New();
  reader->SetFileName(filename.str().c_str());
  reader->Update();

  Image3DType::DirectionType readerDirection3D;
  readerDirection3D = reader->GetOutput()->GetDirection();
  if ((itk::Math::NotExactlyEquals(readerDirection3D[0][0], direction3D[0][0])) ||
      (itk::Math::NotExactlyEquals(readerDirection3D[1][0], direction3D[1][0])) ||
      (itk::Math::NotExactlyEquals(readerDirection3D[2][0], direction3D[2][0])) ||
      (itk::Math::NotExactlyEquals(readerDirection3D[0][1], direction3D[0][1])) ||
      (itk::Math::NotExactlyEquals(readerDirection3D[1][1], direction3D[1][1])) ||
      (itk::Math::NotExactlyEquals(readerDirection3D[2][1], direction3D[2][1])))
  {
    std::cout << "ERROR: read directions does not equal written directions: " << readerDirection3D
              << " != " << direction3D;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
