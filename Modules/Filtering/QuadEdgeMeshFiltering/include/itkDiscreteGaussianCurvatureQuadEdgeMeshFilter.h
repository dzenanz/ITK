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
#ifndef itkDiscreteGaussianCurvatureQuadEdgeMeshFilter_h
#define itkDiscreteGaussianCurvatureQuadEdgeMeshFilter_h

#include "itkDiscreteCurvatureQuadEdgeMeshFilter.h"
#include "itkMath.h"


namespace itk
{
/**
 * \class DiscreteGaussianCurvatureQuadEdgeMeshFilter
 * \brief see the following paper
 * title: Discrete Differential-Geometry Operators for Triangulated 2-Manifolds
 * authors: Mark Meyer, Mathieu Desbrun, Peter Schroder, Alan H. Barr
 * conference: VisMath '02
 * location: Berlin (Germany)
 * \author: Arnaud Gelas, Alexandre Gouaillard
 * \ingroup ITKQuadEdgeMeshFiltering
 */
template <typename TInputMesh, typename TOutputMesh = TInputMesh>
class ITK_TEMPLATE_EXPORT DiscreteGaussianCurvatureQuadEdgeMeshFilter
  : public DiscreteCurvatureQuadEdgeMeshFilter<TInputMesh, TOutputMesh>
{
public:
  ITK_DISALLOW_COPY_AND_MOVE(DiscreteGaussianCurvatureQuadEdgeMeshFilter);

  using Self = DiscreteGaussianCurvatureQuadEdgeMeshFilter;
  using Pointer = SmartPointer<Self>;
  using ConstPointer = SmartPointer<const Self>;
  using Superclass = DiscreteCurvatureQuadEdgeMeshFilter<TInputMesh, TOutputMesh>;

  using typename Superclass::InputMeshType;
  using typename Superclass::InputMeshPointer;

  using typename Superclass::OutputMeshType;
  using typename Superclass::OutputMeshPointer;
  using typename Superclass::OutputPointsContainerPointer;
  using typename Superclass::OutputPointsContainerIterator;
  using typename Superclass::OutputPointType;
  using typename Superclass::OutputVectorType;
  using typename Superclass::OutputCoordType;
  using typename Superclass::OutputPointIdentifier;
  using typename Superclass::OutputCellIdentifier;
  using typename Superclass::OutputQEType;
  using typename Superclass::OutputMeshTraits;
  using typename Superclass::OutputCurvatureType;
  using typename Superclass::TriangleType;

  /** \see LightObject::GetNameOfClass() */
  itkOverrideGetNameOfClassMacro(DiscreteGaussianCurvatureQuadEdgeMeshFilter);

  /** New macro for creation of through a Smart Pointer   */
  itkNewMacro(Self);

  itkConceptMacro(OutputIsFloatingPointCheck, (Concept::IsFloatingPoint<OutputCurvatureType>));

protected:
  DiscreteGaussianCurvatureQuadEdgeMeshFilter() = default;
  ~DiscreteGaussianCurvatureQuadEdgeMeshFilter() override = default;

  OutputCurvatureType
  EstimateCurvature(const OutputPointType & iP) override
  {
    const OutputMeshPointer output = this->GetOutput();

    OutputQEType * qe = iP.GetEdge();

    if (qe != nullptr)
    {
      OutputQEType * qe_it = qe;

      OutputCurvatureType sum_theta = 0.;
      OutputCurvatureType area = 0.;

      do
      {
        // cell_id = qe_it->GetLeft();
        OutputQEType *        qe_it2 = qe_it->GetOnext();
        const OutputPointType q0 = output->GetPoint(qe_it->GetDestination());
        const OutputPointType q1 = output->GetPoint(qe_it2->GetDestination());

        // Compute Angle;
        sum_theta += static_cast<OutputCurvatureType>(TriangleType::ComputeAngle(q0, iP, q1));
        area += this->ComputeMixedArea(qe_it, qe_it2);
        qe_it = qe_it2;
      } while (qe_it != qe);

      return (2.0 * itk::Math::pi - sum_theta) / area;
    }

    return 0.;
  }
};
} // namespace itk

#endif
