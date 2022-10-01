#include "http_session.h"
#include "http_parser.h"
// #include <iostream>
#include "log.h"



namespace sylar {
    namespace http {
        static Logger::ptr g_logger = SYLAR_LOG_NAME("system");

        HttpSession::HttpSession(Socket::ptr sock, bool owner)
            :SocketStream(sock, owner) {}

        HttpRequest::ptr HttpSession::recvRequest() {
            HttpRequestParser::ptr parser(new HttpRequestParser);
            uint64_t buff_size = HttpRequestParser::GetHttpRequestBufferSize();
            // uint64_t buff_size = 100;
            std::shared_ptr<char> buffer(
                new char[buff_size], [](char* ptr) {
                    delete[] ptr;
                });
            char* data = buffer.get();
            int offset = 0;
            do {
                //从前端读取数据，解析
                int len = read(data + offset, buff_size - offset);//len是读取的长度
                //SYLAR_LOG_INFO(g_logger) << "start：[" << len << "] bytes";
                std::cout << data << std::endl;
                if (len <= 0) {
                    close();
                    return nullptr;
                }
                len += offset;//offset为解析剩余的长度，接着下次继续解析
                size_t nparse = parser->execute(data, len);
                if (parser->hasError()) {
                    close();
                    return nullptr;
                }
                offset = len - nparse;
                //SYLAR_LOG_INFO(g_logger) << "leftBytes:[" << offset << "]" << std::endl;
                if (offset == (int) buff_size) {
                    close();
                    return nullptr;
                }
                if (parser->isFinished()) {
                    break;
                }
            } while (true);
            //该作用是解析出请求体
            int64_t length = parser->getContentLength();
            //SYLAR_LOG_INFO(g_logger) << length;
            if (length > 0) {
                std::string body;
                body.resize(length);

                int len = 0;
                if (length >= offset) {
                    memcpy(&body[0], data, offset);
                    // SYLAR_LOG_DEBUG(g_logger) << body;
                    len = offset;
                } else {
                    memcpy(&body[0], data, length);
                    // SYLAR_LOG_DEBUG(g_logger) << body;
                    len = length;
                }
                length -= offset;
                if (length > 0) {
                    if (readFixSize(&body[len], length) <= 0) {
                        close();
                        return nullptr;
                    }
                }
                //解析出请求体
                //SYLAR_LOG_INFO(g_logger) << body;
                //std::cout << "========================" << std::endl;
                parser->getData()->setBody(body);
            }
            //如果是短连接就设置关闭属性
            parser->getData()->init();
            return parser->getData();
        }

        int HttpSession::sendResponse(HttpResponse::ptr rsp) {
            std::stringstream ss;
            ss << *rsp;
            std::string data = ss.str();
            return writeFixSize(data.c_str(), data.size());
        }

    }
}
