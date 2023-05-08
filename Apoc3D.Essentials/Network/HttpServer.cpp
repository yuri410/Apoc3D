#include "HttpServer.h"

#include "civetweb.h"
#include "apoc3D/Core/AppTime.h"
#include "apoc3d.Essentials/Utils/Json.h"
#include "apoc3d/Utility/StringUtils.h"
#include "apoc3D.Essentials/System/Logger.h"

namespace
{
	const int MaxParamBodyLength = 1024 * 1024 * 16;

	void UrlDecode(const char* src, size_t src_len, std::string& dst, bool is_form_url_encoded = true)
	{
		int i, j, a, b;
#define HEXTOI(x) (isdigit(x) ? x - '0' : x - 'W')

		dst.clear();
		for (i = j = 0; i < (int)src_len; i++, j++)
		{
			if (i < (int)src_len - 2 && src[i] == '%'
				&& isxdigit(*(const unsigned char*)(src + i + 1))
				&& isxdigit(*(const unsigned char*)(src + i + 2))) {
				a = tolower(*(const unsigned char*)(src + i + 1));
				b = tolower(*(const unsigned char*)(src + i + 2));
				dst.push_back((char)((HEXTOI(a) << 4) | HEXTOI(b)));
				i += 2;
			}
			else if (is_form_url_encoded && src[i] == '+') {
				dst.push_back(' ');
			}
			else {
				dst.push_back(src[i]);
			}
		}
	}

	std::string UrlDecode(const std::string& src, bool is_form_url_encoded = true)
	{
		std::string dst;
		UrlDecode(src.c_str(), src.length(), dst, is_form_url_encoded);
		return dst;
	}
}

namespace Apoc3D
{
	namespace Network
	{
#pragma region HttpConnection
		//////////////////////////////////////////////////////////////////////////

		HttpConnection::HttpConnection(mg_connection* conn, TrafficStats& stats)
			: m_connection(conn), m_stats(stats)
		{
			
		}
		HttpConnection::~HttpConnection()
		{
			if (m_chunked)
			{
				if (m_chunkBuffer.getCount() > 0)
				{
					int ret = FlushChunk();
					if (ret > 0)
					{
						m_stats.m_uploadedBytes += ret;
						m_stats.m_uploadedBytesTimed += ret;
					}
				}

				mg_send_chunk(m_connection, nullptr, 0);
			}
		}

		int HttpConnection::Read(void* data, int len)
		{
			int ret = mg_read(m_connection, data, len);
			if (ret > 0)
			{
				m_stats.m_downloadedBytes += ret;
				m_stats.m_downloadedBytesTimed += ret;
			}
			return ret;
		}

		bool HttpConnection::Write(const void* data, int len)
		{
			int ret;
			if (m_chunked)
			{
				m_chunkBuffer.AddArray((const char*)data, len);
				ret = len;

				if (m_chunkBuffer.getCount() > 4096)
					FlushChunk();
			}
			else
			{
				ret = mg_write(m_connection, data, len);
			}
			
			if (ret > 0)
			{
				m_stats.m_uploadedBytes += ret;
				m_stats.m_uploadedBytesTimed += ret;
			}
			return ret > 0;
		}

		bool HttpConnection::Print(const std::string& str)
		{
			return Write(str.c_str(), (int)str.length());
		}

		bool HttpConnection::SendResponseHeader(const char* mime, int length)
		{
			return mg_send_http_ok(m_connection, mime, length) >= 0;
		}

		bool HttpConnection::SendResponseHeaderChunked(const char* mime)
		{
			m_chunked = true;
			return SendResponseHeader(mime, -1);
			/*mg_printf(m_connection,
					  "HTTP/1.1 200 OK\r\n"
					  "Content-Type: %s\r\n"
					  "Connection: %s\r\n",
					  mime,
					  "close");

			mg_printf(m_connection, "Transfer-Encoding: chunked\r\n\r\n");
			return true;*/
		}

