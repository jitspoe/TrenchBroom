/*
 Copyright (C) 2010-2017 Kristian Duske

 This file is part of TrenchBroom.

 TrenchBroom is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 TrenchBroom is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with TrenchBroom. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "FloatType.h"
#include "Macros.h"
#include "Model/UVCoordSystem.h"

#include "vm/forward.h"
#include "vm/vec.h"

#include <memory>
#include <tuple>

namespace TrenchBroom::Model
{

class ParallelUVCoordSystemSnapshot : public UVCoordSystemSnapshot
{
private:
  vm::vec3 m_xAxis;
  vm::vec3 m_yAxis;

public:
  ParallelUVCoordSystemSnapshot(const vm::vec3& xAxis, const vm::vec3& yAxis);
  explicit ParallelUVCoordSystemSnapshot(const ParallelUVCoordSystem* coordSystem);

  std::unique_ptr<UVCoordSystemSnapshot> clone() const override;

private:
  void doRestore(ParallelUVCoordSystem& coordSystem) const override;
  void doRestore(ParaxialUVCoordSystem& coordSystem) const override;
};

class ParallelUVCoordSystem : public UVCoordSystem
{
private:
  vm::vec3 m_xAxis;
  vm::vec3 m_yAxis;

  friend class ParallelUVCoordSystemSnapshot;

public:
  ParallelUVCoordSystem(
    const vm::vec3& point0,
    const vm::vec3& point1,
    const vm::vec3& point2,
    const BrushFaceAttributes& attribs);
  ParallelUVCoordSystem(const vm::vec3& xAxis, const vm::vec3& yAxis);

  static std::tuple<std::unique_ptr<UVCoordSystem>, BrushFaceAttributes> fromParaxial(
    const vm::vec3& point0,
    const vm::vec3& point1,
    const vm::vec3& point2,
    const BrushFaceAttributes& attribs);

  std::unique_ptr<UVCoordSystem> clone() const override;
  std::unique_ptr<UVCoordSystemSnapshot> takeSnapshot() const override;
  void restoreSnapshot(const UVCoordSystemSnapshot& snapshot) override;

  vm::vec3 xAxis() const override;
  vm::vec3 yAxis() const override;
  vm::vec3 zAxis() const override;

  void resetCache(
    const vm::vec3& point0,
    const vm::vec3& point1,
    const vm::vec3& point2,
    const BrushFaceAttributes& attribs) override;

  void resetTextureAxes(const vm::vec3& normal) override;
  void resetTextureAxesToParaxial(const vm::vec3& normal, float angle) override;
  void resetTextureAxesToParallel(const vm::vec3& normal, float angle) override;

  vm::vec2f getTexCoords(
    const vm::vec3& point,
    const BrushFaceAttributes& attribs,
    const vm::vec2f& textureSize) const override;

  void setRotation(const vm::vec3& normal, float oldAngle, float newAngle) override;

  void transform(
    const vm::plane3& oldBoundary,
    const vm::plane3& newBoundary,
    const vm::mat4x4& transformation,
    BrushFaceAttributes& attribs,
    const vm::vec2f& textureSize,
    bool lockTexture,
    const vm::vec3& invariant) override;

  void shearTexture(const vm::vec3& normal, const vm::vec2f& factors) override;

  float measureAngle(
    float currentAngle, const vm::vec2f& center, const vm::vec2f& point) const override;

  std::tuple<std::unique_ptr<UVCoordSystem>, BrushFaceAttributes> toParallel(
    const vm::vec3& point0,
    const vm::vec3& point1,
    const vm::vec3& point2,
    const BrushFaceAttributes& attribs) const override;
  std::tuple<std::unique_ptr<UVCoordSystem>, BrushFaceAttributes> toParaxial(
    const vm::vec3& point0,
    const vm::vec3& point1,
    const vm::vec3& point2,
    const BrushFaceAttributes& attribs) const override;

private:
  bool isRotationInverted(const vm::vec3& normal) const override;

  void updateNormalWithProjection(
    const vm::vec3& newNormal, const BrushFaceAttributes& attribs) override;
  void updateNormalWithRotation(
    const vm::vec3& oldNormal,
    const vm::vec3& newNormal,
    const BrushFaceAttributes& attribs) override;

  float computeTextureAngle(
    const vm::plane3& oldBoundary, const vm::mat4x4& transformation) const;

  deleteCopyAndMove(ParallelUVCoordSystem);
};

} // namespace TrenchBroom::Model
