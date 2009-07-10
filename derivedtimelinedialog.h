/////////////////////////////////////////////////////////////////////////////
// Name:        derivedtimelinedialog.h
// Purpose:     
// Author:      Eloy Martinez
// Modified by: 
// Created:     Tue 07 Jul 2009 17:07:35 CEST
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _DERIVEDTIMELINEDIALOG_H_
#define _DERIVEDTIMELINEDIALOG_H_


/*!
 * Includes
 */

////@begin includes
#include "wx/statline.h"
////@end includes

#include "window.h"

/*!
 * Forward declarations
 */

////@begin forward declarations
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_DERIVEDTIMELINEDIALOG 10032
#define ID_DERIVED_NAME 10001
#define ID_TOPCOMPOSE1 10002
#define ID_TOPCOMPOSE2 10000
#define ID_FACTOR_TIMELINE_1 10035
#define ID_TIMELINES_LIST_1 10036
#define ID_OPERATIONS 10037
#define ID_TIMELINES_LIST_2 10038
#define ID_FACTOR_TIMELINE_2 10039
#define ID_SWAP_WINDOWS 10033
#define SYMBOL_DERIVEDTIMELINEDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxTAB_TRAVERSAL
#define SYMBOL_DERIVEDTIMELINEDIALOG_TITLE _("Create Derived Timeline Window")
#define SYMBOL_DERIVEDTIMELINEDIALOG_IDNAME ID_DERIVEDTIMELINEDIALOG
#define SYMBOL_DERIVEDTIMELINEDIALOG_SIZE wxDefaultSize
#define SYMBOL_DERIVEDTIMELINEDIALOG_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * DerivedTimelineDialog class declaration
 */

class DerivedTimelineDialog: public wxDialog
{    
  DECLARE_DYNAMIC_CLASS( DerivedTimelineDialog )
  DECLARE_EVENT_TABLE()

public:
  /// Constructors
  DerivedTimelineDialog();
  DerivedTimelineDialog( wxWindow* parent, wxWindowID id = SYMBOL_DERIVEDTIMELINEDIALOG_IDNAME, const wxString& caption = SYMBOL_DERIVEDTIMELINEDIALOG_TITLE, const wxPoint& pos = SYMBOL_DERIVEDTIMELINEDIALOG_POSITION, const wxSize& size = SYMBOL_DERIVEDTIMELINEDIALOG_SIZE, long style = SYMBOL_DERIVEDTIMELINEDIALOG_STYLE );

  /// Creation
  bool Create( wxWindow* parent, wxWindowID id = SYMBOL_DERIVEDTIMELINEDIALOG_IDNAME, const wxString& caption = SYMBOL_DERIVEDTIMELINEDIALOG_TITLE, const wxPoint& pos = SYMBOL_DERIVEDTIMELINEDIALOG_POSITION, const wxSize& size = SYMBOL_DERIVEDTIMELINEDIALOG_SIZE, long style = SYMBOL_DERIVEDTIMELINEDIALOG_STYLE );

  /// Destructor
  ~DerivedTimelineDialog();

  /// Initialises member variables
  void Init();

  /// Creates the controls and sizers
  void CreateControls();

////@begin DerivedTimelineDialog event handler declarations

  /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_SWAP_WINDOWS
  void OnSwapWindowsClick( wxCommandEvent& event );

  /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
  void OnOkClick( wxCommandEvent& event );

////@end DerivedTimelineDialog event handler declarations

////@begin DerivedTimelineDialog member function declarations

  double GetFactorTimeline1() const { return factorTimeline1 ; }
  void SetFactorTimeline1(double value) { factorTimeline1 = value ; }

  double GetFactorTimeline2() const { return factorTimeline2 ; }
  void SetFactorTimeline2(double value) { factorTimeline2 = value ; }

  vector< Window * > GetTimelines1() const { return timelines1 ; }
  void SetTimelines1(vector< Window * > value) { timelines1 = value ; }

  vector< Window * > GetTimelines2() const { return timelines2 ; }
  void SetTimelines2(vector< Window * > value) { timelines2 = value ; }

  vector< string > GetOperations() const { return operations ; }
  void SetOperations(vector< string > value) { operations = value ; }

  Window * GetCurrentWindow1() const { return currentWindow1 ; }
  void SetCurrentWindow1(Window * value) { currentWindow1 = value ; }

  Window * GetCurrentWindow2() const { return currentWindow2 ; }
  void SetCurrentWindow2(Window * value) { currentWindow2 = value ; }

  vector< string > GetTopCompose1() const { return topCompose1 ; }
  void SetTopCompose1(vector< string > value) { topCompose1 = value ; }

  vector< string > GetTopCompose2() const { return topCompose2 ; }
  void SetTopCompose2(vector< string > value) { topCompose2 = value ; }

  string GetTimelineName() const { return timelineName ; }
  void SetTimelineName(string value) { timelineName = value ; }

  /// Retrieves bitmap resources
  wxBitmap GetBitmapResource( const wxString& name );

  /// Retrieves icon resources
  wxIcon GetIconResource( const wxString& name );
////@end DerivedTimelineDialog member function declarations

  /// Should we show tooltips?
  static bool ShowToolTips();

  bool TransferDataToWindow();
  bool TransferDataFromWindow();

////@begin DerivedTimelineDialog member variables
  wxTextCtrl* widgetName;
  wxChoice* widgetTopCompose1;
  wxChoice* widgetTopCompose2;
  wxTextCtrl* widgetFactorTimeline1;
  wxComboBox* widgetTimelines1;
  wxChoice* widgetOperations;
  wxComboBox* widgetTimelines2;
  wxTextCtrl* widgetFactorTimeline2;
  wxButton* swapWindowsButton;
private:
  double factorTimeline1;
  double factorTimeline2;
  vector< Window * > timelines1;
  vector< Window * > timelines2;
  vector< string > operations;
  Window * currentWindow1;
  Window * currentWindow2;
  vector< string > topCompose1;
  vector< string > topCompose2;
  string timelineName;
////@end DerivedTimelineDialog member variables

  void presetTimelineComboBox( vector< Window * > timelines,
                               Window *currentWindow,
                               wxComboBox *comboBox );
  void presetStringChoiceBox( vector< string > list, wxChoice *choiceBox );
  void presetFactorField( double value, wxTextCtrl *field );
  void presetNameField( string whichName, wxTextCtrl *field );

  void getSelectedString( wxChoice *choiceBox, vector< string > &selection );
  void getSelectedWindow( wxComboBox *comboBox, vector< Window * > &selection );
  void getName( wxTextCtrl *field, string &whichName );
};

#endif
  // _DERIVEDTIMELINEDIALOG_H_