		bool HttpConnection::SendResponse(const char* mime, const std::string& data)
		{
			return SendResponseHeader(mime, (int)data.length()) && Print(data);
		}

		bool HttpConnection::SendResponseJson(const json& j)
		{
			return SendResponse("text", j.dump());
		}

		bool HttpConnection::SendResponseJsonError(const char* error)
		{
			return SendResponseJson({ {"error", error} });
		}

		std::string HttpConnection::GetRequestUri() const
		{
			const mg_request_info* request_info = mg_get_request_info(m_connection);
			assert(request_info);
			return request_info->request_uri;
		}

		std::string HttpConnection::GetRequestUriLocal() const
		{
			const mg_request_info* request_info = mg_get_request_info(m_connection);
			assert(request_info);
			return request_info->local_uri;
		}

		bool HttpConnection::GetCookies(const std::string& cookieName, std::string& cookieValue)
		{
			// Maximum cookie length as per microsoft is 4096.
			// http://msdn.microsoft.com/en-us/library/ms178194.aspx
			char _cookieValue[4096];
			const char* cookie = mg_get_header(m_connection, "Cookie");
			int lRead = mg_get_cookie(cookie, cookieName.c_str(), _cookieValue, sizeof(_cookieValue));
			cookieValue = _cookieValue;
			return lRead >= 0;
		}

		bool HttpConnection::GetHeader(const std::string& headerName, std::string& headerValue)
		{
			const char* h = mg_get_header(m_connection, headerName.c_str());
			if (h)
			{
				headerValue = h;
				return true;
			}
			return false;
		}

		bool HttpConnection::GetParam(const std::string& paramName, std::string& paramValue, int occurrence)
		{
			if (!m_postData.isSet())
			{
				const char* con_len_str = mg_get_header(m_connection, "Content-Length");
				if (con_len_str)
				{
					uint32 con_len;
					if (!StringUtils::ParseUInt32(con_len_str, con_len))
					{
						// invalid header
						return false;
					}

					if ((con_len > 0) && (con_len <= MaxParamBodyLength))
					{
						// Body is within a reasonable range
						std::string postString = ReadPostData();
						m_postData = BuildParamList(postString);
					}
				}
			}

			if (!m_getData.isSet())
			{
				const mg_request_info* ri = mg_get_request_info(m_connection);
				assert(ri != NULL);

				if (ri->query_string)
					m_getData = BuildParamList(ri->query_string);
			}

			ParamList* lists[] = 
			{
				m_postData.isSet() ? &m_postData.getContent() : nullptr,
				m_getData.isSet() ? &m_getData.getContent() : nullptr
			};

			bool found = false;
			for (ParamList* pl : lists)
			{
				if (pl == nullptr)
					continue;

				for (Param& p : *pl)
				{
					if (StringUtils::EqualsNoCase(p.m_name, paramName) &&
						occurrence-- == 0)
					{
						paramValue = p.m_value;
						found = true;
						break;
					}
				}
			}
			return found;
		}

		HttpConnection::ParamList HttpConnection::BuildParamList(const std::string& str)
		{
			return StringUtils::SplitParse<std::string, ParamList, Param>(str, '&', [](const std::string & part)
			{
				Param p;

				size_t pos = part.find('=');
				if (pos == std::string::npos)
				{
					p.m_name = UrlDecode(part);
				}
				else
				{
					p.m_name = UrlDecode(part.substr(0, pos));
					p.m_value = UrlDecode(part.substr(pos + 1));
				}

				return p;
			});
		}

		std::string HttpConnection::ReadPostData()
		{
			//mg_lock_connection(m_connection);
			std::string postdata;
			char buf[4096];
			int r = mg_read(m_connection, buf, sizeof(buf));
			while (r > 0)
			{
				std::string p = std::string(buf, r);
				postdata += p;
				r = mg_read(m_connection, buf, sizeof(buf));
			}
			//mg_unlock_connection(m_connection);
			return postdata;
		}

