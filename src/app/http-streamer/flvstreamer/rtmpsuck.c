/*  RTMP Proxy Server
 *  Copyright (C) 2009 Andrej Stepanchuk
 *  Copyright (C) 2009 Howard Chu
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with FLVStreamer; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

/* This is a Proxy Server that displays the connection parameters from a
 * client and then saves any data streamed to the client.
 */

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>

#include <signal.h>
#include <getopt.h>

#include <assert.h>

#include "rtmp.h"
#include "parseurl.h"

#include "thread.h"

#ifdef linux
#include <linux/netfilter_ipv4.h>
#endif

#define FLVSTREAMER_PROXY_VERSION	"v2.1a"

#define RD_SUCCESS		0
#define RD_FAILED		1
#define RD_INCOMPLETE		2

#define PACKET_SIZE 1024*1024

#ifdef WIN32
#define InitSockets()	{\
        WORD version;			\
        WSADATA wsaData;		\
					\
        version = MAKEWORD(1,1);	\
        WSAStartup(version, &wsaData);	}

#define	CleanupSockets()	WSACleanup()
#else
#define InitSockets()
#define	CleanupSockets()
#endif

enum
{
  STREAMING_ACCEPTING,
  STREAMING_IN_PROGRESS,
  STREAMING_STOPPING,
  STREAMING_STOPPED
};

typedef struct Plist
{
  struct Plist *p_next;
  RTMPPacket p_pkt;
} Plist;

typedef struct
{
  int socket;
  int state;
  uint32_t stamp;
  RTMP rs;
  RTMP rc;
  Plist *rs_pkt[2];	/* head, tail */
  Plist *rc_pkt[2];	/* head, tail */
  FILE *out;

} STREAMING_SERVER;

STREAMING_SERVER *rtmpServer = 0;	// server structure pointer

STREAMING_SERVER *startStreaming(const char *address, int port);
void stopStreaming(STREAMING_SERVER * server);

#define STR2AVAL(av,str)	av.av_val = str; av.av_len = strlen(av.av_val)

#ifdef _DEBUG
uint32_t debugTS = 0;

int pnum = 0;

FILE *netstackdump = NULL;
FILE *netstackdump_read = NULL;
#endif

#define BUFFERTIME	(4*60*60*1000)	/* 4 hours */

#define SAVC(x) static const AVal av_##x = AVC(#x)

SAVC(app);
SAVC(connect);
SAVC(flashVer);
SAVC(swfUrl);
SAVC(pageUrl);
SAVC(tcUrl);
SAVC(fpad);
SAVC(capabilities);
SAVC(audioCodecs);
SAVC(videoCodecs);
SAVC(videoFunction);
SAVC(objectEncoding);
SAVC(_result);
SAVC(createStream);
SAVC(play);
SAVC(fmsVer);
SAVC(mode);
SAVC(level);
SAVC(code);
SAVC(description);
SAVC(secureToken);
SAVC(onStatus);
static const AVal av_NetStream_Failed = AVC("NetStream.Failed");
static const AVal av_NetStream_Play_Failed = AVC("NetStream.Play.Failed");
static const AVal av_NetStream_Play_StreamNotFound =
AVC("NetStream.Play.StreamNotFound");
static const AVal av_NetConnection_Connect_InvalidApp =
AVC("NetConnection.Connect.InvalidApp");
static const AVal av_NetStream_Play_Start = AVC("NetStream.Play.Start");
static const AVal av_NetStream_Play_Complete = AVC("NetStream.Play.Complete");
static const AVal av_NetStream_Play_Stop = AVC("NetStream.Play.Stop");

