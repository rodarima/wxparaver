/////////////////////////////////////////////////////////////////////////////
// Name:        gtimeline.cpp
// Purpose:     
// Author:      Eloy Martinez
// Modified by: 
// Created:     Wed 27 Aug 2008 11:26:03 CEST
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
////@end includes

#include <wx/dcbuffer.h>
#include "gtimeline.h"
#include "window.h"

////@begin XPM images
////@end XPM images


/*!
 * gTimeline type definition
 */

IMPLEMENT_CLASS( gTimeline, wxFrame )


/*!
 * gTimeline event table definition
 */

BEGIN_EVENT_TABLE( gTimeline, wxFrame )

////@begin gTimeline event table entries
  EVT_SIZE( gTimeline::OnSize )

////@end gTimeline event table entries

END_EVENT_TABLE()


/*!
 * gTimeline constructors
 */

gTimeline::gTimeline()
{
  Init();
}

gTimeline::gTimeline( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
  Init();
  Create( parent, id, caption, pos, size, style );
}


/*!
 * gTimeline creator
 */

bool gTimeline::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin gTimeline creation
  wxFrame::Create( parent, id, caption, pos, size, style );

  CreateControls();
////@end gTimeline creation
  return true;
}


/*!
 * gTimeline destructor
 */

gTimeline::~gTimeline()
{
////@begin gTimeline destruction
////@end gTimeline destruction
}


/*!
 * Member initialisation
 */

void gTimeline::Init()
{
////@begin gTimeline member initialisation
  ready = false;
  myWindow = NULL;
  drawZone = NULL;
////@end gTimeline member initialisation
  bufferImage.Create( 1, 1 );
}


/*!
 * Control creation for gTimeline
 */

void gTimeline::CreateControls()
{    
////@begin gTimeline content construction
  gTimeline* itemFrame1 = this;

  wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
  itemFrame1->SetSizer(itemBoxSizer2);

  drawZone = new wxScrolledWindow( itemFrame1, ID_SCROLLEDWINDOW, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxHSCROLL|wxVSCROLL );
  itemBoxSizer2->Add(drawZone, 1, wxGROW|wxALL, 5);
  drawZone->SetScrollbars(1, 1, 0, 0);

  // Connect events and objects
  drawZone->Connect(ID_SCROLLEDWINDOW, wxEVT_PAINT, wxPaintEventHandler(gTimeline::OnPaint), NULL, this);
  drawZone->Connect(ID_SCROLLEDWINDOW, wxEVT_ERASE_BACKGROUND, wxEraseEventHandler(gTimeline::OnEraseBackground), NULL, this);
////@end gTimeline content construction
}


/*!
 * Should we show tooltips?
 */

bool gTimeline::ShowToolTips()
{
  return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap gTimeline::GetBitmapResource( const wxString& name )
{
  // Bitmap retrieval
////@begin gTimeline bitmap retrieval
  wxUnusedVar(name);
  return wxNullBitmap;
////@end gTimeline bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon gTimeline::GetIconResource( const wxString& name )
{
  // Icon retrieval
////@begin gTimeline icon retrieval
  wxUnusedVar(name);
  return wxNullIcon;
////@end gTimeline icon retrieval
}


void gTimeline::redraw()
{
  ready = false;
  bufferImage.Create( drawZone->GetSize().GetWidth(), drawZone->GetSize().GetHeight() );
  wxMemoryDC bufferDraw( bufferImage );
  
  bufferDraw.SetBackground( wxBrush( *wxBLACK_BRUSH ) );
  bufferDraw.Clear();
  bufferDraw.SetPen( wxPen( *wxWHITE, 1 ) );
  bufferDraw.DrawLine( 100, 3, 100, bufferImage.GetHeight() - 25 );
  bufferDraw.DrawLine( 100, bufferImage.GetHeight() - 25, bufferImage.GetWidth() - 15, bufferImage.GetHeight() - 25 );
  bufferDraw.SelectObject(wxNullBitmap);
  
  ready = true;
}


/*!
 * wxEVT_ERASE_BACKGROUND event handler for ID_SCROLLEDWINDOW
 */

void gTimeline::OnEraseBackground( wxEraseEvent& event )
{
}


/*!
 * wxEVT_PAINT event handler for ID_SCROLLEDWINDOW
 */

void gTimeline::OnPaint( wxPaintEvent& event )
{
  wxPaintDC dc( drawZone );
  
  dc.DrawBitmap( bufferImage, 0, 0, false );
}




/*!
 * wxEVT_SIZE event handler for ID_GTIMELINE
 */

void gTimeline::OnSize( wxSizeEvent& event )
{
  if( ready )
    redraw();
  event.Skip();
}