		int HttpConnection::FlushChunk()
		{
			int r = mg_send_chunk(m_connection, &m_chunkBuffer[0], m_chunkBuffer.getCount());
			if (r > 0)
				m_chunkBuffer.Clear();
			return r;
		}

		HttpConnectionConst::HttpConnectionConst(const mg_connection* conn)
			: m_connection(conn)
		{
			
		}

		HttpConnectionConst::~HttpConnectionConst()
		{

		}

#pragma endregion

#pragma region HttpOutStream

		//////////////////////////////////////////////////////////////////////////
		HttpOutStream::HttpOutStream(HttpConnection& conn)
			: m_conn(conn)
		{

		}

		HttpOutStream::~HttpOutStream()
		{
			Flush();
		}

		void HttpOutStream::Write(const char* src, int64 count)
		{
			m_conn.Write(src, (int32)count);
		}

		void HttpOutStream::Flush()
		{
		}
		
#pragma endregion

#pragma region HttpHandler

		//////////////////////////////////////////////////////////////////////////
		bool HttpHandler::OnGet(HttpServer* server, HttpConnection& conn) { return false; }

		bool HttpHandler::OnPost(HttpServer* server, HttpConnection& conn) { return false; }

		bool HttpHandler::OnHead(HttpServer* server, HttpConnection& conn) { return false; }

		bool HttpHandler::OnPut(HttpServer* server, HttpConnection& conn) { return false; }

		bool HttpHandler::OnDelete(HttpServer* server, HttpConnection& conn) { return false; }

		bool HttpHandler::OnOptions(HttpServer* server, HttpConnection& conn) { return false; }

		bool HttpHandler::OnPatch(HttpServer* server, HttpConnection& conn) { return false; }

#pragma endregion

#pragma region WebSocketHandler
		
		//////////////////////////////////////////////////////////////////////////
		bool WebSocketHandler::OnConnection(HttpServer* server, HttpConnectionConst conn) { return true; }

		void WebSocketHandler::OnReadyState(HttpServer* server, HttpConnection& conn) { }

		bool WebSocketHandler::OnData(HttpServer* server, HttpConnection& conn, int bits, char* data, size_t data_len) { return true; }

		void WebSocketHandler::OnClose(HttpServer* server, HttpConnectionConst conn) { }

#pragma endregion

#pragma region HttpServer

		//////////////////////////////////////////////////////////////////////////

		HttpServer::HttpServer(const List<std::string>& options)
		{
			mg_callbacks callbacks;
			memset(&callbacks, 0, sizeof(callbacks));

			callbacks.init_connection = OnOpen;
			callbacks.connection_close = OnClose;

			List<const char*> pointers;
			pointers.ResizeDiscard(options.getCount());
			for (const std::string& e : options)
			{
				pointers.Add(e.c_str());
			}
			pointers.Add(0);

			m_context = mg_start(&callbacks, this, &pointers[0]);

			if (m_context == nullptr)
			{
				LogError("null context when constructing CivetServer. Possible problem binding to port.");
			}
		}

		HttpServer::~HttpServer()
		{
			Close();
		}


		void HttpServer::Close()
		{
			if (m_context)
			{
				mg_stop(m_context);
				m_context = 0;
			}
		}

		void HttpServer::AddHandler(const std::string& uri, HttpHandler* handler)
		{
			if (m_context)
				mg_set_request_handler(m_context, uri.c_str(), OnRequest, handler);
		}

		void HttpServer::AddWebSocketHandler(const std::string& uri, WebSocketHandler* handler)
		{
			if (m_context)
				mg_set_websocket_handler(m_context, uri.c_str(), OnWSConnection, OnWSReady, OnWSData, OnWSClose, handler);
		}