// Returns 0 for OK/Failed/error, 1 for 'Stop or Complete'
int
ServeInvoke(STREAMING_SERVER *server, RTMPPacket *pack, const char *body)
{
  int ret = 0, nRes;
  int nBodySize = pack->m_nBodySize;

  if (body > pack->m_body)
    nBodySize--;

  if (body[0] != 0x02)		// make sure it is a string method name we start with
    {
      Log(LOGWARNING, "%s, Sanity failed. no string method in invoke packet",
	  __FUNCTION__);
      return 0;
    }

  AMFObject obj;
  nRes = AMF_Decode(&obj, body, nBodySize, false);
  if (nRes < 0)
    {
      Log(LOGERROR, "%s, error decoding invoke packet", __FUNCTION__);
      return 0;
    }

  AMF_Dump(&obj);
  AVal method;
  AMFProp_GetString(AMF_GetProp(&obj, NULL, 0), &method);
  Log(LOGDEBUG, "%s, client invoking <%s>", __FUNCTION__, method.av_val);

  if (AVMATCH(&method, &av_connect))
    {
      AMFObject cobj;
      AVal pname, pval;
      int i;
      AMFProp_GetObject(AMF_GetProp(&obj, NULL, 2), &cobj);
      LogPrintf("Processing connect\n");
      for (i=0; i<cobj.o_num; i++)
        {
          pname = cobj.o_props[i].p_name;
          pval.av_val = NULL;
          pval.av_len = 0;
          if (cobj.o_props[i].p_type == AMF_STRING)
            {
              pval = cobj.o_props[i].p_vu.p_aval;
              if (pval.av_val)
                pval.av_val = strdup(pval.av_val);
              LogPrintf("%.*s: %.*s\n", pname.av_len, pname.av_val, pval.av_len, pval.av_val);
            }
          if (AVMATCH(&pname, &av_app))
            {
              server->rc.Link.app = pval;
              pval.av_val = NULL;
            }
          else if (AVMATCH(&pname, &av_flashVer))
            {
              server->rc.Link.flashVer = pval;
              pval.av_val = NULL;
            }
          else if (AVMATCH(&pname, &av_swfUrl))
            {
              server->rc.Link.swfUrl = pval;
              pval.av_val = NULL;
            }
          else if (AVMATCH(&pname, &av_tcUrl))
            {
              char *r1 = NULL, *r2;
              int len;

              server->rc.Link.tcUrl = pval;
              if ((pval.av_val[0] | 0x40) == 'r' &&
                  (pval.av_val[1] | 0x40) == 't' &&
                  (pval.av_val[2] | 0x40) == 'm' &&
                  (pval.av_val[3] | 0x40) == 'p')
                {
                  if (pval.av_val[4] == ':')
                    {
                      server->rc.Link.protocol = RTMP_PROTOCOL_RTMP;
                      r1 = pval.av_val+7;
                    }
                  else if ((pval.av_val[4] | 0x40) == 'e' && pval.av_val[5] == ':')
                    {
                      server->rc.Link.protocol = RTMP_PROTOCOL_RTMPE;
                      r1 = pval.av_val+8;
                    }
                  r2 = strchr(r1, '/');
                  len = r2 - r1;
                  r2 = malloc(len+1);
                  memcpy(r2, r1, len);
                  r2[len] = '\0';
                  server->rc.Link.hostname = (const char *)r2;
                  r1 = strrchr(server->rc.Link.hostname, ':');
                  if (r1)
                    {
                      *r1++ = '\0';
                      server->rc.Link.port = atoi(r1);
                    }
                  else
                    {
                      server->rc.Link.port = 1935;
                    }
                }
              pval.av_val = NULL;
            }
          else if (AVMATCH(&pname, &av_pageUrl))
            {
              server->rc.Link.pageUrl = pval;
              pval.av_val = NULL;
            }
          else if (AVMATCH(&pname, &av_audioCodecs))
            {
              server->rc.m_fAudioCodecs = cobj.o_props[i].p_vu.p_number;
            }
          else if (AVMATCH(&pname, &av_videoCodecs))
            {
              server->rc.m_fVideoCodecs = cobj.o_props[i].p_vu.p_number;
            }
          else if (AVMATCH(&pname, &av_objectEncoding))
            {
              server->rc.m_fEncoding = cobj.o_props[i].p_vu.p_number;
            }
          /* Dup'd a string we didn't recognize? */
          if (pval.av_val)
            free(pval.av_val);
        }

      if (!RTMP_Connect(&server->rc))
        {
          /* failed */
          return 1;
        }
    }
  else if (AVMATCH(&method, &av_play))
    {
      char *file, *p;
      char flvHeader[] = { 'F', 'L', 'V', 0x01,
         0x05,                       // video + audio, we finalize later if the value is different
         0x00, 0x00, 0x00, 0x09,
         0x00, 0x00, 0x00, 0x00      // first prevTagSize=0
       };

      AMFProp_GetString(AMF_GetProp(&obj, NULL, 3), &server->rc.Link.playpath);
      file = malloc(server->rc.Link.playpath.av_len+1);
      memcpy(file, server->rc.Link.playpath.av_val, server->rc.Link.playpath.av_len);
      file[server->rc.Link.playpath.av_len] = '\0';
      for (p=file; *p; p++)
        if (*p == '/')
          *p = '_';
        else if (*p == '?')
          {
            *p = '\0';
            break;
          }
      LogPrintf("Playpath: %.*s, writing to %s\n", server->rc.Link.playpath.av_len,
        server->rc.Link.playpath.av_val, file);
      server->out = fopen(file, "wb");
      if (!server->out)
        ret = 1;
      else
        fwrite(flvHeader, 1, sizeof(flvHeader), server->out);
    }
  else if (AVMATCH(&method, &av_onStatus))
    {
      AMFObject obj2;
      AVal code, level;
      AMFProp_GetObject(AMF_GetProp(&obj, NULL, 3), &obj2);
      AMFProp_GetString(AMF_GetProp(&obj2, &av_code, -1), &code);
      AMFProp_GetString(AMF_GetProp(&obj2, &av_level, -1), &level);

      Log(LOGDEBUG, "%s, onStatus: %s", __FUNCTION__, code.av_val);
      if (AVMATCH(&code, &av_NetStream_Failed)
	  || AVMATCH(&code, &av_NetStream_Play_Failed)
	  || AVMATCH(&code, &av_NetStream_Play_StreamNotFound)
	  || AVMATCH(&code, &av_NetConnection_Connect_InvalidApp))
	{
	  ret = 1;
	}

      if (AVMATCH(&code, &av_NetStream_Play_Start))
	{
	  server->rc.m_bPlaying = true;
	}

      // Return 1 if this is a Play.Complete or Play.Stop
      if (AVMATCH(&code, &av_NetStream_Play_Complete)
	  || AVMATCH(&code, &av_NetStream_Play_Stop))
	{
	  ret = 1;
	}
    }
  AMF_Reset(&obj);
  return ret;
}

