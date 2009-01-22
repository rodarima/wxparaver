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

#include "zoomhistory.cpp"

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
  EVT_RIGHT_DOWN( gTimeline::OnRightDown )

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
  delete zoomHistory;
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
  popUpMenu = NULL;
  drawZone = NULL;
////@end gTimeline member initialisation

  zoomXY = false;
  bufferImage.Create( 1, 1 );
  objectFont = wxFont( 7, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL );
  timeFont = wxFont( 6, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL );
  
  zoomHistory = new ZoomHistory<TTime,TObjectOrder>();
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

  drawZone = new wxScrolledWindow( itemFrame1, ID_SCROLLEDWINDOW, wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxFULL_REPAINT_ON_RESIZE|wxHSCROLL|wxVSCROLL );
  itemBoxSizer2->Add(drawZone, 1, wxGROW|wxALL, 1);
  drawZone->SetScrollbars(1, 1, 0, 0);

  // Connect events and objects
  drawZone->Connect(ID_SCROLLEDWINDOW, wxEVT_PAINT, wxPaintEventHandler(gTimeline::OnPaint), NULL, this);
  drawZone->Connect(ID_SCROLLEDWINDOW, wxEVT_ERASE_BACKGROUND, wxEraseEventHandler(gTimeline::OnEraseBackground), NULL, this);
  drawZone->Connect(ID_SCROLLEDWINDOW, wxEVT_LEFT_DOWN, wxMouseEventHandler(gTimeline::OnLeftDown), NULL, this);
  drawZone->Connect(ID_SCROLLEDWINDOW, wxEVT_LEFT_UP, wxMouseEventHandler(gTimeline::OnLeftUp), NULL, this);
  drawZone->Connect(ID_SCROLLEDWINDOW, wxEVT_RIGHT_DOWN, wxMouseEventHandler(gTimeline::OnRightDown), NULL, this);
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
  if ( zoomHistory->isEmpty() )
    zoomHistory->addZoom( myWindow->getWindowBeginTime(), myWindow->getWindowEndTime(),
                          0, myWindow->getWindowLevelObjects() - 1 );
  ready = false;
  bufferImage.Create( drawZone->GetSize().GetWidth(), drawZone->GetSize().GetHeight() );
  drawImage.Create( drawZone->GetSize().GetWidth(), drawZone->GetSize().GetHeight() );
  commImage.Create( drawZone->GetSize().GetWidth(), drawZone->GetSize().GetHeight() );
  wxMemoryDC bufferDraw( bufferImage );
  wxMemoryDC commdc( commImage );
  commdc.SetBackgroundMode( wxTRANSPARENT );
  commdc.SetBackground( *wxTRANSPARENT_BRUSH );
  commdc.Clear();
  wxBitmap commMask;
  commMask.Create( drawZone->GetSize().GetWidth(), drawZone->GetSize().GetHeight(), 1 );
  wxMemoryDC maskdc( commMask );
  maskdc.SetBackground( *wxBLACK_BRUSH );
  maskdc.SetPen( wxPen( wxColour( 255, 255, 255 ), 1 ) );
  maskdc.Clear();
  
  bufferDraw.SetBackground( wxBrush( *wxBLACK_BRUSH ) );
  bufferDraw.Clear();
  drawAxis( bufferDraw );
  myWindow->init( myWindow->getWindowBeginTime(), CREATECOMMS );
  
  TObjectOrder minObj = zoomHistory->getSecondDimension().first;
  TObjectOrder maxObj = zoomHistory->getSecondDimension().second;
  
  for( TObjectOrder obj = minObj; obj <= maxObj; obj++ )
    drawRow( bufferDraw, commdc, maskdc, obj );
  bufferDraw.SelectObject(wxNullBitmap);
  bufferDraw.SelectObject( drawImage );
  bufferDraw.DrawBitmap( bufferImage, 0, 0, false );
  maskdc.SetPen( *wxBLACK_PEN );
  maskdc.DrawRectangle( 0, 0, objectAxisPos + 1, drawZone->GetSize().GetHeight() );
  maskdc.DrawRectangle( drawZone->GetSize().GetWidth() - drawBorder, 0, drawBorder, drawZone->GetSize().GetHeight() );
  wxMask *mask = new wxMask( commMask );
  commImage.SetMask( mask );
  if( myWindow->getDrawCommLines() )
    bufferDraw.DrawBitmap( commImage, 0, 0, true );
  
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

  // Compute number of objects
  TObjectOrder minObj = zoomHistory->getSecondDimension().first;
  TObjectOrder maxObj = zoomHistory->getSecondDimension().second;
  
  // Get the text extent for the last object (probably the larger one)
  dc.SetFont( objectFont );
  wxSize objectExt = dc.GetTextExtent( LabelConstructor::objectLabel( maxObj - minObj + 1, myWindow->getLevel(), 
                                                                     myWindow->getTrace() ) );
  objectAxisPos = drawBorder + objectExt.GetWidth() + drawBorder;
  
  // Draw axis lines
  dc.DrawLine( objectAxisPos, drawBorder, 
               objectAxisPos, timeAxisPos );
  dc.DrawLine( objectAxisPos, timeAxisPos,
               dc.GetSize().GetWidth() - drawBorder, timeAxisPos );

  // Draw axis labels
  wxCoord y;
  double inc = (double)( timeAxisPos - drawBorder - ( objectExt.GetHeight() * 0.5 ) ) / (double)( maxObj - minObj + 1);

  objectPosList.clear();
  for( TObjectOrder obj = minObj; obj <= maxObj; obj++ )
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

