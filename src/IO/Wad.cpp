/*
 Copyright (C) 2010-2013 Kristian Duske
 
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
 along with TrenchBroom.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Wad.h"

#include "Exceptions.h"
#include "IO/FileSystem.h"
#include "IO/IOUtils.h"

#ifdef _MSC_VER
#include <cstdint>
#elif defined __GNUC__
#include <stdint.h>
#endif

namespace TrenchBroom {
    namespace IO {
        namespace WadLayout {
            static const size_t NumEntriesAddress     = 4;
            static const size_t DirOffsetAddress      = 8;
            static const size_t DirEntryTypeOffset    = 4;
            static const size_t DirEntryNameOffset    = 3;
            static const size_t DirEntryNameSize      = 16;
            static const size_t PalLength             = 256;
            static const size_t TexWidthOffset        = 16;
            static const size_t MaxTextureSize        = 512;
        }

        WadEntry::WadEntry(const String& name, const char type, const size_t address, const size_t size) :
        m_name(name),
        m_type(type),
        m_address(address),
        m_size(size) {}

        const String& WadEntry::name() const {
            return m_name;
        }
        
        char WadEntry::type() const {
            return m_type;
        }
        
        size_t WadEntry::address() const {
            return m_address;
        }
        
        size_t WadEntry::size() const {
            return m_size;
        }

        MipSize::MipSize(const size_t i_width, const size_t i_height) :
        width(i_width),
        height(i_height) {}
        
        bool MipSize::operator== (const MipSize& rhs) const {
            return width == rhs.width && height == rhs.height;
        }

        Wad::Wad(const Path& path) {
            FileSystem fs;
            m_file = fs.mapFile(path, std::ios_base::in);
            if (m_file == NULL)
                throw WadException("Cannot open wad file " + path.asString());
            loadEntries();
        }

        const WadEntryList& Wad::allEntries() const {
            return m_entries;
        }

        const WadEntryList Wad::entriesWithType(const char type) const {
            WadEntryList result;
            WadEntryList::const_iterator it, end;
            for (it = m_entries.begin(), end = m_entries.end(); it != end; ++it) {
                const WadEntry& entry = *it;
                if (entry.type() == type)
                    result.push_back(entry);
            }
            return result;
        }

        void Wad::loadEntries() {
            if (WadLayout::NumEntriesAddress + sizeof(int32_t) >= m_file->size() ||
                WadLayout::DirOffsetAddress + sizeof(int32_t) >= m_file->size())
                throw WadException("Invalid wad layout");
            
            const char* cursor = m_file->begin() + WadLayout::NumEntriesAddress;
            const size_t entryCount = readSize<int32_t>(cursor);
            cursor = m_file->begin() + WadLayout::DirOffsetAddress;
            const size_t directoryAddr = readSize<int32_t>(cursor);
            
            if (directoryAddr  >= m_file->size())
                throw WadException("Wad directory beyond end of file");
            
            char entryType;
            char entryName[WadLayout::DirEntryNameSize];
            
            cursor = m_file->begin() + directoryAddr;
            
            for (size_t i = 0; i < entryCount; i++) {
                size_t entryAddress = readSize<int32_t>(cursor);
                size_t entrySize = readSize<int32_t>(cursor);
                
                if (entryAddress + entrySize >= m_file->size())
                    throw WadException("Wad entry beyond end of file");
                
                cursor += WadLayout::DirEntryTypeOffset;
                readBytes(cursor, &entryType, 1);
                cursor += WadLayout::DirEntryNameOffset;
                readBytes(cursor, entryName, WadLayout::DirEntryNameSize);
                
                // might leak if there are duplicate entries
                m_entries.push_back(WadEntry(entryName, entryType, entryAddress, entrySize));
            }
        }

        const MipSize Wad::mipSize(const WadEntry& entry) const {
            if (entry.type() != WadEntryType::WEMip)
                throw WadException(entry.name() + " is not a Mip entry");
            
            const char* cursor = m_file->begin() + entry.address() + WadLayout::TexWidthOffset;
            const size_t width = readSize<int32_t>(cursor);
            const size_t height = readSize<int32_t>(cursor);
            if (width == 0 || height == 0)
                throw WadException(entry.name() + "has invalid dimensions");
            
            return MipSize(width, height);
        }
    }
}