int
ServePacket(STREAMING_SERVER *server, RTMPPacket *packet)
{
  int ret = 0;

  Log(LOGDEBUG, "%s, received packet type %02X, size %lu bytes", __FUNCTION__,
    packet->m_packetType, packet->m_nBodySize);

  switch (packet->m_packetType)
    {
    case 0x01:
      // chunk size
//      HandleChangeChunkSize(r, packet);
      break;

    case 0x03:
      // bytes read report
      break;

    case 0x04:
      // ctrl
//      HandleCtrl(r, packet);
      break;

    case 0x05:
      // server bw
//      HandleServerBW(r, packet);
      break;

    case 0x06:
      // client bw
 //     HandleClientBW(r, packet);
      break;

    case 0x08:
      // audio data
      //Log(LOGDEBUG, "%s, received: audio %lu bytes", __FUNCTION__, packet.m_nBodySize);
      break;

    case 0x09:
      // video data
      //Log(LOGDEBUG, "%s, received: video %lu bytes", __FUNCTION__, packet.m_nBodySize);
      break;

    case 0x0F:			// flex stream send
      break;

    case 0x10:			// flex shared object
      break;

    case 0x11:			// flex message
      {
	Log(LOGDEBUG, "%s, flex message, size %lu bytes, not fully supported",
	    __FUNCTION__, packet->m_nBodySize);
	//LogHex(packet.m_body, packet.m_nBodySize);

	// some DEBUG code
	/*RTMP_LIB_AMFObject obj;
	   int nRes = obj.Decode(packet.m_body+1, packet.m_nBodySize-1);
	   if(nRes < 0) {
	   Log(LOGERROR, "%s, error decoding AMF3 packet", __FUNCTION__);
	   //return;
	   }

	   obj.Dump(); */

	ret = ServeInvoke(server, packet, packet->m_body + 1);
	break;
      }
    case 0x12:
      // metadata (notify)
      break;

    case 0x13:
      /* shared object */
      break;

    case 0x14:
      // invoke
      Log(LOGDEBUG, "%s, received: invoke %lu bytes", __FUNCTION__,
	  packet->m_nBodySize);
      //LogHex(packet.m_body, packet.m_nBodySize);

      ret = ServeInvoke(server, packet, packet->m_body);
      break;

    case 0x16:
      /* flv */
	break;
    default:
      Log(LOGDEBUG, "%s, unknown packet type received: 0x%02x", __FUNCTION__,
	  packet->m_packetType);
#ifdef _DEBUG
      LogHex(LOGDEBUG, packet->m_body, packet->m_nBodySize);
#endif
    }
  return ret;
}

