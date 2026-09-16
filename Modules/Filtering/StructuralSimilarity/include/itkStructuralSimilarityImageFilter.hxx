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
#ifndef itkStructuralSimilarityImageFilter_hxx
#define itkStructuralSimilarityImageFilter_hxx

#include "itkBinShrinkImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkDiscreteGaussianImageFilter.h"
#include "itkImage.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkMath.h"
#include "itkMultiplyImageFilter.h"
#include "itkProgressReporter.h"
#include "itkTotalProgressReporter.h"

#include <algorithm>
#include <atomic>
#include <cmath>

namespace itk
{
template <typename TInputImage, typename TOutputImage>
StructuralSimilarityImageFilter<TInputImage, TOutputImage>::StructuralSimilarityImageFilter()
{
  this->SetNumberOfRequiredInputs(2);

  m_ScaleWeights.SetSize(5);
  m_ScaleWeights[0] = 0.0448;
  m_ScaleWeights[1] = 0.2856;
  m_ScaleWeights[2] = 0.3001;
  m_ScaleWeights[3] = 0.2363;
  m_ScaleWeights[4] = 0.1333;
}

template <typename TInputImage, typename TOutputImage>
void
StructuralSimilarityImageFilter<TInputImage, TOutputImage>::SetInput2(const InputImageType * image)
{
  this->SetNthInput(1, const_cast<InputImageType *>(image));
}

template <typename TInputImage, typename TOutputImage>
auto
StructuralSimilarityImageFilter<TInputImage, TOutputImage>::GetInput2() const -> const InputImageType *
{
  return itkDynamicCastInDebugMode<const InputImageType *>(this->ProcessObject::GetInput(1));
}

template <typename TInputImage, typename TOutputImage>
void
StructuralSimilarityImageFilter<TInputImage, TOutputImage>::SetScaleWeights(const ScaleWeightsType & weights)
{
  if (m_ScaleWeights != weights)
  {
    m_ScaleWeights = weights;
    this->Modified();
  }
}

template <typename TInputImage, typename TOutputImage>
void
StructuralSimilarityImageFilter<TInputImage, TOutputImage>::EnlargeOutputRequestedRegion(DataObject * data)
{
  Superclass::EnlargeOutputRequestedRegion(data);
  data->SetRequestedRegionToLargestPossibleRegion();
}

template <typename TInputImage, typename TOutputImage>
void
StructuralSimilarityImageFilter<TInputImage, TOutputImage>::VerifyPreconditions() const
{
  Superclass::VerifyPreconditions();

  if (m_ScaleWeights.GetSize() == 0)
  {
    itkExceptionMacro("ScaleWeights array must contain at least one element.");
  }
  if (m_GaussianSigma <= 0.0)
  {
    itkExceptionMacro("GaussianSigma must be strictly positive (got " << m_GaussianSigma << ").");
  }
  if (m_MaximumKernelWidth == 0)
  {
    itkExceptionMacro("MaximumKernelWidth must be at least 1 (got 0).");
  }
  if (m_DynamicRange <= 0.0)
  {
    itkExceptionMacro("DynamicRange must be strictly positive (got " << m_DynamicRange << ").");
  }

  const InputImageType * input1 = this->GetInput1();
  const InputImageType * input2 = this->GetInput2();
  if (input1 == nullptr || input2 == nullptr)
  {
    itkExceptionMacro("StructuralSimilarityImageFilter requires both inputs to be set.");
  }

  if (input1->GetLargestPossibleRegion() != input2->GetLargestPossibleRegion())
  {
    itkExceptionMacro("StructuralSimilarityImageFilter requires the two inputs to have identical regions.");
  }

  // m_ScaleWeights must sum to 1.0
  RealType sum = RealType{};
  for (const auto & w : m_ScaleWeights)
  {
    sum += w;
  }
  if (!Math::FloatAlmostEqual(sum, RealType{ 1 }, 4, 0.0001))
  {
    itkExceptionMacro("ScaleWeights must sum to 1.0 (got " << sum << ").");
  }

  const auto numberOfScales = m_ScaleWeights.GetSize();
  if (numberOfScales > 1)
  {
    const auto & size = input1->GetLargestPossibleRegion().GetSize();
    for (unsigned int d = 0; d < ImageDimension; ++d)
    {
      SizeValueType sz = size[d];
      for (unsigned int s = 1; s < numberOfScales; ++s)
      {
        sz /= 2;
        if (sz == 0)
        {
          itkExceptionMacro("Image dimension " << d << " (size " << size[d]
                                                << ") is too small to be downsampled " << (numberOfScales - 1)
                                                << " times for " << numberOfScales << "-scale MS-SSIM.");
        }
      }
    }
  }
}

template <typename TInputImage, typename TOutputImage>
void
StructuralSimilarityImageFilter<TInputImage, TOutputImage>::GenerateData()
{
  // ---- Build the internal Gaussian-statistics pipeline -----------------
  //
  // Following the composite-filter pattern (see CompositeFilterExample.cxx),
  // we reconnect the mini-pipeline inputs from the external pipeline every
  // time GenerateData() is called.  This ensures the internal sub-filters
  // pick up any new inputs when the external pipeline re-executes.
  //
  // For MS-SSIM (ScaleWeights with more than one element), the loop below
  // repeats the single-scale computation at progressively coarser
  // resolutions, downsampling the (real-valued, cast) images by a factor of
  // 2 between scales using BinShrinkImageFilter (local block averaging,
  // matching the classic MS-SSIM low-pass + downsample step).  Contrast and
  // structure are compared at every scale; luminance is compared only at
  // the coarsest (last) scale.  The per-pixel output image always reports
  // the ordinary single-scale SSIM map computed at the finest (original)
  // resolution.
  using RealImageType = Image<RealType, ImageDimension>;
  using CastFilterType = CastImageFilter<InputImageType, RealImageType>;
  using MultiplyFilterType = MultiplyImageFilter<RealImageType, RealImageType, RealImageType>;
  using GaussianFilterType = DiscreteGaussianImageFilter<RealImageType, RealImageType>;
  using ShrinkFilterType = BinShrinkImageFilter<RealImageType, RealImageType>;
  using RealImageRegionType = typename RealImageType::RegionType;

  // Graft external inputs to disconnect the mini-pipeline from the
  // upstream pipeline, preventing the internal Update() calls from
  // propagating back upstream.
  auto graftedInput1 = InputImageType::New();
  graftedInput1->Graft(this->GetInput1());

  auto graftedInput2 = InputImageType::New();
  graftedInput2->Graft(this->GetInput2());

  auto cast1 = CastFilterType::New();
  cast1->SetInput(graftedInput1);
  cast1->Update();

  auto cast2 = CastFilterType::New();
  cast2->SetInput(graftedInput2);
  cast2->Update();

  typename RealImageType::Pointer currentX = cast1->GetOutput();
  currentX->DisconnectPipeline();
  typename RealImageType::Pointer currentY = cast2->GetOutput();
  currentY->DisconnectPipeline();

  // ---- Allocate output (always at the finest/original resolution) ------
  OutputImageType * output = this->GetOutput();
  output->SetBufferedRegion(output->GetRequestedRegion());
  output->Allocate();

  const auto outputRegion = output->GetRequestedRegion();

  // ---- Pre-compute SSIM constants --------------------------------------
  const RealType K1 = static_cast<RealType>(m_K1);
  const RealType K2 = static_cast<RealType>(m_K2);
  const RealType L = static_cast<RealType>(m_DynamicRange);
  const RealType C1 = (K1 * L) * (K1 * L);
  const RealType C2 = (K2 * L) * (K2 * L);
  const RealType C3 = C2 / static_cast<RealType>(2);

  const bool useSimplifiedFormula = Math::FloatAlmostEqual(m_LuminanceExponent, 1.0) &&
                                    Math::FloatAlmostEqual(m_ContrastExponent, 1.0) &&
                                    Math::FloatAlmostEqual(m_StructureExponent, 1.0);

  const auto alpha = static_cast<RealType>(m_LuminanceExponent);
  const auto beta = static_cast<RealType>(m_ContrastExponent);
  const auto gamma = static_cast<RealType>(m_StructureExponent);

  // Configure a Gaussian filter with the requested sigma and kernel-width
  // ceiling.  We disable image-spacing-aware sigmas: SSIM is defined in
  // pixel coordinates and the canonical reference (Wang 2004) uses an
  // 11x11 unit-spacing window.
  auto makeGaussian = [this](const auto & inputPort) {
    auto g = GaussianFilterType::New();
    g->SetInput(inputPort);
    g->SetSigma(m_GaussianSigma);
    g->SetMaximumKernelWidth(m_MaximumKernelWidth);
    g->SetUseImageSpacing(false);
    g->ReleaseDataFlagOn();
    return g;
  };

  const auto kernelHalfWidth = static_cast<SizeValueType>(m_MaximumKernelWidth / 2);
  const auto numberOfScales = m_ScaleWeights.GetSize();

  RealType msssimProduct{ 1 };

  for (SizeValueType scaleIdx = 0; scaleIdx < numberOfScales; ++scaleIdx)
  {
    const bool isLastScale = (scaleIdx + 1 == numberOfScales);
    const bool writeOutput = (scaleIdx == 0);

    // ---- Build the 5 local-statistics images for the current scale -----
    auto x_times_x = MultiplyFilterType::New();
    x_times_x->SetInput1(currentX);
    x_times_x->SetInput2(currentX);

    auto y_times_y = MultiplyFilterType::New();
    y_times_y->SetInput1(currentY);
    y_times_y->SetInput2(currentY);

    auto x_times_y = MultiplyFilterType::New();
    x_times_y->SetInput1(currentX);
    x_times_y->SetInput2(currentY);

    auto g_x = makeGaussian(currentX);
    auto g_y = makeGaussian(currentY);
    auto g_xx = makeGaussian(x_times_x->GetOutput());
    auto g_yy = makeGaussian(y_times_y->GetOutput());
    auto g_xy = makeGaussian(x_times_y->GetOutput());

    g_x->Update();
    g_y->Update();
    g_xx->Update();
    g_yy->Update();
    g_xy->Update();

    const RealImageType * gx = g_x->GetOutput();
    const RealImageType * gy = g_y->GetOutput();
    const RealImageType * gxx = g_xx->GetOutput();
    const RealImageType * gyy = g_yy->GetOutput();
    const RealImageType * gxy = g_xy->GetOutput();

    const auto scaleRegion = currentX->GetLargestPossibleRegion();

    // ---- Determine the "valid" interior region for mean (MS-)SSIM ------
    //
    // Pixels within the half-width of the discrete Gaussian kernel of the
    // image boundary use boundary-extended values inside the convolution
    // and are therefore less reliable.  scikit-image (matching the MATLAB
    // reference) crops by (win_size - 1)/2 before averaging.  We do the
    // same, at every scale.
    auto interiorRegion = scaleRegion;
    bool interiorIsValid = true;
    for (unsigned int d = 0; d < ImageDimension; ++d)
    {
      const auto sz = interiorRegion.GetSize(d);
      if (sz <= 2 * kernelHalfWidth)
      {
        // Image is too small to crop -- mean (MS-)SSIM falls back to the
        // entire region for this scale.
        interiorIsValid = false;
        break;
      }
    }
    if (interiorIsValid)
    {
      auto idx = interiorRegion.GetIndex();
      auto sz = interiorRegion.GetSize();
      for (unsigned int d = 0; d < ImageDimension; ++d)
      {
        idx[d] += static_cast<IndexValueType>(kernelHalfWidth);
        sz[d] -= 2 * kernelHalfWidth;
      }
      interiorRegion.SetIndex(idx);
      interiorRegion.SetSize(sz);
    }

    // ---- Parallelized per-pixel (contrast, structure[, luminance]) -----
    //
    // Each thread accumulates a partial sum and pixel count over its
    // sub-region.  Atomic doubles aren't portable in C++17, so we serialize
    // the small per-thread reductions through a mutex.

    std::mutex    accumulatorMutex;
    RealType      accumulator{};
    SizeValueType accumulatorCount{};

    TotalProgressReporter progress(this, scaleRegion.GetNumberOfPixels());

    this->GetMultiThreader()->template ParallelizeImageRegion<ImageDimension>(
      scaleRegion,
      [&](const RealImageRegionType & subRegion) {
        // Use an index-aware iterator so we can call GetIndex() inside the
        // inner loop without hitting the ITK_LEGACY_REMOVE deprecation on
        // the index-less iterator's GetIndex() (#6034 CI fix).
        using RealConstIndexIteratorType = ImageRegionConstIteratorWithIndex<RealImageType>;
        using RealConstIteratorType = ImageRegionConstIterator<RealImageType>;

        RealConstIndexIteratorType gxIt(gx, subRegion);
        RealConstIteratorType      gyIt(gy, subRegion);
        RealConstIteratorType      gxxIt(gxx, subRegion);
        RealConstIteratorType      gyyIt(gyy, subRegion);
        RealConstIteratorType      gxyIt(gxy, subRegion);

        ImageRegionIteratorWithIndex<OutputImageType> outIt;
        if (writeOutput)
        {
          outIt = ImageRegionIteratorWithIndex<OutputImageType>(output, subRegion);
        }

        // Per-thread accumulators for the *interior* portion of this region.
        const auto subInterior = [&]() {
          RealImageRegionType r = subRegion;
          if (interiorIsValid)
          {
            if (!r.Crop(interiorRegion))
            {
              r.SetSize(SizeType{}); // empty
            }
          }
          return r;
        }();

        RealType      localSum{};
        SizeValueType localCount{};

        // Hoist subInterior emptiness check out of the inner loop.
        const bool subInteriorIsNonEmpty = (subInterior.GetNumberOfPixels() > 0);

        for (; !gxIt.IsAtEnd(); ++gxIt, ++gyIt, ++gxxIt, ++gyyIt, ++gxyIt)
        {
          const RealType mu_x = gxIt.Get();
          const RealType mu_y = gyIt.Get();
          const RealType mu_xx = gxxIt.Get();
          const RealType mu_yy = gyyIt.Get();
          const RealType mu_xy = gxyIt.Get();

          const RealType var_x = mu_xx - mu_x * mu_x;
          const RealType var_y = mu_yy - mu_y * mu_y;
          const RealType cov_xy = mu_xy - mu_x * mu_y;

          // Numerical floor: floating-point round-off can produce a tiny
          // negative variance for nearly-constant regions.
          const RealType var_x_clipped = std::max(var_x, RealType{});
          const RealType var_y_clipped = std::max(var_y, RealType{});

          RealType ssimFull{};
          RealType csTerm{};
          if (useSimplifiedFormula)
          {
            const RealType numerator = (RealType{ 2 } * mu_x * mu_y + C1) * (RealType{ 2 } * cov_xy + C2);
            const RealType denominator = (mu_x * mu_x + mu_y * mu_y + C1) * (var_x_clipped + var_y_clipped + C2);
            ssimFull = numerator / denominator;

            const RealType sigma_x = std::sqrt(var_x_clipped);
            const RealType sigma_y = std::sqrt(var_y_clipped);
            const RealType c = (RealType{ 2 } * sigma_x * sigma_y + C2) / (var_x_clipped + var_y_clipped + C2);
            const RealType s = (cov_xy + C3) / (sigma_x * sigma_y + C3);
            csTerm = c * s;
          }
          else
          {
            const RealType sigma_x = std::sqrt(var_x_clipped);
            const RealType sigma_y = std::sqrt(var_y_clipped);

            const RealType c_num = RealType{ 2 } * sigma_x * sigma_y + C2;
            const RealType c_den = var_x_clipped + var_y_clipped + C2;
            const RealType c = c_num / c_den;

            const RealType s_num = cov_xy + C3;
            const RealType s_den = sigma_x * sigma_y + C3;
            const RealType s = s_num / s_den;

            csTerm = std::pow(c, beta) * std::pow(s, gamma);

            if (writeOutput || isLastScale)
            {
              const RealType l_num = RealType{ 2 } * mu_x * mu_y + C1;
              const RealType l_den = mu_x * mu_x + mu_y * mu_y + C1;
              const RealType l = l_num / l_den;
              ssimFull = std::pow(l, alpha) * csTerm;
            }
          }

          if (writeOutput)
          {
            outIt.Set(static_cast<OutputPixelType>(ssimFull));
            ++outIt;
          }

          // The value combined into the (MS-)SSIM mean: the full SSIM
          // (including luminance) at the coarsest scale, otherwise the
          // contrast*structure term only.
          const RealType scaleValue = isLastScale ? ssimFull : csTerm;

          // Only accumulate over the interior region for the mean.
          if (subInteriorIsNonEmpty && subInterior.IsInside(gxIt.GetIndex()))
          {
            localSum += scaleValue;
            ++localCount;
          }
        }

        progress.Completed(subRegion.GetNumberOfPixels());

        {
          const std::lock_guard<std::mutex> lock(accumulatorMutex);
          accumulator += localSum;
          accumulatorCount += localCount;
        }
      },
      this);

    const RealType meanValue =
      (accumulatorCount > 0) ? (accumulator / static_cast<RealType>(accumulatorCount)) : RealType{};
    const RealType weight = static_cast<RealType>(m_ScaleWeights[scaleIdx]);
    msssimProduct *= std::pow(meanValue, weight);

    // ---- Downsample by a factor of 2 (local block averaging) for the ----
    // ---- next, coarser scale. -------------------------------------------
    if (!isLastScale)
    {
      auto shrinkX = ShrinkFilterType::New();
      shrinkX->SetInput(currentX);
      shrinkX->SetShrinkFactors(2);
      shrinkX->Update();
      currentX = shrinkX->GetOutput();
      currentX->DisconnectPipeline();

      auto shrinkY = ShrinkFilterType::New();
      shrinkY->SetInput(currentY);
      shrinkY->SetShrinkFactors(2);
      shrinkY->Update();
      currentY = shrinkY->GetOutput();
      currentY->DisconnectPipeline();
    }
  }

  m_MeanSSIM = static_cast<double>(msssimProduct);
}

template <typename TInputImage, typename TOutputImage>
void
StructuralSimilarityImageFilter<TInputImage, TOutputImage>::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "GaussianSigma: " << m_GaussianSigma << std::endl;
  os << indent << "MaximumKernelWidth: " << m_MaximumKernelWidth << std::endl;
  os << indent << "K1: " << m_K1 << std::endl;
  os << indent << "K2: " << m_K2 << std::endl;
  os << indent << "DynamicRange: " << m_DynamicRange << std::endl;
  os << indent << "LuminanceExponent: " << m_LuminanceExponent << std::endl;
  os << indent << "ContrastExponent: " << m_ContrastExponent << std::endl;
  os << indent << "StructureExponent: " << m_StructureExponent << std::endl;
  os << indent << "ScaleWeights: " << m_ScaleWeights << std::endl;
  os << indent << "MeanSSIM: " << m_MeanSSIM << std::endl;
}
} // end namespace itk
#endif