void gTimeline::drawRow( wxDC& dc, wxMemoryDC& commdc, wxDC& maskdc, TObjectOrder row )
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
    RecordList *rl = myWindow->getRecordList( row );
    if( rl != NULL )
      drawComm( commdc, maskdc, rl, currentTime - timeStep, currentTime, timeStep, timePos );
    while( myWindow->getEndTime( row ) < currentTime )
    {
      myWindow->calcNext( row );
      values.push_back( myWindow->getValue( row ) );
      rl = myWindow->getRecordList( row );
      if( rl != NULL )
        drawComm( commdc, maskdc, rl, currentTime - timeStep, currentTime, timeStep, timePos );
    }
    
    TSemanticValue valueToDraw = DrawMode::selectValue( values, myWindow->getDrawModeTime() );
    rgb colorToDraw = myWindow->calcColor( valueToDraw, *myWindow );
    dc.SetPen( wxPen( wxColour( colorToDraw.red, colorToDraw.green, colorToDraw.blue ) ) );
    dc.DrawLine( timePos, objectPos, timePos, objectPos + objectHeight );
    
    timePos++;
  }
}

void gTimeline::drawComm( wxMemoryDC& commdc, wxDC& maskdc, RecordList *comms, TTime from, TTime to, TTime step, wxCoord pos )
{
  RecordList::iterator it = comms->begin();
  while( it != comms->end() && it->getTime() < from )
    ++it;
  while( it != comms->end() && it->getTime() <= to )
  {
    if( it->getType() & RECV ||
        ( it->getType() & SEND && it->getCommPartnerTime() > myWindow->getWindowEndTime() )
      )
    {
      if( it->getType() & LOG )
        commdc.SetPen( wxPen( wxColour( 255, 255, 0 ) ) );
      else if( it->getType() & PHY )
        commdc.SetPen( *wxRED_PEN );
      wxCoord posPartner = (wxCoord)floor( ( it->getCommPartnerTime() - myWindow->getWindowBeginTime() ) * ( 1 / step ) );
      posPartner += objectAxisPos;
      commdc.DrawLine( posPartner, objectPosList[it->getCommPartnerObject()],
                   pos, objectPosList[it->getOrder()] );
      maskdc.DrawLine( posPartner, objectPosList[it->getCommPartnerObject()],
                       pos, objectPosList[it->getOrder()] );
    }
    ++it;
  }
  comms->erase( comms->begin(), it );
}

/*!
 * wxEVT_ERASE_BACKGROUND event handler for ID_SCROLLEDWINDOW
 */

void gTimeline::OnEraseBackground( wxEraseEvent& event )
{
  //event.Skip();
}