int
WriteStream(char **buf,	// target pointer, maybe preallocated
	    unsigned int *plen,	// length of buffer if preallocated
            uint32_t *nTimeStamp,
            RTMPPacket *packet)
{
  uint32_t prevTagSize = 0;
  int ret = -1, len = *plen;

  while (1)
    {
      char *packetBody = packet->m_body;
      unsigned int nPacketLen = packet->m_nBodySize;

      // skip video info/command packets
      if (packet->m_packetType == 0x09 &&
	  nPacketLen == 2 && ((*packetBody & 0xf0) == 0x50))
	{
	  ret = 0;
	  break;
	}

      if (packet->m_packetType == 0x09 && nPacketLen <= 5)
	{
	  Log(LOGWARNING, "ignoring too small video packet: size: %d",
	      nPacketLen);
	  ret = 0;
	  break;
	}
      if (packet->m_packetType == 0x08 && nPacketLen <= 1)
	{
	  Log(LOGWARNING, "ignoring too small audio packet: size: %d",
	      nPacketLen);
	  ret = 0;
	  break;
	}
#ifdef _DEBUG
      Log(LOGDEBUG, "type: %02X, size: %d, TS: %d ms", packet->m_packetType,
	  nPacketLen, packet->m_nTimeStamp);
      if (packet->m_packetType == 0x09)
	Log(LOGDEBUG, "frametype: %02X", (*packetBody & 0xf0));
#endif

      // calculate packet size and reallocate buffer if necessary
      unsigned int size = nPacketLen
	+
	((packet->m_packetType == 0x08 || packet->m_packetType == 0x09
	  || packet->m_packetType == 0x12) ? 11 : 0) + (packet->m_packetType !=
						       0x16 ? 4 : 0);

      if (size + 4 > len)
	{			// the extra 4 is for the case of an FLV stream without a last prevTagSize (we need extra 4 bytes to append it)
	  *buf = (char *) realloc(*buf, size + 4);
	  if (*buf == 0)
	    {
	      Log(LOGERROR, "Couldn't reallocate memory!");
	      ret = -1;		// fatal error
	      break;
	    }
	}
      char *ptr = *buf, *pend = ptr + size+4;

      // audio (0x08), video (0x09) or metadata (0x12) packets :
      // construct 11 byte header then add rtmp packet's data
      if (packet->m_packetType == 0x08 || packet->m_packetType == 0x09
	  || packet->m_packetType == 0x12)
	{
	  // set data type
	  //*dataType |= (((packet->m_packetType == 0x08)<<2)|(packet->m_packetType == 0x09));

	  (*nTimeStamp) = packet->m_nTimeStamp;
	  prevTagSize = 11 + nPacketLen;

	  *ptr++ = packet->m_packetType;
	  ptr = AMF_EncodeInt24(ptr, pend, nPacketLen);
	  ptr = AMF_EncodeInt24(ptr, pend, *nTimeStamp);
	  *ptr = (char) (((*nTimeStamp) & 0xFF000000) >> 24);
	  ptr++;

	  // stream id
	  ptr = AMF_EncodeInt24(ptr, pend, 0);
	}

      memcpy(ptr, packetBody, nPacketLen);
      unsigned int len = nPacketLen;

      // correct tagSize and obtain timestamp if we have an FLV stream
      if (packet->m_packetType == 0x16)
	{
	  unsigned int pos = 0;

	  while (pos + 11 < nPacketLen)
	    {
	      uint32_t dataSize = AMF_DecodeInt24(packetBody + pos + 1);	// size without header (11) and without prevTagSize (4)
	      *nTimeStamp = AMF_DecodeInt24(packetBody + pos + 4);
	      *nTimeStamp |= (packetBody[pos + 7] << 24);

	      // set data type
	      //*dataType |= (((*(packetBody+pos) == 0x08)<<2)|(*(packetBody+pos) == 0x09));

	      if (pos + 11 + dataSize + 4 > nPacketLen)
		{
		  if (pos + 11 + dataSize > nPacketLen)
		    {
		      Log(LOGERROR,
			  "Wrong data size (%lu), stream corrupted, aborting!",
			  dataSize);
		      ret = -2;
		      break;
		    }
		  Log(LOGWARNING, "No tagSize found, appending!");

		  // we have to append a last tagSize!
		  prevTagSize = dataSize + 11;
		  AMF_EncodeInt32(ptr + pos + 11 + dataSize, pend, prevTagSize);
		  size += 4;
		  len += 4;
		}
	      else
		{
		  prevTagSize =
		    AMF_DecodeInt32(packetBody + pos + 11 + dataSize);

#ifdef _DEBUG
		  Log(LOGDEBUG,
		      "FLV Packet: type %02X, dataSize: %lu, tagSize: %lu, timeStamp: %lu ms",
		      (unsigned char) packetBody[pos], dataSize, prevTagSize,
		      *nTimeStamp);
#endif

		  if (prevTagSize != (dataSize + 11))
		    {
#ifdef _DEBUG
		      Log(LOGWARNING,
			  "Tag and data size are not consitent, writing tag size according to dataSize+11: %d",
			  dataSize + 11);
#endif

		      prevTagSize = dataSize + 11;
		      AMF_EncodeInt32(ptr + pos + 11 + dataSize, pend, prevTagSize);
		    }
		}

	      pos += prevTagSize + 4;	//(11+dataSize+4);
	    }
	}
      ptr += len;

      if (packet->m_packetType != 0x16)
	{			// FLV tag packets contain their own prevTagSize
	  AMF_EncodeInt32(ptr, pend, prevTagSize);
	  //ptr += 4;
	}

      ret = size;
      break;
    }

  if (len > *plen)
    *plen = len;

  return ret;			// no more media packets
}

