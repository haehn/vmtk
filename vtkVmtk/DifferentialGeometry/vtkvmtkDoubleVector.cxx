/*=========================================================================

  Program:   VMTK
  Module:    $RCSfile: vtkvmtkDoubleVector.cxx,v $
  Language:  C++
  Date:      $Date: 2006/04/06 16:46:43 $
  Version:   $Revision: 1.3 $

  Copyright (c) Luca Antiga 

  Copyright (c) Luca Antiga, David Steinman. All rights reserved.
  See LICENCE file for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm 
  for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkvmtkDoubleVector.h"
#include "vtkvmtkConstants.h"
#include "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkvmtkDoubleVector, "$Revision: 1.3 $");
vtkStandardNewMacro(vtkvmtkDoubleVector);

vtkvmtkDoubleVector::vtkvmtkDoubleVector()
  {
  this->NormType = VTK_VMTK_L2_NORM;
  this->NumberOfElements = 0;
  this->Array = NULL;
  }

vtkvmtkDoubleVector::~vtkvmtkDoubleVector()
  {
  if (this->Array != NULL)
    {
    delete [] this->Array;
    this->Array = NULL;
    }
  }

void vtkvmtkDoubleVector::Allocate(vtkIdType numberOfElements)
{
  if ( this->Array != NULL )
    {
    delete [] this->Array;
    this->Array = NULL;
    }

  this->NumberOfElements = numberOfElements;
  this->Array = new double[numberOfElements];
}

void vtkvmtkDoubleVector::Fill(double value)
  {
  for (vtkIdType i=0; i<this->NumberOfElements; i++)
    {
    this->Array[i] = value;
    }
  }

void vtkvmtkDoubleVector::Assign(vtkvmtkDoubleVector *src)
  {
  if (this->NumberOfElements != src->NumberOfElements)
    {
    vtkErrorMacro(<< "Vectors have different number of elements.");
    }

  this->NumberOfElements = src->NumberOfElements;
  memcpy(this->Array, src->Array, src->NumberOfElements * sizeof(double));
  }

void vtkvmtkDoubleVector::Assign(vtkIdType numberOfElements, const double *array)
  {
  this->NumberOfElements = numberOfElements;
  memcpy(this->Array, array, numberOfElements * sizeof(double));
  }

double vtkvmtkDoubleVector::ComputeNorm()
  {
  vtkIdType i;
  double norm = 0.0;
  double value;

  if (this->NormType==VTK_VMTK_L2_NORM)
    {
    norm = 0.0;
    for (i=0; i<this->NumberOfElements; i++)
      {
      value = this->Array[i];
      norm +=  value * value;
      }
    norm = sqrt(norm);
    }
  else if (this->NormType==VTK_VMTK_LINF_NORM)
    {
    norm = -VTK_VMTK_LARGE_DOUBLE;
    for (i=0; i<this->NumberOfElements; i++)
      {
      value = this->Array[i];
      if (value>norm)
	norm = value;
      }
    }

  return norm;
  }

void vtkvmtkDoubleVector::Add(vtkvmtkDoubleVector* vectorToAdd)
  {
  if (this->NumberOfElements != vectorToAdd->NumberOfElements)
    {
    vtkErrorMacro(<< "Vector to add does not have the same number of elements.");
    return;
    }

  for (int i=0; i<this->NumberOfElements; i++)
    {
    this->Array[i] += vectorToAdd->GetElement(i);
    }
  }

void vtkvmtkDoubleVector::Subtract(vtkvmtkDoubleVector* vectorToSubtract)
  {
  if (this->NumberOfElements != vectorToSubtract->NumberOfElements)
    {
    vtkErrorMacro(<< "Vector to add does not have the same number of elements.");
    return;
    }

  for (int i=0; i<this->NumberOfElements; i++)
    {
    this->Array[i] -= vectorToSubtract->GetElement(i);
    }
  }

void vtkvmtkDoubleVector::MultiplyBy(double scalar)
  {
  for (int i=0; i<this->NumberOfElements; i++)
    {
    this->Array[i] *= scalar;
    }
  }

double vtkvmtkDoubleVector::Dot(vtkvmtkDoubleVector* vectorToDotWith)
  {
  double dot;

  if (this->NumberOfElements != vectorToDotWith->NumberOfElements)
    {
    vtkErrorMacro(<< "Vector to add does not have the same number of elements.");
    return 0.0;
    }

  dot = 0.0;
  for (int i=0; i<this->NumberOfElements; i++)
    {
    dot += this->Array[i] * vectorToDotWith->Array[i];
    }

  return dot;
  }

void vtkvmtkDoubleVector::CopyIntoArrayComponent(vtkDataArray *array, int component)
  {
  vtkIdType i;

  if (array==NULL)
    {
    vtkErrorMacro(<<"NULL vtkDoubleArray provided!");
    return;
    }

  if (component > array->GetNumberOfComponents()-1)
    array->SetNumberOfComponents(component+1);

  if (this->NumberOfElements>array->GetNumberOfTuples())
    {
    array->SetNumberOfTuples(this->NumberOfElements);
    }

  for (i=0; i<this->NumberOfElements; i++)
    {
    array->SetComponent(i,component,this->Array[i]);
    }
  }

void vtkvmtkDoubleVector::DeepCopy(vtkvmtkDoubleVector *src)
  {
  this->NumberOfElements = src->NumberOfElements;

  if (this->Array != NULL)
    {
    delete [] this->Array;
    this->Array = NULL;
    }

  this->Array = new double[src->NumberOfElements];
  memcpy(this->Array, src->Array, src->NumberOfElements * sizeof(double));
  }