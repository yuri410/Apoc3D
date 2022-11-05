#pragma once

#include "apoc3D.Essentials/EssentialCommon.h"
#include "apoc3D.Essentials/System/Optional.h"
#include "apoc3d/IOLib/Streams.h"

struct mg_connection;
struct mg_context;

using namespace Apoc3D::IO;

namespace Apoc3D
{
	namespace Network
	{
		struct TrafficStats
		{
			std::atomic_ulong m_uploadedBytes = 0;
			std::atomic_ulong m_downloadedBytes = 0;

			std::atomic_int m_uploadedBytesTimed = 0;
			std::atomic_int m_downloadedBytesTimed = 0;

			int m_uploadSpeed = 0;
			int m_downloadSpeed = 0;

			float m_measureTimer = 0;
		};

		class HttpServer;

		class HttpConnection
		{
		public:
			HttpConnection(mg_connection* conn, TrafficStats& stats);
			~HttpConnection();
			
			HttpConnection(const HttpConnection&) = delete;
			HttpConnection& operator=(const HttpConnection&) = delete;

			int  Read(void* data, int len);
			bool Write(const void* data, int len);
			bool Print(const std::string& str);
			bool SendResponseHeader(const char* mime, int length);
			bool SendResponseHeaderChunked(const char* mime);
			bool SendResponse(const char* mime, const std::string& data);
			bool SendResponseJson(const json& j);
			bool SendResponseJsonError(const char* error = "InvalidOperation");

			std::string GetRequestUri() const;
			std::string GetRequestUriLocal() const;

			bool GetCookies(const std::string& cookieName, std::string& cookieValue);
			bool GetHeader(const std::string& headerName, std::string& headerValue);
			bool GetParam(const std::string& paramName, std::string& paramValue, int occurrence = 0);

		private:
			struct Param
			{
				std::string m_name;
				std::string m_value;
			};
			typedef List<Param> ParamList;

			ParamList BuildParamList(const std::string& str);
			std::string ReadPostData();

			int FlushChunk();

			mg_connection*		m_connection;
			bool				m_chunked = false;
			List<char>			m_chunkBuffer;

			Optional<ParamList> m_getData;
			Optional<ParamList> m_postData;

			TrafficStats& m_stats;
		};

		class HttpOutStream : public Stream
		{
		public:
			HttpOutStream(HttpConnection& conn);
			virtual ~HttpOutStream();

			virtual bool IsReadEndianIndependent() const override { return false; }
			virtual bool IsWriteEndianIndependent() const override { return false; }

			virtual bool CanRead() const override { return false; }
			virtual bool CanWrite() const override { return true; }

			virtual int64 getLength() const override { return 0; }

			virtual void setPosition(int64 offset) override { }
			virtual int64 getPosition() override { return 0; }

			virtual int64 Read(char* dest, int64 count) override { return -1; }
			virtual void Write(const char* src, int64 count) override;

			virtual void Seek(int64 offset, SeekMode mode) override { }

			virtual void Flush() override;

		private:
			HttpConnection& m_conn;
		};

		class HttpConnectionConst
		{
		public:
			HttpConnectionConst(const mg_connection* conn);
			~HttpConnectionConst();

		private:
			const mg_connection* m_connection;
		};

		class HttpHandler
		{
		public:
			virtual ~HttpHandler() { }
			
			// All handlers: returns true if implemented, false otherwise
			
			/** GET request */
			virtual bool OnGet(HttpServer* server, HttpConnection& conn);
			
			/** POST request */
			virtual bool OnPost(HttpServer* server, HttpConnection& conn);

			/** HEAD request */
			virtual bool OnHead(HttpServer* server, HttpConnection& conn);

			/** PUT request */
			virtual bool OnPut(HttpServer* server, HttpConnection& conn);

			/** DELETE request */
			virtual bool OnDelete(HttpServer* server, HttpConnection& conn);

			/** OPTIONS request */
			virtual bool OnOptions(HttpServer* server, HttpConnection& conn);

			/** PATCH request */
			virtual bool OnPatch(HttpServer* server, HttpConnection& conn);

		};

		class HttpAuthHandler
		{
		public:

			virtual ~HttpAuthHandler() { }

			/**
			 * Authorization requests
			 * It is up the this handler to generate 401 responses if authorization fails.
			 *
			 * returns true if authorization succeeded, false otherwise
			 */
			virtual bool OnAuthorize(HttpServer* server, HttpConnection& conn) = 0;
		};

		class WebSocketHandler
		{
		public:

			virtual ~WebSocketHandler() { }

			/**
			 * Happens when the client intends to establish a websocket connection, before websocket handshake.
			 *
			 * returns true to keep socket open, false to close it
			 */
			virtual bool OnConnection(HttpServer* server, HttpConnectionConst conn);

			/**
			 * Callback method for when websocket handshake is successfully completed,
			 * and connection is ready for data exchange. 
			 */
			virtual void OnReadyState(HttpServer* server, HttpConnection& conn);

			/**
			 * Callback method for when a data frame has been received from the client.
			 *
			 * @bits: first byte of the websocket frame, see websocket RFC at
			 * http://tools.ietf.org/html/rfc6455, section 5.2
			 * @data, data_len: payload, with mask (if any) already applied.
			 * returns true to keep socket open, false to close it
			 */
			virtual bool OnData(HttpServer* server, HttpConnection& conn, int bits, char* data, size_t data_len);

			/** Callback method for when the connection is closed. */
			virtual void OnClose(HttpServer* server, HttpConnectionConst conn);
		};


		class HttpServer
		{
		public:
			HttpServer(const List<std::string>& options);
			~HttpServer();

			/** Stops server and frees resources. */
			void Close();
			
			/**
			 * Adds a URI handler. 
			 * If there is existing URI handler, it will be replaced.
			 */
			void AddHandler(const std::string &uri, HttpHandler* handler);

			/**
			 * Adds a WebSocket handler for a specific URI.
			 * If there is existing URI handler, it will be replaced.
			 */
			void AddWebSocketHandler(const std::string& uri, WebSocketHandler* handler);

			/**
			 * Adds a URI authorization handler.
			 * If there is existing URI authorization handler, it will be replaced.
			 */
			void AddAuthHandler(const std::string& uri, HttpAuthHandler* handler);

			void RemoveHandler(const std::string& uri);
			void RemoveWebSocketHandler(const std::string& uri);
			void RemoveAuthHandler(const std::string& uri);

			void UpdateStats(const Core::AppTime* time);

			List<int> GetListeningPorts() const;

			const TrafficStats& getTrafficStats() const { return m_trafficStats; }

			bool isOperational() const { return m_context != nullptr; }
			
		protected:

			mg_context* m_context = nullptr;

			//HashMap<mg_connection*, HttpConnection> m_connections;

		private:

			TrafficStats m_trafficStats;

			static int  OnRequest(mg_connection* conn, void* _handler);
			static int  OnAuth(mg_connection* conn, void* _handler);

			static int  OnOpen(const mg_connection* conn, void** conn_data);
			static void OnClose(const mg_connection* conn);

			static int  OnWSConnection(const mg_connection* conn, void* _handler);
			static void OnWSReady(mg_connection* conn, void* _handler);
			static int  OnWSData(mg_connection* conn, int bits, char* data, size_t data_len, void* _handler);
			static void OnWSClose(const mg_connection* conn, void* _handler);

		};
	}
}