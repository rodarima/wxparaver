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
#include "labelconstructor.h"
#include "drawmode.h"

#define wxTEST_GRAPHICS 1

#if wxTEST_GRAPHICS
#include "wx/graphics.h"
#if wxUSE_GRAPHICS_CONTEXT == 0
#undef wxTEST_GRAPHICS
#define wxTEST_GRAPHICS 0
#endif
#else
#undef wxUSE_GRAPHICS_CONTEXT
#define wxUSE_GRAPHICS_CONTEXT 0
#endif

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
  EVT_CLOSE( gTimeline::OnCloseWindow )
  EVT_SIZE( gTimeline::OnSize )
  EVT_IDLE( gTimeline::OnIdle )

  EVT_UPDATE_UI( ID_SCROLLEDWINDOW, gTimeline::OnScrolledwindowUpdate )

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
  objectHeight = 1;
  zooming = false;
  drawZone = NULL;
////@end gTimeline member initialisation
  bufferImage.Create( 1, 1 );
  objectFont = wxFont( 9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL );
  timeFont = wxFont( 7, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL );
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

  drawZone = new wxScrolledWindow( itemFrame1, ID_SCROLLEDWINDOW, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxFULL_REPAINT_ON_RESIZE|wxHSCROLL|wxVSCROLL );
  itemBoxSizer2->Add(drawZone, 1, wxGROW|wxALL, 5);
  drawZone->SetScrollbars(1, 1, 0, 0);

  // Connect events and objects
  drawZone->Connect(ID_SCROLLEDWINDOW, wxEVT_PAINT, wxPaintEventHandler(gTimeline::OnPaint), NULL, this);
  drawZone->Connect(ID_SCROLLEDWINDOW, wxEVT_ERASE_BACKGROUND, wxEraseEventHandler(gTimeline::OnEraseBackground), NULL, this);
  drawZone->Connect(ID_SCROLLEDWINDOW, wxEVT_LEFT_DOWN, wxMouseEventHandler(gTimeline::OnLeftDown), NULL, this);
  drawZone->Connect(ID_SCROLLEDWINDOW, wxEVT_LEFT_UP, wxMouseEventHandler(gTimeline::OnLeftUp), NULL, this);
  drawZone->Connect(ID_SCROLLEDWINDOW, wxEVT_MOTION, wxMouseEventHandler(gTimeline::OnMotion), NULL, this);
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
  drawImage.Create( drawZone->GetSize().GetWidth(), drawZone->GetSize().GetHeight() );
  wxMemoryDC bufferDraw( bufferImage );
  
  bufferDraw.SetBackground( wxBrush( *wxBLACK_BRUSH ) );
  bufferDraw.Clear();
  drawAxis( bufferDraw );
  myWindow->init( myWindow->getWindowBeginTime(), NOCREATE );
  for( TObjectOrder obj = 0; obj < myWindow->getWindowLevelObjects(); obj++ )
    drawRow( bufferDraw, obj );
  bufferDraw.SelectObject(wxNullBitmap);
  bufferDraw.SelectObject( drawImage );
  bufferDraw.DrawBitmap( bufferImage, 0, 0, false );
  
  ready = true;
}

void gTimeline::drawAxis( wxDC& dc )
{
  dc.SetPen( wxPen( *wxWHITE, 1 ) );
  dc.SetTextForeground( *wxWHITE );
  
  // Get the text extent for time label
  dc.SetFont( timeFont );
  wxSize timeExt = dc.GetTextExtent( LabelConstructor::timeLabel( myWindow->getWindowBeginTime(), myWindow->getTimeUnit() ) );
  timeAxisPos = dc.GetSize().GetHeight() - ( drawBorder + timeExt.GetHeight() + drawBorder );
  
  // Get the text extent for the last object (probably the larger one)
  dc.SetFont( objectFont );
  wxSize objectExt = dc.GetTextExtent( LabelConstructor::objectLabel( myWindow->getWindowLevelObjects() - 1, myWindow->getLevel(), 
                                                                     myWindow->getTrace() ) );
  objectAxisPos = drawBorder + objectExt.GetWidth() + drawBorder;
  
  // Draw axis lines
  dc.DrawLine( objectAxisPos, drawBorder, 
               objectAxisPos, timeAxisPos );
  dc.DrawLine( objectAxisPos, timeAxisPos,
               dc.GetSize().GetWidth() - drawBorder, timeAxisPos );
  
  // Draw axis labels
  wxCoord y;
  double inc = (double)( timeAxisPos - drawBorder ) / (double)myWindow->getWindowLevelObjects();
//  double inc = (double)( timeAxisPos - drawBorder - objectExt.GetHeight() ) 
//               / (double)myWindow->getWindowLevelObjects();
  objectPosList.clear();
  for( TObjectOrder obj = 0; obj < myWindow->getWindowLevelObjects(); obj++ )
  {
    y = ( (wxCoord) ( inc * ( obj + 0.5 ) ) ) + drawBorder;
    objectPosList.push_back( y );
    dc.DrawText( LabelConstructor::objectLabel( obj, myWindow->getLevel(), myWindow->getTrace() ),
                 drawBorder, y );
  }
  objectHeight = timeAxisPos - y;
  
  dc.SetFont( timeFont );
  dc.DrawText( LabelConstructor::timeLabel( myWindow->getWindowBeginTime(), myWindow->getTimeUnit() ),
               objectAxisPos, timeAxisPos + drawBorder );
  dc.DrawText( LabelConstructor::timeLabel( myWindow->getWindowEndTime(), myWindow->getTimeUnit() ),
               dc.GetSize().GetWidth() -
               ( dc.GetTextExtent( LabelConstructor::timeLabel( myWindow->getWindowEndTime(), myWindow->getTimeUnit() ) )
               .GetWidth() + drawBorder ),
               timeAxisPos + drawBorder );
}