		void HttpServer::AddAuthHandler(const std::string& uri, HttpAuthHandler* handler)
		{
			if (m_context)
				mg_set_auth_handler(m_context, uri.c_str(), OnAuth, handler);
		}

		void HttpServer::RemoveHandler(const std::string& uri)
		{
			if (m_context)
				mg_set_request_handler(m_context, uri.c_str(), nullptr, nullptr);
		}

		void HttpServer::RemoveWebSocketHandler(const std::string& uri)
		{
			if (m_context)
				mg_set_websocket_handler(m_context, uri.c_str(), nullptr, nullptr, nullptr, nullptr, nullptr);
		}

		void HttpServer::RemoveAuthHandler(const std::string& uri)
		{
			if (m_context)
				mg_set_auth_handler(m_context, uri.c_str(), nullptr, nullptr);
		}

		List<int> HttpServer::GetListeningPorts() const
		{
			if (m_context == nullptr)
			{
				return {};
			}

			List<mg_server_ports> server_ports;
			server_ports.ReserveDiscard(50);

			int size = mg_get_server_ports(m_context, server_ports.getCount(), &server_ports[0]);
			if (size <= 0)
			{
				return {};
			}
			server_ports.Reserve(size);

			List<int> ports = List<int>(size);
			for (const mg_server_ports& p : server_ports)
			{
				ports.Add(p.port);
			}
			return ports;
		}

		void HttpServer::UpdateStats(const Core::AppTime* time)
		{
			m_trafficStats.m_measureTimer += time->ElapsedRealTime;

			if (m_trafficStats.m_measureTimer > 1)
			{
				m_trafficStats.m_measureTimer = 0;
				
				m_trafficStats.m_uploadSpeed = m_trafficStats.m_uploadedBytesTimed;
				m_trafficStats.m_downloadSpeed = m_trafficStats.m_downloadedBytesTimed;

				m_trafficStats.m_uploadedBytesTimed = 0;
				m_trafficStats.m_downloadedBytesTimed = 0;
			}
		}

		//////////////////////////////////////////////////////////////////////////

		int HttpServer::OnRequest(mg_connection* mgConn, void* dataPtr)
		{
			const mg_request_info* request_info = mg_get_request_info(mgConn);
			assert(request_info);

			HttpServer* me = (HttpServer*)(request_info->user_data);
			assert(me);

			if (!me->isOperational())
				return 0;

			const int sizeEstimate = (int)request_info->content_length;
			if (sizeEstimate > 0)
			{
				me->m_trafficStats.m_downloadedBytes += sizeEstimate;
				me->m_trafficStats.m_downloadedBytesTimed += sizeEstimate;
			}
			if (request_info->query_string)
			{
				int queryLen = strlen(request_info->query_string);
				me->m_trafficStats.m_downloadedBytes += queryLen;
				me->m_trafficStats.m_downloadedBytesTimed += queryLen;
			}

			if (dataPtr)
			{
				HttpConnection conn(mgConn, me->m_trafficStats);
				HttpHandler* handler = (HttpHandler*)dataPtr;

				if (strcmp(request_info->request_method, "GET") == 0)
				{
					return handler->OnGet(me, conn);
				}
				else if (strcmp(request_info->request_method, "POST") == 0)
				{
					return handler->OnPost(me, conn);
				}
				else if (strcmp(request_info->request_method, "HEAD") == 0)
				{
					return handler->OnHead(me, conn);
				}
				else if (strcmp(request_info->request_method, "PUT") == 0)
				{
					return handler->OnPut(me, conn);
				}
				else if (strcmp(request_info->request_method, "DELETE") == 0)
				{
					return handler->OnDelete(me, conn);
				}
				else if (strcmp(request_info->request_method, "OPTIONS") == 0)
				{
					return handler->OnOptions(me, conn);
				}
				else if (strcmp(request_info->request_method, "PATCH") == 0)
				{
					return handler->OnPatch(me, conn);
				}
			}

			//mg_lock_context(me->m_context);
			//me->m_connections.AddOrReplace(mgConn, std::move(conn));
			//mg_unlock_context(me->m_context);

			return 0;
		}

