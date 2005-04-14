/*=========================================================================
 
Program:   Medical Imaging & Interaction Toolkit
Module:    $RCSfile$
Language:  C++
Date:      $Date$
Version:   $Revision$
 
Copyright (c) German Cancer Research Center, Division of Medical and
Biological Informatics. All rights reserved.
See MITKCopyright.txt or http://www.mitk.org/copyright.html for details.
 
This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.
 
=========================================================================*/
#include "QmitkPropertyListViewItem.h"
#include "mitkPropertyList.h"
#include "mitkProperties.h"
#include "mitkColorProperty.h"

#include <qcheckbox.h>
#include <qlineedit.h>
#include <qlabel.h>
#include "mitkRenderWindow.h"
#include <qpushbutton.h>
#include <qpixmap.h>
#include <qcolordialog.h>

QmitkPropertyListViewItem* QmitkPropertyListViewItem::CreateInstance(mitk::PropertyList *propList, const std::string name, QWidget* parent)
{
  QmitkPropertyListViewItem* newItem = new QmitkPropertyListViewItem(name,propList,NULL,NULL);
  mitk::BaseProperty* baseProp = newItem->m_PropertyList->GetProperty(newItem->m_Name.c_str());
  newItem->m_EnabledButton = new QPushButton(parent);
  newItem->UpdateEnabledView();
  connect(
(QObject*)(newItem->m_EnabledButton),
SIGNAL(clicked()),
(QObject*)(newItem),
SLOT(EnabledButtonClicked())
);
   newItem->m_Label = new QLabel(name.c_str(),parent);
  if (mitk::BoolProperty* boolProp = dynamic_cast<mitk::BoolProperty*>(baseProp))
  {
    newItem->m_Control = new QCheckBox(parent);
    ((QCheckBox*)(newItem->m_Control))->setChecked(boolProp->GetValue());
    connect((QObject*)(newItem->m_Control),SIGNAL(stateChanged(int)),(QObject*)(newItem),SLOT(CheckBoxControlActivated(int)));
  }
  else if (mitk::StringProperty* stringProp = dynamic_cast<mitk::StringProperty*>(baseProp))
  {
    newItem->m_Control = new QLineEdit(QString(stringProp->GetValue()),parent);
    connect((QObject*)(newItem->m_Control),SIGNAL(textChanged(const QString &)),(QObject*)(newItem),SLOT(StringControlActivated(const QString &)));
  }
  else if (mitk::ColorProperty* colorProp = dynamic_cast<mitk::ColorProperty*>(baseProp))
  {
    newItem->m_Control = new QPushButton(parent);
    QPixmap pm(20,20);
    mitk::Color col = colorProp->GetColor();
    QColor qcol((int)(col.GetRed() * 255), (int)(col.GetGreen() * 255),(int)( col.GetBlue() * 255));
    pm.fill(qcol);
    // newItem->m_Control->setBackgroundPixmap(pm);
    newItem->m_Control->setPaletteBackgroundColor(qcol);
    connect((QObject*)(newItem->m_Control),SIGNAL(clicked()),(QObject*)(newItem),SLOT(ColorControlActivated()));
  }
  else
  {
    newItem->m_Control = new QLabel(QString(baseProp->GetValueAsString().c_str()),parent);
  }
  newItem->m_EnabledButton->show();
  newItem->m_Label->show();
  newItem->m_Control->show();
  return newItem;
}
void QmitkPropertyListViewItem::CheckBoxControlActivated(int state)
{
  m_PropertyList->SetProperty(m_Name.c_str(), new mitk::BoolProperty(state));
  mitk::RenderWindow::UpdateAllInstances();
}

void QmitkPropertyListViewItem::StringControlActivated(const QString &text)
{
  std::cout << "setting string property to:" << text.ascii() << std::endl;
  m_PropertyList->SetProperty(m_Name.c_str(), new mitk::StringProperty(text.ascii()));
}
void QmitkPropertyListViewItem::ColorControlActivated()
{
  std::cout << "color control" << std::endl;
  mitk::ColorProperty* colorProp = dynamic_cast<mitk::ColorProperty*>(m_PropertyList->GetProperty(m_Name.c_str()).GetPointer());
  mitk::Color col = colorProp->GetColor();
  QColor result = QColorDialog::getColor(QColor((int)(col.GetRed() * 255), (int)(col.GetGreen() * 255), (int)(col.GetBlue() * 255)));
  if (result.isValid())
  {
    col.SetRed(result.red() / 255.0);
    col.SetGreen(result.green() / 255.0);
    col.SetBlue(result.blue() / 255.0);
    colorProp->SetColor(col);
    m_PropertyList->SetProperty(m_Name.c_str(), colorProp);
    m_Control->setPaletteBackgroundColor(result);
    mitk::RenderWindow::UpdateAllInstances();
  }
}
void QmitkPropertyListViewItem::UpdateView()
{
  mitk::BaseProperty* baseProp = m_PropertyList->GetProperty(m_Name.c_str());
  if (mitk::BoolProperty* boolProp = dynamic_cast<mitk::BoolProperty*>(baseProp))
  {
    ((QCheckBox*)(m_Control))->setChecked(boolProp->GetValue());
  }
  else if (mitk::StringProperty* stringProp = dynamic_cast<mitk::StringProperty*>(baseProp))
  {
    ((QLineEdit*)(m_Control))->setText(QString(stringProp->GetValue()));
  }
  else if (mitk::ColorProperty* colorProp = dynamic_cast<mitk::ColorProperty*>(baseProp))
  {
    mitk::Color col = colorProp->GetColor();
    QColor qcol((int)(col.GetRed() * 255), (int)(col.GetGreen() * 255),(int)( col.GetBlue() * 255));
    ((QPushButton*)(m_Control))->setPaletteBackgroundColor(qcol);
  }
}
void QmitkPropertyListViewItem::UpdateEnabledView()
{
  static const QPixmap enabledPix((const char **)enabled_xpm);
  static const QPixmap disabledPix((const char **)disabled_xpm);
  mitk::BaseProperty* baseProp = m_PropertyList->GetProperty(m_Name.c_str());
  if (baseProp->GetEnabled())
  {
    m_EnabledButton->setPixmap(enabledPix);
  }
  else
  {
    m_EnabledButton->setPixmap(disabledPix);
  }
}
void QmitkPropertyListViewItem::EnabledButtonClicked() {
  mitk::BaseProperty* baseProp = m_PropertyList->GetProperty(m_Name.c_str());
  baseProp->SetEnabled(! baseProp->GetEnabled());
  UpdateEnabledView();
}