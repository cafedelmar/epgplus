/*
 * epgplus.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include <vdr/epg.h>
#include <vdr/plugin.h>
#include <vdr/tools.h>
#include "KompexSQLiteDatabase.h"
#include "KompexSQLiteStatement.h"

static const char *VERSION        = "0.0.1";
static const char *DESCRIPTION    = tr("EPG handler for enrichment of events");

// --- cBroadcastedEpgHandler --------------------------------------------------------

class cBroadcastedEpgHandler : public cEpgHandler {
private:
  Kompex::SQLiteDatabase *epgDatabase;
public:
  virtual void cBroadcastedEpgHandler(Kompex::SQLiteDatabase *epgDB);                     
  virtual bool HandleEvent(cEvent *Event);
  };

cBroadcastedEpgHandler(Kompex::SQLiteDatabase *epgDatabase)
{
  epgDatabase = epgDB; 
}

bool cBroadcastedEpgHandler::HandleEvent(cEvent *Event)
{
  Kompex::SQLiteStatement *epgDatabaseQuery = Kompex::SQLiteStatement(*epgDatabase);  
  epgDatabaseQuery->Sql("SELECT * FROM epg WHERE startTime = '0'");
  cString chanInfo = cString::sprintf("Changing Description 4 for Channel %s", *Event->ChannelID().ToString());
  isyslog(chanInfo);
  cString desc = cString::sprintf("%s\n\n!!! NEUER TEST 4 !!!", Event->Description());
  cString title = cString::sprintf("%s (NEUER TEST 4)", Event->Title());
  Event->SetDescription(*desc); 
  Event->SetTitle(*title);
  epgDatabaseQuery->FreeQuery(); 
  return false;
}

// --- cPluginEpgplus ----------------------------------------------------

class cPluginEpgplus : public cPlugin {
private:
  Kompex::SQLiteDatabase *epgDatabase;
public:
  virtual ~cPluginEpgplus(void);
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return DESCRIPTION; }
  virtual bool Initialize(void);
  };

cPluginEpgplus::~cPluginEpgplus(void)
{
  epgDatabase->Close();
  free(epgDatabase);
}

bool cPluginEpgplus::Initialize(void)
{
  new cBroadcastedEpgHandler;
  cString epgDatabaseFilename = cString::sprintf("%s/epgplus.sqlite", cPlugin::ConfigDirectory("epgplus"));
  epgDatabase = new Kompex::SQLiteDatabase(epgDatabaseFilename, SQLITE_OPEN_READWRITE, 0);
  return true;
}

VDRPLUGINCREATOR(cPluginEpgplus); // Don't touch this!