TFTYPE
controlServerThread(void *unused)
{
  char ich;
  while (1)
    {
      ich = getchar();
      switch (ich)
	{
	case 'q':
	  LogPrintf("Exiting\n");
	  stopStreaming(rtmpServer);
	  exit(0);
	  break;
	default:
	  LogPrintf("Unknown command \'%c\', ignoring\n", ich);
	}
    }
  TFRET();
}

void doServe(STREAMING_SERVER * server,	// server socket and state (our listening socket)
  int sockfd	// client connection socket
  )
{
  RTMPPacket pc = { 0 }, ps = { 0 };
  char *buf;
  unsigned int buflen = 131072;
  bool paused = false;

  // timeout for http requests
  fd_set rfds;
  struct timeval tv;

  server->state = STREAMING_IN_PROGRESS;

  memset(&tv, 0, sizeof(struct timeval));
  tv.tv_sec = 5;

  FD_ZERO(&rfds);
  FD_SET(sockfd, &rfds);

  if (select(sockfd + 1, &rfds, NULL, NULL, &tv) <= 0)
    {
      Log(LOGERROR, "Request timeout/select failed, ignoring request");
      goto quit;
    }
  else
    {
      RTMP_Init(&server->rs);
      RTMP_Init(&server->rc);
      server->rs.m_socket = sockfd;
      if (!RTMP_Serve(&server->rs))
        {
          Log(LOGERROR, "Handshake failed");
          goto cleanup;
        }
    }

  buf = malloc(buflen);

  /* Just process the Connect request */
  while (RTMP_IsConnected(&server->rs) && RTMP_ReadPacket(&server->rs, &ps))
    {
      if (!RTMPPacket_IsReady(&ps))
        continue;
      ServePacket(server, &ps);
      RTMPPacket_Free(&ps);
      if (RTMP_IsConnected(&server->rc))
        break;
    }

  /* We have our own timeout in select() */
  server->rc.Link.timeout = 10;
  server->rs.Link.timeout = 10;
  while (RTMP_IsConnected(&server->rs) || RTMP_IsConnected(&server->rc))
    {
      int n;
      int sr, cr;

      cr = server->rc.m_nBufferSize;
      sr = server->rs.m_nBufferSize;

      if (cr || sr)
        {
        }
      else
        {
          n = server->rs.m_socket;
	  if (server->rc.m_socket > n)
	    n = server->rc.m_socket;
	  FD_ZERO(&rfds);
	  if (RTMP_IsConnected(&server->rs))
	    FD_SET(sockfd, &rfds);
	  if (RTMP_IsConnected(&server->rc))
	    FD_SET(server->rc.m_socket, &rfds);

          /* give more time to start up if we're not playing yet */
	  tv.tv_sec = server->out ? 30 : 60;
	  tv.tv_usec = 0;

	  if (select(n + 1, &rfds, NULL, NULL, &tv) <= 0)
	    {
              if (server->out && server->rc.m_mediaChannel && !paused)
                {
                  server->rc.m_pauseStamp = server->rc.m_channelTimestamp[server->rc.m_mediaChannel];
                  if (RTMP_ToggleStream(&server->rc))
                    {
                      paused = true;
                      continue;
                    }
                }
	      Log(LOGERROR, "Request timeout/select failed, ignoring request");
	      goto cleanup;
	    }
          if (FD_ISSET(server->rs.m_socket, &rfds))
            sr = 1;
          if (FD_ISSET(server->rc.m_socket, &rfds))
            cr = 1;
        }
      if (sr)
        {
          while (RTMP_ReadPacket(&server->rs, &ps))
            if (RTMPPacket_IsReady(&ps))
              {
                /* change chunk size */
                if (ps.m_packetType == 0x01)
                  {
                    if (ps.m_nBodySize >= 4)
                      {
                        server->rs.m_inChunkSize = AMF_DecodeInt32(ps.m_body);
                        Log(LOGDEBUG, "%s, client: chunk size change to %d", __FUNCTION__,
                            server->rs.m_inChunkSize);
                        server->rc.m_outChunkSize = server->rs.m_inChunkSize;
                      }
                  }
                /* ctrl */
                else if (ps.m_packetType == 0x04)
                  {
                    short nType = AMF_DecodeInt16(ps.m_body);
                    /* UpdateBufferMS */
                    if (nType == 0x03)
                      {
                        char *ptr = ps.m_body+2;
                        int id;
                        int len;
                        id = AMF_DecodeInt32(ptr);
                        /* Assume the interesting media is on a non-zero stream */
                        if (id)
                          {
                            len = AMF_DecodeInt32(ptr+4);
                            /* request a big buffer */
                            if (len < BUFFERTIME)
                              {
                                AMF_EncodeInt32(ptr+4, ptr+8, BUFFERTIME);
                              }
                            Log(LOGDEBUG, "%s, client: BufferTime change in stream %d to %d", __FUNCTION__,
                                id, len);
                          }
                      }
                  }
                else if (!server->out && (ps.m_packetType == 0x11 || ps.m_packetType == 0x14))
                  ServePacket(server, &ps);
                RTMP_SendPacket(&server->rc, &ps, false);
                RTMPPacket_Free(&ps);
                break;
              }
        }
      if (cr)
        {
          while (RTMP_ReadPacket(&server->rc, &pc))
            if (RTMPPacket_IsReady(&pc))
              {
                int sendit = 1;
                if (paused)
                  {
                    if (pc.m_nTimeStamp <= server->rc.m_mediaStamp)
                      continue;
                    paused = 0;
                    server->rc.m_pausing = 0;
                  }
                /* change chunk size */
                if (pc.m_packetType == 0x01)
                  {
                    if (pc.m_nBodySize >= 4)
                      {
                        server->rc.m_inChunkSize = AMF_DecodeInt32(pc.m_body);
                        Log(LOGDEBUG, "%s, server: chunk size change to %d", __FUNCTION__,
                            server->rc.m_inChunkSize);
                        server->rs.m_outChunkSize = server->rc.m_inChunkSize;
                      }
                  }
                else if (pc.m_packetType == 0x04)
                  {
                    short nType = AMF_DecodeInt16(pc.m_body);
                    /* SWFverification */
                    if (nType == 0x1a && server->rc.Link.SWFHash.av_len)
                      {
                        RTMP_SendCtrl(&server->rc, 0x1b, 0, 0);
                        sendit = 0;
                      }
                  }
                else if (server->out && (
                     pc.m_packetType == 0x08 ||
                     pc.m_packetType == 0x09 ||
                     pc.m_packetType == 0x12 ||
                     pc.m_packetType == 0x16) &&
                     RTMP_ClientPacket(&server->rc, &pc))
                  {
                    int len = WriteStream(&buf, &buflen, &server->stamp, &pc);
                    if (len > 0 && fwrite(buf, 1, len, server->out) != len)
                      goto cleanup;
                  }
                else if (server->out && (
                     pc.m_packetType == 0x11 ||
                     pc.m_packetType == 0x14))
                  {
                    if (ServePacket(server, &pc))
                      {
                        fclose(server->out);
                        server->out = NULL;
                      }
                  }

                if (sendit && RTMP_IsConnected(&server->rs))
                  RTMP_SendPacket(&server->rs, &pc, false);
                RTMPPacket_Free(&pc);
                break;
              }
        }
    }

cleanup:
  LogPrintf("Closing connection... ");
  RTMP_Close(&server->rs);
  if (server->out)
    {
      fclose(server->out);
      server->out = NULL;
    }
  /* Should probably be done by RTMP_Close() ... */
  free((void *)server->rc.Link.hostname);
  server->rc.Link.hostname = NULL;
  free(server->rc.Link.tcUrl.av_val);
  server->rc.Link.tcUrl.av_val = NULL;
  free(server->rc.Link.swfUrl.av_val);
  server->rc.Link.swfUrl.av_val = NULL;
  free(server->rc.Link.pageUrl.av_val);
  server->rc.Link.pageUrl.av_val = NULL;
  free(server->rc.Link.app.av_val);
  server->rc.Link.app.av_val = NULL;
  free(server->rc.Link.auth.av_val);
  server->rc.Link.auth.av_val = NULL;
  free(server->rc.Link.flashVer.av_val);
  server->rc.Link.flashVer.av_val = NULL;
  LogPrintf("done!\n\n");

quit:
  if (server->state == STREAMING_IN_PROGRESS)
    server->state = STREAMING_ACCEPTING;

  return;
}

