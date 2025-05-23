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
#ifndef itkConnectedThresholdImageFilter_h
#define itkConnectedThresholdImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkSimpleDataObjectDecorator.h"
#include "ITKRegionGrowingExport.h"

namespace itk
{
/** \class ConnectedThresholdImageFilterEnums
 * \brief Contains all the enum classes used by the ConnectedThresholdImageFilter class.
 * \ingroup RegionGrowingSegmentation
 * \ingroup ITKRegionGrowing
 */
class ConnectedThresholdImageFilterEnums
{
public:
  /**
   * \class Connectivity
   * \ingroup RegionGrowingSegmentation
   * \ingroup ITKRegionGrowing
   * Face connectivity is 4 connected in 2D, 6  connected in 3D, 2*n   in ND
   * Full connectivity is 8 connected in 2D, 26 connected in 3D, 3^n-1 in ND
   * Default is to use FaceConnectivity.
   */
  enum class Connectivity : uint8_t
  {
    FaceConnectivity,
    FullConnectivity
  };
};

// Define how to print enumeration
extern ITKRegionGrowing_EXPORT std::ostream &
operator<<(std::ostream & out, const ConnectedThresholdImageFilterEnums::Connectivity value);
/**
 * \class ConnectedThresholdImageFilter
 * \brief Label pixels that are connected to a seed and lie within a range of values
 *
 * ConnectedThresholdImageFilter labels pixels with ReplaceValue that are
 * connected to an initial Seed AND lie within a Lower and Upper
 * threshold range.
 *
 * \ingroup RegionGrowingSegmentation
 * \ingroup ITKRegionGrowing
 * \sphinx
 * \sphinxexample{Segmentation/RegionGrowing/ConnectedComponentsInImage,Connected Components In Image}
 * \endsphinx
 */
template <typename TInputImage, typename TOutputImage>
class ITK_TEMPLATE_EXPORT ConnectedThresholdImageFilter : public ImageToImageFilter<TInputImage, TOutputImage>
{
public:
  ITK_DISALLOW_COPY_AND_MOVE(ConnectedThresholdImageFilter);

  /** Standard class type aliases. */
  using Self = ConnectedThresholdImageFilter;
  using Superclass = ImageToImageFilter<TInputImage, TOutputImage>;
  using Pointer = SmartPointer<Self>;
  using ConstPointer = SmartPointer<const Self>;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** \see LightObject::GetNameOfClass() */
  itkOverrideGetNameOfClassMacro(ConnectedThresholdImageFilter);

  using InputImageType = TInputImage;
  using InputImagePointer = typename InputImageType::Pointer;
  using InputImageConstPointer = typename InputImageType::ConstPointer;
  using InputImageRegionType = typename InputImageType::RegionType;
  using InputImagePixelType = typename InputImageType::PixelType;
  using IndexType = typename InputImageType::IndexType;
  using SeedContainerType = typename std::vector<IndexType>;
  using SizeType = typename InputImageType::SizeType;

  using OutputImageType = TOutputImage;
  using OutputImagePointer = typename OutputImageType::Pointer;
  using OutputImageRegionType = typename OutputImageType::RegionType;
  using OutputImagePixelType = typename OutputImageType::PixelType;

  void
  PrintSelf(std::ostream & os, Indent indent) const override;

  /** Set seed point. */
  void
  SetSeed(const IndexType & seed);

  void
  AddSeed(const IndexType & seed);

  /** Clear the seed list. */
  void
  ClearSeeds();

  /** Method to access seed container. */
  virtual const SeedContainerType &
  GetSeeds() const;

  /** Set/Get value to replace thresholded pixels. Pixels that lie *
   *  within Lower and Upper (inclusive) will be replaced with this
   *  value. The default is 1. */
  /** @ITKStartGrouping */
  itkSetMacro(ReplaceValue, OutputImagePixelType);
  itkGetConstMacro(ReplaceValue, OutputImagePixelType);
  /** @ITKEndGrouping */
  /** Type of DataObjects to use for scalar inputs */
  using InputPixelObjectType = SimpleDataObjectDecorator<InputImagePixelType>;

