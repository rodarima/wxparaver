/*****************************************************************************\
 *                        ANALYSIS PERFORMANCE TOOLS                         *
 *                                  wxparaver                                *
 *              Paraver Trace Visualization and Analysis Tool                *
 *****************************************************************************
 *     ___     This library is free software; you can redistribute it and/or *
 *    /  __         modify it under the terms of the GNU LGPL as published   *
 *   /  /  _____    by the Free Software Foundation; either version 2.1      *
 *  /  /  /     \   of the License, or (at your option) any later version.   *
 * (  (  ( B S C )                                                           *
 *  \  \  \_____/   This library is distributed in hope that it will be      *
 *   \  \__         useful but WITHOUT ANY WARRANTY; without even the        *
 *    \___          implied warranty of MERCHANTABILITY or FITNESS FOR A     *
 *                  PARTICULAR PURPOSE. See the GNU LGPL for more details.   *
 *                                                                           *
 * You should have received a copy of the GNU Lesser General Public License  *
 * along with this library; if not, write to the Free Software Foundation,   *
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA          *
 * The GNU LEsser General Public License is contained in the file COPYING.   *
 *                                 ---------                                 *
 *   Barcelona Supercomputing Center - Centro Nacional de Supercomputacion   *
\*****************************************************************************/


#ifndef _TIMELINETREESELECTOR_H_ 
#define _TIMELINETREESELECTOR_H_

//#define USE_WXDIALOG 1

#ifdef USE_WXDIALOG
#include <wx/dialog.h>
#else // wxMiniFrame
#include <wx/minifram.h>
#endif

class wxTreeCtrl;

#ifdef USE_WXDIALOG
class TimelineTreeSelector : public wxDialog
#else // wxMiniFrame
class TimelineTreeSelector : public wxMiniFrame
#endif
{
  DECLARE_DYNAMIC_CLASS( TimelineTreeSelector )
  DECLARE_EVENT_TABLE()

  public:
    TimelineTreeSelector()
    {}

    TimelineTreeSelector( wxWindow* parent,
                          wxWindowID id,
                          const wxString& title,
                          const wxPoint& pos = wxDefaultPosition,
                          const wxSize& size = wxDefaultSize,
#ifdef USE_WXDIALOG
                          long style = wxDEFAULT_DIALOG_STYLE,
                          const wxString& name = "dialogBox" );
#else // wxMiniFrame
                          long style = wxCAPTION | wxRESIZE_BORDER | wxCLOSE_BOX,
                          const wxString& name = "frame" );
#endif

    ~TimelineTreeSelector()
    {}
    
  private:
    void CreateControls();
    
    wxTreeCtrl *timelineTree;
};

#endif //_TIMELINETREESELECTOR_H_