TFTYPE
serverThread(void *arg)
{
  STREAMING_SERVER *server = arg;
  server->state = STREAMING_ACCEPTING;

  while (server->state == STREAMING_ACCEPTING)
    {
      struct sockaddr_in addr;
      socklen_t addrlen = sizeof(struct sockaddr_in);
      int sockfd =
	accept(server->socket, (struct sockaddr *) &addr, &addrlen);

      if (sockfd > 0)
	{
#ifdef linux
          struct sockaddr_in dest;
	  char destch[16];
          socklen_t destlen = sizeof(struct sockaddr_in);
	  getsockopt(sockfd, SOL_IP, SO_ORIGINAL_DST, &dest, &destlen);
          strcpy(destch, inet_ntoa(dest.sin_addr));
	  Log(LOGDEBUG, "%s: accepted connection from %s to %s\n", __FUNCTION__,
	      inet_ntoa(addr.sin_addr), destch);
#else
	  Log(LOGDEBUG, "%s: accepted connection from %s\n", __FUNCTION__,
	      inet_ntoa(addr.sin_addr));
#endif
	  /* Create a new thread and transfer the control to that */
	  doServe(server, sockfd);
	  Log(LOGDEBUG, "%s: processed request\n", __FUNCTION__);
	}
      else
	{
	  Log(LOGERROR, "%s: accept failed", __FUNCTION__);
	}
    }
  server->state = STREAMING_STOPPED;
  TFRET();
}

