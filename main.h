/*
 * main.h
 *
 * OpenH323 call generator
 *
 * Copyright (c) 2001 Benny L. Prijono <seventhson@theseventhson.freeserve.co.uk>
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.0 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 * the License for the specific language governing rights and limitations
 * under the License.
 *
 * The Original Code is CallGen323.
 *
 * The Initial Developer of the Original Code is Benny L. Prijono
 *
 * Contributor(s): Equivalence Pty. Ltd.
 *
 * $Log$
 *
 *
 * 25 Jan 2002 Substantial improvement [Equivalence Pty. Ltd.]
 * 25 Jan 2000 Update to incorporate openh323 v.01 alpha2 and fix gatekeeper
 *              related codes [bennylp]
 */


#include <ptclib/delaychan.h>
#include <ptclib/pwavfile.h>

#include <h323.h>

///////////////////////////////////////////////////////////////////////////////

class PlayMessage : public PDelayChannel
{
    PCLASSINFO(PlayMessage, PDelayChannel);
  public:
    PlayMessage(const PString & filename, unsigned frameDelay, unsigned frameSize);
    virtual BOOL Read(void *, PINDEX);
    virtual BOOL Close();
  protected:
    PWAVFile wavFile;
    BOOL     reallyClose;
};


///////////////////////////////////////////////////////////////////////////////

class RecordMessage : public PDelayChannel
{
    PCLASSINFO(RecordMessage, PDelayChannel);
  public:
    RecordMessage(const PString & filename, unsigned frameDelay, unsigned frameSize);
    virtual BOOL Write(const void *, PINDEX);
    virtual BOOL Close();
  protected:
    BOOL reallyClose;
};

///////////////////////////////////////////////////////////////////////////////

struct CallDetail
{
  CallDetail()
    : openedTransmitMedia(0),
      openedReceiveMedia(0),
      receivedMedia(0)
    { }

  PTime                openedTransmitMedia;
  PTime                openedReceiveMedia;
  PTime                receivedMedia;
  H323TransportAddress mediaGateway;

  void Drop(H323Connection & connection);

  void OnRTPStatistics(const RTP_Session & session, const PString & token);
};


///////////////////////////////////////////////////////////////////////////////

class MyH323EndPoint;

class MyH323Connection : public H323Connection
{
    PCLASSINFO(MyH323Connection, H323Connection);
  public:
    MyH323Connection(MyH323EndPoint & ep,unsigned callRef);

    virtual BOOL OpenAudioChannel(
      BOOL isEncoding,              /// Direction of data flow
      unsigned bufferSize,          /// Size of each sound buffer
      H323AudioCodec & codec        /// codec that is doing the opening
    );

    virtual void OnRTPStatistics(
      const RTP_Session & session   /// Session with statistics
    ) const;

    CallDetail details;

  protected:
    MyH323EndPoint & endpoint;
};


///////////////////////////////////////////////////////////////////////////////

class MyH323EndPoint : public H323EndPoint 
{
    PCLASSINFO(MyH323EndPoint, H323EndPoint);
  public:
    MyH323EndPoint();

    // override from H323EndPoint
    virtual H323Connection * CreateConnection(unsigned callReference);

    virtual void OnConnectionEstablished(
      H323Connection & connection,    /// Connection that was established
      const PString & token           /// Token for identifying connection
    );
    virtual void OnConnectionCleared(
      H323Connection & connection,    /// Connection that was established
      const PString & token           /// Token for identifying connection
    );
    BOOL OnStartLogicalChannel(
      H323Connection & connection,
      H323Channel & PTRACE_channel
    );
};

///////////////////////////////////////////////////////////////////////////////

class CallGen;

struct CallParams
{
  CallParams(CallGen & app)
    : callgen(app) { }

  CallGen & callgen;

  unsigned repeat;
  PTimeInterval tmax_est;
  PTimeInterval tmin_call;
  PTimeInterval tmax_call;
  PTimeInterval tmin_wait;
  PTimeInterval tmax_wait;
};


///////////////////////////////////////////////////////////////////////////////

class CallThread : public PThread
{
  PCLASSINFO(CallThread, PThread);
  public:
    CallThread(
      unsigned index,
      const PStringArray & destinations,
      const CallParams & params
    );
    void Main();
    void Stop();

  protected:
    PStringArray destinations;
    unsigned     index;
    CallParams   params;
    PSyncPoint   exit;
};

PLIST(CallThreadList, CallThread);


///////////////////////////////////////////////////////////////////////////////

class CallGen : public PProcess 
{
  PCLASSINFO(CallGen, PProcess)

  public:
    CallGen();
    void Main();
    static CallGen & Current() { return (CallGen&)PProcess::Current(); }

    PString    outgoingMessageFile;
    PString    incomingAudioDirectory;
    PTextFile  cdrFile;

    PSyncPoint threadEnded;
    unsigned   totalAttempts;
    unsigned   totalEstablished;
    PMutex     coutMutex;

  MyH323EndPoint * h323;

  BOOL Start(const PString & destination, PString & token) {
    return h323->MakeCall(destination, token) != NULL;
  }
  BOOL Exists(const PString & token) {
    return h323->HasConnection(token);
  }
  BOOL IsEstablished(const PString & token) {
    return h323->IsConnectionEstablished(token);
  }
  BOOL Clear(PString & token) {
    return h323->ClearCallSynchronous(token);
  }
  void ClearAll() {
    h323->ClearAllCalls();
  }

  protected:
    PDECLARE_NOTIFIER(PThread, CallGen, Cancel);
    PConsoleChannel console;
    CallThreadList threadList;
};


///////////////////////////////////////////////////////////////////////////////
