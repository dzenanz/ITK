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

#include <iostream>
#include "itkShrinkImageFilter.h"
#include "itkStreamingImageFilter.h"
#include "itkImageRegionSplitterMultidimensional.h"
#include "itkXMLFileOutputWindow.h"
#include "itkPipelineMonitorImageFilter.h"


int
itkStreamingImageFilterTest2(int, char *[])
{

  constexpr unsigned int                  numberOfStreamDivisions = 25;
  const itk::XMLFileOutputWindow::Pointer logger = itk::XMLFileOutputWindow::New();
  logger->SetInstance(logger);

  // type alias to simplify the syntax
  using ShortImage = itk::Image<short, 2>;

  // Test the creation of an image with native type
  auto if2 = ShortImage::New();

  // fill in an image
  constexpr ShortImage::IndexType index = { { 0, 0 } };
  constexpr ShortImage::SizeType  size = { { 42, 64 } };
  const ShortImage::RegionType    region{ index, size };
  if2->SetLargestPossibleRegion(region);
  if2->SetBufferedRegion(region);
  if2->Allocate();

  itk::ImageRegionIterator<ShortImage> iterator(if2, region);

  short i = 0;
  for (; !iterator.IsAtEnd(); ++iterator, ++i)
  {
    const short scalar = i;
    iterator.Set(scalar);
  }

  // Create a filter
  const itk::ShrinkImageFilter<ShortImage, ShortImage>::Pointer shrink =
    itk::ShrinkImageFilter<ShortImage, ShortImage>::New();
  shrink->SetInput(if2);

  unsigned int factors[2] = { 2, 3 };
  shrink->SetShrinkFactors(factors);
  shrink->DebugOn();

  const itk::ImageRegionSplitterMultidimensional::Pointer splitter = itk::ImageRegionSplitterMultidimensional::New();
  splitter->DebugOn();

  // monitor what's going on
  const itk::PipelineMonitorImageFilter<ShortImage>::Pointer monitor =
    itk::PipelineMonitorImageFilter<ShortImage>::New();
  monitor->SetInput(shrink->GetOutput());

  const itk::StreamingImageFilter<ShortImage, ShortImage>::Pointer streamer =
    itk::StreamingImageFilter<ShortImage, ShortImage>::New();
  streamer->SetInput(monitor->GetOutput());
  streamer->SetNumberOfStreamDivisions(numberOfStreamDivisions);
  streamer->SetRegionSplitter(splitter);
  streamer->Update();

  std::cout << "Input spacing: " << if2->GetSpacing()[0] << ", " << if2->GetSpacing()[1] << std::endl;
  std::cout << "Output spacing: " << streamer->GetOutput()->GetSpacing()[0] << ", "
            << streamer->GetOutput()->GetSpacing()[1] << std::endl;


  // check if the pipeline executed as expected
  if (monitor->GetNumberOfUpdates() != numberOfStreamDivisions ||
      monitor->GetOutputRequestedRegions().size() != numberOfStreamDivisions)
  {
    std::cout << monitor;
    std::cout << "ImageStreaming Filter test failed because pipeline didn't execute as expected." << std::endl;
    return EXIT_FAILURE;
  }


  //
  // The rest of this code determines whether the shrink code produced
  // the image we expected.
  //
  const ShortImage::RegionType requestedRegion = streamer->GetOutput()->GetRequestedRegion();

  itk::ImageRegionIterator<ShortImage> iterator2(streamer->GetOutput(), requestedRegion);

  // If size is not a multiple of the shrink factors, then adjust the
  // row/col indices
  short rowOffset = 0;
  short colOffset = 0;
  if (region.GetSize()[1] % shrink->GetShrinkFactors()[1])
  {
    rowOffset = static_cast<short>(region.GetSize()[1] / 2.0 - ((region.GetSize()[1] / shrink->GetShrinkFactors()[1]) /
                                                                2.0 * shrink->GetShrinkFactors()[1]));
  }
  if (region.GetSize()[0] % shrink->GetShrinkFactors()[0])
  {
    colOffset = static_cast<short>(region.GetSize()[0] / 2.0 - ((region.GetSize()[0] / shrink->GetShrinkFactors()[0]) /
                                                                2.0 * shrink->GetShrinkFactors()[0]));
  }

  bool passed = true;
  for (; !iterator2.IsAtEnd(); ++iterator2)
  {
    auto col = itk::Math::RoundHalfIntegerUp<short>(static_cast<float>(
      shrink->GetShrinkFactors()[0] * iterator2.GetIndex()[0] + (shrink->GetShrinkFactors()[0]) / 2.0));
    col += colOffset;

    auto row = itk::Math::RoundHalfIntegerUp<short>(static_cast<float>(
      shrink->GetShrinkFactors()[1] * iterator2.GetIndex()[1] + (shrink->GetShrinkFactors()[1]) / 2.0));
    row += rowOffset;
    const short trueValue = col + region.GetSize()[0] * row;

    if (iterator2.Get() != trueValue)
    {
      passed = false;
      std::cout << "Pixel " << iterator2.GetIndex() << " expected " << trueValue << " but got " << iterator2.Get()
                << std::endl;
    }
  }

  if (passed)
  {
    std::cout << "ImageStreamingFilter test passed." << std::endl;
    try
    {
      return EXIT_SUCCESS;
    }
    catch (...)
    {
      std::cout << "Caught an exception on exit" << std::endl;
      return EXIT_FAILURE;
    }
  }
  else
  {
    std::cout << "ImageStreaming Filter test failed." << std::endl;
    return EXIT_FAILURE;
  }
}