STREAMING_SERVER *
startStreaming(const char *address, int port)
{
  struct sockaddr_in addr;
  int sockfd, tmp;
  STREAMING_SERVER *server;

  sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sockfd == -1)
    {
      Log(LOGERROR, "%s, couldn't create socket", __FUNCTION__);
      return 0;
    }

  tmp = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
				(char *) &tmp, sizeof(tmp) );

  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(address);	//htonl(INADDR_ANY);
  addr.sin_port = htons(port);

  if (bind(sockfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in)) ==
      -1)
    {
      Log(LOGERROR, "%s, TCP bind failed for port number: %d", __FUNCTION__,
	  port);
      return 0;
    }

  if (listen(sockfd, 10) == -1)
    {
      Log(LOGERROR, "%s, listen failed", __FUNCTION__);
      closesocket(sockfd);
      return 0;
    }

  server = (STREAMING_SERVER *) calloc(1, sizeof(STREAMING_SERVER));
  server->socket = sockfd;

  ThreadCreate(serverThread, server);

  return server;
}

void
stopStreaming(STREAMING_SERVER * server)
{
  assert(server);

  if (server->state != STREAMING_STOPPED)
    {
      if (server->state == STREAMING_IN_PROGRESS)
	{
	  server->state = STREAMING_STOPPING;

	  // wait for streaming threads to exit
	  while (server->state != STREAMING_STOPPED)
	    msleep(1);
	}

      if (closesocket(server->socket))
	Log(LOGERROR, "%s: Failed to close listening socket, error %d",
	    GetSockError());

      server->state = STREAMING_STOPPED;
    }
}


