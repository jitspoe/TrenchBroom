/*
 Copyright (C) 2010-2014 Kristian Duske
 
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

#ifndef __TrenchBroom__ModelFactoryImpl__
#define __TrenchBroom__ModelFactoryImpl__

#include "TrenchBroom.h"
#include "VecMath.h"
#include "StringUtils.h"
#include "Model/MapFormat.h"
#include "Model/ModelFactory.h"
#include "Model/ModelTypes.h"

namespace TrenchBroom {
    namespace Model {
        class BrushContentTypeBuilder;
        
        class ModelFactoryImpl : public ModelFactory {
        private:
            MapFormat::Type m_format;
            BrushContentTypeBuilder* m_brushContentTypeBuilder;
        public:
            ModelFactoryImpl();
            ModelFactoryImpl(MapFormat::Type format, BrushContentTypeBuilder* brushContentTypeBuilder);
            MapFormat::Type format() const;
        private: // implement ModelFactory interface
            World* doCreateWorld() const;
            Layer* doCreateLayer(const String& name) const;
            Group* doCreateGroup(const String& name) const;
            Entity* doCreateEntity() const;
            Brush* doCreateBrush(const BBox3& worldBounds, const BrushFaceList& faces) const;
            BrushFace* doCreateFace(const Vec3& point1, const Vec3& point2, const Vec3& point3, const String& textureName) const;
            BrushFace* doCreateFace(const Vec3& point1, const Vec3& point2, const Vec3& point3, const String& textureName, const Vec3& texAxisX, const Vec3& texAxisY) const;
        };
    }
}

#endif /* defined(__TrenchBroom__ModelFactoryImpl__) */