/*!
 * wxEVT_PAINT event handler for ID_SCROLLEDWINDOW
 */

void gTimeline::OnPaint( wxPaintEvent& event )
{
  wxPaintDC dc( drawZone );
  
  if( ready )
    dc.DrawBitmap( drawImage, 0, 0, false );
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
  zoomBeginX = event.GetX();
  zoomBeginY = event.GetY();
}


/*!
 * wxEVT_LEFT_UP event handler for ID_SCROLLEDWINDOW
 */

void gTimeline::OnLeftUp( wxMouseEvent& event )
{
  wxMemoryDC dc( bufferImage );
  zoomEndX = event.GetX();
  zoomEndY = event.GetY();
  wxSize objectExt = dc.GetTextExtent( LabelConstructor::objectLabel( myWindow->getWindowLevelObjects() - 1, myWindow->getLevel(), 
                                                                     myWindow->getTrace() ) );

  if( ready && zoomBeginX != zoomEndX && zoomBeginY != zoomEndY )
  {
    // TIME zoom limits
    if( zoomEndX < zoomBeginX )
    {
      long tmp = zoomEndX; zoomEndX = zoomBeginX; zoomBeginX = tmp;
    }
    if( zoomBeginX < objectAxisPos )
      zoomBeginX = 0;
    else
      zoomBeginX -= objectAxisPos;
      
    if( zoomEndX > dc.GetSize().GetWidth() - drawBorder )
      zoomEndX = dc.GetSize().GetWidth() - drawBorder;
    zoomEndX -= objectAxisPos;
  
    // Detect begin and end TIME
    TTime timeStep = ( myWindow->getWindowEndTime() - myWindow->getWindowBeginTime() ) /
                     ( dc.GetSize().GetWidth() - objectAxisPos - drawBorder );

    TTime endTimeX = ( timeStep * zoomEndX ) + myWindow->getWindowBeginTime();
    TTime beginTimeX = ( timeStep * zoomBeginX ) + myWindow->getWindowBeginTime();

    // ROW zoom limits
    TObjectOrder beginRow;
    TObjectOrder endRow;

    if( !zoomXY )
    {
      // Use current zoom rows
      cout << "OnLeftUp - Normal zoom." << endl;
      beginRow = zoomHistory->getSecondDimension().first;
      endRow = zoomHistory->getSecondDimension().second;
    }
    else
    {
      cout << "OnLeftUp - Zoom XY." << endl;
      if( zoomEndY < zoomBeginY )
      {
        long tmp = zoomEndY; zoomEndY = zoomBeginY; zoomBeginY = tmp;
      }
      if( zoomBeginY > timeAxisPos )
        zoomBeginY = timeAxisPos - 1;
      if( zoomBeginY < drawBorder )
        zoomBeginY = drawBorder;

      if( zoomEndY > timeAxisPos )
        zoomEndY = timeAxisPos - 1;
      if( zoomEndY < drawBorder )
        zoomEndY = drawBorder;

      TObjectOrder numObjects = zoomHistory->getSecondDimension().second - zoomHistory->getSecondDimension().first + 1;

      double heightPerRow = (double)(( timeAxisPos - drawBorder ) / numObjects );
      beginRow = TObjectOrder( trunc( (zoomBeginY - drawBorder - 1) / heightPerRow ) );
      endRow = TObjectOrder( trunc( (zoomEndY - drawBorder - 1) / heightPerRow ) );

      cout << "rowStep       :" << heightPerRow << endl;
      cout << "height        :" << dc.GetSize().GetHeight() << endl;
      cout << "objectAxisPos :" << objectAxisPos  << endl;
      cout << "drawBorder    :" << drawBorder  << endl;
      cout << "timeAxisPos   :" << timeAxisPos << endl;
      cout << "numObjects    :" << numObjects  << endl;

      cout << "zoomBeginY    :" << zoomBeginY << endl;
      cout << "zoomEndY      :" << zoomEndY   << endl;

      cout << "beginRow      :" << beginRow << endl;
      cout << "endRow        :" << endRow   << endl << endl;
    }
    
    zoomHistory->addZoom( beginTimeX, endTimeX, beginRow, endRow );

    // Update window properties
    myWindow->setWindowBeginTime( beginTimeX );
    myWindow->setWindowEndTime( endTimeX );
    myWindow->setRedraw( true );
    myWindow->setChanged( true );
  }
  zooming = false;
  zoomXY = false;
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


void gTimeline::OnPopUpClone()
{
  printf("CATCHED clone!\n");
}


void gTimeline::OnPopUpFitTimeScale()
{
  myWindow->setWindowBeginTime( 0 );
  myWindow->setWindowEndTime( myWindow->getTrace()->getEndTime() );
  zoomHistory->addZoom( 0, myWindow->getTrace()->getEndTime() );
  myWindow->setRedraw( true );
  myWindow->setChanged( true );
}


void gTimeline::OnPopUpFitSemanticScale()
{
  printf("CATCHED fit sem!\n");
}


void gTimeline::OnPopUpCodeColor()
{
  myWindow->setCodeColorMode();
  myWindow->setRedraw( true );
  myWindow->setChanged( true );
}


void gTimeline::OnPopUpGradientColor()
{
  myWindow->setGradientColorMode();
  myWindow->setRedraw( true );
  myWindow->setChanged( true );
}

void gTimeline::OnPopUpUndoZoom( wxUpdateUIEvent& event )
{
//  wxMenuItem *menuItem = (wxMenuItem *)event.GetClientData();

  if ( !zoomHistory->emptyPrevZoom() )
  {
    zoomHistory->prevZoom();
    myWindow->setWindowBeginTime( zoomHistory->getFirstDimension().first );
    myWindow->setWindowEndTime( zoomHistory->getFirstDimension().second );
    myWindow->setRedraw( true );
    myWindow->setChanged( true );

      // tmp->Enable( !checked );
    //printf("client data %x\n", menuItem);
    //menuItem->Enable( !zoomHistory->EmptyPrevZoom() );
  }
  else
  {
    cout << "EMPTY!!!" << endl;
  }
}

void gTimeline::OnPopUpRedoZoom()
{
  if ( !zoomHistory->emptyNextZoom() )
  {
    zoomHistory->nextZoom();
    myWindow->setWindowBeginTime( zoomHistory->getFirstDimension().first );
    myWindow->setWindowEndTime( zoomHistory->getFirstDimension().second );
    myWindow->setRedraw( true );
    myWindow->setChanged( true );
  }
}

void gTimeline::BuildItem( wxMenu *popUp,
                           const wxString &title,
                           wxObjectEventFunction handler,
                           ItemType itemType,
                           bool checked )
{
  wxMenuItem *tmp;

  switch ( itemType )
  {
    case ITEMNORMAL:
    {
      tmp = new wxMenuItem( popUp, wxID_ANY, title, "", wxITEM_NORMAL );
      break;
    }
    case ITEMRADIO:
    {
      tmp = new wxMenuItem( popUp, wxID_ANY, title, "", wxITEM_RADIO );
      break;
    }
    case ITEMCHECK:
    {
      tmp = new wxMenuItem( popUp, wxID_ANY, title, "", wxITEM_CHECK );
      break;
    }
    default:
    {
      tmp = new wxMenuItem( popUp, wxID_ANY, title, "", wxITEM_NORMAL );
      break;
    }
  }

  int tmpid = tmp->GetId();
  popUp->Append( tmp );
  if ( tmp->IsCheckable() )
    tmp->Check( checked );

//  popUp->Connect(tmpid, wxEVT_COMMAND_MENU_SELECTED, handler, tmp, this );
  popUp->Connect(tmpid, wxEVT_COMMAND_MENU_SELECTED, handler, NULL, this );
}

/*!
 * wxEVT_RIGHT_DOWN event handler for ID_SCROLLEDWINDOW
 */

void gTimeline::OnRightDown( wxMouseEvent& event )
{
  if (popUpMenu == NULL)
  {
    popUpMenu = new wxMenu;
    popUpMenuColor = new wxMenu;

//    BuildItem( popUpMenu, wxString("&Clone"), (wxObjectEventFunction)&gTimeline::OnPopUpClone, MENUITEM);

//    popUpMenu->AppendSeparator();
    BuildItem( popUpMenu, wxString( "Undo Zoom" ), ( wxObjectEventFunction )&gTimeline::OnPopUpUndoZoom, ITEMNORMAL, zoomHistory->emptyPrevZoom() );
    BuildItem( popUpMenu, wxString( "Redo Zoom" ), ( wxObjectEventFunction )&gTimeline::OnPopUpRedoZoom, ITEMNORMAL, zoomHistory->emptyNextZoom() );

    popUpMenu->AppendSeparator();

    BuildItem( popUpMenu, wxString( "Fit &Time Scale" ), ( wxObjectEventFunction )&gTimeline::OnPopUpFitTimeScale, ITEMNORMAL );
    BuildItem( popUpMenu, wxString( "Fit &Semantic Scale" ), ( wxObjectEventFunction )&gTimeline::OnPopUpFitSemanticScale, ITEMNORMAL );

    popUpMenu->AppendSeparator();
    
    BuildItem( popUpMenuColor, wxString( "Code Color" ), ( wxObjectEventFunction )&gTimeline::OnPopUpCodeColor, ITEMRADIO, myWindow->IsCodeColorSet()  );
    BuildItem( popUpMenuColor, wxString( "Gradient Color" ), ( wxObjectEventFunction )&gTimeline::OnPopUpGradientColor, ITEMRADIO, myWindow->IsGradientColorSet() );
    popUpMenu->AppendSubMenu( popUpMenuColor, wxString( "Color" ));

//    BuildItem( popUpMenu, wxString("Not Null Gradient Color"), (wxObjectEventFunction)&gTimeline::OnPopUpClone);
  }

  PopupMenu( popUpMenu, event.GetPosition());
  
  //SetFocus();
}


/*!
 * wxEVT_MOTION event handler for ID_SCROLLEDWINDOW
 */

void gTimeline::OnMotion( wxMouseEvent& event )
{
  if( !zooming )
    return;

  zoomXY = event.ControlDown();

  wxMemoryDC memdc( drawImage );
  memdc.SetBackgroundMode( wxTRANSPARENT );
  memdc.SetBackground( *wxTRANSPARENT_BRUSH );
  memdc.Clear();
#ifdef __WXGTK__
  wxGCDC dc( memdc );
  dc.SetBrush( wxBrush( wxColour( 255, 255, 255, 80 ) ) );
#else
  wxDC& dc = memdc;
  dc.SetBrush( *wxTRANSPARENT_BRUSH );
#endif
  dc.SetPen( *wxWHITE_PEN );
  dc.SetBrush( wxBrush( wxColour( 255, 255, 255, 80 ) ) );

  long beginX = zoomBeginX > event.GetX() ? event.GetX() : zoomBeginX;
  long beginY = drawBorder;
  long endX = zoomBeginX < event.GetX() ? event.GetX() : zoomBeginX;
  if( beginX < objectAxisPos )
    beginX = objectAxisPos;
  if( endX > drawImage.GetWidth() - drawBorder )
    endX = drawImage.GetWidth() - drawBorder;
  wxCoord width = endX - beginX;
  wxCoord height = timeAxisPos - drawBorder + 1;

  if ( zoomXY )
  {
    beginY = zoomBeginY > event.GetY() ? event.GetY() : zoomBeginY;
    long endY = zoomBeginY < event.GetY() ? event.GetY() : zoomBeginY;
    if( beginY > timeAxisPos )
      beginY = timeAxisPos;
    if( endY > timeAxisPos )
      endY = timeAxisPos;
    if( beginY < drawBorder )
      beginY = drawBorder;
    if( endY < drawBorder )
      endY = drawBorder;
    height = endY - beginY;
  }

  dc.DrawBitmap( bufferImage, 0, 0, false );
  if( myWindow->getDrawCommLines() )
    dc.DrawBitmap( commImage, 0, 0, true );
  dc.DrawRectangle( beginX, beginY, width, height );

  drawZone->Refresh();
}
