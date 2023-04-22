/*
 Copyright (C) 2010-2023 Kristian Duske, Nathan "jitspoe" Wulf

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

#include "CreatePrimitiveBrushToolPage.h"
#include "CreatePrimitiveBrushTool.h"

#include "View/MapDocument.h"
#include "View/ViewConstants.h"

#include <kdl/memory_utils.h>
#include <vecmath/vec.h>
#include <vecmath/vec_io.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>

namespace TrenchBroom
{
namespace View
{
CreatePrimitiveBrushToolPage::CreatePrimitiveBrushToolPage(
  std::weak_ptr<MapDocument> document, CreatePrimitiveBrushTool& tool, QWidget* parent)
  : QWidget(parent)
  , m_document(document)
  , m_tool(tool)
  , m_offset(nullptr)
  , m_button(nullptr)
{
  createGui();
  connectObservers();
  updateGui();
}

void CreatePrimitiveBrushToolPage::connectObservers()
{
  auto document = kdl::mem_lock(m_document);
  m_notifierConnection += document->selectionDidChangeNotifier.connect(
    this, &CreatePrimitiveBrushToolPage::selectionDidChange);
}

void CreatePrimitiveBrushToolPage::createGui()
{
  QLabel* numSidesLabel = new QLabel(tr("Number of Sides: "));
  QSpinBox* radiusBox = new QSpinBox();
  QSpinBox* numSidesBox = new QSpinBox();
  m_offset = new QLineEdit("0.0 0.0 0.0");
  m_button = new QPushButton(tr("Apply"));

  numSidesBox->setRange(3, 256); // set before connecting callbacks because it will override the values
  numSidesBox->setValue(m_tool.m_primitiveBrushData.numSides);

  connect(m_button, &QAbstractButton::clicked, this, &CreatePrimitiveBrushToolPage::applyMove); // JITODO: Remove
  connect(m_offset, &QLineEdit::returnPressed, this, &CreatePrimitiveBrushToolPage::applyMove);

  connect(numSidesBox, QOverload<int>::of(&QSpinBox::valueChanged), this,
    [=](int numSidesValue) {
      this->m_tool.m_primitiveBrushData.numSides = numSidesValue;
      this->m_tool.update();
    });
  auto* layout = new QHBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(LayoutConstants::MediumHMargin);

  layout->addWidget(numSidesLabel, 0, Qt::AlignVCenter);
  layout->addWidget(numSidesBox, 0, Qt::AlignVCenter);
  //layout->addWidget(m_button, 0, Qt::AlignVCenter);
  layout->addStretch(1);

  setLayout(layout);
}

void CreatePrimitiveBrushToolPage::updateGui()
{
  // NOTE: This gets called after creating a brush, so we can't consider this to only be called when selecting brushes manually.
  auto document = kdl::mem_lock(m_document);
  m_button->setEnabled(document->hasSelectedNodes());
}

void CreatePrimitiveBrushToolPage::selectionDidChange(const Selection&)
{
  updateGui();
}

void CreatePrimitiveBrushToolPage::applyMove()
{
  if (const auto delta = vm::parse<FloatType, 3>(m_offset->text().toStdString()))
  {
    auto document = kdl::mem_lock(m_document);
    document->translateObjects(*delta);
  }
}
} // namespace View
} // namespace TrenchBroom