void
sigIntHandler(int sig)
{
  RTMP_ctrlC = true;
  LogPrintf("Caught signal: %d, cleaning up, just a second...\n", sig);
  if (rtmpServer)
    stopStreaming(rtmpServer);
  signal(SIGINT, SIG_DFL);
}

int
main(int argc, char **argv)
{
  int nStatus = RD_SUCCESS;

  // rtmp streaming server
  char DEFAULT_RTMP_STREAMING_DEVICE[] = "0.0.0.0";	// 0.0.0.0 is any device

  char *rtmpStreamingDevice = DEFAULT_RTMP_STREAMING_DEVICE;	// streaming device, default 0.0.0.0
  int nRtmpStreamingPort = 1935;	// port

  LogPrintf("RTMP Proxy Server %s\n", FLVSTREAMER_PROXY_VERSION);
  LogPrintf("(c) 2009 Andrej Stepanchuk, Howard Chu; license: GPL\n\n");

  debuglevel = LOGDEBUG;

  if (argc > 1 && !strcmp(argv[1], "-z"))
    debuglevel = LOGALL;

  signal(SIGINT, sigIntHandler);
#ifndef WIN32
  signal(SIGPIPE, SIG_IGN);
#endif

#ifdef _DEBUG
  netstackdump = fopen("netstackdump", "wb");
  netstackdump_read = fopen("netstackdump_read", "wb");
#endif

  InitSockets();

  // start text UI
  ThreadCreate(controlServerThread, 0);

  // start http streaming
  if ((rtmpServer =
       startStreaming(rtmpStreamingDevice, nRtmpStreamingPort)) == 0)
    {
      Log(LOGERROR, "Failed to start RTMP server, exiting!");
      return RD_FAILED;
    }
  LogPrintf("Streaming on rtmp://%s:%d\n", rtmpStreamingDevice,
	    nRtmpStreamingPort);

  while (rtmpServer->state != STREAMING_STOPPED)
    {
      sleep(1);
    }
  Log(LOGDEBUG, "Done, exiting...");

  CleanupSockets();

#ifdef _DEBUG
  if (netstackdump != 0)
    fclose(netstackdump);
  if (netstackdump_read != 0)
    fclose(netstackdump_read);
#endif
  return nStatus;
}