void gTimeline::drawRow( wxDC& dc, TObjectOrder row )
{
  TTime timeStep = ( myWindow->getWindowEndTime() - myWindow->getWindowBeginTime() ) /
                   ( dc.GetSize().GetWidth() - objectAxisPos - drawBorder );
  vector<TSemanticValue> values;
  wxCoord timePos = objectAxisPos + 1;
  wxCoord objectPos = objectPosList[row];
  for( TTime currentTime = myWindow->getWindowBeginTime() + timeStep; 
       currentTime <= myWindow->getWindowEndTime(); 
       currentTime += timeStep )
  {
    values.clear();
    
    while( myWindow->getEndTime( row ) <= currentTime - timeStep )
      myWindow->calcNext( row );
      
    values.push_back( myWindow->getValue( row ) );
    while( myWindow->getEndTime( row ) < currentTime )
    {
      myWindow->calcNext( row );
      values.push_back( myWindow->getValue( row ) );
    }
    
    TSemanticValue valueToDraw = DrawMode::selectValue( values, myWindow->getDrawModeTime() );
    rgb colorToDraw = myWindow->calcColor( valueToDraw, *myWindow );
    dc.SetPen( wxPen( wxColour( colorToDraw.red, colorToDraw.green, colorToDraw.blue ) ) );
    dc.DrawLine( timePos, objectPos, timePos, objectPos + objectHeight );
    
    timePos++;
  }
}

/*!
 * wxEVT_ERASE_BACKGROUND event handler for ID_SCROLLEDWINDOW
 */

void gTimeline::OnEraseBackground( wxEraseEvent& event )
{
  event.Skip();
}


/*!
 * wxEVT_PAINT event handler for ID_SCROLLEDWINDOW
 */

void gTimeline::OnPaint( wxPaintEvent& event )
{
  wxPaintDC dc( drawZone );
  
  if( ready )
    dc.DrawBitmap( drawImage, 0, 0, false );

/*  if( zooming )
    dc.DrawBitmap( zoomingImage, 0, 0, true );*/
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


/*!
 * wxEVT_IDLE event handler for ID_GTIMELINE
 */

void gTimeline::OnIdle( wxIdleEvent& event )
{
  this->SetTitle( myWindow->getName() );
  
  if( myWindow->getShowWindow() )
  {
    this->Show();
    if( !ready )
      redraw();
  }
  else
    this->Show( false );
}


/*!
 * wxEVT_CLOSE_WINDOW event handler for ID_GTIMELINE
 */

void gTimeline::OnCloseWindow( wxCloseEvent& event )
{
  myWindow->setShowWindow( false );
}


/*!
 * wxEVT_LEFT_DOWN event handler for ID_SCROLLEDWINDOW
 */

void gTimeline::OnLeftDown( wxMouseEvent& event )
{
  zooming = true;
  zoomBegin = event.GetX();
}


/*!
 * wxEVT_LEFT_UP event handler for ID_SCROLLEDWINDOW
 */

void gTimeline::OnLeftUp( wxMouseEvent& event )
{
  wxMemoryDC dc( bufferImage );
  zoomEnd = event.GetX();
  if( ready && zoomBegin != zoomEnd )
  {
    if( zoomEnd < zoomBegin )
    {
      long tmp = zoomEnd; zoomEnd = zoomBegin; zoomBegin = tmp;
    }
    if( zoomBegin < objectAxisPos )
      zoomBegin = 0;
    else
      zoomBegin -= objectAxisPos;
    zoomEnd -= objectAxisPos;
    TTime timeStep = ( myWindow->getWindowEndTime() - myWindow->getWindowBeginTime() ) /
                     ( dc.GetSize().GetWidth() - objectAxisPos - drawBorder );
    myWindow->setWindowEndTime( ( timeStep * zoomEnd ) + myWindow->getWindowBeginTime() );
    myWindow->setWindowBeginTime( ( timeStep * zoomBegin ) + myWindow->getWindowBeginTime() );
    myWindow->setRedraw( true );
    myWindow->setChanged( true );
  }
  zooming = false;
}


/*!
 * wxEVT_UPDATE_UI event handler for ID_SCROLLEDWINDOW
 */

void gTimeline::OnScrolledwindowUpdate( wxUpdateUIEvent& event )
{
  if( this->IsShown() )
  {
    if( myWindow->getRedraw() )
    {
      myWindow->setRedraw( false );
      redraw();
      drawZone->Refresh();
    }
  }
}


/*!
 * wxEVT_MOTION event handler for ID_SCROLLEDWINDOW
 */

void gTimeline::OnMotion( wxMouseEvent& event )
{
  if( !zooming )
    return;
    
  wxMemoryDC memdc( drawImage );
  memdc.SetBackgroundMode( wxTRANSPARENT );
  memdc.SetBackground( *wxTRANSPARENT_BRUSH );
  memdc.Clear();
#ifdef __WXGTK__
  wxGCDC dc( memdc );
#else
  wxDC& dc = memdc;
#endif
  dc.SetPen( *wxWHITE_PEN );
  dc.SetBrush( wxBrush( wxColour( 255, 255, 255, 64 ) ) );
  
  long begin = zoomBegin > event.GetX() ? event.GetX() : zoomBegin;
  if( begin < objectAxisPos )
    begin = objectAxisPos;
  wxCoord width = zoomBegin > event.GetX() ? zoomBegin - event.GetX() : event.GetX() - zoomBegin;
  
  dc.DrawBitmap( bufferImage, 0, 0, false );
  dc.DrawRectangle( begin, drawBorder, width, timeAxisPos - drawBorder + 1 );
  drawZone->Refresh();
}