  /** Set Upper and Lower Threshold inputs as values */
  /** @ITKStartGrouping */
  virtual void SetUpper(InputImagePixelType);
  virtual void SetLower(InputImagePixelType);
  /** @ITKEndGrouping */
  /** Set Threshold inputs that are connected to the pipeline */
  /** @ITKStartGrouping */
  virtual void
  SetUpperInput(const InputPixelObjectType *);
  virtual void
  SetLowerInput(const InputPixelObjectType *);
  /** @ITKEndGrouping */
  /** Get Upper and Lower Threshold inputs as values. */
  /** @ITKStartGrouping */
  virtual InputImagePixelType
  GetUpper() const;
  virtual InputImagePixelType
  GetLower() const;
  /** @ITKEndGrouping */
  /** Get Threshold inputs that are connected to the pipeline. */
  /** @ITKStartGrouping */
  virtual InputPixelObjectType *
  GetUpperInput();
  virtual InputPixelObjectType *
  GetLowerInput();
  /** @ITKEndGrouping */
  /** Image dimension constants. */
  static constexpr unsigned int InputImageDimension = TInputImage::ImageDimension;
  static constexpr unsigned int OutputImageDimension = TOutputImage::ImageDimension;

  itkConceptMacro(OutputEqualityComparableCheck, (Concept::EqualityComparable<OutputImagePixelType>));
  itkConceptMacro(InputEqualityComparableCheck, (Concept::EqualityComparable<InputImagePixelType>));
  itkConceptMacro(InputConvertibleToOutputCheck, (Concept::Convertible<InputImagePixelType, OutputImagePixelType>));
  itkConceptMacro(SameDimensionCheck, (Concept::SameDimension<InputImageDimension, OutputImageDimension>));
  itkConceptMacro(IntConvertibleToInputCheck, (Concept::Convertible<int, InputImagePixelType>));
  itkConceptMacro(OutputOStreamWritableCheck, (Concept::OStreamWritable<OutputImagePixelType>));

  using ConnectivityEnum = ConnectedThresholdImageFilterEnums::Connectivity;
#if !defined(ITK_LEGACY_REMOVE)
  using ConnectivityEnumType = ConnectedThresholdImageFilterEnums::Connectivity;
  /**Exposes enums values for backwards compatibility*/
  static constexpr ConnectedThresholdImageFilterEnums::Connectivity FaceConnectivity =
    ConnectedThresholdImageFilterEnums::Connectivity::FaceConnectivity;
  static constexpr ConnectedThresholdImageFilterEnums::Connectivity FullConnectivity =
    ConnectedThresholdImageFilterEnums::Connectivity::FullConnectivity;
#endif

  /** Type of connectivity to use (fully connected OR 4(2D), 6(3D),
   * 2*N(ND) connectivity). */
  /** @ITKStartGrouping */
  itkSetEnumMacro(Connectivity, ConnectedThresholdImageFilterEnums::Connectivity);
  itkGetEnumMacro(Connectivity, ConnectedThresholdImageFilterEnums::Connectivity);
  /** @ITKEndGrouping */
protected:
  ConnectedThresholdImageFilter();
  ~ConnectedThresholdImageFilter() override = default;

  // Override since the filter needs all the data for the algorithm.
  void
  GenerateInputRequestedRegion() override;

  // Override since the filter produces the entire dataset.
  void
  EnlargeOutputRequestedRegion(DataObject * output) override;

  void
  GenerateData() override;

private:
  SeedContainerType m_Seeds{};

  OutputImagePixelType m_ReplaceValue{};

  ConnectedThresholdImageFilterEnums::Connectivity m_Connectivity{ ConnectivityEnum::FaceConnectivity };
};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#  include "itkConnectedThresholdImageFilter.hxx"
#endif

#endif
