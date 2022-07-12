#include "http/http_server.h"
#include "log.h"
// #include "http/servlets/config_servlet.h"
// #include "http/servlets/status_servlet.h"

namespace sylar {
    namespace http {

        static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

        HttpServer::HttpServer(bool keepalive
            , sylar::IOManager* worker
            , sylar::IOManager* io_worker
            , sylar::IOManager* accept_worker)
            :TcpServer(worker, io_worker, accept_worker)
            , m_isKeepalive(keepalive) {
            m_dispatch.reset(new ServletDispatch);

            m_type = "http";
            // m_dispatch->addServlet("/_/status", Servlet::ptr(new StatusServlet));
            // m_dispatch->addServlet("/_/config", Servlet::ptr(new ConfigServlet));
        }

        void HttpServer::setName(const std::string& v) {
            TcpServer::setName(v);
            //m_dispatch->setDefault(std::make_shared<NotFoundServlet>(v));
        }

        void HttpServer::handleClient(Socket::ptr client) {
            //BUG:只能接收两个请求  之后的请求就不能到这里了
            SYLAR_LOG_DEBUG(g_logger) << "handleClient " << *client;
            HttpSession::ptr session(new HttpSession(client));
            do {
                auto req = session->recvRequest();
                if (!req) {
                    SYLAR_LOG_DEBUG(g_logger) << "recv http request fail, errno="
                        << errno << " errstr=" << strerror(errno)
                        << " cliet:" << *client << " keep_alive=" << m_isKeepalive;
                    break;
                }

                HttpResponse::ptr rsp(new HttpResponse(req->getVersion()
                    , req->isClose() || !m_isKeepalive));
                //rsp->setBody("hello xlb!");
                rsp->setHeader("Server", getName());
                m_dispatch->handle(req, rsp, session);
                int res = session->sendResponse(rsp);
                if (res < 0) {
                    SYLAR_LOG_ERROR(g_logger) << "HttpServer::sendResponse error," << *client;
                } else {
                    SYLAR_LOG_INFO(g_logger) << "HttpServer::sendResponse success, send bytes = " << res << *client;
                }

                //短连接处理一次就退出
                if (!m_isKeepalive || req->isClose()) {
                    break;
                }
            } while (true);
            SYLAR_LOG_DEBUG(g_logger) << "HttpServer::handleClient exit(0)";
            session->close();
        }

    }
}