		int HttpServer::OnAuth(mg_connection* mgConn, void* dataPtr)
		{
			const mg_request_info* request_info = mg_get_request_info(mgConn);
			assert(request_info);

			HttpServer* me = (HttpServer*)(request_info->user_data);
			assert(me);

			if (!me->isOperational())
				return 0;

			const int sizeEstimate = (int)request_info->content_length;
			me->m_trafficStats.m_downloadedBytes += sizeEstimate;
			me->m_trafficStats.m_downloadedBytesTimed += sizeEstimate;

			if (dataPtr)
			{
				HttpAuthHandler* handler = (HttpAuthHandler*)dataPtr;
				
				return handler->OnAuthorize(me, HttpConnection(mgConn, me->m_trafficStats));
			}

			//mg_lock_context(me->m_context);
			//me->m_connections.AddOrReplace(mgConn, std::move(conn));
			//mg_unlock_context(me->m_context);

			return 0; // unauthorized
		}

		int HttpServer::OnOpen(const mg_connection* mgConn, void** conn_data)
		{
			// conn is not fully initalized

			return 0; // must return zero
		}

		void HttpServer::OnClose(const mg_connection* mgConn)
		{
			HttpServer* me = (HttpServer*)mg_get_user_data(mg_get_context(mgConn));
			assert(me);

			if (!me->isOperational())
				return;

			

			//mg_lock_context(me->m_context);
			//me->m_connections.Remove(const_cast<mg_connection*>(mgConn));
			//mg_unlock_context(me->m_context);
		}



		int HttpServer::OnWSConnection(const mg_connection* mgConn, void* dataPtr)
		{
			const mg_request_info* request_info = mg_get_request_info(mgConn);
			assert(request_info);
			HttpServer* me = (HttpServer*)(request_info->user_data);
			assert(me);

			if (!me->isOperational())
				return 0;

			if (dataPtr)
			{
				WebSocketHandler* handler = (WebSocketHandler*)dataPtr;

				return handler->OnConnection(me, mgConn);
			}

			return 1; // No handler found, close connection
		}

		void HttpServer::OnWSReady(mg_connection* mgConn, void* dataPtr)
		{
			const mg_request_info* request_info = mg_get_request_info(mgConn);
			assert(request_info);

			HttpServer* me = (HttpServer*)(request_info->user_data);
			assert(me);

			if (!me->isOperational())
				return;

			if (dataPtr)
			{
				WebSocketHandler* handler = (WebSocketHandler*)dataPtr;

				handler->OnReadyState(me, HttpConnection(mgConn, me->m_trafficStats));
			}
		}

		int HttpServer::OnWSData(mg_connection* mgConn, int bits, char* data, size_t data_len, void* dataPtr)
		{
			const mg_request_info* request_info = mg_get_request_info(mgConn);
			assert(request_info);

			HttpServer* me = (HttpServer*)(request_info->user_data);
			assert(me);

			if (!me->isOperational())
				return 0;

			if (dataPtr)
			{
				WebSocketHandler* handler = (WebSocketHandler*)dataPtr;

				return handler->OnData(me, HttpConnection(mgConn, me->m_trafficStats), bits, data, data_len);
			}

			return 1; // No handler found
		}

		void HttpServer::OnWSClose(const mg_connection* mgConn, void* dataPtr)
		{
			const mg_request_info* request_info = mg_get_request_info(mgConn);
			assert(request_info);
			
			HttpServer* me = (HttpServer*)(request_info->user_data);
			assert(me);

			if (!me->isOperational())
				return;

			if (dataPtr)
			{
				WebSocketHandler* handler = (WebSocketHandler*)dataPtr;

				handler->OnClose(me, HttpConnectionConst(mgConn));
			}
		}
	}
#pragma endregion
}